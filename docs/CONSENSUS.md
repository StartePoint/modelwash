# ModelWash — Design consensus (grilling complete pending your OK)

Generated from the grilling session. Source of truth: `CONTEXT.md` + `docs/adr/`.

## One-liner
**ModelWash / 洗模型**：Windows 优先的本地 3D「清洗」工具——洗干净再用，顺便抹一下；美图式交互，不是 Blender。

## Who / job
- Personas: **AssetShipper** + **AICleanup**（首次启动选 PersonaProfile）
- Differentiation: **Wash（清洗）** + **TouchUp（修一修）**
- Assemble / Present 都做，但是分阶段对外

## Explicit non-goals (v1 / 1.0 bar)
- No mesh boolean
- No DCC features (retopo, rig, node materials, full UV, physics)
- No accounts / cloud processing（仅可选更新检查）
- GLB 质量保证；其它格式走 FormatAdapter（Assimp 导入面广，导出仍走专用 GLB）

## ReleaseTrain
| Slice | Ships |
|-------|--------|
| 0.1 | PersonaProfile, GLB 预览, Wash 网页/游戏, 导出, FormatAdapter+OBJ 尽力, LGPL 关于页 |
| 0.2 | TouchUpKit（三笔刷+两滑条+一键光滑） |
| 0.3 | Assemble（贴地/包围盒对齐/**SurfaceSnap**/合成导出） |
| 0.4 | Present 打磨（灯光/背景/BeautyPresets/截图等） |
| 1.0 | 安装体验与产品抛光；路线图格式仍挂 Adapter |

功能 backlog **不主动砍**（ADR-013）；用分期对外避免大爆炸。

## Tech (locked)
- **All C++ / Qt 6**；UI = **QML**；视口 = **Qt Quick 3D**
- **CMake + vcpkg**
- Import **Assimp**；Wash 导出 **专用 glTF/GLB 写入**
- Qt：**LGPL + 动态链接**走 0.x；商业许可以后再说
- 开干时 **删除 Tauri/Vite 空壳**

## Soft budgets
- ~15 万三角：TouchUp 可能卡，警告
- ~50 万三角：强烈建议先清洗
- 有 skin/morph/动画：默认跳过几何减面并提示

## Docs map
- Glossary: `CONTEXT.md`
- ADRs: `docs/adr/ADR-001` … `ADR-026`
- UI: `docs/ui/UI设计.md`
- 概要设计: `docs/design/概要设计.md`
- 详细设计: `docs/design/详细设计.md`
