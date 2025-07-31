@echo off
REM OTTO Phase 2.3.1 - Complete Windows Development Workflow
REM Comprehensive build, test, and deployment workflow for Windows 11 Parallels

setlocal enabledelayedexpansion

echo 🚀 OTTO Phase 2.3.1 - Complete Windows Workflow
echo ================================================
echo 🎯 Unified build, test, and deployment process

REM Colors for output
set "STATUS_COLOR=[32m"
set "ERROR_COLOR=[31m"
set "WARNING_COLOR=[33m"
set "INFO_COLOR=[36m"
set "RESET_COLOR=[0m"

echo %STATUS_COLOR%🔍 Phase 1: Environment Setup%RESET_COLOR%
echo =====================================

REM Step 1: Run Parallels optimization
echo %INFO_COLOR%⚡ Running Parallels optimization...%RESET_COLOR%
if exist "scripts\optimize_parallels_windows.bat" (
    call "scripts\optimize_parallels_windows.bat"
    if !ERRORLEVEL! neq 0 (
        echo %ERROR_COLOR%❌ Parallels optimization failed%RESET_COLOR%
        pause
        exit /b 1
    )
    echo %STATUS_COLOR%  ✅ Parallels optimization complete%RESET_COLOR%
) else (
    echo %WARNING_COLOR%  ⚠️  Parallels optimization script not found%RESET_COLOR%
)

echo.
echo %STATUS_COLOR%🔧 Phase 2: Build Process%RESET_COLOR%
echo =================================

REM Step 2: Clean previous builds
echo %INFO_COLOR%🧹 Cleaning previous builds...%RESET_COLOR%
if exist "Builds\VisualStudio2022" (
    rmdir /s /q "Builds\VisualStudio2022" >nul 2>&1
    echo %STATUS_COLOR%  ✅ Previous builds cleaned%RESET_COLOR%
) else (
    echo %INFO_COLOR%  ℹ️  No previous builds to clean%RESET_COLOR%
)

REM Step 3: Run Windows build
echo %INFO_COLOR%🏗️  Starting Windows build process...%RESET_COLOR%
if exist "build_windows.bat" (
    call "build_windows.bat" --release
    if !ERRORLEVEL! neq 0 (
        echo %ERROR_COLOR%❌ Windows build failed!%RESET_COLOR%
        echo %INFO_COLOR%💡 Check the build output above for errors%RESET_COLOR%
        pause
        exit /b 1
    )
    echo %STATUS_COLOR%  ✅ Windows build complete%RESET_COLOR%
) else (
    echo %ERROR_COLOR%❌ build_windows.bat not found!%RESET_COLOR%
    pause
    exit /b 1
)

echo.
echo %STATUS_COLOR%🧪 Phase 3: Plugin Testing%RESET_COLOR%
echo ==================================

REM Step 4: Run Reaper integration testing
echo %INFO_COLOR%🎵 Starting Reaper plugin testing...%RESET_COLOR%
if exist "scripts\test_reaper_integration.bat" (
    call "scripts\test_reaper_integration.bat"
    if !ERRORLEVEL! neq 0 (
        echo %ERROR_COLOR%❌ Reaper testing failed!%RESET_COLOR%
        echo %INFO_COLOR%💡 Check Reaper installation and plugin paths%RESET_COLOR%
        pause
        exit /b 1
    )
    echo %STATUS_COLOR%  ✅ Reaper testing initiated%RESET_COLOR%
) else (
    echo %WARNING_COLOR%  ⚠️  Reaper testing script not found%RESET_COLOR%
)

echo.
echo %STATUS_COLOR%📦 Phase 4: Plugin Deployment%RESET_COLOR%
echo ====================================

REM Step 5: Install plugins to system directories
echo %INFO_COLOR%🔌 Installing plugins to system directories...%RESET_COLOR%

set "VST3_INSTALL_PATH=%PROGRAMFILES%\Common Files\VST3"
set "CLAP_INSTALL_PATH=%PROGRAMFILES%\Common Files\CLAP"
set "BUILD_DIR=Builds\VisualStudio2022\CMake-Release"

REM Install VST3
if exist "!BUILD_DIR!\*_artefacts\Release\VST3\OTTO.vst3" (
    echo   Installing VST3 plugin...
    if not exist "!VST3_INSTALL_PATH!" mkdir "!VST3_INSTALL_PATH!" >nul 2>&1
    
    for /d %%i in ("!BUILD_DIR!\*_artefacts\Release\VST3\OTTO.vst3") do (
        xcopy "%%i" "!VST3_INSTALL_PATH!\OTTO.vst3" /E /I /Y >nul 2>&1
        if !ERRORLEVEL! equ 0 (
            echo %STATUS_COLOR%    ✅ VST3 installed successfully%RESET_COLOR%
        ) else (
            echo %WARNING_COLOR%    ⚠️  VST3 installation failed (may require admin rights)%RESET_COLOR%
        )
    )
) else (
    echo %WARNING_COLOR%  ⚠️  VST3 plugin not found in build directory%RESET_COLOR%
)

REM Install CLAP
if exist "!BUILD_DIR!\*_artefacts\Release\CLAP\OTTO.clap" (
    echo   Installing CLAP plugin...
    if not exist "!CLAP_INSTALL_PATH!" mkdir "!CLAP_INSTALL_PATH!" >nul 2>&1
    
    for %%i in ("!BUILD_DIR!\*_artefacts\Release\CLAP\OTTO.clap") do (
        copy "%%i" "!CLAP_INSTALL_PATH!\OTTO.clap" >nul 2>&1
        if !ERRORLEVEL! equ 0 (
            echo %STATUS_COLOR%    ✅ CLAP installed successfully%RESET_COLOR%
        ) else (
            echo %WARNING_COLOR%    ⚠️  CLAP installation failed (may require admin rights)%RESET_COLOR%
        )
    )
) else (
    echo %WARNING_COLOR%  ⚠️  CLAP plugin not found in build directory%RESET_COLOR%
)

echo.
echo %STATUS_COLOR%🔄 Phase 5: Cross-Platform Sync%RESET_COLOR%
echo =====================================

REM Step 6: Sync to macOS via shared folders (if available)
echo %INFO_COLOR%🌐 Checking for Parallels shared folders...%RESET_COLOR%

set "SHARED_FOLDER_FOUND=false"
for %%d in (Z: E: F: D:) do (
    if exist "%%d\" (
        echo %STATUS_COLOR%  ✅ Shared folder detected: %%d%RESET_COLOR%
        set "SHARED_FOLDER=%%d\"
        set "SHARED_FOLDER_FOUND=true"
        goto :sync_plugins
    )
)

:sync_plugins
if "!SHARED_FOLDER_FOUND!"=="true" (
    echo %INFO_COLOR%📤 Syncing plugins to macOS...%RESET_COLOR%
    
    set "MAC_PLUGIN_DIR=!SHARED_FOLDER!OTTO_Plugins"
    
    REM Create Mac plugin directory
    if not exist "!MAC_PLUGIN_DIR!" (
        mkdir "!MAC_PLUGIN_DIR!" >nul 2>&1
        mkdir "!MAC_PLUGIN_DIR!\VST3" >nul 2>&1
        mkdir "!MAC_PLUGIN_DIR!\CLAP" >nul 2>&1
        mkdir "!MAC_PLUGIN_DIR!\Standalone" >nul 2>&1
        echo %STATUS_COLOR%    ✅ Mac plugin directories created%RESET_COLOR%
    )
    
    REM Sync VST3
    if exist "!BUILD_DIR!\*_artefacts\Release\VST3\OTTO.vst3" (
        for /d %%i in ("!BUILD_DIR!\*_artefacts\Release\VST3\OTTO.vst3") do (
            xcopy "%%i" "!MAC_PLUGIN_DIR!\VST3\OTTO.vst3" /E /I /Y >nul 2>&1
            echo %STATUS_COLOR%    ✅ VST3 synced to Mac%RESET_COLOR%
        )
    )
    
    REM Sync CLAP
    if exist "!BUILD_DIR!\*_artefacts\Release\CLAP\OTTO.clap" (
        for %%i in ("!BUILD_DIR!\*_artefacts\Release\CLAP\OTTO.clap") do (
            copy "%%i" "!MAC_PLUGIN_DIR!\CLAP\OTTO.clap" >nul 2>&1
            echo %STATUS_COLOR%    ✅ CLAP synced to Mac%RESET_COLOR%
        )
    )
    
    REM Sync Standalone
    if exist "!BUILD_DIR!\*_artefacts\Release\Standalone\OTTO.exe" (
        for %%i in ("!BUILD_DIR!\*_artefacts\Release\Standalone\OTTO.exe") do (
            copy "%%i" "!MAC_PLUGIN_DIR!\Standalone\OTTO.exe" >nul 2>&1
            echo %STATUS_COLOR%    ✅ Standalone synced to Mac%RESET_COLOR%
        )
    )
    
    REM Create sync info
    echo OTTO Plugin Sync - %date% %time% > "!MAC_PLUGIN_DIR!\sync_info.txt"
    echo Build: Windows 11 Parallels >> "!MAC_PLUGIN_DIR!\sync_info.txt"
    echo Phase: 2.3.1 >> "!MAC_PLUGIN_DIR!\sync_info.txt"
    
    echo %STATUS_COLOR%  ✅ Plugin sync to Mac complete%RESET_COLOR%
) else (
    echo %WARNING_COLOR%  ⚠️  No shared folders detected - manual sync required%RESET_COLOR%
)

echo.
echo %STATUS_COLOR%📊 Phase 6: Validation & Reporting%RESET_COLOR%
echo ==========================================

echo %INFO_COLOR%🔍 Validating build results...%RESET_COLOR%

REM Check build artifacts
set "VST3_OK=false"
set "CLAP_OK=false"
set "STANDALONE_OK=false"

if exist "!BUILD_DIR!\*_artefacts\Release\VST3\OTTO.vst3" (
    set "VST3_OK=true"
    echo %STATUS_COLOR%  ✅ VST3 plugin: Available%RESET_COLOR%
) else (
    echo %ERROR_COLOR%  ❌ VST3 plugin: Missing%RESET_COLOR%
)

if exist "!BUILD_DIR!\*_artefacts\Release\CLAP\OTTO.clap" (
    set "CLAP_OK=true"
    echo %STATUS_COLOR%  ✅ CLAP plugin: Available%RESET_COLOR%
) else (
    echo %ERROR_COLOR%  ❌ CLAP plugin: Missing%RESET_COLOR%
)

if exist "!BUILD_DIR!\*_artefacts\Release\Standalone\OTTO.exe" (
    set "STANDALONE_OK=true"
    echo %STATUS_COLOR%  ✅ Standalone app: Available%RESET_COLOR%
) else (
    echo %ERROR_COLOR%  ❌ Standalone app: Missing%RESET_COLOR%
)

REM Check plugin installations
echo.
echo %INFO_COLOR%🔍 Checking plugin installations...%RESET_COLOR%

if exist "!VST3_INSTALL_PATH!\OTTO.vst3" (
    echo %STATUS_COLOR%  ✅ VST3 system installation: OK%RESET_COLOR%
) else (
    echo %WARNING_COLOR%  ⚠️  VST3 system installation: Not found%RESET_COLOR%
)

if exist "!CLAP_INSTALL_PATH!\OTTO.clap" (
    echo %STATUS_COLOR%  ✅ CLAP system installation: OK%RESET_COLOR%
) else (
    echo %WARNING_COLOR%  ⚠️  CLAP system installation: Not found%RESET_COLOR%
)

echo.
echo %STATUS_COLOR%✨ OTTO Phase 2.3.1 Windows Workflow Complete!%RESET_COLOR%
echo ========================================================

echo.
echo %INFO_COLOR%📋 Workflow Summary:%RESET_COLOR%
echo   ✅ Environment optimization applied
echo   ✅ Windows build process completed
echo   ✅ Plugin testing initiated in Reaper
echo   ✅ System plugin installation attempted
if "!SHARED_FOLDER_FOUND!"=="true" (
    echo   ✅ Cross-platform sync to Mac completed
) else (
    echo   ⚠️  Cross-platform sync skipped (no shared folders)
)
echo   ✅ Build validation completed

echo.
echo %INFO_COLOR%🎯 Results:%RESET_COLOR%
echo   VST3 Plugin: !VST3_OK!
echo   CLAP Plugin: !CLAP_OK!
echo   Standalone: !STANDALONE_OK!

echo.
echo %INFO_COLOR%💡 Next Steps:%RESET_COLOR%
echo   1. Check Reaper for loaded OTTO plugins
echo   2. Test plugin functionality in DAW
echo   3. Run performance benchmarks if needed
echo   4. Report any issues for Phase 2.3.2

if "!VST3_OK!"=="true" or "!CLAP_OK!"=="true" or "!STANDALONE_OK!"=="true" (
    echo.
    echo %STATUS_COLOR%🎉 SUCCESS: OTTO Windows build workflow completed!%RESET_COLOR%
    echo %STATUS_COLOR%🎵 Ready for audio production testing!%RESET_COLOR%
) else (
    echo.
    echo %ERROR_COLOR%💥 FAILURE: No plugins were successfully built%RESET_COLOR%
    echo %INFO_COLOR%🔧 Please check build errors and try again%RESET_COLOR%
)

echo.
pause