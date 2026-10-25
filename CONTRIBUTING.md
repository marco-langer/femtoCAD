# Contributing to femtoCAD

## Build & test

See [docs/building.md](docs/building.md).

## Layout

- `src/libs/` — the libraries, layered as `core ← geo ← domain ← io` (ADR 0002). A library may
  only depend on libraries to its left. Each one is the CMake target `femtoCAD-<library>` in the
  namespace `fcad::<library>`:
  - `core` — domain-agnostic building blocks: `Angle`, `NonNegative`, the error types `Error` and
    `Result` (ADR 0003), vector utilities.
  - `geo` — geometric value types (`Coordinate`, `Line`, `Circle`, `Arc`, `Radius`, ...) and
    geometric algorithms.
  - `domain` — the CAD model: `Layer`, `Document`, `Workspace`.
  - `io` — import and export, today the DXF import. Only `io` uses opendxf.

  The libraries depend on QtCore at most, never on QtGui/QtWidgets. Each library has the folders:
  - `include/<library>/` — public headers, included as `"<library>/..."`.
  - `src/` — implementation and private headers.
  - `tests/` — the GoogleTest/GoogleMock executable `femtoCAD-<library>-tests`, with the test
    helpers and test data only it uses.
- `src/libs/test_utils/` — test code that several test executables share, such as the gmock
  matchers (`IsCircle`, `AreLines`, ...) with floating-point tolerance. Only test executables link
  it.
- `src/app/` — the `femtoCAD` executable in the namespace `fcad::app`: `MainWindow`, `SceneView`
  (QOpenGLWidget), `Settings`. Widgets of a single feature live in `features/`, e.g.
  `features/io/dxf/` for the DXF import. Includes are relative to `src/app/`.
- `cmake/dependencies/` — one file per dependency, each exposing a `femtoCAD::<name>` interface
  target. Link against those aliases, not the raw package targets.
- `docs/` — documentation. `docs/adr/` holds the architecture decision records (ADRs).

New source files must be added explicitly to the relevant `CMakeLists.txt` (no globbing),
headers included.

## Definition of done

A change is done when:

1. `cmake --build --preset release` succeeds.
2. `ctest --preset release` passes. Skipped regression tests (see below) are allowed.
3. Every C++ file the commit touches is formatted in full with `clang-format` using the repo's
   `.clang-format`.
4. Documentation is updated if the change affects it.

The debug preset is for manual testing and isn't part of the definition of done.

## Changes stay focused

Change only what the task needs. Renaming, refactoring, reformatting or fixing typos outside the
task's scope belongs in a separate change. If you find a bug along the way, open an issue instead
of fixing it in the same change.

## Bugs and regression tests

Bug fixes come with a regression test where that's feasible. Some bugs are hard to cover with a
test, and sometimes a test isn't worth the effort.

A fix with a regression test takes two commits on the `fix/` branch:

1. **Expose the bug:** add a test that fails because of the bug, and disable it with
   `GTEST_SKIP() << "regression: <short bug description>";` so CI stays green. Reviewers can
   remove the skip locally to check that the bug is real.
2. **Fix the bug:** change the production code and remove the `GTEST_SKIP()`.

## Branches

Work happens on branches, never directly on `main`. Branch names are `<prefix>/<kebab-case-topic>`,
e.g. `fix/dxf-arc-negative-radius`.

| Prefix     | Purpose                          | May modify                                                   |
|------------|----------------------------------|--------------------------------------------------------------|
| `feature/` | New functionality                | Anything needed                                              |
| `fix/`     | Bug fixes                        | Anything needed                                              |
| `refactor/` | Restructuring without behavior change | Production code, tests, build files and the docs they affect; no behavior changes |
| `tests/`   | New or improved tests            | Tests only (`src/libs/*/tests/`, `src/libs/test_utils/`, test CMake). **No production code.** |
| `docs/`    | Documentation                    | `docs/`, `CLAUDE.md`, `CONTRIBUTING.md`, `README.md`, `CHANGELOG.md` only |
| `chore/`   | Build, CMake, CI, dependencies, reformatting | Build/tooling files; no behavior changes        |

## Git

- History on `main` is linear. Branches are rebased onto `main`; there are no merge commits.
- Keep commits small and focused. Each commit should build and pass the tests.

### Commit messages

Follow [Conventional Commits](https://www.conventionalcommits.org/) and the usual git message
rules:

```
<type>(<optional scope>): <imperative summary, lowercase, no period, ≤ 72 chars>

<body: what changed and why, wrapped at 72 columns. Explain the motivation
and any non-obvious decisions or trade-offs. Mention what was tested if it
isn't obvious.>

<optional footer: trailers such as Co-Authored-By>
```

- Types: `feat`, `fix`, `test`, `docs`, `chore`, `refactor`. They usually match the branch prefix.
- Scopes are optional and name the area: `core`, `dxf`, `app`, `view`, `build`, `ci`.
- Every commit gets a body unless the summary already says everything (rare).

### Pull requests

Use the template in `.github/pull_request_template.md`.
