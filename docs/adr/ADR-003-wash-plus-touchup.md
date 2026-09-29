# ADR-003: Differentiation is Wash + TouchUp

## Status
Accepted

## Date
2026-09-29

## Context
Offline “view + compress GLB” tools already exist (e.g. Zipoly, Ya-Anything, browser optimizers). Pure “studio beauty” or “assemble only” would either collide with compressors or become a weak viewer.

## Decision
Product one-liner: **洗干净再用，顺便抹一下。**  
Primary differentiation = **Wash** (slim/clean/export compression) + **TouchUp** (light sculpt for AI mess). Present/Assemble are supporting, not the slogan.

## Alternatives Considered

### Optimize-only
- Collides with existing compressors; weak “Meitu” story

### Beauty/assemble-first
- Misaligned with chosen personas (AssetShipper + AICleanup)

## Consequences
- Roadmap priority: Wash pipeline and TouchUp brushes before deep beauty or boolean
- Marketing and UI should center 一键瘦身 and 抹平/鼓一点, not “mini Blender”
