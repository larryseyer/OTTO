#pragma once

#include "JUCE8_CODING_STANDARDS.h"
#include <JuceHeader.h>
#include "../../ColorScheme.h"
#include "../../INIConfig.h"
#include "../../ComponentState.h"

/**
 * @file HapticFeedback.h
 * @brief Cross-platform haptic feedback system for OTTO JUCE 8 application
 * 
 * This component provides comprehensive haptic feedback capabilities across
 * all supported platforms with intelligent fallback mechanisms and
 * customizable feedback patterns for enhanced user interaction.
 * 
 * FEATURES:
 * - Cross-platform haptic feedback (iOS, Android, macOS, Windows, Linux)
 * - Multiple feedback intensities and patterns
 * - Audio feedback fallback for non-haptic devices
 * - Visual feedback integration for accessibility
 * - Custom haptic pattern creation and playback
 * - Performance optimization with feedback caching
 * - Integration with touch-optimized controls
 * 
 * TECHNICAL SPECIFICATIONS:
 * - Platform-specific implementations for optimal performance
 * - Feedback latency: <10ms on supported platforms
 * - Pattern duration: 10ms to 2000ms configurable
 * - Intensity levels: Light, Medium, Heavy, Custom
 * - Fallback modes: Audio, Visual, None
 * - Memory-efficient pattern caching system
 * - Thread-safe feedback triggering
 * 
 * INTEGRATION:
 * - Uses ColorScheme for visual feedback theming
 * - INI-driven configuration for all parameters
 * - Integrates with TouchOptimizedControls
 * - Supports ComponentState for persistence
 * 
 * @author OTTO Development Team
 * @version 1.0
 * @date 2025
 */

namespace OTTO {
namespace UI {
namespace Feedback {

/**
 * @class HapticFeedback
 * @brief Cross-platform haptic feedback system with intelligent fallbacks
 * 
 * This class provides comprehensive haptic feedback capabilities with
 * platform-specific optimizations and fallback mechanisms for devices
 * without haptic support.
 */
class HapticFeedback
{
public:
    /**
     * @enum FeedbackType
     * @brief Types of haptic feedback
     */
    enum class FeedbackType {
        Light,          ///< Light haptic feedback (selection, hover)
        Medium,         ///< Medium haptic feedback (button press, toggle)
        Heavy,          ///< Heavy haptic feedback (error, important action)
        Success,        ///< Success feedback pattern
        Warning,        ///< Warning feedback pattern
        Error,          ///< Error feedback pattern
        Selection,      ///< Selection change feedback
        Impact,         ///< Impact feedback (collision, snap)
        Notification,   ///< Notification feedback
        Custom          ///< Custom pattern feedback
    };

    /**
     * @enum FallbackMode
     * @brief Fallback modes when haptic feedback is unavailable
     */
    enum class FallbackMode {
        None,           ///< No fallback
        Audio,          ///< Audio feedback fallback
        Visual,         ///< Visual feedback fallback
        AudioVisual     ///< Combined audio and visual fallback
    };

    /**
     * @enum Platform
     * @brief Supported platforms for haptic feedback
     */
    enum class Platform {
        iOS,            ///< iOS devices with Taptic Engine
        Android,        ///< Android devices with vibration
        macOS,          ///< macOS with Force Touch trackpad
        Windows,        ///< Windows with haptic devices
        Linux,          ///< Linux with haptic support
        Unsupported     ///< Platform without haptic support
    };

    /**
     * @struct HapticPattern
     * @brief Custom haptic feedback pattern definition
     */
    struct HapticPattern {
        juce::String name;                      ///< Pattern name
        std::vector<float> intensities;         ///< Intensity values (0.0-1.0)
        std::vector<int> durations;             ///< Duration for each intensity (ms)
        std::vector<int> delays;                ///< Delay between pulses (ms)
        bool looping = false;                   ///< Whether pattern loops
        int repeatCount = 1;                    ///< Number of repetitions
        
        // Validation
        bool isValid() const {
            return !intensities.empty() && 
                   intensities.size() == durations.size() && 
                   intensities.size() == delays.size();
        }
    };

    /**
     * @struct FeedbackSettings
     * @brief Configuration settings for haptic feedback
     */
    struct FeedbackSettings {
        bool enabled = true;                    ///< Enable haptic feedback
        float globalIntensity = 1.0f;           ///< Global intensity multiplier
        FallbackMode fallbackMode = FallbackMode::AudioVisual;
        bool adaptToSystemSettings = true;     ///< Respect system haptic settings
        int maxConcurrentFeedbacks = 3;        ///< Maximum concurrent feedback instances
        bool enableCustomPatterns = true;      ///< Enable custom pattern support
        int feedbackLatency = 10;              ///< Target feedback latency (ms)
        bool enableDebugMode = false;          ///< Enable debug logging
        
        // Platform-specific settings
        struct {
            bool useSystemHaptics = true;      ///< Use system haptic APIs
            float vibrationIntensity = 0.8f;   ///< Android vibration intensity
            bool useForceTouch = true;         ///< macOS Force Touch integration
        } platformSettings;
    };

    /**
     * @struct FeedbackCapabilities
     * @brief Platform haptic feedback capabilities
     */
    struct FeedbackCapabilities {
        bool hasHapticSupport = false;          ///< Platform has haptic support
        bool hasVariableIntensity = false;     ///< Supports variable intensity
        bool hasCustomPatterns = false;        ///< Supports custom patterns
        bool hasAudioFallback = false;         ///< Has audio fallback capability
        bool hasVisualFallback = false;        ///< Has visual fallback capability
        int maxPatternLength = 0;              ///< Maximum pattern length (ms)
        int minFeedbackInterval = 0;           ///< Minimum interval between feedbacks (ms)
        juce::String platformName;             ///< Platform identifier
        juce::String deviceModel;              ///< Device model information
    };

    // Singleton access
    static HapticFeedback& getInstance();

    // Core Feedback Methods
    /**
     * @brief Trigger haptic feedback
     * @param type Type of feedback to trigger
     * @param intensity Intensity override (0.0-1.0, -1 for default)
     * @return Success status
     */
    bool triggerFeedback(FeedbackType type, float intensity = -1.0f);

    /**
     * @brief Trigger custom haptic pattern
     * @param pattern Custom pattern to play
     * @param intensity Intensity multiplier
     * @return Success status
     */
    bool triggerCustomPattern(const HapticPattern& pattern, float intensity = 1.0f);

    /**
     * @brief Trigger feedback with delay
     * @param type Type of feedback to trigger
     * @param delayMs Delay before triggering (ms)
     * @param intensity Intensity override
     * @return Success status
     */
    bool triggerDelayedFeedback(FeedbackType type, int delayMs, float intensity = -1.0f);

    /**
     * @brief Stop all active haptic feedback
     */
    void stopAllFeedback();

    /**
     * @brief Stop specific feedback type
     * @param type Type of feedback to stop
     */
    void stopFeedback(FeedbackType type);

    // Configuration
    /**
     * @brief Set feedback settings
     * @param settings New settings to apply
     */
    void setSettings(const FeedbackSettings& settings);

    /**
     * @brief Get current feedback settings
     * @return Current settings
     */
    const FeedbackSettings& getSettings() const { return settings; }

    /**
     * @brief Enable/disable haptic feedback
     * @param enabled Whether to enable feedback
     */
    void setEnabled(bool enabled);

    /**
     * @brief Check if haptic feedback is enabled
     * @return Whether feedback is enabled
     */
    bool isEnabled() const { return settings.enabled; }

    /**
     * @brief Set global intensity multiplier
     * @param intensity Global intensity (0.0-1.0)
     */
    void setGlobalIntensity(float intensity);

    /**
     * @brief Get global intensity multiplier
     * @return Global intensity
     */
    float getGlobalIntensity() const { return settings.globalIntensity; }

    /**
     * @brief Set fallback mode
     * @param mode Fallback mode to use
     */
    void setFallbackMode(FallbackMode mode);

    /**
     * @brief Get current fallback mode
     * @return Current fallback mode
     */
    FallbackMode getFallbackMode() const { return settings.fallbackMode; }

    // Platform Information
    /**
     * @brief Get platform capabilities
     * @return Platform haptic capabilities
     */
    const FeedbackCapabilities& getCapabilities() const { return capabilities; }

    /**
     * @brief Get current platform
     * @return Current platform type
     */
    Platform getCurrentPlatform() const { return currentPlatform; }

    /**
     * @brief Check if haptic feedback is supported
     * @return Whether haptic feedback is supported
     */
    bool isHapticSupported() const { return capabilities.hasHapticSupport; }

    /**
     * @brief Check if custom patterns are supported
     * @return Whether custom patterns are supported
     */
    bool areCustomPatternsSupported() const { return capabilities.hasCustomPatterns; }

    // Pattern Management
    /**
     * @brief Register custom haptic pattern
     * @param pattern Pattern to register
     * @return Success status
     */
    bool registerCustomPattern(const HapticPattern& pattern);

    /**
     * @brief Unregister custom pattern
     * @param patternName Name of pattern to remove
     * @return Success status
     */
    bool unregisterCustomPattern(const juce::String& patternName);

    /**
     * @brief Get registered custom pattern
     * @param patternName Name of pattern to retrieve
     * @return Pattern if found, empty pattern otherwise
     */
    HapticPattern getCustomPattern(const juce::String& patternName) const;

    /**
     * @brief Get list of registered pattern names
     * @return List of pattern names
     */
    juce::StringArray getRegisteredPatternNames() const;

    /**
     * @brief Clear all custom patterns
     */
    void clearCustomPatterns();

    // Fallback Methods
    /**
     * @brief Trigger audio fallback
     * @param type Type of feedback for audio selection
     * @param intensity Intensity for audio volume
     */
    void triggerAudioFallback(FeedbackType type, float intensity);

    /**
     * @brief Trigger visual fallback
     * @param type Type of feedback for visual selection
     * @param intensity Intensity for visual effect
     * @param component Component to apply visual feedback to
     */
    void triggerVisualFallback(FeedbackType type, float intensity, juce::Component* component = nullptr);

    // Utility Methods
    /**
     * @brief Test haptic feedback functionality
     * @return Test results and capabilities
     */
    juce::String testHapticFeedback();

    /**
     * @brief Get feedback type name
     * @param type Feedback type
     * @return Human-readable name
     */
    static juce::String getFeedbackTypeName(FeedbackType type);

    /**
     * @brief Get platform name
     * @param platform Platform type
     * @return Human-readable platform name
     */
    static juce::String getPlatformName(Platform platform);

    /**
     * @brief Create predefined pattern
     * @param type Feedback type to create pattern for
     * @return Predefined pattern
     */
    static HapticPattern createPredefinedPattern(FeedbackType type);

    // State Management
    /**
     * @brief Save haptic settings to component state
     * @param state Component state to save to
     */
    void saveToState(ComponentState& state) const;

    /**
     * @brief Load haptic settings from component state
     * @param state Component state to load from
     */
    void loadFromState(const ComponentState& state);

    // Performance Monitoring
    /**
     * @brief Get feedback statistics
     * @return Performance and usage statistics
     */
    struct FeedbackStats {
        int totalFeedbacksTriggered = 0;
        int successfulFeedbacks = 0;
        int fallbacksUsed = 0;
        float averageLatency = 0.0f;
        int activeFeedbacks = 0;
        juce::uint32 lastFeedbackTime = 0;
    };

    /**
     * @brief Get feedback statistics
     * @return Current statistics
     */
    const FeedbackStats& getStatistics() const { return stats; }

    /**
     * @brief Reset feedback statistics
     */
    void resetStatistics();

    // Listeners
    /**
     * @class Listener
     * @brief Interface for haptic feedback event notifications
     */
    class Listener {
    public:
        virtual ~Listener() = default;
        
        /**
         * @brief Called when feedback is triggered
         * @param type Type of feedback triggered
         * @param intensity Actual intensity used
         * @param success Whether feedback was successful
         */
        virtual void feedbackTriggered(FeedbackType type, float intensity, bool success) {}
        
        /**
         * @brief Called when fallback is used
         * @param type Type of feedback that triggered fallback
         * @param fallbackMode Fallback mode used
         */
        virtual void fallbackUsed(FeedbackType type, FallbackMode fallbackMode) {}
        
        /**
         * @brief Called when settings change
         * @param newSettings New settings applied
         */
        virtual void settingsChanged(const FeedbackSettings& newSettings) {}
    };

    /**
     * @brief Add feedback event listener
     * @param listener Listener to add
     */
    void addListener(Listener* listener);

    /**
     * @brief Remove feedback event listener
     * @param listener Listener to remove
     */
    void removeListener(Listener* listener);

private:
    HapticFeedback();
    ~HapticFeedback();

    // Platform-specific implementations
    bool triggerPlatformFeedback(FeedbackType type, float intensity);
    bool triggerIOSFeedback(FeedbackType type, float intensity);
    bool triggerAndroidFeedback(FeedbackType type, float intensity);
    bool triggerMacOSFeedback(FeedbackType type, float intensity);
    bool triggerWindowsFeedback(FeedbackType type, float intensity);
    bool triggerLinuxFeedback(FeedbackType type, float intensity);

    // Pattern playback
    bool playCustomPattern(const HapticPattern& pattern, float intensity);
    void schedulePatternStep(const HapticPattern& pattern, int stepIndex, float intensity);

    // Initialization
    void initializePlatform();
    void detectCapabilities();
    void loadPredefinedPatterns();

    // Utility methods
    void notifyListeners(std::function<void(Listener*)> callback);
    float calculateEffectiveIntensity(float requestedIntensity) const;
    bool shouldUseFallback() const;
    void updateStatistics(bool success, float latency);

    // Audio fallback implementation
    void initializeAudioFallback();
    void playFeedbackSound(FeedbackType type, float intensity);

    // Visual fallback implementation
    void initializeVisualFallback();
    void showVisualFeedback(FeedbackType type, float intensity, juce::Component* component);

    // Member variables
    FeedbackSettings settings;
    FeedbackCapabilities capabilities;
    Platform currentPlatform = Platform::Unsupported;
    FeedbackStats stats;

    // Pattern storage
    std::map<juce::String, HapticPattern> customPatterns;
    std::map<FeedbackType, HapticPattern> predefinedPatterns;

    // Active feedback tracking
    std::vector<std::pair<FeedbackType, juce::uint32>> activeFeedbacks;
    juce::CriticalSection feedbackLock;

    // Fallback resources
    std::unique_ptr<juce::AudioFormatManager> audioFormatManager;
    std::map<FeedbackType, std::unique_ptr<juce::AudioFormatReaderSource>> audioSources;
    std::unique_ptr<juce::AudioSourcePlayer> audioPlayer;
    std::unique_ptr<juce::AudioDeviceManager> audioDeviceManager;

    // Visual feedback
    std::unique_ptr<juce::Component> visualFeedbackOverlay;
    std::unique_ptr<juce::ComponentAnimator> visualAnimator;

    // Listeners
    juce::ListenerList<Listener> listeners;

    // Platform-specific data
    #if JUCE_IOS
    void* hapticEngine = nullptr;  // CHHapticEngine*
    #elif JUCE_ANDROID
    void* vibrator = nullptr;      // Android Vibrator
    #elif JUCE_MAC
    bool forceTouch = false;       // Force Touch availability
    #elif JUCE_WINDOWS
    void* hapticDevice = nullptr;  // Windows Haptic Device
    #endif

    // Constants
    static constexpr int MAX_PATTERN_LENGTH = 2000;  // ms
    static constexpr int MIN_FEEDBACK_INTERVAL = 10; // ms
    static constexpr float DEFAULT_INTENSITY = 0.7f;
    static constexpr int FEEDBACK_TIMEOUT = 5000;    // ms

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HapticFeedback)
};

/**
 * @class HapticFeedbackComponent
 * @brief Component wrapper for easy haptic feedback integration
 */
class HapticFeedbackComponent : public juce::Component,
                               public HapticFeedback::Listener
{
public:
    HapticFeedbackComponent();
    ~HapticFeedbackComponent() override;

    // Component overrides
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;

    // Configuration
    void setFeedbackType(HapticFeedback::FeedbackType type);
    HapticFeedback::FeedbackType getFeedbackType() const { return feedbackType; }
    
    void setTriggerOnMouseDown(bool enabled);
    bool isTriggerOnMouseDown() const { return triggerOnMouseDown; }
    
    void setTriggerOnMouseUp(bool enabled);
    bool isTriggerOnMouseUp() const { return triggerOnMouseUp; }
    
    void setTriggerOnHover(bool enabled);
    bool isTriggerOnHover() const { return triggerOnHover; }

    // HapticFeedback::Listener overrides
    void feedbackTriggered(HapticFeedback::FeedbackType type, float intensity, bool success) override;

private:
    HapticFeedback::FeedbackType feedbackType = HapticFeedback::FeedbackType::Light;
    bool triggerOnMouseDown = true;
    bool triggerOnMouseUp = false;
    bool triggerOnHover = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HapticFeedbackComponent)
};

} // namespace Feedback
} // namespace UI
} // namespace OTTO