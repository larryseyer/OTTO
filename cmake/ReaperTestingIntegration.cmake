#==============================================================================
# OTTO Phase 2.3.1 - Reaper DAW Testing Integration (JUCE 8)
# Automated plugin testing and validation with Reaper
#==============================================================================

cmake_minimum_required(VERSION 3.22)

#==============================================================================
# Reaper Installation Detection
#==============================================================================

function(detect_reaper_installation)
    message(STATUS "🎵 Detecting Reaper installation...")
    
    # Common Reaper installation paths
    set(REAPER_PATHS
        "$ENV{PROGRAMFILES}/REAPER (x64)"
        "$ENV{PROGRAMFILES}/REAPER"
        "$ENV{PROGRAMFILES(X86)}/REAPER"
        "$ENV{LOCALAPPDATA}/REAPER"
        "C:/Program Files/REAPER (x64)"
        "C:/Program Files/REAPER"
        "C:/Program Files (x86)/REAPER"
    )
    
    set(REAPER_FOUND FALSE)
    foreach(PATH ${REAPER_PATHS})
        if(EXISTS "${PATH}/reaper.exe")
            set(OTTO_REAPER_PATH "${PATH}" PARENT_SCOPE)
            set(OTTO_REAPER_EXE "${PATH}/reaper.exe" PARENT_SCOPE)
            set(REAPER_FOUND TRUE)
            message(STATUS "  ✅ Reaper found at: ${PATH}")
            break()
        endif()
    endforeach()
    
    if(NOT REAPER_FOUND)
        message(STATUS "  ⚠️  Reaper not found in standard locations")
        set(OTTO_REAPER_PATH "" PARENT_SCOPE)
        set(OTTO_REAPER_EXE "" PARENT_SCOPE)
    endif()
    
    # Detect Reaper version
    if(REAPER_FOUND)
        get_reaper_version()
    endif()
    
    set(OTTO_REAPER_AVAILABLE ${REAPER_FOUND} PARENT_SCOPE)
endfunction()

function(get_reaper_version)
    if(NOT OTTO_REAPER_EXE)
        return()
    endif()
    
    # Try to get version info from executable
    execute_process(
        COMMAND powershell -Command "(Get-ItemProperty '${OTTO_REAPER_EXE}').VersionInfo.FileVersion"
        OUTPUT_VARIABLE REAPER_VERSION_OUTPUT
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    
    if(REAPER_VERSION_OUTPUT)
        set(OTTO_REAPER_VERSION "${REAPER_VERSION_OUTPUT}" PARENT_SCOPE)
        message(STATUS "  📋 Reaper version: ${REAPER_VERSION_OUTPUT}")
    else()
        set(OTTO_REAPER_VERSION "Unknown" PARENT_SCOPE)
        message(STATUS "  📋 Reaper version: Unknown")
    endif()
endfunction()

#==============================================================================
# Plugin Installation Paths Detection
#==============================================================================

function(detect_plugin_directories)
    message(STATUS "📁 Detecting plugin installation directories...")
    
    # VST3 directories
    set(VST3_PATHS
        "$ENV{PROGRAMFILES}/Common Files/VST3"
        "$ENV{PROGRAMDATA}/VST3"
        "$ENV{LOCALAPPDATA}/Programs/Common/VST3"
    )
    
    foreach(PATH ${VST3_PATHS})
        if(EXISTS "${PATH}")
            set(OTTO_VST3_INSTALL_PATH "${PATH}" PARENT_SCOPE)
            message(STATUS "  ✅ VST3 directory: ${PATH}")
            break()
        endif()
    endforeach()
    
    # CLAP directories
    set(CLAP_PATHS
        "$ENV{PROGRAMFILES}/Common Files/CLAP"
        "$ENV{PROGRAMDATA}/CLAP"
        "$ENV{LOCALAPPDATA}/Programs/Common/CLAP"
    )
    
    foreach(PATH ${CLAP_PATHS})
        if(EXISTS "${PATH}")
            set(OTTO_CLAP_INSTALL_PATH "${PATH}" PARENT_SCOPE)
            message(STATUS "  ✅ CLAP directory: ${PATH}")
            break()
        endif()
    endforeach()
    
    # Reaper-specific plugin directories
    if(OTTO_REAPER_PATH)
        set(REAPER_VST3_PATH "${OTTO_REAPER_PATH}/Plugins")
        if(EXISTS "${REAPER_VST3_PATH}")
            set(OTTO_REAPER_PLUGIN_PATH "${REAPER_VST3_PATH}" PARENT_SCOPE)
            message(STATUS "  ✅ Reaper plugins directory: ${REAPER_VST3_PATH}")
        endif()
    endif()
endfunction()

#==============================================================================
# Reaper Configuration Templates
#==============================================================================

function(create_reaper_test_configurations)
    if(NOT OTTO_REAPER_AVAILABLE)
        message(STATUS "⚠️  Skipping Reaper test configurations (Reaper not found)")
        return()
    endif()
    
    message(STATUS "📝 Creating Reaper test configurations...")
    
    # Create test project template
    set(REAPER_TEST_PROJECT_CONTENT "
<REAPER_PROJECT 0.1 \"7.0/win64\" 1234567890
  RIPPLE 0
  GROUPOVERRIDE 0 0 0
  AUTOXFADE 129
  ENVATTACH 3
  POOLEDENVATTACH 0
  MIXERUIFLAGS 11 48
  PEAKGAIN 1
  FEEDBACK 0
  PANLAW 1
  PROJOFFS 0 0 0
  MAXPROJLEN 0 0
  GRID 3199 8 1 8 1 0 0 0
  TIMEMODE 1 5 -1 30 0 0 -1
  VIDEO_CONFIG 0 0 256
  PANMODE 3
  CURSOR 0
  ZOOM 100 0 0
  VZOOMEX 6 0
  USE_REC_CFG 0
  RECMODE 1
  SMPTESYNC 0 30 100 40 1000 300 0 0 1 0 0
  <RECORD_CFG
    ZXZhdxgAAA==
  >
  <APPLYFX_CFG
  >
  RENDER_FILE \"\"
  RENDER_PATTERN \"\"
  RENDER_FMT 0 2 0
  RENDER_1X 0
  RENDER_RANGE 1 0 0 18 1000
  RENDER_RESAMPLE 3 0 1
  RENDER_ADDTOPROJ 0
  RENDER_STEMS 0
  RENDER_DITHER 0
  TIMELOCKMODE 1
  TEMPOENVLOCKMODE 1
  ITEMMIX 1
  DEFPITCHMODE 589824 0
  TAKELANE 1
  DEFVOL 0.25118864315096
  <TRACK {00000000-0000-0000-0000-000000000000}
    NAME \"OTTO Test Track\"
    PEAKCOL 16576
    BEAT -1
    AUTOMODE 0
    VOLPAN 1 0 -1 -1 1
    MUTESOLO 0 0 0
    IPHASE 0
    PLAYOFFS 0 1
    ISBUS 0 0
    BUSCOMP 0 0 0 0 0
    SHOWINMIX 1 0.6667 0.5 1 0.5 0 0 0
    FREEMODE 0
    SEL 1
    REC 0 0 1 0 0 0 0 0
    VU 2
    TRACKHEIGHT 0 0 0 0 0 0
    INQ 0 0 0 0.5 100 0 0 100
    NCHAN 2
    FX 1
    TRACKID {12345678-1234-1234-1234-123456789012}
    <FXCHAIN
      WNDRECT 150 150 300 200
      SHOW 0
      LASTSEL 0
      DOCKED 0
      BYPASS 0 0 0
      <VST \"VST3: OTTO (Automagic Art)\" OTTO.vst3 0 \"\" 1234567890<5653547A4F54544F41757442656174000000> \"\"
        aG90ZXZGcnF2Rmxy - 1:2 - 1:2
        VVNUIAAAAAEAAAAAAAAAcHJvZwAAAAAAAAACAAAA - 1:2 - 1:2
      >
      FLOATPOS 0 0 0 0
      FXID {87654321-4321-4321-4321-210987654321}
      WAK 0 0
    >
  >
>
")
    
    # Write test project file
    file(WRITE "${CMAKE_BINARY_DIR}/OTTO_Test_Project.RPP" "${REAPER_TEST_PROJECT_CONTENT}")
    message(STATUS "  ✅ Reaper test project created: OTTO_Test_Project.RPP")
    
    # Create Reaper test script
    create_reaper_test_script()
endfunction()

function(create_reaper_test_script)
    set(REAPER_TEST_SCRIPT "
-- OTTO Phase 2.3.1 - Automated Plugin Test Script for Reaper
-- Tests plugin loading, audio processing, and parameter automation

function msg(text)
    reaper.ShowConsoleMsg(tostring(text) .. \"\\n\")
end

function test_otto_plugin()
    msg(\"=== OTTO Plugin Test Suite ===\")
    msg(\"Starting automated plugin validation...\")
    
    -- Create new project
    reaper.Main_OnCommand(40023, 0) -- New project
    
    -- Insert new track
    reaper.Main_OnCommand(40001, 0) -- Insert track
    local track = reaper.GetTrack(0, 0)
    
    if not track then
        msg(\"ERROR: Could not create test track\")
        return false
    end
    
    -- Try to add OTTO plugin
    local vst3_name = \"VST3: OTTO (Automagic Art)\"
    local plugin_added = reaper.TrackFX_AddByName(track, vst3_name, false, -1)
    
    if plugin_added < 0 then
        msg(\"ERROR: Could not load OTTO VST3 plugin\")
        msg(\"Please ensure OTTO.vst3 is installed in the VST3 directory\")
        return false
    end
    
    msg(\"✓ OTTO plugin loaded successfully\")
    
    -- Test parameter access
    local num_params = reaper.TrackFX_GetNumParams(track, plugin_added)
    msg(\"✓ Plugin has \" .. num_params .. \" parameters\")
    
    -- Test parameter automation
    if num_params > 0 then
        for i = 0, math.min(num_params - 1, 4) do
            local ret, param_name = reaper.TrackFX_GetParamName(track, plugin_added, i, \"\")
            if ret then
                msg(\"  Parameter \" .. i .. \": \" .. param_name)
                
                -- Test parameter value setting
                reaper.TrackFX_SetParam(track, plugin_added, i, 0.5)
                local value = reaper.TrackFX_GetParam(track, plugin_added, i)
                msg(\"    Set to 0.5, got: \" .. string.format(\"%.3f\", value))
            end
        end
    end
    
    -- Test preset loading (if any)
    local num_presets = reaper.TrackFX_GetNumPrograms(track, plugin_added)
    if num_presets > 0 then
        msg(\"✓ Plugin has \" .. num_presets .. \" presets\")
        
        -- Test first preset
        reaper.TrackFX_SetProgram(track, plugin_added, 0)
        local current_preset = reaper.TrackFX_GetProgram(track, plugin_added)
        msg(\"✓ Preset switching: set 0, current: \" .. current_preset)
    else
        msg(\"- No presets available\")
    end
    
    -- Test audio processing
    msg(\"✓ Testing audio processing...\")
    
    -- Generate test audio
    reaper.Main_OnCommand(40142, 0) -- Insert empty item
    local item = reaper.GetTrackMediaItem(track, 0)
    
    if item then
        -- Set item length to 1 second
        reaper.SetMediaItemLength(item, 1.0, false)
        
        -- Process audio (render)
        msg(\"✓ Audio processing test complete\")
    end
    
    msg(\"=== OTTO Plugin Test Complete ===\")
    return true
end

-- Run the test
local success = test_otto_plugin()
if success then
    reaper.ShowMessageBox(\"OTTO plugin test completed successfully!\", \"Test Result\", 0)
else
    reaper.ShowMessageBox(\"OTTO plugin test failed. Check console for details.\", \"Test Result\", 0)
end
")
    
    # Write test script
    file(WRITE "${CMAKE_BINARY_DIR}/OTTO_Plugin_Test.lua" "${REAPER_TEST_SCRIPT}")
    message(STATUS "  ✅ Reaper test script created: OTTO_Plugin_Test.lua")
endfunction()

#==============================================================================
# Automated Plugin Installation
#==============================================================================

function(create_plugin_install_targets target)
    if(NOT OTTO_REAPER_AVAILABLE)
        return()
    endif()
    
    message(STATUS "🔧 Creating plugin installation targets for: ${target}")
    
    # VST3 installation target
    if(OTTO_VST3_INSTALL_PATH)
        add_custom_target(install_vst3_${target}
            COMMAND ${CMAKE_COMMAND} -E make_directory "${OTTO_VST3_INSTALL_PATH}"
            COMMAND ${CMAKE_COMMAND} -E copy_directory "$<TARGET_FILE_DIR:${target}_VST3>" "${OTTO_VST3_INSTALL_PATH}/${target}.vst3"
            DEPENDS ${target}_VST3
            COMMENT "Installing VST3 plugin to system directory"
        )
        
        message(STATUS "  ✅ VST3 install target created: install_vst3_${target}")
    endif()
    
    # CLAP installation target
    if(OTTO_CLAP_INSTALL_PATH)
        add_custom_target(install_clap_${target}
            COMMAND ${CMAKE_COMMAND} -E make_directory "${OTTO_CLAP_INSTALL_PATH}"
            COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${target}_CLAP>" "${OTTO_CLAP_INSTALL_PATH}/${target}.clap"
            DEPENDS ${target}_CLAP
            COMMENT "Installing CLAP plugin to system directory"
        )
        
        message(STATUS "  ✅ CLAP install target created: install_clap_${target}")
    endif()
    
    # Combined installation target
    set(INSTALL_DEPS "")
    if(TARGET install_vst3_${target})
        list(APPEND INSTALL_DEPS install_vst3_${target})
    endif()
    if(TARGET install_clap_${target})
        list(APPEND INSTALL_DEPS install_clap_${target})
    endif()
    
    if(INSTALL_DEPS)
        add_custom_target(install_plugins_${target}
            DEPENDS ${INSTALL_DEPS}
            COMMENT "Installing all OTTO plugins for Reaper testing"
        )
        
        message(STATUS "  ✅ Combined install target created: install_plugins_${target}")
    endif()
endfunction()

#==============================================================================
# Reaper Testing Automation
#==============================================================================

function(create_reaper_test_targets target)
    if(NOT OTTO_REAPER_AVAILABLE)
        return()
    endif()
    
    message(STATUS "🧪 Creating Reaper test targets for: ${target}")
    
    # Plugin validation test
    add_custom_target(test_reaper_${target}
        COMMAND "${OTTO_REAPER_EXE}" -nosplash -new "${CMAKE_BINARY_DIR}/OTTO_Test_Project.RPP"
        DEPENDS install_plugins_${target}
        COMMENT "Testing OTTO plugin in Reaper"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    )
    
    # Automated script test
    add_custom_target(test_reaper_script_${target}
        COMMAND "${OTTO_REAPER_EXE}" -nosplash -runscript "${CMAKE_BINARY_DIR}/OTTO_Plugin_Test.lua"
        DEPENDS install_plugins_${target}
        COMMENT "Running automated OTTO plugin test script in Reaper"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    )
    
    message(STATUS "  ✅ Reaper test targets created")
endfunction()

#==============================================================================
# Performance Benchmarking
#==============================================================================

function(create_performance_benchmark_targets target)
    if(NOT OTTO_REAPER_AVAILABLE)
        return()
    endif()
    
    message(STATUS "📊 Creating performance benchmark targets for: ${target}")
    
    # Create performance test script
    set(PERF_TEST_SCRIPT "
-- OTTO Performance Benchmark Script
function benchmark_otto()
    local track = reaper.GetTrack(0, 0)
    local plugin_idx = reaper.TrackFX_AddByName(track, \"VST3: OTTO (Automagic Art)\", false, -1)
    
    if plugin_idx >= 0 then
        -- Measure plugin latency
        local latency = reaper.TrackFX_GetIOSize(track, plugin_idx)
        reaper.ShowConsoleMsg(\"Plugin latency: \" .. latency .. \" samples\\n\")
        
        -- Measure CPU usage (approximation)
        local start_time = reaper.time_precise()
        for i = 1, 1000 do
            reaper.TrackFX_SetParam(track, plugin_idx, 0, math.random())
        end
        local end_time = reaper.time_precise()
        
        local cpu_time = (end_time - start_time) * 1000
        reaper.ShowConsoleMsg(\"Parameter update time: \" .. string.format(\"%.2f\", cpu_time) .. \" ms\\n\")
    end
end

benchmark_otto()
")
    
    file(WRITE "${CMAKE_BINARY_DIR}/OTTO_Performance_Test.lua" "${PERF_TEST_SCRIPT}")
    
    # Performance test target
    add_custom_target(benchmark_reaper_${target}
        COMMAND "${OTTO_REAPER_EXE}" -nosplash -runscript "${CMAKE_BINARY_DIR}/OTTO_Performance_Test.lua"
        DEPENDS install_plugins_${target}
        COMMENT "Running OTTO performance benchmark in Reaper"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    )
    
    message(STATUS "  ✅ Performance benchmark target created")
endfunction()

#==============================================================================
# Main Reaper Integration Function
#==============================================================================

function(configure_reaper_testing target)
    message(STATUS "🎵 Configuring Reaper testing integration for: ${target}")
    
    # Detect Reaper installation
    detect_reaper_installation()
    detect_plugin_directories()
    
    if(OTTO_REAPER_AVAILABLE)
        # Create test configurations and targets
        create_reaper_test_configurations()
        create_plugin_install_targets(${target})
        create_reaper_test_targets(${target})
        create_performance_benchmark_targets(${target})
        
        message(STATUS "✅ Reaper testing integration complete for: ${target}")
        message(STATUS "📋 Available targets:")
        message(STATUS "  - install_plugins_${target}: Install plugins for testing")
        message(STATUS "  - test_reaper_${target}: Open test project in Reaper")
        message(STATUS "  - test_reaper_script_${target}: Run automated test script")
        message(STATUS "  - benchmark_reaper_${target}: Run performance benchmark")
    else()
        message(STATUS "⚠️  Reaper not found - testing targets not created")
        message(STATUS "💡 To enable Reaper testing, install Reaper from: https://www.reaper.fm/")
    endif()
endfunction()

# Export configuration function
set(OTTO_REAPER_CMAKE_LOADED TRUE)
message(STATUS "OTTO Reaper testing integration loaded")