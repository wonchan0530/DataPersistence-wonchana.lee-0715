# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Repository purpose

This repo (`DataPersistence`) is one of several standalone PoC (Proof of Concept) repositories required for a
larger assignment: the "반도체 시료 생산주문관리 시스템" (semiconductor sample production/order management
console system). Each PoC repo validates one technical concern in isolation before it gets reused in the final
`SampleOrderSystem` project.

This PoC's scope, per `docs/[CRA_AI] Day3_개인과제_반도체시료관리.pdf`: a CRUD (Create, Read, Update, Delete)
console application that manages data as JSON files. "데이터 영속성" means the application can be restarted and
still see previously saved data, because state lives in JSON files on disk rather than only in memory.

The data domain is the "시료(Sample)" and "주문(Order)" entities from the semiconductor sample order system
(`PRD.md` has the full data model and functional requirements). `PLAN.md` has the Phase 0–8 roadmap that was
followed to build this; `DESIGN.md` has the per-Phase design/implementation/feedback log — read it before making
further changes, since it records *why* things are structured this way (e.g. why some duplication was left alone
under Rule of Three, why a lightweight test harness was chosen over GoogleTest).

## Build / run / test

Requires a C++17 compiler. Developed against MSVC (Visual Studio 2022 Build Tools) + CMake + Ninja on Windows.

```powershell
# From a "Developer PowerShell/Command Prompt for VS 2022" (so cl.exe/cmake/ninja are on PATH).
# From a plain powershell.exe/cmd.exe instead, set up the env first — note `call` is a cmd.exe-ism,
# so from plain PowerShell wrap it: cmd /c '"...\vcvars64.bat" && cmake ... && cmake --build build'

cmake -S . -B build -G Ninja
cmake --build build

# Run the console app (creates data/samples.json, data/orders.json on first write)
.\build\DataPersistenceApp.exe

# Run the test suite
ctest --test-dir build --output-on-failure
# or directly:
.\build\DataPersistenceTests.exe
```

**Important MSVC gotcha** (found in Phase 0 PoC): source files contain Korean comments/strings in UTF-8. Without
the `/utf-8` compiler flag, MSVC misinterprets them using the default codepage and fails with confusing syntax
errors. `CMakeLists.txt` already adds `/utf-8 /W4` for MSVC — don't remove it, and add it to any new MSVC-only
build target.

There is no lint step; `/W4` warnings-as-you-go is the only static check. A clean build currently has zero
warnings — keep it that way.

## Architecture

Layered structure, designed so it can be lifted into `SampleOrderSystem`'s MVC Model layer later with minimal
changes:

```
include/model/        Sample.hpp, Order.hpp       Pure data structs + to_json/from_json (nlohmann ADL hooks)
include/storage/       JsonFileStore.hpp            Generic vector<T> <-> JSON file load/save, no domain knowledge
include/repository/    SampleRepository, OrderRepository, RepositoryResult
                                                     Domain CRUD rules on top of JsonFileStore (uniqueness,
                                                     referential integrity, partial-field updates)
include/console/       ConsoleIO.hpp, SampleMenu, OrderMenu
                                                     Menu loops; only print what Repository returns
src/main.cpp           Composition root: wires repositories + menus together
third_party/nlohmann/  json.hpp (vendored, v3.11.3, header-only, MIT)
tests/test_main.cpp    Lightweight self-contained CHECK-based test harness (no GoogleTest/Catch2 — see
                       DESIGN.md Phase 6 for why)
```

Key design decisions (see `DESIGN.md` for the full reasoning per Phase):

- **Repository ↔ Storage split**: `JsonFileStore<T>` knows nothing about Sample/Order-specific rules (duplicate
  IDs, referential integrity); `SampleRepository`/`OrderRepository` know nothing about file I/O. Every Repository
  call re-reads and re-writes the whole file — intentionally simple over performant, since this is a single-user
  console PoC.
- **`OrderRepository` depends on `SampleRepository`** (one-way) to validate that an order's `sampleId` references
  an existing sample. Don't add a reverse dependency.
- **`SampleUpdate`/`OrderUpdate` patch structs** use `std::optional<T>` fields so "leave unspecified fields
  unchanged" doesn't need special-casing in the Repository.
- **`RepositoryResult{success, message}`** is the return type for all Create/Update/Delete calls; the Console
  layer just prints `result.message` — it doesn't know *why* something failed.
- Two occurrences of similar code (e.g. `SampleMenu`/`OrderMenu`'s run-loop shape) were deliberately left
  un-abstracted per the Rule of Three; three-or-more repeated blocks (e.g. the `find_if`-by-id pattern inside each
  Repository) were extracted. If you're about to add a third near-duplicate of something, that's the signal to
  extract, not before.

## Data files

`data/samples.json` and `data/orders.json` are runtime output, not source — they're gitignored and auto-created
(including missing parent directories) on first save. Tests use a separate `test_data/` directory so running
`ctest` never touches your local `data/`.
