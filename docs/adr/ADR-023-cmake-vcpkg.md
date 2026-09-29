# ADR-023: CMake + vcpkg

## Status
Accepted

## Date
2026-09-29

## Context
Native deps (Assimp, meshoptimizer, Draco, tests) need a reproducible Windows fetch/build story alongside Qt.

## Decision
Use **CMake** as the build system and **vcpkg** for third-party C++ packages. Qt may come from official Qt installer or vcpkg — pin in README when scaffolding starts.

## Alternatives Considered

### Conan
- Fine; no strong reason over vcpkg for this solo Windows-first app

### Hand-vendored libs only
- Painful upgrades; rejected as default

## Consequences
- Manifest (`vcpkg.json`) lists Assimp, meshoptimizer, draco, gtest, etc.
- CI (when added) should restore vcpkg binary cache
