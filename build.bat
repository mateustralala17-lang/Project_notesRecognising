@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" > nul
cl.exe /Zi /EHsc /nologo /Iinclude /Feapp.exe src\*.cpp
if %errorlevel% equ 0 app.exe