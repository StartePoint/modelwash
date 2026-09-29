# ADR-018: Release 0.1 acceptance (Wash slice)

## Status
Accepted

## Date
2026-09-29

## Context
ReleaseTrain (ADR-014) needs a concrete public bar for the first downloadable slice. User accepted the recommended checklist.

## Decision
**0.1 is accepted when all of the following work locally:**
1. PersonaProfile first-run choice persisted
2. Open **GLB/glTF** into a previewable Document
3. Wash targets **网页** / **游戏** with before/after face count and approximate size
4. Export GLB; StaticWashOnly safety (ADR-015)
5. **FormatAdapter** interface landed; **OBJ** best-effort import wired; non-supported formats show convert-to-GLB guidance (FBX etc. stub/roadmap only)
6. LocalFirst (ADR-017)

Out of 0.1: TouchUp, Assemble/SurfaceSnap, full Present polish (those are 0.2–0.4).

## Consequences
- Engineering order: core Document/IO/Wash before shell finalization
- 0.1 may dogfood in a temporary web or provisional shell wrapper
