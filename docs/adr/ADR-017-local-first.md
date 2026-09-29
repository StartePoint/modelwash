# ADR-017: Local-first — no accounts; optional update check only

## Status
Accepted

## Date
2026-09-29

## Context
3D assets are often confidential. Competitors in the compress/view space advertise offline trust.

## Decision
Default **100% local** processing. No login. No cloud share in v1. Network use limited to **optional update checks** (user-disableable).

## Consequences
- No backend required for v1 product loop
- Telemetry, if ever added, needs a new ADR and opt-in
