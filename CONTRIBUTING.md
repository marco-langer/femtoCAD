# Contributing to femtoCAD

## Build & test

Requires `VCPKG_ROOT` to point at a vcpkg checkout (see `README.md`).

    cmake --preset release          # or: debug (for manual testing)
    cmake --build --preset release
    ctest --preset release

Binaries land in `build/<preset>/`. The app is `build/<preset>/src/app/femtoCAD`, the tests are
`build/<preset>/tests/femtoCAD-core-tests`.

## Layout

- `src/core/` — static library `femtoCAD-core`: geometry model, workspace, DXF import. Public
  headers in `src/core/include/core/`, included as `"core/..."`. Depends on QtCore only, never on
  QtGui/QtWidgets.
- `src/app/` — the `femtoCAD` executable: `MainWindow`, `SceneView` (QOpenGLWidget), `Settings`.
- `tests/` — GoogleTest/GoogleMock tests for core. `tests/Matchers/` holds custom gmock matchers
  (`IsCircle`, `AreLines`, ...) with floating-point tolerance. Test data in `tests/TestData/`.
- `cmake/dependencies/` — one file per dependency, each exposing a `femtoCAD::<name>` interface
  target. Link against those aliases, not the raw package targets.

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
| `tests/`   | New or improved tests            | Tests only (`tests/`, test CMake). **No production code.**   |
| `docs/`    | Documentation                    | `docs/`, `CLAUDE.md`, `CONTRIBUTING.md`, `README.md`, `CHANGELOG.md` only |
| `chore/`   | Build, CMake, CI, dependencies, reformatting | Build/tooling files; no behaviour changes        |

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

- **Title:** like a commit summary.
- **Summary:** what and why, in a few sentences.
- **Changes:** a bullet list of the notable changes.
- **Testing:** how it was verified (tests added, manual checks).
- **Notes:** open questions, follow-ups, known limitations (optional).
