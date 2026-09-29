@echo off
call "%~dp0clion-build.bat"
if errorlevel 1 exit /b 1
start "" "%~dp0..\cmake-build-release\src\app\ModelWash.exe"
exit /b 0
