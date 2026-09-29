# ADR-012: Assemble aids include surface snap

## Status
Accepted

## Date
2026-09-29

## Context
Full Assemble was already accepted. Align quality needed a ceiling: bbox/ground only vs object-to-object surface snap vs CAD-grade constraints.

## Decision
Assemble v1 includes:
- Ground (贴地)
- Bounding-box center/align helpers
- **SurfaceSnap**: ray-based stick of a moving object onto another object’s surface

Out of v1: angle snap suites, arrays, measurement tools, constraint solvers.

## Consequences
- Need clear failure UX when ray miss / non-manifold targets
- Snap is a transform Command (undoable), not a live physics constraint
- Implementation complexity is non-trivial; schedule under Assemble, not as a one-day add-on
