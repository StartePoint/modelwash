# ADR-013: No voluntary scope cut — accept a long v1

## Status
Accepted

## Date
2026-09-29

## Context
When asked what to cut under Wash + TouchUp + full Assemble + public-product bar, the user chose **not to cut** and accept that v1 takes longer.

## Decision
Do not drop HDRI/transparent capture/recent files/session autosave/etc. merely to shrink scope. Calendar elongates instead. Feature set remains the union of accepted ADRs unless a future ADR supersedes this.

## Alternatives Considered

### Cut polish items (HDRI, transparent screenshot, …)
- Rejected by user

### Downgrade Assemble
- Would require superseding ADR-007; rejected

## Consequences
- **Ship definition becomes critical** (beta vs 1.0) — must be decided explicitly
- Risk of never shipping a coherent “1.0”; mitigate with phased public milestones without deleting features from the backlog
- Engineering order still follows dependency (core → Wash → TouchUp → Assemble → Present polish), even if nothing is deleted
