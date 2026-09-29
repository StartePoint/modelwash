@echo off
setlocal
cd /d "%~dp0\.."

set "VCVARS=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
if not exist "%VCVARS%" (
  echo [ERROR] VS2022 vcvars64.bat not found
  exit /b 1
)

call "%VCVARS%" >nul
set "NINJA=%CD%\tools\ninja.exe"
set "QT=C:/Qt/6.5.3/msvc2019_64"
set "ASSIMP=%CD:\=/%/build/_deps/assimp-src"
set "MESH=%CD:\=/%/build/_deps/meshoptimizer-src"
set "OUT=%CD%\cmake-build-release"

if not exist "%OUT%\build.ninja" (
  cmake -S "%CD%" -B "%OUT%" -G Ninja ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_MAKE_PROGRAM="%NINJA%" ^
    -DCMAKE_PREFIX_PATH="%QT%" ^
    -DFETCHCONTENT_SOURCE_DIR_ASSIMP="%ASSIMP%" ^
    -DFETCHCONTENT_SOURCE_DIR_MESHOPTIMIZER="%MESH%" ^
    -DFETCHCONTENT_FULLY_DISCONNECTED=ON
  if errorlevel 1 exit /b 1
)

cmake --build "%OUT%" --target ModelWash -j
if errorlevel 1 exit /b 1

echo.
echo OK: %OUT%\src\app\ModelWash.exe
exit /b 0
