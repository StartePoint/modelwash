# ADR-021: Supersede ADR-008 — Qt app is the shell from day one

## Status
Accepted

## Date
2026-09-29

## Context
ADR-008 deferred the desktop shell and pushed a shell-agnostic TypeScript core. ADR-020 selects a full C++/Qt product, which makes “defer shell” counterproductive: the application framework is Qt.

## Decision
**ADR-008 is superseded.** Engineering starts as a **CMake C++ project with Qt**:
- Still keep a layered core (`core/` Document, Command, Wash, FormatAdapter) separate from `app/` UI
- Core remains testable without GUI (GTest), but is C++, not TS
- Provisional `src-tauri/` / Vite React template is not the product baseline

## Consequences
- Dogfooding happens via running the Qt app (or a `modelwash-cli` for Wash-only tests)
- Web dogfood app is out of scope unless a future ADR revives it
