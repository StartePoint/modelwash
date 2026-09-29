# ADR-001: Dual primary personas — AssetShipper + AICleanup

## Status
Accepted

## Date
2026-09-29

## Context
ModelWash was framed as a “3D Meitu” for everyone with a model (ecommerce, games, print, AI). Those jobs conflict on defaults (beauty lighting vs face budget vs manifold solids vs topology repair).

## Decision
v1 primary personas are **AssetShipper** (web/small-game delivery) and **AICleanup** (post-AI mesh fix). Print and ecommerce-hero photography are non-primary.

## Alternatives Considered

### Single persona only
- Pros: sharper defaults
- Cons: user explicitly wants both B and C
- Rejected as sole option; dual focus accepted with later conflict-resolution rules

### Print-first
- Rejected: boolean/manifold quality bar fights “light consumer” positioning

## Consequences
- Optimize (Wash) and light sculpt (TouchUp) outrank studio-beauty and boolean
- Defaults and copy must serve shipping assets and cleaning AI meshes; when those conflict, a later ADR must pick a default winner
