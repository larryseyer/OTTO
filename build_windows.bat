@echo off
REM Windows Build Script for OTTO (JUCE 8)
REM Unified CMake build system that matches ProJucer configuration

setlocal enabledelayedexpansion

echo 🪟 Building OTTO for Windows...
echo ===============================

REM Function equivalents for colored output
set "STATUS_COLOR=[32m"
set "ERROR_COLOR=[31m"
set "WARNING_COLOR=[33m"
set "RESET_COLOR=[0m"

REM Check for Visual Studio installation
call :check_vs_installation
if !ERRORLEVEL! neq 0 exit /b 1

REM Parse command line arguments
set "VS_GENERATOR=Visual Studio 17 2022"
set "ARCHITECTURE=x64"
set "BUILD_CONFIG=Release"

:parse_args
if "%1"=="" goto :done_parsing
if "%1"=="--vs2019" (
    set "VS_GENERATOR=Visual Studio 16 2019"
    shift
    goto :parse_args
)
if "%1"=="--vs2022" (
    set "VS_GENERATOR=Visual Studio 17 2022"
    shift
    goto :parse_args
)
if "%1"=="--arch" (
    set "ARCHITECTURE=%2"
    shift
    shift
    goto :parse_args
)
if "%1"=="--debug" (
    set "BUILD_CONFIG=Debug"
    shift
    goto :parse_args
)
echo Unknown option: %1
echo Usage: %0 [--vs2019^|--vs2022] [--arch x64^|Win32] [--debug]
exit /b 1

:done_parsing

echo %STATUS_COLOR%🏗️  Generator: !VS_GENERATOR!%RESET_COLOR%
echo %STATUS_COLOR%🏗️  Architecture: !ARCHITECTURE!%RESET_COLOR%
echo %STATUS_COLOR%🔧 Configuration: !BUILD_CONFIG!%RESET_COLOR%

REM Create build directory in proper unified structure
set "BUILD_DIR=Builds\VisualStudio2022\CMake-!BUILD_CONFIG!"
echo %STATUS_COLOR%📁 Build directory: !BUILD_DIR!%RESET_COLOR%

if exist "!BUILD_DIR!\CMakeCache.txt" del /q "!BUILD_DIR!\CMakeCache.txt"
if exist "!BUILD_DIR!\CMakeFiles" rmdir /s /q "!BUILD_DIR!\CMakeFiles"

mkdir "!BUILD_DIR!" 2>nul
cd "!BUILD_DIR!"

echo %STATUS_COLOR%⚙️  Configuring CMake...%RESET_COLOR%

REM Configure with CMake using unified settings
cmake ..\..\.. ^
    -DCMAKE_BUILD_TYPE=!BUILD_CONFIG! ^
    -G "!VS_GENERATOR!" ^
    -A !ARCHITECTURE!

if !ERRORLEVEL! neq 0 (
    echo %ERROR_COLOR%❌ CMake configuration failed!%RESET_COLOR%
    exit /b 1
)

echo %STATUS_COLOR%🔧 Building OTTO...%RESET_COLOR%

REM Determine number of cores for parallel building
for /f "tokens=2 delims==" %%i in ('wmic cpu get NumberOfLogicalProcessors /value ^| find "="') do set "NUM_CORES=%%i"
if "!NUM_CORES!"=="" set "NUM_CORES=4"

echo %STATUS_COLOR%🚀 Starting build with !NUM_CORES! parallel jobs...%RESET_COLOR%

REM Build with proper parallel processing
cmake --build . --config !BUILD_CONFIG! --parallel !NUM_CORES!

REM Check build result
if !ERRORLEVEL! equ 0 (
    echo %STATUS_COLOR%✅ Windows build complete!%RESET_COLOR%
    echo.
    
    echo 📦 Build artifacts:
    echo ===================
    
    REM Look for build artifacts in expected locations
    set "FOUND_ARTIFACTS=false"
    for %%d in (..\!BUILD_CONFIG! !BUILD_CONFIG! ..\..\!BUILD_CONFIG!) do (
        if exist "%%d" (
            echo 🔍 Checking %%d...
            for /r "%%d" %%f in (*.exe *.dll *.vst3) do (
                echo %%f
                set "FOUND_ARTIFACTS=true"
                goto :found_artifacts
            )
        )
    )
    
    :found_artifacts
    if "!FOUND_ARTIFACTS!"=="false" (
        echo %WARNING_COLOR%⚠️  No plugin artifacts found in expected locations%RESET_COLOR%
        echo 🔍 Searching for build outputs...
        for /r ..\..\ %%f in (*.exe *.dll *.vst3) do echo %%f
    )
    
    echo.
    echo %STATUS_COLOR%📁 All Windows builds are organized in: Builds\VisualStudio2022\%RESET_COLOR%
    echo.
    echo %STATUS_COLOR%💡 Installation Tips:%RESET_COLOR%
    echo   VST3 plugins: %%PROGRAMFILES%%\Common Files\VST3\
    echo   Standalone:   Run directly from build directory
    echo.
    echo %STATUS_COLOR%✨ Windows build completed successfully!%RESET_COLOR%
    
) else (
    echo %ERROR_COLOR%❌ Windows build failed!%RESET_COLOR%
    echo.
    echo 📋 Troubleshooting tips:
    echo   - Ensure Visual Studio 2019/2022 is installed
    echo   - Check that Windows SDK is available
    echo   - Try different architecture: --arch Win32
    exit /b 1
)

goto :eof

:check_vs_installation
echo %STATUS_COLOR%🔍 Checking Visual Studio installation...%RESET_COLOR%

REM Check for Visual Studio 2022
where /q devenv.exe
if !ERRORLEVEL! equ 0 (
    echo %STATUS_COLOR%✅ Visual Studio detected%RESET_COLOR%
    goto :vs_found
)

REM Check for Visual Studio Build Tools
where /q msbuild.exe
if !ERRORLEVEL! equ 0 (
    echo %STATUS_COLOR%✅ Visual Studio Build Tools detected%RESET_COLOR%
    goto :vs_found
)

REM Check for VS installation via vswhere
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if exist "!VSWHERE!" (
    for /f "tokens=*" %%i in ('"!VSWHERE!" -latest -property installationPath') do (
        if not "%%i"=="" (
            echo %STATUS_COLOR%✅ Visual Studio found at: %%i%RESET_COLOR%
            goto :vs_found
        )
    )
)

echo %ERROR_COLOR%❌ Visual Studio not found!%RESET_COLOR%
echo 📥 Please install Visual Studio 2019 or 2022 with C++ development tools
echo    Download from: https://visualstudio.microsoft.com/downloads/
exit /b 1

:vs_found
exit /b 0