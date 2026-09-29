# ADR-004: No boolean in v1; TouchUp sculpt allowed

## Status
Accepted

## Date
2026-09-29

## Context
Boolean union/difference on consumer meshes (especially AI output) fails often and pulls expectation toward CAD. Light sculpt (smooth/inflate/flatten) maps better to Meitu-style “touch up.”

## Decision
**v1 includes limited TouchUp sculpt. v1 excludes mesh boolean.** Failed-boolean fallback messaging is unnecessary because the feature is out.

Assemble-by-bake (merge transforms into one GLB without CSG) remains separately decidable.

## Alternatives Considered

### Boolean as best-effort with soft failure
- Pros: checkbox feature
- Cons: support burden, trust damage when it fails on AI meshes
- Deferred past v1

### No sculpt either
- Safer, but undercuts AICleanup persona

## Consequences
- Do not depend on manifold-3d for v1
- Sculpt scope must stay tiny (few brushes, two sliders) — detail in a later ADR after success criteria are grilled
