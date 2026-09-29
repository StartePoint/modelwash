# ADR-005: First-run PersonaProfile selects defaults

## Status
Accepted

## Date
2026-09-29

## Context
v1 serves both AssetShipper and AICleanup. A single default set will feel wrong to half of users (e.g. aggressive TouchUp vs runtime-safe Wash).

## Decision
On first launch, ask the user to pick a **PersonaProfile** (AssetShipper vs AICleanup). Persist the choice (changeable in settings). It drives default WashPreset emphasis, onboarding copy, and how strongly TouchUp is suggested.

## Alternatives Considered

### Always AssetShipper defaults
- Rejected: under-serves AICleanup

### No defaults / fully manual every time
- Rejected: not 美图-like

## Consequences
- Settings must expose “我更像：资源交付 / AI 修模”
- Wash UI still offers 网页/游戏 targets; persona only sets initial recommendations
- Analytics/accounts not required to store this — local preference is enough
