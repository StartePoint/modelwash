# ADR-011: Full Present (lighting, background, beauty presets) in v1

## Status
Accepted

## Date
2026-09-29

## Context
Wash + TouchUp are the slogan, but the user wants a strong first impression: multiple lighting presets, backgrounds, and one-click beauty materials (matte/metal/ceramic/plastic).

## Decision
v1 ships **full Present**:
- Lighting presets (e.g. studio / outdoor / soft / product white)
- Background presets (solid / gradient / HDRI thumbnails as available)
- BeautyPresets for PBR quick looks + basic color/metalness/roughness/texture swap

## Alternatives Considered

### Minimal Present only
- Faster; user chose full Present instead

## Consequences
- Present competes with Assemble and Wash for calendar time; user accepted long v1 (see ADR-013)
- Keep Present as presets, never a material node graph
