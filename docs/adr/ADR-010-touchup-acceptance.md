# ADR-010: TouchUp acceptance — three brushes + global smooth

## Status
Accepted

## Date
2026-09-29

## Context
AICleanup needs light fix-up without becoming a sculpt DCC. Brush count and “done” criteria must be capped.

## Decision
TouchUp v1 includes:
1. Brushes: **抹平** (smooth), **鼓一点** (inflate/deflate), **压平** (flatten)
2. Controls: **radius** + **strength** only
3. **One-click whole-mesh smooth** (single Laplacian-style pass, undoable)
4. Operates on the selected object’s mesh; every stroke/pass is a **Command** (undo/redo)
5. Interactive target: usable on meshes up to ~150k triangles (soft budget; warn above)

Out of v1: masks, symmetry, dynamic topology, brush preset stores.

## Consequences
- TouchUp lives in core as geometry ops, not only in the viewport layer
- UI copy stays consumer Chinese; no sculpt jargon in primary chrome
