@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=amd64
where cmake
cmake --build e:\AIProject\modelwash\build --config Release --target ModelWash
exit /b %ERRORLEVEL%
