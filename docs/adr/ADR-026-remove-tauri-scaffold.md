# ADR-026: Replace Tauri/Vite scaffold with Qt/CMake tree

## Status
Accepted

## Date
2026-09-29

## Context
An earlier bootstrap left a Tauri + Vite React template in the repo. ADR-020/021 made NativeStack the product baseline; keeping the JS shell invites wrong implementation paths.

## Decision
When implementation starts, **remove** the Tauri/Vite React scaffold and replace it with the CMake + Qt layout (`app/`, `core/`, QML, vcpkg). Do not maintain dual product runtimes.

## Consequences
- Grilling docs (`CONTEXT.md`, `docs/adr/*`) stay
- No partially-ported React “viewport” work should be continued
