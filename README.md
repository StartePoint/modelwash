# ModelWash / 洗模型

Windows-first native **C++ / Qt 6** tool: wash 3D assets until they are light and usable.

## Docs

- [共识](docs/CONSENSUS.md)
- [UI 设计](docs/ui/UI设计.md) · [HTML 线框](docs/ui/ui-mockup.html)
- [概要设计](docs/design/概要设计.md) · [详细设计](docs/design/详细设计.md)
- [ADR](docs/adr/) · [术语表](CONTEXT.md)

## Build (0.1)

Requirements:

- Windows x64
- Visual Studio 2022 (MSVC)
- CMake ≥ 3.21
- Qt 6.5+ with **Quick** and **Quick3D** (and Quick Timeline for deploy)

Example:

```bat
set QT_ROOT=C:\Qt\6.5.3\msvc2019_64
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH=%QT_ROOT%
cmake --build build --config Release
%QT_ROOT%\bin\windeployqt.exe --release --qmldir src\app\qml build\src\app\Release\ModelWash.exe
```

Or: `scripts\build.bat` then deploy, then `scripts\run.bat`.

Run:

```bat
scripts\run.bat
```

Executable: `build\src\app\Release\ModelWash.exe`

## 0.1 status

- Persona onboarding
- Open GLB/glTF/OBJ (Assimp)
- Qt Quick 3D preview
- Wash presets 网页 / 游戏 + preview/apply + undo
- Export GLB (dedicated writer)
- About (LGPL note)

Not in 0.1: 修一修 / 拼一拼 / 摆好看 (UI entries disabled).
