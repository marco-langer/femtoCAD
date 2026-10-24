# 0003. Report errors through std::expected, not exceptions

- Status: Accepted
- Date: 2026-10-24

## Context

femtoCAD is a Qt application. Qt doesn't use exceptions, and its event loop and signal–slot
mechanism aren't exception-safe: throwing an exception from a slot called by a signal–slot
connection is undefined behavior unless the slot handles it itself. Almost all of our code runs
below a slot or an event handler, e.g. the DXF import below the Import action, so an exception
thrown anywhere in the libraries can end up crossing into Qt.

Today the code mixes several approaches. The DXF import returns `std::expected` with a `QString`
message, geometric functions return `std::optional` when there's no result, and a few places throw
or call APIs that throw.

## Decision

### Our code doesn't throw

- Production code in `src/libs/` and `src/app/` never throws at runtime.
- A `throw` in a `consteval` function is allowed. It never runs: an argument that would reach it
  makes the call a compile error. This lets a constructor for literals reject invalid values at
  compile time.
- Test code may throw. An exception fails the test, which keeps tests short and readable.
- Exceptions stay enabled in the build. The standard library, fmt, GSL and GoogleTest keep working
  as specified.

### Errors are values

- A function that can fail returns `fcad::core::Result<T>`, our vocabulary type for errors. It is
  an alias for `std::expected<T, fcad::core::Error>`, declared in `core/Result.hpp`:

  ```cpp
  template <typename T>
  using Result = std::expected<T, Error>;
  ```

- `fcad::core::Error` holds:
  - `std::error_code code`: what went wrong, for callers that need to branch on it.
  - `QString message`: a description for the user, with specifics such as a line number.
- Each library, or area within a library, defines its own error enum and `std::error_category`,
  and makes the enum usable as a `std::error_code` through `std::is_error_code_enum` and
  `make_error_code`. There is no central list of codes. Codes from the standard library, such as
  `std::errc` or those from `std::filesystem`, are passed on unchanged.
- `std::optional` stays where having no result is a normal outcome, such as the bounding box of an
  empty layer or the intersection of two parallel lines. `std::expected` is for cases where
  something went wrong and the caller needs to know why.

### APIs that throw

- Prefer non-throwing alternatives to APIs that throw on failure, e.g. a checked conversion that
  returns `std::expected` instead of `gsl::narrow`.
- On `std::optional` and `std::expected`, use `.value()` after checking `has_value()`, not
  `operator*`. The check makes the throw unreachable, and compilers usually remove the second check
  in release builds. If a check is ever missing, `.value()` throws instead of reading a value that
  isn't there.
- The same goes for `.at()` on containers when the index is valid by construction, e.g. because
  it comes from a lookup in a list that is kept in sync with the container. If that invariant is
  ever broken, `.at()` throws instead of reading out of bounds.
- Where no non-throwing alternative exists, catch the exception at the call and convert it into an
  `Error`.

### Out of memory

- `std::bad_alloc` isn't handled. Running out of memory terminates the app.

### Scope

- This ADR covers errors the program expects and can recover from, such as invalid input files.
  Programming errors, such as violated preconditions or broken invariants, are decided separately.

## Consequences

- No exception from our code reaches Qt, so slots and event handlers need no `try`/`catch`.
- A function's signature shows whether it can fail and how.
- Every error domain costs some boilerplate: an enum, a category class with `name()` and
  `message()`, and the `std::is_error_code_enum` specialization.
- `core` links QtCore for `QString`.
- Existing code that throws, or that calls APIs that throw, has to change. The DXF import returns
  `Result<Document>` instead of `std::expected<Document, QString>`.
- Tests of production code check the returned error instead of using `EXPECT_THROW`.
- Exceptions that are left, such as `std::bad_alloc` or one from a throwing API we missed,
  terminate the app, or are undefined behavior if they cross into Qt.

## Open decisions

- **Programming errors:** how violated preconditions and invariants are checked and reported.
- **Translation:** whether error messages are translatable through Qt's `tr()`.
