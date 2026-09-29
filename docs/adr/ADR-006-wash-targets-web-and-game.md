# ADR-006: Wash presets named by runtime target (Web + Game)

## Status
Accepted

## Date
2026-09-29

## Context
AssetShipper cares which runtime will load the file. Abstract 轻/中/强 alone hides Draco vs meshopt, texture caps, and animation-safety choices.

## Decision
Primary Wash UI exposes named targets **网页** and **游戏** (plus custom/advanced). Both share one optimize pipeline; parameters differ (compression family, max texture edge, decimation aggressiveness, conservative rules for skinned/morph meshes when present).

## Alternatives Considered

### Only 轻/中/强
- Simpler UI, weaker for AssetShipper

### Add mobile AR / print targets in v1
- Scope expansion; deferred

## Consequences
- Need documented parameter tables for 网页 vs 游戏 (later ADR or preset JSON)
- Advanced panel may still show numeric 轻/中/强 or face-count goals
- Validate exports against the chosen target’s common loaders where practical
