# ADR-025: Qt LGPL + dynamic link for early public releases

## Status
Accepted

## Date
2026-09-29

## Context
ModelWash is a public Windows product that may remain closed-source. Qt offers LGPL (with compliance duties) and commercial licenses. Early ReleaseTrain slices should not block on buying a commercial license.

## Decision
- Develop and distribute **0.x** using **Qt open-source (LGPLv3) with dynamic linking**
- Ship LGPL notices / offer of Qt sources as required
- Re-evaluate **Qt commercial** if/when static single-file installs, certain store policies, or compliance burden become blockers

## Alternatives Considered

### Commercial Qt from day one
- Cleaner for some distribution cases; deferred for cost

### Ignore LGPL obligations
- Rejected — unacceptable for a public product

## Consequences
- Installer must deploy Qt DLLs (or use an approved shared mechanism), not silently static-link LGPL Qt
- “About” / license screen is part of 0.1 polish bar
- Not legal advice; revisit if product/legal context changes
