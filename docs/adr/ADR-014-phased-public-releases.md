# ADR-014: Phased public ReleaseTrain (not a big-bang 1.0)

## Status
Accepted

## Date
2026-09-29

## Context
ADR-013 keeps the full feature backlog. Without ship slices, “public Windows product” risks never releasing. User accepted phased external downloads.

## Decision
Ship a **ReleaseTrain**:
- **0.1** — Open/preview GLB + Wash (网页/游戏) + export; PersonaProfile onboarding
- **0.2** — TouchUpKit
- **0.3** — Assemble (align/ground/SurfaceSnap + bake-merge)
- **0.4** — Present polish (full lighting/background/BeautyPresets, capture polish)
- **1.0** — Shell packing, polish bar, roadmap formats still via FormatAdapter

Nothing in accepted ADRs is deleted from the backlog; only the *public* readiness order is sliced.

## Consequences
- Each slice needs a written acceptance checklist before coding that slice
- Core must be testable without waiting for 0.4 Present
- Marketing must say Beta/0.x honestly until 1.0
