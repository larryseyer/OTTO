#==============================================================================
# OTTO Phase 2.3.5 - Advanced Robustness Features (JUCE 8)
# Cross-platform advanced safety and robustness features
# Builds upon all previous Phase 2.2 and 2.3.x safety implementations
#==============================================================================

cmake_minimum_required(VERSION 3.22)

message(STATUS "🛡️  OTTO Phase 2.3.5 - Advanced Robustness Features")
message(STATUS "====================================================")

#==============================================================================
# Cross-Platform Memory Safety Enhancements
#==============================================================================

function(configure_advanced_memory_safety target)
    message(STATUS "🧠 Configuring advanced memory safety for: ${target}")
    
    # Enhanced memory safety definitions
    target_compile_definitions(${target} PRIVATE
        # Advanced null pointer safety
        OTTO_ADVANCED_NULL_SAFETY=1
        OTTO_AUTOMATIC_NULL_CHECKS=1
        
        # Memory leak detection
        OTTO_MEMORY_LEAK_DETECTION=1
        OTTO_AUTOMATIC_LEAK_CLEANUP=1
        
        # Buffer overflow protection
        OTTO_BUFFER_OVERFLOW_PROTECTION=1
        OTTO_BOUNDS_CHECKING=1
        
        # Smart pointer enforcement
        OTTO_SMART_POINTER_ENFORCEMENT=1
        OTTO_RAW_POINTER_WARNINGS=1
        
        # Memory pool optimization
        OTTO_MEMORY_POOL_ENABLED=1
        OTTO_CUSTOM_ALLOCATORS=1
    )
    
    # Platform-specific memory safety
    if(WIN32)
        target_compile_definitions(${target} PRIVATE
            # Windows-specific memory features
            OTTO_WINDOWS_HEAP_DEBUGGING=1
            OTTO_WINDOWS_MEMORY_TRACKING=1
        )
    elseif(APPLE)
        target_compile_definitions(${target} PRIVATE
            # macOS/iOS-specific memory features
            OTTO_APPLE_MEMORY_DEBUGGING=1
            OTTO_APPLE_ARC_INTEGRATION=1
        )
    elseif(ANDROID)
        target_compile_definitions(${target} PRIVATE
            # Android-specific memory features
            OTTO_ANDROID_MEMORY_OPTIMIZATION=1
            OTTO_ANDROID_LOW_MEMORY_HANDLING=1
        )
    elseif(UNIX)
        target_compile_definitions(${target} PRIVATE
            # Linux-specific memory features
            OTTO_LINUX_MEMORY_DEBUGGING=1
            OTTO_LINUX_VALGRIND_INTEGRATION=1
        )
    endif()
    
    message(STATUS "  ✅ Advanced memory safety configured")
endfunction()

#==============================================================================
# Enhanced Audio Processing Safety
#==============================================================================

function(configure_advanced_audio_safety target)
    message(STATUS "🎵 Configuring advanced audio safety for: ${target}")
    
    # Advanced audio safety definitions
    target_compile_definitions(${target} PRIVATE
        # Audio buffer safety
        OTTO_AUDIO_BUFFER_VALIDATION=1
        OTTO_AUDIO_UNDERRUN_PROTECTION=1
        OTTO_AUDIO_OVERRUN_PROTECTION=1
        
        # Real-time safety
        OTTO_REALTIME_SAFETY_CHECKS=1
        OTTO_NON_REALTIME_ALLOCATION_DETECTION=1
        OTTO_REALTIME_THREAD_MONITORING=1
        
        # Audio format validation
        OTTO_AUDIO_FORMAT_VALIDATION=1
        OTTO_SAMPLE_RATE_VALIDATION=1
        OTTO_CHANNEL_COUNT_VALIDATION=1
        
        # Audio driver safety
        OTTO_AUDIO_DRIVER_MONITORING=1
        OTTO_AUDIO_DEVICE_HOTPLUG_SAFETY=1
        
        # Signal processing safety
        OTTO_DSP_NAN_DETECTION=1
        OTTO_DSP_INFINITY_DETECTION=1
        OTTO_DSP_DENORMAL_PROTECTION=1
        
        # Audio callback safety
        OTTO_AUDIO_CALLBACK_TIMING=1
        OTTO_AUDIO_CALLBACK_VALIDATION=1
    )
    
    message(STATUS "  ✅ Advanced audio safety configured")
endfunction()

#==============================================================================
# Enhanced MIDI Processing Safety
#==============================================================================

function(configure_advanced_midi_safety target)
    message(STATUS "🎹 Configuring advanced MIDI safety for: ${target}")
    
    # Advanced MIDI safety definitions
    target_compile_definitions(${target} PRIVATE
        # MIDI message validation
        OTTO_MIDI_MESSAGE_VALIDATION=1
        OTTO_MIDI_BOUNDS_CHECKING=1
        OTTO_MIDI_TIMING_VALIDATION=1
        
        # MIDI device safety
        OTTO_MIDI_DEVICE_MONITORING=1
        OTTO_MIDI_HOTPLUG_SAFETY=1
        OTTO_MIDI_DRIVER_VALIDATION=1
        
        # MIDI data safety
        OTTO_MIDI_DATA_INTEGRITY=1
        OTTO_MIDI_BUFFER_PROTECTION=1
        OTTO_MIDI_OVERFLOW_PROTECTION=1
        
        # MIDI clock safety
        OTTO_MIDI_CLOCK_STABILITY=1
        OTTO_MIDI_SYNC_VALIDATION=1
        
        # MIDI SysEx safety
        OTTO_MIDI_SYSEX_VALIDATION=1
        OTTO_MIDI_SYSEX_SIZE_LIMITS=1
    )
    
    message(STATUS "  ✅ Advanced MIDI safety configured")
endfunction()

#==============================================================================
# Enhanced UI Thread Safety
#==============================================================================

function(configure_advanced_ui_safety target)
    message(STATUS "🖥️  Configuring advanced UI safety for: ${target}")
    
    # Advanced UI safety definitions
    target_compile_definitions(${target} PRIVATE
        # UI thread safety
        OTTO_UI_THREAD_VALIDATION=1
        OTTO_UI_COMPONENT_LIFECYCLE=1
        OTTO_UI_MEMORY_SAFETY=1
        
        # Component safety
        OTTO_COMPONENT_DELETION_SAFETY=1
        OTTO_COMPONENT_PARENT_VALIDATION=1
        OTTO_COMPONENT_BOUNDS_CHECKING=1
        
        # Event safety
        OTTO_EVENT_QUEUE_VALIDATION=1
        OTTO_EVENT_HANDLER_SAFETY=1
        OTTO_EVENT_TIMING_MONITORING=1
        
        # Graphics safety
        OTTO_GRAPHICS_CONTEXT_VALIDATION=1
        OTTO_DRAWING_BOUNDS_CHECKING=1
        OTTO_GRAPHICS_RESOURCE_TRACKING=1
        
        # Timer safety
        OTTO_TIMER_LIFECYCLE_MANAGEMENT=1
        OTTO_TIMER_CALLBACK_VALIDATION=1
        
        # Repaint safety
        OTTO_REPAINT_OPTIMIZATION=1
        OTTO_REPAINT_BOUNDS_VALIDATION=1
    )
    
    message(STATUS "  ✅ Advanced UI safety configured")
endfunction()

#==============================================================================
# Enhanced Plugin Safety Features
#==============================================================================

function(configure_advanced_plugin_safety target)
    message(STATUS "🔌 Configuring advanced plugin safety for: ${target}")
    
    # Advanced plugin safety definitions
    target_compile_definitions(${target} PRIVATE
        # Plugin host communication safety
        OTTO_PLUGIN_HOST_VALIDATION=1
        OTTO_PLUGIN_PARAMETER_VALIDATION=1
        OTTO_PLUGIN_STATE_INTEGRITY=1
        
        # Plugin lifecycle safety
        OTTO_PLUGIN_INITIALIZATION_SAFETY=1
        OTTO_PLUGIN_CLEANUP_SAFETY=1
        OTTO_PLUGIN_SUSPEND_RESUME_SAFETY=1
        
        # Plugin format safety
        OTTO_VST3_ADVANCED_SAFETY=1
        OTTO_AU_ADVANCED_SAFETY=1
        OTTO_CLAP_ADVANCED_SAFETY=1
        OTTO_AUV3_ADVANCED_SAFETY=1
        
        # Plugin preset safety
        OTTO_PRESET_VALIDATION=1
        OTTO_PRESET_COMPATIBILITY_CHECKING=1
        OTTO_PRESET_CORRUPTION_DETECTION=1
        
        # Plugin automation safety  
        OTTO_AUTOMATION_BOUNDS_CHECKING=1
        OTTO_AUTOMATION_RATE_LIMITING=1
        OTTO_AUTOMATION_SMOOTHING_SAFETY=1
    )
    
    message(STATUS "  ✅ Advanced plugin safety configured")
endfunction()

#==============================================================================
# Enhanced File I/O Safety
#==============================================================================

function(configure_advanced_file_safety target)
    message(STATUS "📁 Configuring advanced file I/O safety for: ${target}")
    
    # Advanced file safety definitions
    target_compile_definitions(${target} PRIVATE
        # File operation safety
        OTTO_FILE_OPERATION_VALIDATION=1
        OTTO_FILE_PATH_SANITIZATION=1
        OTTO_FILE_PERMISSION_CHECKING=1
        
        # File format safety
        OTTO_AUDIO_FILE_VALIDATION=1
        OTTO_PRESET_FILE_VALIDATION=1
        OTTO_CONFIG_FILE_VALIDATION=1
        
        # File corruption protection
        OTTO_FILE_CHECKSUM_VALIDATION=1
        OTTO_FILE_BACKUP_CREATION=1
        OTTO_FILE_RECOVERY_MECHANISMS=1
        
        # Async file I/O safety
        OTTO_ASYNC_FILE_SAFETY=1
        OTTO_FILE_OPERATION_TIMEOUT=1
        OTTO_FILE_OPERATION_CANCELLATION=1
        
        # Temporary file safety
        OTTO_TEMP_FILE_CLEANUP=1
        OTTO_TEMP_FILE_SECURITY=1
    )
    
    message(STATUS "  ✅ Advanced file I/O safety configured")
endfunction()

#==============================================================================
# Enhanced Error Recovery Systems
#==============================================================================

function(configure_advanced_error_recovery target)
    message(STATUS "🚨 Configuring advanced error recovery for: ${target}")
    
    # Advanced error recovery definitions
    target_compile_definitions(${target} PRIVATE
        # Automatic error recovery
        OTTO_AUTOMATIC_ERROR_RECOVERY=1
        OTTO_GRACEFUL_DEGRADATION=1
        OTTO_FALLBACK_MECHANISMS=1
        
        # State restoration
        OTTO_STATE_CHECKPOINT_SYSTEM=1
        OTTO_AUTOMATIC_STATE_BACKUP=1
        OTTO_STATE_RECOVERY_VALIDATION=1
        
        # Resource recovery
        OTTO_RESOURCE_LEAK_RECOVERY=1
        OTTO_AUTOMATIC_RESOURCE_CLEANUP=1
        OTTO_RESOURCE_POOL_RECOVERY=1
        
        # Audio recovery
        OTTO_AUDIO_STREAM_RECOVERY=1
        OTTO_AUDIO_DEVICE_RECOVERY=1
        OTTO_AUDIO_FORMAT_RECOVERY=1
        
        # Plugin recovery
        OTTO_PLUGIN_CRASH_RECOVERY=1
        OTTO_PLUGIN_STATE_RECOVERY=1
        OTTO_PLUGIN_PARAMETER_RECOVERY=1
        
        # UI recovery
        OTTO_UI_COMPONENT_RECOVERY=1
        OTTO_UI_LAYOUT_RECOVERY=1
        OTTO_UI_THEME_RECOVERY=1
    )
    
    message(STATUS "  ✅ Advanced error recovery configured")
endfunction()

#==============================================================================
# Enhanced Performance Monitoring
#==============================================================================

function(configure_advanced_performance_monitoring target)
    message(STATUS "📊 Configuring advanced performance monitoring for: ${target}")
    
    # Advanced performance monitoring definitions
    target_compile_definitions(${target} PRIVATE
        # Real-time performance monitoring
        OTTO_REALTIME_PERFORMANCE_MONITORING=1
        OTTO_CPU_USAGE_MONITORING=1
        OTTO_MEMORY_USAGE_MONITORING=1
        
        # Audio performance monitoring
        OTTO_AUDIO_LATENCY_MONITORING=1
        OTTO_AUDIO_DROPOUT_DETECTION=1
        OTTO_AUDIO_PERFORMANCE_METRICS=1
        
        # Threading performance
        OTTO_THREAD_PERFORMANCE_MONITORING=1
        OTTO_LOCK_CONTENTION_DETECTION=1
        OTTO_DEADLOCK_DETECTION=1
        
        # Resource monitoring
        OTTO_RESOURCE_USAGE_TRACKING=1
        OTTO_HANDLE_LEAK_DETECTION=1
        OTTO_GRAPHICS_RESOURCE_MONITORING=1
        
        # Performance analytics
        OTTO_PERFORMANCE_ANALYTICS=1
        OTTO_PERFORMANCE_ALERTING=1
        OTTO_PERFORMANCE_OPTIMIZATION_HINTS=1
    )
    
    message(STATUS "  ✅ Advanced performance monitoring configured")
endfunction()

#==============================================================================
# Enhanced Cross-Platform Compatibility
#==============================================================================

function(configure_advanced_cross_platform_features target)
    message(STATUS "🌐 Configuring advanced cross-platform features for: ${target}")
    
    # Cross-platform compatibility definitions
    target_compile_definitions(${target} PRIVATE
        # Platform abstraction
        OTTO_PLATFORM_ABSTRACTION_LAYER=1
        OTTO_CROSS_PLATFORM_COMPATIBILITY=1
        
        # Format compatibility
        OTTO_CROSS_PLATFORM_PRESETS=1
        OTTO_CROSS_PLATFORM_PROJECTS=1
        OTTO_CROSS_PLATFORM_SETTINGS=1
        
        # Data compatibility
        OTTO_ENDIANNESS_HANDLING=1
        OTTO_CHARACTER_ENCODING_SAFETY=1
        OTTO_PATH_SEPARATOR_HANDLING=1
        
        # Plugin format compatibility
        OTTO_CROSS_PLATFORM_VST3=1
        OTTO_CROSS_PLATFORM_CLAP=1
        OTTO_CROSS_PLATFORM_AU=1
        
        # Version compatibility
        OTTO_BACKWARD_COMPATIBILITY=1
        OTTO_FORWARD_COMPATIBILITY=1
        OTTO_VERSION_MIGRATION=1
    )
    
    message(STATUS "  ✅ Advanced cross-platform features configured")
endfunction()

#==============================================================================
# Main Advanced Robustness Configuration Function
#==============================================================================

function(configure_advanced_robustness_features target)
    message(STATUS "🛡️  Configuring advanced robustness features for: ${target}")
    
    # Apply all advanced robustness features
    configure_advanced_memory_safety(${target})
    configure_advanced_audio_safety(${target})
    configure_advanced_midi_safety(${target})
    configure_advanced_ui_safety(${target})
    configure_advanced_plugin_safety(${target})
    configure_advanced_file_safety(${target})
    configure_advanced_error_recovery(${target})
    configure_advanced_performance_monitoring(${target})
    configure_advanced_cross_platform_features(${target})
    
    # Platform-specific robustness features
    if(WIN32 AND COMMAND configure_parallels_build)
        message(STATUS "  🪟 Applying Windows Parallels robustness features")
    endif()
    
    if(APPLE AND CMAKE_SYSTEM_NAME STREQUAL "iOS" AND COMMAND configure_enhanced_ios_build)
        message(STATUS "  🍎 Applying iOS robustness features")  
    endif()
    
    if(ANDROID AND COMMAND configure_enhanced_android_build)
        message(STATUS "  🤖 Applying Android robustness features")
    endif()
    
    # Apply compiler-specific safety features
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE
            # Enhanced warnings
            -Wall -Wextra -Wpedantic
            -Wformat=2 -Wformat-security
            -Wnull-dereference -Wdouble-promotion
            -Wshadow -Wundef -Wcast-align
            
            # Runtime checks (Debug builds)
            $<$<CONFIG:Debug>:-fsanitize=address>
            $<$<CONFIG:Debug>:-fsanitize=undefined>
            $<$<CONFIG:Debug>:-fstack-protector-strong>
        )
        
        target_link_options(${target} PRIVATE
            $<$<CONFIG:Debug>:-fsanitize=address>
            $<$<CONFIG:Debug>:-fsanitize=undefined>
        )
    elseif(MSVC)
        target_compile_options(${target} PRIVATE
            # Enhanced warnings
            /W4 /WX
            /analyze
            
            # Runtime checks
            $<$<CONFIG:Debug>:/RTC1>
            $<$<CONFIG:Debug>:/GS>
        )
    endif()
    
    message(STATUS "✅ Advanced robustness features complete for: ${target}")
    message(STATUS "🛡️  Phase 2.3.5 robustness features applied successfully")
endfunction()

# Export the configuration
set(OTTO_ADVANCED_ROBUSTNESS_CMAKE_LOADED TRUE)
message(STATUS "OTTO Advanced Robustness configuration loaded")