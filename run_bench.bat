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
        echo [ERROR] MSVC compiler not found.
        exit /b 1
    )
)

cl.exe /O2 /EHsc /nologo /Iinclude /Febench.exe benchmarks\bench_main.cpp src\AppController.cpp src\State.cpp src\PianoKeyboard.cpp src\ChordMatrix.cpp src\IntervalMatrix.cpp src\ChordRecognizer.cpp src\IntervalRecognizer.cpp
if %errorlevel% equ 0 bench.exe