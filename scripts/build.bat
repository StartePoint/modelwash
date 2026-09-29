# Prerequisites:
# - Visual Studio 2022
# - Qt 6.5+ msvc2019_64 (or compatible) with Quick3D
# Set QT_ROOT before running, e.g.:
#   set QT_ROOT=C:\Qt\6.5.3\msvc2019_64

if "%QT_ROOT%"=="" (
  echo Set QT_ROOT to your Qt kit, e.g. C:\Qt\6.5.3\msvc2019_64
  exit /b 1
)

cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH=%QT_ROOT%
if errorlevel 1 exit /b 1
cmake --build build --config Release
if errorlevel 1 exit /b 1
echo Built: build\src\app\Release\ModelWash.exe
