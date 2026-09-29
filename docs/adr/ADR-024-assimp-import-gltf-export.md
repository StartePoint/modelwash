# ADR-024: Assimp for import; dedicated glTF write path

## Status
Accepted

## Date
2026-09-29

## Context
ADR-009 requires GLB-first quality, OBJ best-effort, and FormatAdapter seams for later formats. User chose **Assimp** as the primary importer (broader formats) rather than tinygltf-only.

## Decision
- **Import**: Assimp behind `FormatAdapter` (glTF/GLB, OBJ, and future formats Assimp can read)
- **Export (Wash deliverable)**: **dedicated glTF/GLB writer** (not “Assimp export and hope”), so 网页/游戏 presets control Draco/meshopt/texture rules reliably
- Unsupported/broken imports: Chinese error + “请先转 GLB” when Assimp fails or quality is unsafe

## Alternatives Considered

### tinygltf-first + Assimp only for OBJ
- Lighter GLB path; user preferred Assimp-in for format breadth

## Consequences
- Must validate Assimp→Document→GLB round-trip; do not trust Assimp glTF export for product output
- FormatAdapter still isolates Assimp so a future glTF-only fast path can be added without UI changes
- License: Assimp BSD; comply when shipping
