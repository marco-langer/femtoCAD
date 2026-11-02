# 0007. Run femtoCAD without a window

- Status: Proposed
- Date: 2026-11-02

## Context

ADR 0006 gives femtoCAD an API that the GUI serves. Scripts, CI and agents also need femtoCAD
where nobody looks at a window or where there's no display at all, e.g. to query a document's
layers or to render it to an image.

## Considered options

- **Small tools per task:** e.g. one program that prints a document's layers and another that
  renders it. Each is simple, but each needs its own code and command line, and none offers the
  API.
- **A separate headless program that serves the API:** it could leave out the GUI code, but it's
  one more program to build and ship, and it could drift from the GUI's behavior.
- **A mode of femtoCAD itself (chosen):** the same program without a window, so both modes run
  the same code.

## Decision

- femtoCAD has a headless mode. In it, femtoCAD creates no window and needs no display, and it
  serves the API of ADR 0006 like the GUI does.
- Apart from the window, the headless mode runs the same code as the GUI.

## Consequences

- Scripts, CI and agents can use femtoCAD on machines without a display.
- Tests through the API in headless mode also cover what the GUI does, except the widgets.
- femtoCAD's startup splits into a part that both modes share and a part that only the GUI runs.

## Open decisions

- **Lifetime:** when a headless femtoCAD quits, e.g. through an API method, when its last client
  disconnects, or both.
- **One-shot commands:** whether the headless mode also offers commands without a client, e.g.
  exporting a PDF from the command line.
