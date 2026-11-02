# 0006. Control femtoCAD through a local API

- Status: Proposed
- Date: 2026-11-02

## Context

femtoCAD is meant to be used by AI agents as well as by people, and agents should be first-class
users, not an afterthought. Today an agent can only build the app, start it and look at the
window, or write throwaway code against the libraries. It can't open a file in the running app or
ask what the document contains, e.g. its layers or the bounding box of its entities, and it can't
see what the view shows.

There are two ways an agent or a script needs to use femtoCAD:

- **Live:** in the running app, next to the user, who sees every change.
- **Headless:** without a window, e.g. to query a document or to render it in a script or in CI.

Editing hasn't come to the GUI yet. If the automation interface comes first, editing can be
designed so that the GUI and the interface use the same operations, instead of the interface
imitating the GUI later.

The Model Context Protocol (MCP) is the common way for agents to use tools today. It's a young
protocol that still changes, and only agents speak it.

## Considered options

### How clients reach femtoCAD

- **MCP built into the app:** the shortest path for agents. It ties the app to MCP and its
  changes, and scripts and tests would need an MCP client too.
- **Command line only:** one-shot commands such as "open this file and print its layers". Simple,
  but every call loads the document again, and nothing can reach the running GUI.
- **Our own API, with MCP as an adapter on top (chosen):** the app only depends on its own
  protocol. Scripts and tests use the API directly, and MCP is one client among others.

### Transport and protocol

Transport and protocol constrain each other, so the options are combinations:

- **HTTP/REST over TCP on localhost:** the best tooling (curl, any HTTP client). But every local
  process and user can connect, so it needs a token, and pushing changes to clients needs extra
  machinery such as server-sent events or WebSockets.
- **HTTP/REST over a local socket:** possible, but browsers can't connect and other tools rarely
  can, so HTTP loses most of its advantage. It still lacks a way to push changes.
- **JSON-RPC over stdio:** the simplest option, but only the process that started femtoCAD can
  talk to it, so nothing can attach to the GUI the user already runs.
- **gRPC:** a typed schema, generated clients and built-in streaming. But MCP describes tools
  with JSON Schemas, so the adapter would translate between two schema systems, and every script
  would need generated code and a gRPC library. Messages are binary and hard to inspect, calls
  arrive on gRPC's own threads, and it's a heavy new dependency.
- **JSON-RPC 2.0 over a local socket (chosen):** the operating system restricts access to the
  user, messages flow both ways over one connection, and it needs nothing beyond Qt. MCP is
  JSON-RPC as well and describes its tools with JSON Schemas, so the adapter can turn API methods
  into tools without code for each method.

### How the API is described

- **Only a written document:** easy to start, but clients and the adapter can't read it, so the
  adapter needs code for each method, and the document drifts from the code unnoticed.
- **A format of our own:** machine-readable, but we'd design and maintain it, and no tool knows
  it.
- **OpenRPC (chosen):** the standard description format for JSON-RPC 2.0, like OpenAPI for HTTP.
  It describes parameters and results as JSON Schemas, which MCP uses too. Its ecosystem is much
  smaller than OpenAPI's.

## Decision

### A local API

- femtoCAD has an API through which scripts, tests and agents control it. The API speaks
  JSON-RPC 2.0.
- The API is served over a local socket: a Unix domain socket on Linux and macOS, a named pipe on
  Windows. It's never served over TCP. Only the user who runs femtoCAD can connect, so the API has
  no authentication of its own.
- The API describes itself in OpenRPC: every method with its parameters and result as JSON
  Schemas, and its errors. Clients get the description through the method `rpc.discover`.
- The API is versioned, and the version is part of its description.
- Errors from the libraries (ADR 0003) become JSON-RPC errors that keep their category and
  message.

### The API in the GUI

- The GUI serves the API on the workspace it shows, but only when the user enables it.
- API calls run on the main thread, between the GUI's events. The API and the user never change
  the document at the same time, and the view shows every change the API makes.
- Headless use is decided in ADR 0007.

### MCP through an adapter

- MCP support is a separate program that agents start over stdio. It connects to a running
  femtoCAD or starts one without a window (ADR 0007), and translates between MCP tools and API
  methods.
- The adapter builds its tools from the API's description, so a new API method becomes a tool
  without a change to the adapter.
- The adapter has no logic of its own and doesn't link the libraries. Everything an agent can do
  through MCP, a script can do through the API.

### The GUI and the API share their operations

- GUI actions and API methods call the same operations in the libraries. Neither front end does
  work the other one can't do. A later ADR on editing defines these operations as commands with
  undo.

### A new library

- The server and the methods live in a new library `api`, which builds on `io`. The dependency
  graph of ADR 0002 becomes:

  ```
  core  ←  geo  ←  domain  ←  io  ←  api  ←  app
  ```

- `api` may use QtNetwork in addition to QtCore.

## Consequences

- Agents and scripts can open and inspect documents without the GUI, and they can follow and
  drive the user's session in the GUI.
- New features get an API method next to their GUI action.
- The API is a public interface. Changing a method breaks clients, so changes go through its
  version.
- The OpenRPC description is the API's reference documentation.
- The app links QtNetwork, and the MCP adapter is one more program to build and ship.
- The API isn't reachable from another machine. Remote use would need forwarding, or a later
  decision.

## Open decisions

- **Several instances:** how sockets are named when several femtoCADs run, and how the adapter
  picks one.
- **Enabling the API in the GUI:** a setting, a command line flag or both, and the default.
- **The first methods:** which methods the first version offers.
- **Source of the description:** a hand-written OpenRPC document in the repository that a test
  checks against the server's methods, or a document the server builds from its methods.
- **Notifications:** whether the API tells clients about changes, e.g. when the user edits the
  document.
