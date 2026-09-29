# ADR-007: Full Assemble in v1 (align / ground / snap)

## Status
Accepted

## Date
2026-09-29

## Context
Wash + TouchUp are the slogan, but users may bring multiple AI parts or kit pieces and need one deliverable GLB. Weak assemble (transform only) vs full align/ground/snap was debated.

## Decision
v1 includes **full Assemble**: multi-object scene, align helpers, ground, simple snap, and bake-merge export to one GLB. Still **no boolean/CSG** (ADR-004).

## Alternatives Considered

### Defer assemble / weak assemble only
- Faster v1; user rejected in favor of full assemble

## Consequences
- Significant UX and interaction cost; something else in the original mega-plan must slip or thin out (to be grilled)
- Document model is multi-object from day one
- Snap/align quality bar must be defined so it does not become a CAD constraint solver
