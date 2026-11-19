@echo off
setlocal

where cl.exe >nul 2>&1
if %errorlevel% neq 0 (
    set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
    if exist "%VSWHERE%" (
        for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
            set "VS_PATH=%%i"
        )
    )
    if defined VS_PATH (
        call "%VS_PATH%\VC\Auxiliary\Build\vcvars64.bat" >nul
    ) else (
        echo [ERROR] MSVC compiler not found. Run from Developer Command Prompt for VS.
        exit /b 1
    )
)

cl.exe /Zi /EHsc /nologo /Iinclude /Feapp.exe src\*.cpp
if %errorlevel% equ 0 app.exe