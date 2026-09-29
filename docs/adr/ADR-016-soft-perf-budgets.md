# ADR-016: Soft performance budgets with warnings

## Status
Accepted

## Date
2026-09-29

## Context
TouchUp and SurfaceSnap need a public expectation without promising ZBrush-scale meshes.

## Decision
Soft budgets (guidance, not hard locks):
- **~150k tris**: warn that TouchUp may hitch
- **~500k tris**: strongly recommend Wash first; heavy interactivity not promised

No hard “feature disabled” gate in v1 unless a later ADR adds one.

## Consequences
- Import and mode-switch UX must surface face counts
- Perf work prioritizes Wash on huge meshes over sculpt fluidity
