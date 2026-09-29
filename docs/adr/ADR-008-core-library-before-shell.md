# ADR-008: Build core library before committing desktop shell

## Status
Accepted

## Date
2026-09-29

## Context
A Tauri scaffold already exists in the repo from an earlier bootstrap. For a public product, the costly part is Wash/TouchUp/Assemble correctness, not the window chrome. Premature shell coupling slows testing of mesh pipelines.

## Decision
**Defer shell choice.** Implement and test **core** (Document, Command, IO, Wash pipeline, TouchUp ops) as a shell-agnostic TypeScript (and WASM) library first. Wrap with a desktop shell only after core paths are real. Existing `src-tauri/` is provisional, not an accepted platform ADR.

## Alternatives Considered

### Commit Tauri 2 now
- Convenient, but locks packaging before core exists

### Commit Electron now
- Heavier; same premature coupling

## Consequences
- Prefer `packages/core` (or `src/core`) with Node/browser-capable tests
- UI may start as Vite web for dogfooding; desktop packager decided in a later ADR
- Do not invest in Tauri plugins until shell ADR is accepted
