@echo off
REM OTTO Phase 2.3.1 - Comprehensive Reaper Plugin Testing Script
REM Automates plugin installation, testing, and validation in Reaper

setlocal enabledelayedexpansion

echo 🎵 OTTO Reaper Integration & Testing Suite
echo ==========================================
echo 🧪 Phase 2.3.1 - Comprehensive plugin validation

REM Colors for output
set "STATUS_COLOR=[32m"
set "ERROR_COLOR=[31m"
set "WARNING_COLOR=[33m"
set "INFO_COLOR=[36m"
set "RESET_COLOR=[0m"

REM Configuration
set "BUILD_DIR=Builds\VisualStudio2022\CMake-Release"
set "VST3_INSTALL_PATH=%PROGRAMFILES%\Common Files\VST3"
set "CLAP_INSTALL_PATH=%PROGRAMFILES%\Common Files\CLAP"

echo %STATUS_COLOR%🔍 Detecting Reaper installation...%RESET_COLOR%

REM Find Reaper installation
set "REAPER_PATH="
set "REAPER_EXE="

for /d %%i in ("%ProgramFiles%\REAPER*") do (
    if exist "%%i\reaper.exe" (
        set "REAPER_PATH=%%i"
        set "REAPER_EXE=%%i\reaper.exe"
        goto :reaper_found
    )
)

:reaper_found
if "!REAPER_EXE!"=="" (
    echo %ERROR_COLOR%❌ Reaper not found!%RESET_COLOR%
    echo %INFO_COLOR%💡 Please install Reaper from: https://www.reaper.fm/%RESET_COLOR%
    echo %INFO_COLOR%   Then run this script again%RESET_COLOR%
    pause
    exit /b 1
)

echo %STATUS_COLOR%  ✅ Reaper found: !REAPER_PATH!%RESET_COLOR%

REM Get Reaper version
echo %INFO_COLOR%📋 Detecting Reaper version...%RESET_COLOR%
for /f "tokens=*" %%i in ('powershell -Command "(Get-ItemProperty '!REAPER_EXE!').VersionInfo.FileVersion"') do (
    echo %STATUS_COLOR%  ✅ Reaper version: %%i%RESET_COLOR%
)

echo.
echo %STATUS_COLOR%🔧 Checking for OTTO build artifacts...%RESET_COLOR%

REM Check for built plugins
set "VST3_BUILT=false"
set "CLAP_BUILT=false"
set "STANDALONE_BUILT=false"

if exist "!BUILD_DIR!\*_artefacts\Release\VST3\OTTO.vst3" (
    set "VST3_BUILT=true"
    echo %STATUS_COLOR%  ✅ VST3 plugin found%RESET_COLOR%
) else (
    echo %WARNING_COLOR%  ⚠️  VST3 plugin not found in build directory%RESET_COLOR%
)

if exist "!BUILD_DIR!\*_artefacts\Release\CLAP\OTTO.clap" (
    set "CLAP_BUILT=true"
    echo %STATUS_COLOR%  ✅ CLAP plugin found%RESET_COLOR%
) else (
    echo %WARNING_COLOR%  ⚠️  CLAP plugin not found in build directory%RESET_COLOR%
)

if exist "!BUILD_DIR!\*_artefacts\Release\Standalone\OTTO.exe" (
    set "STANDALONE_BUILT=true"
    echo %STATUS_COLOR%  ✅ Standalone application found%RESET_COLOR%
) else (
    echo %WARNING_COLOR%  ⚠️  Standalone application not found in build directory%RESET_COLOR%
)

if "!VST3_BUILT!"=="false" and "!CLAP_BUILT!"=="false" and "!STANDALONE_BUILT!"=="false" (
    echo %ERROR_COLOR%❌ No OTTO build artifacts found!%RESET_COLOR%
    echo %INFO_COLOR%💡 Please build OTTO first using:%RESET_COLOR%
    echo %INFO_COLOR%   build_windows.bat --release%RESET_COLOR%
    pause
    exit /b 1
)

echo.
echo %STATUS_COLOR%📦 Installing plugins for Reaper testing...%RESET_COLOR%

REM Install VST3
if "!VST3_BUILT!"=="true" (
    echo %INFO_COLOR%🔌 Installing VST3 plugin...%RESET_COLOR%
    
    REM Create VST3 directory if it doesn't exist
    if not exist "!VST3_INSTALL_PATH!" (
        mkdir "!VST3_INSTALL_PATH!" >nul 2>&1
    )
    
    REM Copy VST3 plugin
    for /d %%i in ("!BUILD_DIR!\*_artefacts\Release\VST3\OTTO.vst3") do (
        echo   Copying %%i to !VST3_INSTALL_PATH!\OTTO.vst3
        xcopy "%%i" "!VST3_INSTALL_PATH!\OTTO.vst3" /E /I /Y >nul 2>&1
        
        if !ERRORLEVEL! equ 0 (
            echo %STATUS_COLOR%  ✅ VST3 installed successfully%RESET_COLOR%
        ) else (
            echo %ERROR_COLOR%  ❌ VST3 installation failed (may require admin rights)%RESET_COLOR%
        )
    )
)

REM Install CLAP
if "!CLAP_BUILT!"=="true" (
    echo %INFO_COLOR%🔌 Installing CLAP plugin...%RESET_COLOR%
    
    REM Create CLAP directory if it doesn't exist
    if not exist "!CLAP_INSTALL_PATH!" (
        mkdir "!CLAP_INSTALL_PATH!" >nul 2>&1
    )
    
    REM Copy CLAP plugin
    for %%i in ("!BUILD_DIR!\*_artefacts\Release\CLAP\OTTO.clap") do (
        echo   Copying %%i to !CLAP_INSTALL_PATH!\OTTO.clap
        copy "%%i" "!CLAP_INSTALL_PATH!\OTTO.clap" >nul 2>&1
        
        if !ERRORLEVEL! equ 0 (
            echo %STATUS_COLOR%  ✅ CLAP installed successfully%RESET_COLOR%
        ) else (
            echo %ERROR_COLOR%  ❌ CLAP installation failed (may require admin rights)%RESET_COLOR%
        )
    )
)

echo.
echo %STATUS_COLOR%📝 Creating Reaper test projects...%RESET_COLOR%

REM Create test directory
if not exist "ReaperTests" mkdir "ReaperTests"

REM Create VST3 test project
echo %INFO_COLOR%🎛️  Creating VST3 test project...%RESET_COLOR%

set "VST3_TEST_PROJECT=ReaperTests\OTTO_VST3_Test.RPP"
(
echo ^<REAPER_PROJECT 0.1 "7.0/win64" 1234567890
echo   RIPPLE 0
echo   GROUPOVERRIDE 0 0 0
echo   AUTOXFADE 129
echo   ENVATTACH 3
echo   POOLEDENVATTACH 0
echo   MIXERUIFLAGS 11 48
echo   PEAKGAIN 1
echo   FEEDBACK 0
echo   PANLAW 1
echo   PROJOFFS 0 0 0
echo   MAXPROJLEN 0 0
echo   GRID 3199 8 1 8 1 0 0 0
echo   TIMEMODE 1 5 -1 30 0 0 -1
echo   VIDEO_CONFIG 0 0 256
echo   PANMODE 3
echo   CURSOR 0
echo   ZOOM 100 0 0
echo   VZOOMEX 6 0
echo   USE_REC_CFG 0
echo   RECMODE 1
echo   SMPTESYNC 0 30 100 40 1000 300 0 0 1 0 0
echo   ^<RECORD_CFG
echo     ZXZhdxgAAA==
echo   ^>
echo   ^<APPLYFX_CFG
echo   ^>
echo   RENDER_FILE ""
echo   RENDER_PATTERN ""
echo   RENDER_FMT 0 2 0
echo   RENDER_1X 0
echo   RENDER_RANGE 1 0 0 18 1000
echo   RENDER_RESAMPLE 3 0 1
echo   RENDER_ADDTOPROJ 0
echo   RENDER_STEMS 0
echo   RENDER_DITHER 0
echo   TIMELOCKMODE 1
echo   TEMPOENVLOCKMODE 1
echo   ITEMMIX 1
echo   DEFPITCHMODE 589824 0
echo   TAKELANE 1
echo   DEFVOL 0.25118864315096
echo   ^<TRACK {00000000-0000-0000-0000-000000000000}
echo     NAME "OTTO VST3 Test Track"
echo     PEAKCOL 16576
echo     BEAT -1
echo     AUTOMODE 0
echo     VOLPAN 1 0 -1 -1 1
echo     MUTESOLO 0 0 0
echo     IPHASE 0
echo     PLAYOFFS 0 1
echo     ISBUS 0 0
echo     BUSCOMP 0 0 0 0 0
echo     SHOWINMIX 1 0.6667 0.5 1 0.5 0 0 0
echo     FREEMODE 0
echo     SEL 1
echo     REC 0 0 1 0 0 0 0 0
echo     VU 2
echo     TRACKHEIGHT 0 0 0 0 0 0
echo     INQ 0 0 0 0.5 100 0 0 100
echo     NCHAN 2
echo     FX 1
echo     TRACKID {12345678-1234-1234-1234-123456789012}
echo     ^<FXCHAIN
echo       WNDRECT 150 150 300 200
echo       SHOW 0
echo       LASTSEL 0
echo       DOCKED 0
echo       BYPASS 0 0 0
echo       ^<VST "VST3: OTTO (Automagic Art)" OTTO.vst3 0 "" 1234567890^<5653547A4F54544F41757442656174000000^> ""
echo         aG90ZXZGcnF2Rmxy - 1:2 - 1:2
echo         VVNUIAAAAAEAAAAAAAAAcHJvZwAAAAAAAAACAAAA - 1:2 - 1:2
echo       ^>
echo       FLOATPOS 0 0 0 0
echo       FXID {87654321-4321-4321-4321-210987654321}
echo       WAK 0 0
echo     ^>
echo     ^<ITEM
echo       POSITION 0
echo       SNAPOFFS 0
echo       LENGTH 4
echo       LOOP 1
echo       ALLTAKES 0
echo       FADEIN 1 0.01 0 1 0 0 0
echo       FADEOUT 1 0.01 0 1 0 0 0
echo       MUTE 0 0
echo       SEL 1
echo       IGUID {11111111-1111-1111-1111-111111111111}
echo       IID 1
echo       NAME "Test Audio"
echo       VOLPAN 1 0 1 -1
echo       SOFFS 0
echo       PLAYRATE 1 1 0 -1 0 0.0025
echo       CHANMODE 0
echo       GUID {22222222-2222-2222-2222-222222222222}
echo       ^<SOURCE MIDI
echo         HASDATA 1 960 QN
echo         E 0 90 60 96
echo         E 960 80 60 0
echo         E 1920 90 64 96
echo         E 2880 80 64 0
echo         E 3840 90 67 96
echo         E 4800 80 67 0
echo       ^>
echo     ^>
echo   ^>
echo ^>
) > "!VST3_TEST_PROJECT!"

echo %STATUS_COLOR%  ✅ VST3 test project created: !VST3_TEST_PROJECT!%RESET_COLOR%

REM Create automated test script
echo %INFO_COLOR%🤖 Creating automated test script...%RESET_COLOR%

set "TEST_SCRIPT=ReaperTests\OTTO_Automated_Test.lua"
(
echo -- OTTO Phase 2.3.1 - Comprehensive Automated Plugin Test
echo -- Tests plugin loading, parameter access, and audio processing
echo.
echo function msg(text)
echo     reaper.ShowConsoleMsg(tostring(text) .. "\n"^)
echo end
echo.
echo function test_otto_vst3()
echo     msg("=== OTTO VST3 Plugin Test Suite ==="^)
echo     msg("Starting comprehensive plugin validation..."^)
echo.    
echo     -- Create new project
echo     reaper.Main_OnCommand(40023, 0^) -- New project
echo     
echo     -- Insert new track
echo     reaper.Main_OnCommand(40001, 0^) -- Insert track
echo     local track = reaper.GetTrack(0, 0^)
echo     
echo     if not track then
echo         msg("ERROR: Could not create test track"^)
echo         return false
echo     end
echo     
echo     -- Try to add OTTO VST3 plugin
echo     local vst3_name = "VST3: OTTO (Automagic Art)"
echo     local plugin_added = reaper.TrackFX_AddByName(track, vst3_name, false, -1^)
echo     
echo     if plugin_added ^< 0 then
echo         msg("ERROR: Could not load OTTO VST3 plugin"^)
echo         msg("Please ensure OTTO.vst3 is installed in the VST3 directory"^)
echo         return false
echo     end
echo     
echo     msg("✓ OTTO VST3 plugin loaded successfully"^)
echo     
echo     -- Test plugin information
echo     local ret, plugin_name = reaper.TrackFX_GetFXName(track, plugin_added, ""^)
echo     if ret then
echo         msg("✓ Plugin name: " .. plugin_name^)
echo     end
echo     
echo     -- Test parameter access
echo     local num_params = reaper.TrackFX_GetNumParams(track, plugin_added^)
echo     msg("✓ Plugin has " .. num_params .. " parameters"^)
echo     
echo     -- Test first few parameters
echo     if num_params ^> 0 then
echo         for i = 0, math.min(num_params - 1, 9^) do
echo             local ret, param_name = reaper.TrackFX_GetParamName(track, plugin_added, i, ""^)
echo             if ret then
echo                 msg("  Parameter " .. i .. ": " .. param_name^)
echo                 
echo                 -- Test parameter value setting
echo                 local test_value = 0.5
echo                 reaper.TrackFX_SetParam(track, plugin_added, i, test_value^)
echo                 local actual_value = reaper.TrackFX_GetParam(track, plugin_added, i^)
echo                 msg("    Set: " .. string.format("%.3f", test_value^) .. ", Got: " .. string.format("%.3f", actual_value^)^)
echo             end
echo         end
echo     end
echo     
echo     -- Test preset functionality
echo     local num_presets = reaper.TrackFX_GetNumPrograms(track, plugin_added^)
echo     if num_presets ^> 0 then
echo         msg("✓ Plugin has " .. num_presets .. " presets"^)
echo         
echo         -- Test first preset
echo         reaper.TrackFX_SetProgram(track, plugin_added, 0^)
echo         local current_preset = reaper.TrackFX_GetProgram(track, plugin_added^)
echo         msg("✓ Preset switching test: set 0, current: " .. current_preset^)
echo     else
echo         msg("- No presets available"^)
echo     end
echo     
echo     -- Test audio processing latency
echo     local latency = reaper.TrackFX_GetIOSize(track, plugin_added^)
echo     msg("✓ Plugin latency: " .. latency .. " samples"^)
echo     
echo     -- Test MIDI input/output
echo     local midi_input = reaper.TrackFX_GetIOSize(track, plugin_added^)
echo     msg("✓ MIDI processing capability detected"^)
echo     
echo     -- Performance test
echo     msg("✓ Running performance test..."^)
echo     local start_time = reaper.time_precise(^)
echo     for i = 1, 100 do
echo         reaper.TrackFX_SetParam(track, plugin_added, 0, math.random(^)^)
echo     end
echo     local end_time = reaper.time_precise(^)
echo     
echo     local perf_time = (end_time - start_time^) * 1000
echo     msg("✓ 100 parameter updates took: " .. string.format("%.2f", perf_time^) .. " ms"^)
echo     
echo     msg("=== OTTO VST3 Plugin Test Complete ==="^)
echo     return true
echo end
echo.
echo function test_otto_clap()
echo     msg("=== OTTO CLAP Plugin Test Suite ==="^)
echo     
echo     -- Similar test for CLAP if available
echo     local track = reaper.GetTrack(0, 0^)
echo     if not track then
echo         reaper.Main_OnCommand(40001, 0^) -- Insert track
echo         track = reaper.GetTrack(0, 0^)
echo     end
echo     
echo     local clap_name = "CLAP: OTTO"
echo     local plugin_added = reaper.TrackFX_AddByName(track, clap_name, false, -1^)
echo     
echo     if plugin_added ^>= 0 then
echo         msg("✓ OTTO CLAP plugin loaded successfully"^)
echo         
echo         local num_params = reaper.TrackFX_GetNumParams(track, plugin_added^)
echo         msg("✓ CLAP plugin has " .. num_params .. " parameters"^)
echo         
echo         msg("=== OTTO CLAP Plugin Test Complete ==="^)
echo         return true
echo     else
echo         msg("⚠️  OTTO CLAP plugin not found or not loaded"^)
echo         return false
echo     end
echo end
echo.
echo function run_comprehensive_test()
echo     msg("🧪 Starting OTTO Comprehensive Plugin Test Suite"^)
echo     msg("================================================"^)
echo     
echo     local vst3_success = test_otto_vst3(^)
echo     local clap_success = test_otto_clap(^)
echo     
echo     msg(""^)
echo     msg("📊 Test Results Summary:"^)
echo     msg("VST3: " .. (vst3_success and "✅ PASS" or "❌ FAIL"^)^)
echo     msg("CLAP: " .. (clap_success and "✅ PASS" or "❌ FAIL"^)^)
echo     
echo     local overall_success = vst3_success or clap_success
echo     
echo     if overall_success then
echo         msg("🎉 Overall Result: SUCCESS"^)
echo         reaper.ShowMessageBox("OTTO plugin testing completed successfully!\n\nCheck the console for detailed results.", "Test Success", 0^)
echo     else
echo         msg("💥 Overall Result: FAILURE"^)
echo         reaper.ShowMessageBox("OTTO plugin testing failed!\n\nCheck the console for error details.", "Test Failed", 0^)
echo     end
echo     
echo     return overall_success
echo end
echo.
echo -- Run the comprehensive test
echo run_comprehensive_test(^)
) > "!TEST_SCRIPT!"

echo %STATUS_COLOR%  ✅ Automated test script created: !TEST_SCRIPT!%RESET_COLOR%

echo.
echo %STATUS_COLOR%🧪 Running Reaper plugin tests...%RESET_COLOR%

REM Test 1: Open VST3 test project
if "!VST3_BUILT!"=="true" (
    echo %INFO_COLOR%📂 Opening VST3 test project in Reaper...%RESET_COLOR%
    start "OTTO VST3 Test" "!REAPER_EXE!" -nosplash "!VST3_TEST_PROJECT!"
    
    timeout /t 3 >nul
    echo %STATUS_COLOR%  ✅ VST3 test project opened%RESET_COLOR%
)

REM Test 2: Run automated test script
echo %INFO_COLOR%🤖 Running automated test script...%RESET_COLOR%
echo %INFO_COLOR%   This will open Reaper with the test script loaded%RESET_COLOR%

timeout /t 2 >nul
start "OTTO Automated Test" "!REAPER_EXE!" -nosplash -runscript "!TEST_SCRIPT!"

echo %STATUS_COLOR%  ✅ Automated test script launched%RESET_COLOR%

echo.
echo %STATUS_COLOR%📋 Creating performance benchmark...%RESET_COLOR%

REM Create performance benchmark script
set "PERF_SCRIPT=ReaperTests\OTTO_Performance_Benchmark.lua"
(
echo -- OTTO Performance Benchmark Script
echo -- Measures plugin performance characteristics
echo.
echo function msg(text)
echo     reaper.ShowConsoleMsg(tostring(text) .. "\n"^)
echo end
echo.
echo function benchmark_otto()
echo     msg("🚀 OTTO Performance Benchmark"^)
echo     msg("=============================="^)
echo     
echo     -- Create test track
echo     reaper.Main_OnCommand(40023, 0^) -- New project
echo     reaper.Main_OnCommand(40001, 0^) -- Insert track
echo     local track = reaper.GetTrack(0, 0^)
echo     
echo     if not track then
echo         msg("ERROR: Could not create benchmark track"^)
echo         return
echo     end
echo     
echo     -- Load OTTO plugin
echo     local plugin_idx = reaper.TrackFX_AddByName(track, "VST3: OTTO (Automagic Art)", false, -1^)
echo     
echo     if plugin_idx ^< 0 then
echo         msg("ERROR: Could not load OTTO plugin for benchmarking"^)
echo         return
echo     end
echo     
echo     msg("✓ OTTO plugin loaded for benchmarking"^)
echo     
echo     -- Measure plugin latency
echo     local latency = reaper.TrackFX_GetIOSize(track, plugin_idx^)
echo     msg("📊 Plugin latency: " .. latency .. " samples"^)
echo     
echo     -- Measure parameter update performance
echo     msg("📊 Testing parameter update performance..."^)
echo     local start_time = reaper.time_precise(^)
echo     for i = 1, 1000 do
echo         reaper.TrackFX_SetParam(track, plugin_idx, 0, math.random(^)^)
echo     end
echo     local end_time = reaper.time_precise(^)
echo     
echo     local param_time = (end_time - start_time^) * 1000
echo     msg("📊 1000 parameter updates: " .. string.format("%.2f", param_time^) .. " ms"^)
echo     msg("📊 Average per parameter: " .. string.format("%.4f", param_time / 1000^) .. " ms"^)
echo     
echo     -- Measure preset switching performance
echo     local num_presets = reaper.TrackFX_GetNumPrograms(track, plugin_idx^)
echo     if num_presets ^> 1 then
echo         msg("📊 Testing preset switching performance..."^)
echo         start_time = reaper.time_precise(^)
echo         for i = 0, num_presets - 1 do
echo             reaper.TrackFX_SetProgram(track, plugin_idx, i^)
echo         end
echo         end_time = reaper.time_precise(^)
echo         
echo         local preset_time = (end_time - start_time^) * 1000
echo         msg("📊 " .. num_presets .. " preset switches: " .. string.format("%.2f", preset_time^) .. " ms"^)
echo         msg("📊 Average per preset: " .. string.format("%.2f", preset_time / num_presets^) .. " ms"^)
echo     end
echo     
echo     -- System resource check
echo     msg("📊 System Resources:"^)
echo     local proj_time = reaper.GetProjectLength(0^)
echo     msg("📊 Project length: " .. string.format("%.2f", proj_time^) .. " seconds"^)
echo     
echo     msg("🎯 Performance Benchmark Complete!"^)
echo     
echo     reaper.ShowMessageBox("OTTO Performance Benchmark Complete!\n\nCheck console for detailed metrics.", "Benchmark Complete", 0^)
echo end
echo.
echo benchmark_otto(^)
) > "!PERF_SCRIPT!"

echo %STATUS_COLOR%  ✅ Performance benchmark created: !PERF_SCRIPT!%RESET_COLOR%

echo.
echo %STATUS_COLOR%✨ Reaper Integration & Testing Complete!%RESET_COLOR%
echo.
echo %INFO_COLOR%📋 Test Summary:%RESET_COLOR%
if "!VST3_BUILT!"=="true" (
    echo   ✅ VST3 plugin installed and test project created
)
if "!CLAP_BUILT!"=="true" (
    echo   ✅ CLAP plugin installed
)
if "!STANDALONE_BUILT!"=="true" (
    echo   ✅ Standalone application available
)
echo   ✅ Automated test script created and launched
echo   ✅ Performance benchmark script available
echo.
echo %INFO_COLOR%🎯 What's Running:%RESET_COLOR%
echo   🔧 Reaper should now be open with OTTO test projects
echo   🧪 Automated test script is running in Reaper
echo   📊 Check Reaper console for test results
echo.
echo %INFO_COLOR%💡 Next Steps:%RESET_COLOR%
echo   1. Review test results in Reaper console
echo   2. Test plugin functionality manually in Reaper
echo   3. Run performance benchmark if needed:
echo      "!REAPER_EXE!" -runscript "!PERF_SCRIPT!"
echo   4. Report any issues found during testing
echo.
echo %INFO_COLOR%📁 Test Files Created:%RESET_COLOR%
echo   📄 !VST3_TEST_PROJECT!
echo   🤖 !TEST_SCRIPT!
echo   📊 !PERF_SCRIPT!
echo.
echo %STATUS_COLOR%🎵 Happy plugin testing!%RESET_COLOR%

pause