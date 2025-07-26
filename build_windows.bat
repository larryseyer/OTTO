@echo off
REM Windows Build Script for OTTO
echo Building OTTO for Windows...

REM Create build directory in proper JUCE 8 structure
set BUILD_DIR=Builds\VisualStudio2022\CMake
mkdir "%BUILD_DIR%" 2>nul
cd "%BUILD_DIR%"

REM Configure with CMake using JUCE 8 conventions (Visual Studio 2022)
cmake ..\..\.. ^
    -DCMAKE_BUILD_TYPE=Release ^
    -G "Visual Studio 17 2022" ^
    -A x64

REM Build
cmake --build . --config Release --parallel

REM Check build result
if %ERRORLEVEL% EQU 0 (
    echo ✅ Windows build complete!
    echo.
    echo Build artifacts location:
    dir ..\Release /s /b | findstr /E ".dll .exe .vst3" | head
    echo.
    echo 📁 All Windows builds are now in: Builds\VisualStudio2022\
) else (
    echo ❌ Windows build failed!
    exit /b 1
)