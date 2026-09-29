# ADR-002: Public Windows product, not a private toy

## Status
Accepted

## Date
2026-09-29

## Context
Scope and polish differ wildly between a personal script, a portfolio demo, and a shipped desktop product (installer, crash UX, unsupported-format copy, performance on large meshes).

## Decision
Treat ModelWash as a **public product** with at least a **Windows** build. Design and engineering choices optimize for someone downloading and using it without hand-holding.

## Alternatives Considered

### Personal tool / learning demo
- Pros: faster cuts
- Cons: contradicts stated intent (Q2=B)
- Rejected for v1 planning bar

## Consequences
- Friendly errors, recent files, drag-drop, and installable packaging are in-scope
- Feature cuts should favor reliability over experimental mesh ops
- Monetization not decided yet; do not build accounts/cloud into v1 unless a later ADR says so
