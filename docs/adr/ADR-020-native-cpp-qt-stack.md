# ADR-020: Entire product in C++ / Qt (NativeStack)

## Status
Accepted

## Date
2026-09-29

## Context
The author is far more productive in C++ than in TypeScript. WASM-from-C++ was considered for compute-only. The user chose to implement the **whole application** in C++ rather than a TS UI shell.

## Decision
ModelWash ships as a **native Windows (first) C++ desktop app**:
- UI framework: **Qt** (Widgets vs QML chosen in a follow-up ADR)
- 3D viewport: native rendering stack (exact API chosen in a follow-up ADR)
- Wash / TouchUp / IO: native C++ libraries (meshoptimizer, Draco, tinygltf/Assimp, etc.)
- **Not** product runtime: React, Three.js, Tauri, Electron

## Alternatives Considered

### TS UI + C++ WASM compute
- Better Meitu UI velocity; rejected for author familiarity / ownership

### Full C++ but non-Qt (Win32, wx, custom)
- Possible; Qt chosen as default industry path for desktop + styling unless superseded

## Consequences
- Supersedes ADR-008 (“core TS library before shell”) — see ADR-021
- Original Tauri scaffold in repo is disposable / non-binding
- 美图-level polish is owned in Qt (likely QML); schedule risk sits in viewport gizmos and SurfaceSnap
- FormatAdapter, Document, Command become C++ types
- ReleaseTrain (0.1→1.0) remains valid; 0.1 as native Wash tool is a natural first slice
