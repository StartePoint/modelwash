@echo off
set QT_ROOT=C:\Qt\6.5.3\msvc2019_64
set APP_DIR=%~dp0..\build\src\app\Release
set PATH=%APP_DIR%;%QT_ROOT%\bin;%SystemRoot%\System32;%SystemRoot%
set QT_PLUGIN_PATH=%APP_DIR%
set QML2_IMPORT_PATH=%APP_DIR%\qml
set QML_IMPORT_PATH=%APP_DIR%\qml
cd /d "%APP_DIR%"
start "" "%APP_DIR%\ModelWash.exe"
