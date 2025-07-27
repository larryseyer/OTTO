#==============================================================================
# OTTO Phase 2.3.1 - Windows Parallels VM Optimization (JUCE 8)
# Advanced configuration for optimal audio development in Parallels
#==============================================================================

cmake_minimum_required(VERSION 3.22)

# Ensure we're in a Parallels environment (detect VM characteristics)
if(NOT WIN32)
    message(FATAL_ERROR "WindowsParallelsOptimization.cmake can only be used on Windows platforms")
endif()

#==============================================================================
# Parallels VM Detection & Configuration
#==============================================================================

function(detect_parallels_environment)
    message(STATUS "🔍 Detecting Parallels environment...")
    
    # Check for Parallels Tools installation
    set(PARALLELS_DETECTED FALSE)
    
    # Check for Parallels service
    execute_process(
        COMMAND sc query "Parallels Tools Service"
        OUTPUT_VARIABLE SC_OUTPUT
        ERROR_QUIET
        RESULT_VARIABLE SC_RESULT
    )
    
    if(SC_RESULT EQUAL 0)
        set(PARALLELS_DETECTED TRUE)
        message(STATUS "  ✅ Parallels Tools Service detected")
    endif()
    
    # Check for Parallels graphics adapter
    execute_process(
        COMMAND wmic path win32_VideoController get name
        OUTPUT_VARIABLE GPU_OUTPUT
        ERROR_QUIET
    )
    
    if(GPU_OUTPUT MATCHES "Parallels")
        set(PARALLELS_DETECTED TRUE)
        message(STATUS "  ✅ Parallels graphics adapter detected")
    endif()
    
    # Check for Parallels network adapter
    execute_process(
        COMMAND wmic path win32_NetworkAdapter get name
        OUTPUT_VARIABLE NET_OUTPUT
        ERROR_QUIET
    )
    
    if(NET_OUTPUT MATCHES "Parallels")
        set(PARALLELS_DETECTED TRUE)
        message(STATUS "  ✅ Parallels network adapter detected")
    endif()
    
    if(PARALLELS_DETECTED)
        message(STATUS "🪟 Parallels VM environment confirmed")
        set(OTTO_PARALLELS_VM TRUE PARENT_SCOPE)
        
        # Set Parallels-specific optimizations
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DOTTO_PARALLELS_VM=1" PARENT_SCOPE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DOTTO_PARALLELS_VM=1" PARENT_SCOPE)
    else()
        message(STATUS "🖥️  Native Windows environment detected")
        set(OTTO_PARALLELS_VM FALSE PARENT_SCOPE)
    endif()
endfunction()

#==============================================================================
# Parallels Audio Optimization
#==============================================================================

function(configure_parallels_audio_optimizations target)
    if(NOT OTTO_PARALLELS_VM)
        return()
    endif()
    
    message(STATUS "🎵 Configuring Parallels audio optimizations for: ${target}")
    
    # Parallels-specific audio definitions
    target_compile_definitions(${target} PRIVATE
        # Prefer WASAPI over DirectSound in VM
        JUCE_PARALLELS_WASAPI_PREFERRED=1
        
        # Increase buffer sizes for VM stability
        JUCE_PARALLELS_DEFAULT_BUFFER_SIZE=512
        
        # Reduce sample rate options for stability
        JUCE_PARALLELS_MAX_SAMPLE_RATE=48000
        
        # Enable additional VM-specific error checking
        JUCE_PARALLELS_ENHANCED_ERROR_CHECKING=1
    )
    
    # VM-specific compiler optimizations
    target_compile_options(${target} PRIVATE
        # Reduce aggressive optimizations that may cause issues in VM
        $<$<CONFIG:Release>:/O1>  # Use /O1 instead of /O2 for VM stability
        
        # Disable problematic optimizations
        $<$<CONFIG:Release>:/Oy->  # Disable frame pointer omission
    )
    
    message(STATUS "  ✅ Parallels audio optimizations applied")
endfunction()

#==============================================================================
# Parallels Performance Tuning
#==============================================================================

function(configure_parallels_performance target)
    if(NOT OTTO_PARALLELS_VM)
        return()
    endif()
    
    message(STATUS "⚡ Configuring Parallels performance optimizations for: ${target}")
    
    # Memory management optimizations for VM
    target_compile_definitions(${target} PRIVATE
        # Reduce memory footprint
        JUCE_PARALLELS_REDUCED_MEMORY_USAGE=1
        
        # Optimize garbage collection for VM
        JUCE_PARALLELS_CONSERVATIVE_GC=1
        
        # Limit concurrent operations
        JUCE_PARALLELS_CONSERVATIVE_THREADING=1
    )
    
    # Link-time optimizations adjusted for VM
    target_link_options(${target} PRIVATE
        # Reduce link-time optimization intensity
        $<$<CONFIG:Release>:/LTCG:INCREMENTAL>
        
        # VM-friendly stack size
        /STACK:2097152  # 2MB stack (conservative)
    )
    
    message(STATUS "  ✅ Parallels performance optimizations applied")
endfunction()

#==============================================================================
# Parallels Development Environment Setup
#==============================================================================

function(setup_parallels_development_environment)
    if(NOT OTTO_PARALLELS_VM)
        return()
    endif()
    
    message(STATUS "🛠️  Setting up Parallels development environment...")
    
    # Create VM-specific build scripts
    configure_file(
        "${CMAKE_SOURCE_DIR}/cmake/templates/parallels_build_setup.bat.in"
        "${CMAKE_BINARY_DIR}/parallels_build_setup.bat"
        @ONLY
    )
    
    # Create Reaper integration scripts
    configure_file(
        "${CMAKE_SOURCE_DIR}/cmake/templates/reaper_plugin_install.bat.in"
        "${CMAKE_BINARY_DIR}/reaper_plugin_install.bat"
        @ONLY
    )
    
    # Create performance monitoring script
    configure_file(
        "${CMAKE_SOURCE_DIR}/cmake/templates/monitor_vm_performance.bat.in"
        "${CMAKE_BINARY_DIR}/monitor_vm_performance.bat"
        @ONLY
    )
    
    message(STATUS "  ✅ Parallels development scripts created")
endfunction()

#==============================================================================
# Parallels Resource Monitoring
#==============================================================================

function(configure_parallels_resource_monitoring target)
    if(NOT OTTO_PARALLELS_VM)
        return()
    endif()
    
    message(STATUS "📊 Configuring resource monitoring for Parallels VM...")
    
    # Add resource monitoring capabilities
    target_compile_definitions(${target} PRIVATE
        JUCE_PARALLELS_RESOURCE_MONITORING=1
        JUCE_PARALLELS_CPU_USAGE_MONITORING=1
        JUCE_PARALLELS_MEMORY_USAGE_MONITORING=1
    )
    
    # Include Windows performance counters
    target_link_libraries(${target} PRIVATE
        pdh.lib  # Performance Data Helper
    )
    
    message(STATUS "  ✅ Resource monitoring configured")
endfunction()

#==============================================================================
# Parallels Network Optimization for Plugin Sharing
#==============================================================================

function(configure_parallels_network_sharing)
    if(NOT OTTO_PARALLELS_VM)
        return()
    endif()
    
    message(STATUS "🌐 Configuring Parallels network sharing...")
    
    # Check for shared folders
    if(EXISTS "Z:\\")
        message(STATUS "  ✅ Parallels shared folders detected (Z:)")
        set(OTTO_PARALLELS_SHARED_FOLDER "Z:/" PARENT_SCOPE)
    elseif(EXISTS "E:\\")
        message(STATUS "  ✅ Parallels shared folders detected (E:)")
        set(OTTO_PARALLELS_SHARED_FOLDER "E:/" PARENT_SCOPE)
    else()
        message(STATUS "  ⚠️  No shared folders detected - manual plugin transfer required")
        set(OTTO_PARALLELS_SHARED_FOLDER "" PARENT_SCOPE)
    endif()
    
    # Create plugin sharing script
    if(OTTO_PARALLELS_SHARED_FOLDER)
        configure_file(
            "${CMAKE_SOURCE_DIR}/cmake/templates/sync_plugins_to_mac.bat.in"
            "${CMAKE_BINARY_DIR}/sync_plugins_to_mac.bat"
            @ONLY
        )
        message(STATUS "  ✅ Plugin sync script created")
    endif()
endfunction()

#==============================================================================
# Main Parallels Configuration Function
#==============================================================================

function(configure_parallels_build target)
    message(STATUS "🪟 Configuring Parallels-optimized build for: ${target}")
    
    # Detect Parallels environment
    detect_parallels_environment()
    
    if(OTTO_PARALLELS_VM)
        # Apply Parallels-specific configurations
        configure_parallels_audio_optimizations(${target})
        configure_parallels_performance(${target})
        configure_parallels_resource_monitoring(${target})
        
        # Setup development environment
        setup_parallels_development_environment()
        configure_parallels_network_sharing()
        
        message(STATUS "✅ Parallels VM optimization complete for: ${target}")
    else()
        message(STATUS "ℹ️  Skipping Parallels optimizations (native Windows detected)")
    endif()
endfunction()

# Export configuration function
set(OTTO_PARALLELS_CMAKE_LOADED TRUE)
message(STATUS "OTTO Parallels optimization loaded")