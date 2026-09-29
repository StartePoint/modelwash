# ADR-022: QML UI + Qt Quick 3D viewport

## Status
Accepted

## Date
2026-09-29

## Context
NativeStack (ADR-020) needs a UI toolkit and a realtime viewport. Meitu-like layout favors Qt Quick; Widgets are more C++-direct but weaker for consumer polish.

## Decision
- Application chrome: **Qt Quick (QML)** — modes 清洗 / 修一修 / 拼一拼 / 摆好看, wizards, cards
- 3D preview: **Qt Quick 3D** (`View3D`), same process as UI
- Domain logic stays in C++ (`core/`), exposed to QML via QObject properties/models

## Alternatives Considered

### Widgets + QOpenGLWidget
- Strong for pure C++ UI; weaker Meitu motion/layout; deferred

### Third-party engines (bgfx/Filament)
- Extra integration cost; deferred unless Quick 3D blocks TouchUp/SurfaceSnap

## Consequences
- Team must accept QML ↔ C++ binding as a skill tax
- Custom picking/gizmos/sculpt may need Quick 3D extensions or a hybrid render path later
- Prefer Qt 6.x LTS line when pinning versions
