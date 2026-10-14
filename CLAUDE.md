# femtoCAD

A small 2D CAD application in C++20 with Qt6 (Widgets + OpenGL). Currently it imports and
displays DXF files; editing and a native file format are planned.

The project rules for everyone (build, layout, definition of done, branches, commits, PRs)
are in CONTRIBUTING.md and apply in full:

@CONTRIBUTING.md

The rules below apply to Claude in addition.

## Workflow

- **Never assume.** If anything about a task is unclear (scope, intent, expected behaviour),
  ask the user before starting. Ask during a task as well when something new comes up.
- When an established best practice (e.g. the C++ Core Guidelines) conflicts with existing code
  or with a request, point out the conflict and ask. Don't pick one silently.
- **No drive-by changes**, not even typo fixes, unless the user agrees first. Point them out
  instead.
- **Report bugs found along the way to the user and record them in memory.** Don't fix them as
  part of the current task.
- **Before writing a regression test, ask the user whether one is needed.**
- If a task needs changes outside its branch's scope (see the branch table), stop and ask.
- Never change `.clang-format` without asking the user first.
- Never add, remove or bump a dependency without asking the user first. This covers
  `vcpkg.json` (packages and the `builtin-baseline`), the FetchContent pins in
  `cmake/dependencies/` (e.g. opendxf's `GIT_TAG`) and system packages such as Qt.

## Git

- Commit on your own once the definition of done is met.
- Commits that haven't landed on `main` yet may be amended or reworked. Never rewrite `main`.
- Never push and never merge. The user does both.
- End commit messages with `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`.
- The user opens PRs. When asked, draft the description following CONTRIBUTING.md and end it with
  `🤖 Generated with [Claude Code](https://claude.com/claude-code)`.
