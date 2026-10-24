# 0002. Structure the code as layered libraries

- Status: Accepted
- Date: 2026-10-24

## Context

All non-UI code lives in one library, `femtoCAD-core`: math helpers, geometry, layers, the
workspace and the DXF import. Nothing stops geometry code from depending on layers or on the DXF
import, and the DXF parser is a dependency of everything that links core. All tests build into one
executable.

ADR 0001 separates geometric value types from CAD entities and leaves open where they live. More
code that is domain logic but not UI is coming: documents, undo, more file formats. The code needs
a structure whose dependency rules the build enforces, before the refactor for ADR 0001 and before
the app gets tests.

## Decision

- The code is split into libraries with a fixed dependency direction:

  ```
  core  ←  geo  ←  domain  ←  io  ←  app
  ```

  A library may only depend on libraries to its left. CMake enforces this through
  `target_link_libraries`.
- Contents:
  - `core`: domain-agnostic building blocks such as `Angle`, `NonNegative` and utilities.
  - `geo`: geometric value types (`Coordinate`, `Line`, `Circle`, `Arc`, `Radius`, ...) and
    geometric algorithms.
  - `domain`: the CAD model and its logic, without UI: entities, `EntityId`, `Layer`, `LayerId`,
    `Workspace`.
  - `io`: importers and exporters, today the DXF import. They map file formats to the domain and
    back, but the domain knows nothing about file formats.
  - `app`: the executable with the widgets.
- Only `app` uses QtGui and QtWidgets. The libraries use QtCore at most. Third-party file format
  libraries such as opendxf are used only by `io`.
- Each library has its own namespace `fcad::<library>`. The app uses `fcad::app`. The geometric
  line is `fcad::geo::Line`, the line entity `fcad::domain::Line`.
- Entities get no library of their own. Everything that uses entities also needs layers and the
  workspace, and entities and layers depend on each other, so a separate library would add a
  boundary that protects nothing.
- Each library is a CMake target `femtoCAD-<library>` and lives in `src/libs/<library>/`:
  - `include/<library>/`: public headers, included as `"<library>/..."`.
  - `src/`: implementation and private headers.
  - `tests/`: the library's test executable `femtoCAD-<library>-tests`, together with the test
    helpers and test data that only this executable uses.
- Test code that several test executables need, such as gmock matchers, goes into the library
  `test_utils` in `src/libs/test_utils/`. Only test executables link it.
- The app stays an executable in `src/app/`.

## Consequences

- The build rejects dependencies against the direction, e.g. geometry code using layers.
- opendxf no longer reaches `domain` and below, so these build and test without it.
- Each library is tested on its own, close to its code. `ctest` still runs all test executables.
- More libraries and test executables mean more CMake boilerplate.
- This introduces namespaces into a codebase that has none so far. Include paths and namespaces
  change throughout the code. Moving the code is a large, mechanical refactor, and the layout
  section in CONTRIBUTING.md has to follow it.
- `test_utils` depends on every library whose types it supports. It only takes code that more
  than one test executable needs, so it doesn't become a dumping ground.
- The app can't be tested yet, since the code lives in the executable.

## Open decisions

- **App tests:** whether `app` becomes a library with a thin `main` once there is a testing
  strategy for the UI.
