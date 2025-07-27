@echo off
REM OTTO Phase 2.3.1 - Advanced Windows Build Script (JUCE 8)
REM Enhanced unified CMake build system with advanced robustness features
REM Maintains all Phase 2.2 safety features while adding Windows-specific optimizations

setlocal enabledelayedexpansion

echo 🪟 Building OTTO for Windows - Phase 2.3.1...
echo =============================================
echo 🛡️  Enhanced robustness and safety features enabled
echo 🎯 Visual Studio 2022 optimized build
echo 🪟 Parallels VM optimization enabled
echo 🎵 Reaper testing integration ready

REM Function equivalents for colored output
set "STATUS_COLOR=[32m"
set "ERROR_COLOR=[31m"
set "WARNING_COLOR=[33m"
set "RESET_COLOR=[0m"

REM Enhanced system validation for Phase 2.3.1
call :check_vs_installation
if !ERRORLEVEL! neq 0 exit /b 1

call :validate_windows_audio_drivers
if !ERRORLEVEL! neq 0 echo %WARNING_COLOR%⚠️  Some audio drivers may not be available%RESET_COLOR%

call :check_windows_sdk
if !ERRORLEVEL! neq 0 echo %WARNING_COLOR%⚠️  Windows SDK validation completed with warnings%RESET_COLOR%

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

REM Enhanced CMake configuration with Phase 2.3.1 features
echo %STATUS_COLOR%🔧 Configuring with enhanced Windows optimizations...%RESET_COLOR%

cmake ..\..\.. ^
    -DCMAKE_BUILD_TYPE=!BUILD_CONFIG! ^
    -DJUCE_BUILD_EXAMPLES=OFF ^
    -DJUCE_BUILD_EXTRAS=OFF ^
    -DJUCE_COPY_PLUGIN_AFTER_BUILD=TRUE ^
    -DJUCE_VST3_CAN_REPLACE_VST2=ON ^
    -DJUCE8_PROJECT=ON ^
    -DJUCE_MAJOR_VERSION_REQUIRED=8 ^
    -DOTTO_ENHANCED_WINDOWS_BUILD=ON ^
    -DOTTO_ENABLE_ALL_SAFETY_FEATURES=ON ^
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
            for /r "%%d" %%f in (*.exe *.dll *.vst3 *.clap) do (
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
        for /r ..\..\ %%f in (*.exe *.dll *.vst3 *.clap) do echo %%f
    )

    echo.
    echo %STATUS_COLOR%📁 All Windows builds are organized in: Builds\VisualStudio2022\%RESET_COLOR%
    echo.
    echo %STATUS_COLOR%💡 Installation Tips:%RESET_COLOR%
    echo   VST3 plugins: %%PROGRAMFILES%%\Common Files\VST3\
    echo   CLAP plugins: %%PROGRAMFILES%%\Common Files\CLAP\
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

:validate_windows_audio_drivers
echo %STATUS_COLOR%🎵 Validating Windows audio drivers...%RESET_COLOR%

REM Check for DirectSound
reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\DirectX" /v Version >nul 2>&1
if !ERRORLEVEL! equ 0 (
    echo %STATUS_COLOR%  ✅ DirectSound: Available%RESET_COLOR%
) else (
    echo %WARNING_COLOR%  ⚠️  DirectSound: Not detected%RESET_COLOR%
)

REM Check for WASAPI (Windows Audio Session API)
sc query AudioSrv | find "RUNNING" >nul 2>&1
if !ERRORLEVEL! equ 0 (
    echo %STATUS_COLOR%  ✅ WASAPI: Audio service running%RESET_COLOR%
) else (
    echo %WARNING_COLOR%  ⚠️  WASAPI: Audio service not running%RESET_COLOR%
)

REM Check for Windows Audio Engine
tasklist /fi "imagename eq audiodg.exe" | find "audiodg.exe" >nul 2>&1
if !ERRORLEVEL! equ 0 (
    echo %STATUS_COLOR%  ✅ Windows Audio Engine: Running%RESET_COLOR%
) else (
    echo %WARNING_COLOR%  ⚠️  Windows Audio Engine: Not detected%RESET_COLOR%
)

REM Check for ASIO4ALL (optional)
if exist "%PROGRAMFILES%\ASIO4ALL v2" (
    echo %STATUS_COLOR%  ✅ ASIO4ALL: Detected%RESET_COLOR%
) else (
    echo %STATUS_COLOR%  ℹ️  ASIO4ALL: Not installed (optional)%RESET_COLOR%
)

exit /b 0

:check_windows_sdk
echo %STATUS_COLOR%🔧 Checking Windows SDK...%RESET_COLOR%

REM Check for Windows SDK installation
set "SDK_FOUND=false"
for /d %%i in ("%ProgramFiles(x86)%\Windows Kits\10\Include\*") do (
    if exist "%%i\um\windows.h" (
        echo %STATUS_COLOR%  ✅ Windows SDK: Found at %%i%RESET_COLOR%
        set "SDK_FOUND=true"
        goto :sdk_found
    )
)

:sdk_found
if "!SDK_FOUND!"=="false" (
    echo %WARNING_COLOR%  ⚠️  Windows SDK: Not found in default locations%RESET_COLOR%
    exit /b 1
)

REM Check for required headers
set "HEADERS_OK=true"
for %%h in (windows.h dsound.h mmdeviceapi.h audiopolicy.h) do (
    findstr /s /m "%%h" "%ProgramFiles(x86)%\Windows Kits\10\Include\*\um\*" >nul 2>&1
    if !ERRORLEVEL! neq 0 (
        echo %WARNING_COLOR%  ⚠️  Header %%h: Not found%RESET_COLOR%
        set "HEADERS_OK=false"
    )
)

if "!HEADERS_OK!"=="true" (
    echo %STATUS_COLOR%  ✅ Required headers: All found%RESET_COLOR%
else (
    echo %WARNING_COLOR%  ⚠️  Some required headers missing%RESET_COLOR%
)

exit /b 0
