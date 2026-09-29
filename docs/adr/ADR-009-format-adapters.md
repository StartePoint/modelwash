# ADR-009: GLB-first support; FormatAdapter seam for the rest

## Status
Accepted

## Date
2026-09-29

## Context
Public products lose trust when “supported formats” load half-broken. The Wash pipeline is glTF-native. The user still wants non-GLB formats on the roadmap with code seams reserved early.

## Decision
- **v1 guaranteed**: glTF / GLB.
- **v1 best-effort**: OBJ (+ common MTL/textures), with honest quality limits in UI.
- **Not v1-guaranteed**: FBX and others — listed in the roadmap, implemented only behind a **FormatAdapter** interface (detect → import to Document / export from Document).
- Unsupported files show a clear message: convert to GLB first.

## Alternatives Considered

### Guarantee many formats in v1
- Rejected: quality and support cost

### No interface until second format
- Rejected by user: reserve the seam while coding v1

## Consequences
- Core IO depends on `FormatAdapter`, not scattered loaders in UI
- Roadmap may name FBX/STL/etc. without treating them as v1 acceptance criteria
- Tests gate on GLB round-trip; OBJ tests are best-effort fixtures
