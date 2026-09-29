# ModelWash — Domain Glossary

Living glossary from design grilling. Prefer these names in code, UI, and ADRs.

## Product

| Term | Meaning |
|------|---------|
| **ModelWash** | Desktop app: wash a 3D model until it is light enough and good enough to ship. Not a DCC. |
| **美图定位** | Consumer-simple UX (few modes, presets first). Explicitly not Blender-depth modeling. |
| **NativeStack** | Entire app in **C++** (Qt UI + native 3D viewport + native Wash/TouchUp). No React/Three/Tauri as product runtime. |
| **UiKit** | **Qt Quick / QML** for chrome (美图-style modes and panels). |
| **ViewportStack** | **Qt Quick 3D** (`View3D`) for the realtime preview. |
| **BuildSystem** | **CMake + vcpkg** for Qt app and third-party native deps. |
| **QtLicensePath** | Develop and ship early releases under **Qt LGPL with dynamic linking**; revisit commercial Qt if store/static needs appear. |

## Personas (v1 focus)

| Term | Meaning |
|------|---------|
| **AssetShipper** | Web / small-game creator who needs reliable, smaller GLB (or similar) for runtime. |
| **AICleanup** | User who got a messy AI mesh and needs light fix + slim before use. |

Primary v1 audience: **AssetShipper + AICleanup**. Not print-first, not ecommerce-photo-first.

## Jobs

| Term | Meaning |
|------|---------|
| **Wash** | Main job: clean + reduce cost (faces, textures, unused data, export compression). |
| **TouchUp** | Secondary job: light sculpt (smooth / inflate / flatten) to fix AI mess. No boolean in v1. |
| **Present** | Supporting: lighting/background presets so the model looks acceptable while washing. |
| **Assemble** | v1 multi-model place, align/ground/snap, bake-merge export (no CSG boolean). |
| **PersonaProfile** | First-run choice: AssetShipper vs AICleanup; drives default WashPreset and copy. |
| **WashTarget** | Named export intent: **网页** (Web/Three-like) or **游戏** (Unity-like pack), not raw 轻/中/强 in primary UI. |

## Objects

| Term | Meaning |
|------|---------|
| **Document** | In-memory project: scene objects, materials, edit history. |
| **WashPreset** | Named slim profile (e.g. light/medium/strong or runtime-oriented). |
| **BeautyPreset** | One-click PBR look (matte/metal/ceramic/plastic) — not a node graph. |
| **Command** | Undoable mutation of the Document. |

## Explicit non-goals (v1)

| Term | Meaning |
|------|---------|
| **NoBoolean** | No union/difference mesh boolean in v1 (failure-prone; not 美图-like). |
| **NoDCC** | No retopo, rigging, node materials, full UV tools, physics. |
| **FormatAdapter** | Import/export plug-in seam. v1 ships GlTF adapter (+ best-effort OBJ); other formats planned behind the same interface. |
| **TouchUpKit** | Smooth / inflate / flatten brushes + radius/strength; plus one-click whole-mesh smooth. |
| **SurfaceSnap** | Assemble aid: ray-based object-to-object surface sticking (beyond ground/bbox align). |
| **ReleaseTrain** | Public phased builds: 0.1 Wash → 0.2 TouchUp → 0.3 Assemble → 0.4 Present polish (features stay on backlog, ship in slices). |
| **StaticWashOnly** | Wash geometry decimation skips skinned / morph / animated meshes by default; warn instead of corrupting. |
| **LocalFirst** | Models and edits stay on device; no account; network only for optional update check. |
