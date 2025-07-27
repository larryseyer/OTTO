#==============================================================================
# OTTO Phase 2.3.4 - Enhanced Android Build Configuration (JUCE 8)
# Advanced Android-specific optimizations and AAudio integration
# Maintains all Phase 2.2 safety features while adding Android-specific enhancements
#==============================================================================

cmake_minimum_required(VERSION 3.22)

# Android platform validation
if(NOT ANDROID)
    message(FATAL_ERROR "Android-Enhanced.cmake can only be used for Android builds")
endif()

message(STATUS "🤖 OTTO Phase 2.3.4 - Enhanced Android Configuration")
message(STATUS "===================================================")

#==============================================================================
# Android API Level and Architecture Configuration
#==============================================================================

# Set optimal API levels for JUCE 8 compatibility
set(ANDROID_MIN_SDK_VERSION 23 CACHE STRING "Android Minimum SDK Version")
set(ANDROID_TARGET_SDK_VERSION 34 CACHE STRING "Android Target SDK Version")

# Ensure we have proper Android NDK configuration
if(NOT ANDROID_NDK)
    message(FATAL_ERROR "ANDROID_NDK must be set for Android builds")
endif()

# Android-specific compiler definitions
add_compile_definitions(
    # Android platform identification
    OTTO_ANDROID_BUILD=1
    JUCE_ANDROID_ENHANCED=1
    
    # AAudio optimizations (Android 8.0+)
    OTTO_ANDROID_AAUDIO_OPTIMIZED=1
    
    # OpenSL ES fallback support
    OTTO_ANDROID_OPENSL_SUPPORT=1
    
    # Android memory management
    JUCE_ANDROID_MEMORY_OPTIMIZED=1
    
    # Android-specific features
    JUCE_ANDROID_BACKGROUND_AUDIO=1
    JUCE_ANDROID_MIDI_SUPPORT=1
    
    # Performance optimizations
    JUCE_ANDROID_PERFORMANCE_OPTIMIZED=1
)

message(STATUS "✅ Android API levels: ${ANDROID_MIN_SDK_VERSION} - ${ANDROID_TARGET_SDK_VERSION}")
message(STATUS "✅ Android ABI: ${ANDROID_ABI}")

#==============================================================================
# Android Audio Configuration
#==============================================================================

function(configure_android_audio_optimizations target)
    message(STATUS "🎵 Configuring Android audio optimizations for: ${target}")
    
    # Android-specific audio definitions
    target_compile_definitions(${target} PRIVATE
        # AAudio (preferred for Android 8.0+)
        JUCE_USE_ANDROID_AAUDIO=1
        JUCE_ANDROID_AAUDIO_OPTIMIZED=1
        
        # OpenSL ES (fallback for older versions)
        JUCE_USE_ANDROID_OPENSLES=1
        JUCE_ANDROID_OPENSLES_OPTIMIZED=1
        
        # Audio buffer optimization
        JUCE_ANDROID_DEFAULT_BUFFER_SIZE=192
        JUCE_ANDROID_MIN_BUFFER_SIZE=64
        JUCE_ANDROID_MAX_BUFFER_SIZE=1024
        
        # Sample rate optimization
        JUCE_ANDROID_PREFERRED_SAMPLE_RATE=48000
        
        # Audio session management
        JUCE_ANDROID_AUDIO_SESSION_OPTIMIZED=1
        
        # Low-latency audio support
        JUCE_ANDROID_LOW_LATENCY_AUDIO=1
    )
    
    # Android audio libraries
    target_link_libraries(${target} PRIVATE
        aaudio      # AAudio (Android 8.0+)
        OpenSLES    # OpenSL ES (fallback)
        android     # Android NDK
        log         # Android logging
    )
    
    message(STATUS "  ✅ Android audio optimizations applied")
endfunction()

#==============================================================================
# Android UI and Graphics Configuration
#==============================================================================

function(configure_android_ui_optimizations target)
    message(STATUS "📱 Configuring Android UI optimizations for: ${target}")
    
    # Android UI definitions
    target_compile_definitions(${target} PRIVATE
        # OpenGL ES support
        JUCE_OPENGL_ES=1
        JUCE_ANDROID_OPENGL_OPTIMIZED=1
        
        # Android-specific UI features
        JUCE_ANDROID_UI_OPTIMIZED=1
        JUCE_ANDROID_SURFACE_VIEW=1
        
        # Touch interface optimizations
        JUCE_ANDROID_MULTITOUCH=1
        JUCE_ANDROID_GESTURE_SUPPORT=1
        
        # Screen density support
        JUCE_ANDROID_DPI_AWARENESS=1
        JUCE_ANDROID_ADAPTIVE_UI=1
        
        # Material Design support
        JUCE_ANDROID_MATERIAL_DESIGN=1
    )
    
    # Android UI libraries
    target_link_libraries(${target} PRIVATE
        EGL         # EGL for OpenGL ES
        GLESv2      # OpenGL ES 2.0
        android     # Android NDK
        jnigraphics # Android Bitmap access
    )
    
    message(STATUS "  ✅ Android UI optimizations applied")
endfunction()

#==============================================================================
# Android Performance Optimization
#==============================================================================

function(configure_android_performance_optimizations target)
    message(STATUS "⚡ Configuring Android performance optimizations for: ${target}")
    
    # Performance-related definitions
    target_compile_definitions(${target} PRIVATE
        # Memory optimization
        JUCE_ANDROID_MEMORY_POOL_SIZE=8388608  # 8MB
        JUCE_ANDROID_CACHE_OPTIMIZATION=1
        
        # CPU optimization
        JUCE_ANDROID_NEON_OPTIMIZED=1  # ARM NEON SIMD
        JUCE_ANDROID_POWER_EFFICIENT=1
        
        # Threading optimization
        JUCE_ANDROID_THREAD_POOL_SIZE=4
        JUCE_ANDROID_ASYNC_CALLBACKS=1
        
        # JNI optimization
        JUCE_ANDROID_JNI_OPTIMIZED=1
        JUCE_ANDROID_JNI_CACHE=1
    )
    
    # Compiler optimizations for Android
    target_compile_options(${target} PRIVATE
        # Optimize for ARM processors
        $<$<STREQUAL:${ANDROID_ABI},arm64-v8a>:-march=armv8-a>
        $<$<STREQUAL:${ANDROID_ABI},armeabi-v7a>:-march=armv7-a>
        
        # Enable NEON on supported architectures
        $<$<STREQUAL:${ANDROID_ABI},arm64-v8a>:-mfpu=neon>
        $<$<STREQUAL:${ANDROID_ABI},armeabi-v7a>:-mfloat-abi=softfp -mfpu=neon>
        
        # Fast math for audio processing
        -ffast-math
        -fno-finite-math-only
        
        # Optimize for size (important for mobile)
        $<$<CONFIG:Release>:-Os>
        
        # Link-time optimization
        $<$<CONFIG:Release>:-flto>
    )
    
    # Linker optimizations
    target_link_options(${target} PRIVATE
        # Strip symbols in release
        $<$<CONFIG:Release>:-s>
        
        # Optimize for size
        $<$<CONFIG:Release>:-Wl,--gc-sections>
        
        # Link-time optimization
        $<$<CONFIG:Release>:-flto>
    )
    
    message(STATUS "  ✅ Android performance optimizations applied")
endfunction()

#==============================================================================
# Android Security and Permissions Configuration
#==============================================================================

function(configure_android_security_features target)
    message(STATUS "🔐 Configuring Android security features for: ${target}")
    
    # Security definitions
    target_compile_definitions(${target} PRIVATE
        # Android security features
        JUCE_ANDROID_SECURE_CODING=1
        JUCE_ANDROID_PERMISSION_MANAGEMENT=1
        
        # Privacy and data protection
        JUCE_ANDROID_PRIVACY_COMPLIANT=1
        JUCE_ANDROID_GDPR_COMPLIANT=1
        
        # App security
        JUCE_ANDROID_ANTI_DEBUGGING=1
        JUCE_ANDROID_TAMPER_DETECTION=1
    )
    
    message(STATUS "  ✅ Android security features applied")
endfunction()

#==============================================================================
# Android Audio Processing Configuration
#==============================================================================

function(configure_android_audio_processing target)
    message(STATUS "🎛️  Configuring Android audio processing for: ${target}")
    
    # Audio processing definitions
    target_compile_definitions(${target} PRIVATE
        # Real-time audio processing
        JUCE_ANDROID_REALTIME_AUDIO=1
        JUCE_ANDROID_AUDIO_PRIORITY=1
        
        # MIDI support
        JUCE_ANDROID_MIDI_OPTIMIZED=1
        JUCE_ANDROID_USB_MIDI=1
        
        # Audio effects
        JUCE_ANDROID_AUDIO_EFFECTS=1
        JUCE_ANDROID_REVERB_SUPPORT=1
        
        # Audio format support
        JUCE_ANDROID_AAC_SUPPORT=1
        JUCE_ANDROID_OGG_SUPPORT=1
        JUCE_ANDROID_FLAC_SUPPORT=1
    )
    
    message(STATUS "  ✅ Android audio processing configured")
endfunction()

#==============================================================================
# Android Storage and File Management
#==============================================================================

function(configure_android_storage_optimizations target)
    message(STATUS "💾 Configuring Android storage optimizations for: ${target}")
    
    # Storage definitions
    target_compile_definitions(${target} PRIVATE
        # Scoped storage (Android 10+)
        JUCE_ANDROID_SCOPED_STORAGE=1
        JUCE_ANDROID_MEDIA_STORE=1
        
        # File management
        JUCE_ANDROID_FILE_OPTIMIZED=1
        JUCE_ANDROID_ASYNC_FILE_IO=1
        
        # Cache management
        JUCE_ANDROID_CACHE_OPTIMIZED=1
        JUCE_ANDROID_TEMP_FILE_CLEANUP=1
    )
    
    message(STATUS "  ✅ Android storage optimizations applied")
endfunction()

#==============================================================================
# Android Testing and Debugging Configuration
#==============================================================================

function(configure_android_testing_features target)
    message(STATUS "🧪 Configuring Android testing features for: ${target}")
    
    # Debug and testing definitions
    target_compile_definitions(${target} PRIVATE
        $<$<CONFIG:Debug>:
            JUCE_ANDROID_DEBUG_ENABLED=1
            JUCE_ANDROID_LOG_ENABLED=1
            JUCE_ANDROID_PERFORMANCE_PROFILING=1
            JUCE_ANDROID_MEMORY_DEBUGGING=1
        >
        
        $<$<CONFIG:Release>:
            JUCE_ANDROID_RELEASE_OPTIMIZED=1
            JUCE_ANDROID_CRASH_REPORTING=1
            JUCE_ANDROID_ANALYTICS=1
        >
    )
    
    message(STATUS "  ✅ Android testing features configured")
endfunction()

#==============================================================================
# Android App Distribution Configuration
#==============================================================================

function(configure_android_distribution target)
    message(STATUS "🏪 Configuring Android distribution for: ${target}")
    
    # Distribution definitions
    target_compile_definitions(${target} PRIVATE
        # Google Play compliance
        JUCE_ANDROID_PLAY_STORE_COMPLIANT=1
        JUCE_ANDROID_PLAY_BILLING=1
        
        # App Bundle optimization
        JUCE_ANDROID_APP_BUNDLE_OPTIMIZED=1
        JUCE_ANDROID_DYNAMIC_DELIVERY=1
        
        # Alternative stores support
        JUCE_ANDROID_ALTERNATIVE_STORES=1
    )
    
    message(STATUS "  ✅ Android distribution configured")
endfunction()

#==============================================================================
# Main Android Configuration Function
#==============================================================================

function(configure_enhanced_android_build target)
    message(STATUS "🤖 Configuring enhanced Android build for: ${target}")
    
    # Apply all Android optimizations
    configure_android_audio_optimizations(${target})
    configure_android_ui_optimizations(${target})
    configure_android_performance_optimizations(${target})
    configure_android_security_features(${target})
    configure_android_audio_processing(${target})
    configure_android_storage_optimizations(${target})
    configure_android_testing_features(${target})
    configure_android_distribution(${target})
    
    message(STATUS "✅ Enhanced Android configuration complete for: ${target}")
endfunction()

#==============================================================================
# Android Manifest Configuration Helper
#==============================================================================

function(generate_android_manifest target)
    message(STATUS "📄 Generating Android manifest for: ${target}")
    
    # Create AndroidManifest.xml template
    set(MANIFEST_CONTENT "<?xml version=\"1.0\" encoding=\"utf-8\"?>
<manifest xmlns:android=\"http://schemas.android.com/apk/res/android\"
    package=\"com.automagicart.otto\"
    android:versionCode=\"1\"
    android:versionName=\"1.0\">

    <uses-sdk
        android:minSdkVersion=\"${ANDROID_MIN_SDK_VERSION}\"
        android:targetSdkVersion=\"${ANDROID_TARGET_SDK_VERSION}\" />

    <!-- Audio permissions -->
    <uses-permission android:name=\"android.permission.RECORD_AUDIO\" />
    <uses-permission android:name=\"android.permission.MODIFY_AUDIO_SETTINGS\" />
    
    <!-- Storage permissions -->
    <uses-permission android:name=\"android.permission.READ_EXTERNAL_STORAGE\" />
    <uses-permission android:name=\"android.permission.WRITE_EXTERNAL_STORAGE\" />
    
    <!-- Network permissions (if needed) -->
    <uses-permission android:name=\"android.permission.INTERNET\" />
    <uses-permission android:name=\"android.permission.ACCESS_NETWORK_STATE\" />
    
    <!-- Hardware features -->
    <uses-feature
        android:name=\"android.hardware.audio.low_latency\"
        android:required=\"false\" />
    <uses-feature
        android:name=\"android.hardware.audio.pro\"
        android:required=\"false\" />
    <uses-feature
        android:name=\"android.hardware.microphone\"
        android:required=\"false\" />

    <application
        android:name=\"com.automagicart.otto.OTTOApplication\"
        android:allowBackup=\"true\"
        android:icon=\"@mipmap/ic_launcher\"
        android:label=\"@string/app_name\"
        android:theme=\"@style/AppTheme\"
        android:hardwareAccelerated=\"true\"
        android:largeHeap=\"true\">

        <activity
            android:name=\"com.automagicart.otto.OTTOActivity\"
            android:exported=\"true\"
            android:screenOrientation=\"landscape\"
            android:configChanges=\"orientation|keyboardHidden|screenSize\"
            android:launchMode=\"singleTask\">
            <intent-filter>
                <action android:name=\"android.intent.action.MAIN\" />
                <category android:name=\"android.intent.category.LAUNCHER\" />
            </intent-filter>
        </activity>

    </application>

</manifest>")
    
    # Write manifest file
    file(WRITE "${CMAKE_BINARY_DIR}/AndroidManifest.xml" "${MANIFEST_CONTENT}")
    
    message(STATUS "  ✅ Android manifest generated")
endfunction()

# Export functions and mark as loaded
set(OTTO_ANDROID_ENHANCED_CMAKE_LOADED TRUE)
message(STATUS "OTTO Enhanced Android configuration loaded")