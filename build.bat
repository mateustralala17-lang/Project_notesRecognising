@echo off
setlocal

where cl.exe >nul 2>&1
if %errorlevel% equ 0 goto :build

set "VCVARS="

for %%D in (C D) do (
    for %%V in (18 2026 2022 2019) do (
        for %%E in (Community Professional Enterprise BuildTools Preview) do (
            if not defined VCVARS (
                if exist "%%D:\Program Files\Microsoft Visual Studio\%%V\%%E\VC\Auxiliary\Build\vcvars64.bat" (
                    set "VCVARS=%%D:\Program Files\Microsoft Visual Studio\%%V\%%E\VC\Auxiliary\Build\vcvars64.bat"
                )
            )
            if not defined VCVARS (
                if exist "%%D:\Program Files (x86)\Microsoft Visual Studio\%%V\%%E\VC\Auxiliary\Build\vcvars64.bat" (
                    set "VCVARS=%%D:\Program Files (x86)\Microsoft Visual Studio\%%V\%%E\VC\Auxiliary\Build\vcvars64.bat"
                )
            )
        )
    )
)

if not defined VCVARS (
    set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
    if exist "%VSWHERE%" (
        for /f "tokens=*" %%i in ('""%VSWHERE%" -latest -products * -property installationPath"') do (
            if exist "%%i\VC\Auxiliary\Build\vcvars64.bat" set "VCVARS=%%i\VC\Auxiliary\Build\vcvars64.bat"
        )
    )
)

if defined VCVARS (
    call "%VCVARS%" >nul
) else (
    echo [ERROR] MSVC compiler not found. Install "Desktop development with C++" in VS Installer.
    exit /b 1
)

:build
cl.exe /Zi /EHsc /nologo /Iinclude /Feapp.exe src\*.cpp
if %errorlevel% equ 0 app.exe