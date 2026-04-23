# Repository Guidelines

## Project Structure & Module Organization
`src/` contains production C++ code, organized by domain: `model/`, `view/`, `viewmodel/`, `mvvm/`, `window/`, `input/`, `utils/`, and `ais/`. Entry points are `src/main.cpp` and `src/Application*.{h,cpp}`.  
`tests/` contains Boost.Test suites named `*_test.cpp`; test assets live in `tests/data/`.  
`IFR/installed/` stores prebuilt feature-recognition binaries and sample data used by optional IFR integration.

## Build, Test, and Development Commands
- `cmake --preset debug`  
  Configure with Visual Studio 2022, vcpkg toolchain, and output to `build/DebugWT`.
- `cmake --build --preset debug`  
  Build app and tests.
- `ctest --test-dir build/DebugWT -C Debug -V`  
  Run all tests with verbose output.
- `ctest --test-dir build/DebugWT -C Debug -R model_importer_test -V`  
  Run one test target by regex.
- `format_code.bat src` or `format_code.bat tests`  
  Apply `clang-format` recursively to C/C++ files.

## Coding Style & Naming Conventions
Use C++17 and keep code simple and deterministic. Follow existing naming patterns:
- Types/classes: `PascalCase` (example: `ApplicationBootstrapper`)
- Methods/functions: `camelCase` (example: `initialize()`)
- Member fields: `my` prefix (example: `myLogger`)
- Test files: `snake_case` + `_test.cpp`

Use 4-space indentation and brace style consistent with current sources. Add Doxygen-style comments for new or changed public classes/functions in headers (`/** ... */` with `@brief` where helpful).

## Testing Guidelines
Testing uses Boost.Test via CTest (`add_boost_test(...)` in `CMakeLists.txt`). For each behavior change:
- Add or update at least one focused unit/integration test in `tests/`.
- Prefer deterministic checks and include negative-path assertions.
- Put required fixtures/files under `tests/data/` to keep tests self-contained.

## Commit & Pull Request Guidelines
This working copy has no `.git` metadata, so follow repository house rules:
- Commit format: `feat/fix/docs/test/chore : <summary>`
- One logical change per commit; keep diffs reviewable.
- PRs should include: problem statement, design notes, test evidence (`ctest` command/result), and screenshots/GIFs for UI changes.
- Link related issue/task IDs when available.
