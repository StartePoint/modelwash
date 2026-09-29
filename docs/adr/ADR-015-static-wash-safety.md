# ADR-015: Wash geometry ops are static-mesh-safe by default

## Status
Accepted

## Date
2026-09-29

## Context
AssetShipper assets may include skin, morph targets, or animations. Aggressive decimation without a dedicated animation pipeline corrupts them.

## Decision
By default, Wash **skips geometry simplification** on skinned / morph / animated meshes, shows a clear Chinese warning, and still may apply safe texture/prune/compression steps that do not break those channels. No v1 claim of “full animated asset optimize.”

## Consequences
- Importer must detect and flag these cases on Document
- Advanced overrides (if any) are opt-in and scary-copy, not default
