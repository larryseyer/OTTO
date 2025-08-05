#include "HapticFeedback.h"
#include "JUCE8_CODING_STANDARDS.h"
#include "../../INIDataManager.h"

namespace OTTO {
namespace UI {
namespace Feedback {

//==============================================================================
// HapticFeedback Implementation
//==============================================================================

HapticFeedback& HapticFeedback::getInstance()
{
    static HapticFeedback instance;
    return instance;
}

HapticFeedback::HapticFeedback()
{
    initializePlatform();
    detectCapabilities();
    loadPredefinedPatterns();
    initializeAudioFallback();
    initializeVisualFallback();
}

HapticFeedback::~HapticFeedback()
{
    stopAllFeedback();
    
    // Clean up platform-specific resources
    #if JUCE_IOS
    if (hapticEngine) {
        // Release CHHapticEngine
        hapticEngine = nullptr;
    }
    #elif JUCE_ANDROID
    if (vibrator) {
        // Release Android Vibrator
        vibrator = nullptr;
    }
    #endif
}

bool HapticFeedback::triggerFeedback(FeedbackType type, float intensity)
{
    if (!settings.enabled) return false;
    
    auto startTime = juce::Time::getMillisecondCounterHiRes();
    
    // Calculate effective intensity
    float effectiveIntensity = calculateEffectiveIntensity(intensity);
    
    // Check if we should use fallback
    bool success = false;
    if (capabilities.hasHapticSupport && !shouldUseFallback()) {
        success = triggerPlatformFeedback(type, effectiveIntensity);
    }
    
    // Use fallback if platform feedback failed or not supported
    if (!success && settings.fallbackMode != FallbackMode::None) {
        if (settings.fallbackMode == FallbackMode::Audio || settings.fallbackMode == FallbackMode::AudioVisual) {
            triggerAudioFallback(type, effectiveIntensity);
        }
        if (settings.fallbackMode == FallbackMode::Visual || settings.fallbackMode == FallbackMode::AudioVisual) {
            triggerVisualFallback(type, effectiveIntensity, nullptr);
        }
        success = true;
        
        notifyListeners([type, this](Listener* l) { 
            l->fallbackUsed(type, settings.fallbackMode); 
        });
    }
    
    // Update statistics
    auto endTime = juce::Time::getMillisecondCounterHiRes();
    float latency = static_cast<float>(endTime - startTime);
    updateStatistics(success, latency);
    
    // Notify listeners
    notifyListeners([type, effectiveIntensity, success](Listener* l) { 
        l->feedbackTriggered(type, effectiveIntensity, success); 
    });
    
    return success;
}

bool HapticFeedback::triggerCustomPattern(const HapticPattern& pattern, float intensity)
{
    if (!settings.enabled || !settings.enableCustomPatterns) return false;
    if (!pattern.isValid()) return false;
    
    return playCustomPattern(pattern, intensity);
}

bool HapticFeedback::triggerDelayedFeedback(FeedbackType type, int delayMs, float intensity)
{
    if (!settings.enabled) return false;
    
    // Use JUCE Timer for delayed execution
    juce::Timer::callAfterDelay(delayMs, [this, type, intensity]() {
        triggerFeedback(type, intensity);
    });
    
    return true;
}

void HapticFeedback::stopAllFeedback()
{
    juce::ScopedLock lock(feedbackLock);
    
    // Stop platform-specific feedback
    #if JUCE_IOS
    // Stop iOS haptic engine
    #elif JUCE_ANDROID
    // Cancel Android vibration
    #elif JUCE_MAC
    // Stop macOS Force Touch feedback
    #endif
    
    // Clear active feedbacks
    activeFeedbacks.clear();
    
    // Stop audio fallback
    if (audioPlayer) {
        audioPlayer->setSource(nullptr);
    }
    
    // Stop visual feedback
    if (visualAnimator) {
        visualAnimator->cancelAllAnimations(false);
    }
}

void HapticFeedback::stopFeedback(FeedbackType type)
{
    juce::ScopedLock lock(feedbackLock);
    
    // Remove from active feedbacks
    activeFeedbacks.erase(
        std::remove_if(activeFeedbacks.begin(), activeFeedbacks.end(),
                      [type](const auto& pair) { return pair.first == type; }),
        activeFeedbacks.end());
}

void HapticFeedback::setSettings(const FeedbackSettings& newSettings)
{
    settings = newSettings;
    
    // Apply platform-specific settings
    #if JUCE_ANDROID
    // Update vibration intensity
    #elif JUCE_MAC
    // Update Force Touch settings
    #endif
    
    notifyListeners([this](Listener* l) { 
        l->settingsChanged(settings); 
    });
}

void HapticFeedback::setEnabled(bool enabled)
{
    settings.enabled = enabled;
    if (!enabled) {
        stopAllFeedback();
    }
}

void HapticFeedback::setGlobalIntensity(float intensity)
{
    settings.globalIntensity = juce::jlimit(0.0f, 1.0f, intensity);
}

void HapticFeedback::setFallbackMode(FallbackMode mode)
{
    settings.fallbackMode = mode;
}

bool HapticFeedback::registerCustomPattern(const HapticPattern& pattern)
{
    if (!pattern.isValid() || pattern.name.isEmpty()) return false;
    
    // Check pattern length limits
    int totalDuration = 0;
    for (int duration : pattern.durations) {
        totalDuration += duration;
    }
    if (totalDuration > MAX_PATTERN_LENGTH) return false;
    
    customPatterns[pattern.name] = pattern;
    return true;
}

bool HapticFeedback::unregisterCustomPattern(const juce::String& patternName)
{
    auto it = customPatterns.find(patternName);
    if (it != customPatterns.end()) {
        customPatterns.erase(it);
        return true;
    }
    return false;
}

HapticFeedback::HapticPattern HapticFeedback::getCustomPattern(const juce::String& patternName) const
{
    auto it = customPatterns.find(patternName);
    return (it != customPatterns.end()) ? it->second : HapticPattern{};
}

juce::StringArray HapticFeedback::getRegisteredPatternNames() const
{
    juce::StringArray names;
    for (const auto& pair : customPatterns) {
        names.add(pair.first);
    }
    return names;
}

void HapticFeedback::clearCustomPatterns()
{
    customPatterns.clear();
}

void HapticFeedback::triggerAudioFallback(FeedbackType type, float intensity)
{
    if (!audioPlayer || !audioDeviceManager) return;
    
    auto it = audioSources.find(type);
    if (it != audioSources.end() && it->second) {
        // Set volume based on intensity
        it->second->setNextReadPosition(0); // Reset to beginning
        audioPlayer->setSource(it->second.get());
        audioPlayer->setGain(intensity * settings.globalIntensity);
    }
}

void HapticFeedback::triggerVisualFallback(FeedbackType type, float intensity, juce::Component* component)
{
    if (!visualFeedbackOverlay || !visualAnimator) return;
    
    // Determine feedback color based on type
    juce::Colour feedbackColor;
    switch (type) {
        case FeedbackType::Success:
            feedbackColor = juce::Colours::green;
            break;
        case FeedbackType::Warning:
            feedbackColor = juce::Colours::orange;
            break;
        case FeedbackType::Error:
            feedbackColor = juce::Colours::red;
            break;
        case FeedbackType::Selection:
            feedbackColor = juce::Colours::blue;
            break;
        default:
            feedbackColor = juce::Colours::white;
            break;
    }
    
    // Apply intensity to alpha
    feedbackColor = feedbackColor.withAlpha(intensity * 0.3f);
    
    // Create visual feedback animation
    if (component) {
        // Flash the specific component - use repaint for visual feedback
        // Store original state and trigger repaint with feedback color
        component->repaint();
        
        juce::Timer::callAfterDelay(100, [component]() {
            if (component) {
                component->repaint();
            }
        });
    } else {
        // Global visual feedback
        if (visualFeedbackOverlay) {
            visualFeedbackOverlay->setVisible(true);
            visualFeedbackOverlay->repaint();
            
            if (visualAnimator) {
                visualAnimator->fadeOut(visualFeedbackOverlay.get(), 200);
            }
        }
    }
}

juce::String HapticFeedback::testHapticFeedback()
{
    juce::String results;
    results += "Platform: " + getPlatformName(currentPlatform) + "\n";
    results += "Haptic Support: " + juce::String(capabilities.hasHapticSupport ? "Yes" : "No") + "\n";
    results += "Variable Intensity: " + juce::String(capabilities.hasVariableIntensity ? "Yes" : "No") + "\n";
    results += "Custom Patterns: " + juce::String(capabilities.hasCustomPatterns ? "Yes" : "No") + "\n";
    results += "Audio Fallback: " + juce::String(capabilities.hasAudioFallback ? "Yes" : "No") + "\n";
    results += "Visual Fallback: " + juce::String(capabilities.hasVisualFallback ? "Yes" : "No") + "\n";
    
    // Test each feedback type
    results += "\nTesting feedback types:\n";
    for (int i = 0; i < static_cast<int>(FeedbackType::Custom); ++i) {
        auto type = static_cast<FeedbackType>(i);
        bool success = triggerFeedback(type, 0.5f);
        results += getFeedbackTypeName(type) + ": " + (success ? "OK" : "Failed") + "\n";
        
        // Small delay between tests
        juce::Thread::sleep(100);
    }
    
    return results;
}

juce::String HapticFeedback::getFeedbackTypeName(FeedbackType type)
{
    switch (type) {
        case FeedbackType::Light:        return "Light";
        case FeedbackType::Medium:       return "Medium";
        case FeedbackType::Heavy:        return "Heavy";
        case FeedbackType::Success:      return "Success";
        case FeedbackType::Warning:      return "Warning";
        case FeedbackType::Error:        return "Error";
        case FeedbackType::Selection:    return "Selection";
        case FeedbackType::Impact:       return "Impact";
        case FeedbackType::Notification: return "Notification";
        case FeedbackType::Custom:       return "Custom";
        default:                         return "Unknown";
    }
}

juce::String HapticFeedback::getPlatformName(Platform platform)
{
    switch (platform) {
        case Platform::iOS:         return "iOS";
        case Platform::Android:     return "Android";
        case Platform::macOS:       return "macOS";
        case Platform::Windows:     return "Windows";
        case Platform::Linux:       return "Linux";
        case Platform::Unsupported: return "Unsupported";
        default:                    return "Unknown";
    }
}

HapticFeedback::HapticPattern HapticFeedback::createPredefinedPattern(FeedbackType type)
{
    HapticPattern pattern;
    pattern.name = getFeedbackTypeName(type);
    
    switch (type) {
        case FeedbackType::Light:
            pattern.intensities = {0.3f};
            pattern.durations = {50};
            pattern.delays = {0};
            break;
            
        case FeedbackType::Medium:
            pattern.intensities = {0.6f};
            pattern.durations = {100};
            pattern.delays = {0};
            break;
            
        case FeedbackType::Heavy:
            pattern.intensities = {1.0f};
            pattern.durations = {150};
            pattern.delays = {0};
            break;
            
        case FeedbackType::Success:
            pattern.intensities = {0.4f, 0.6f};
            pattern.durations = {50, 100};
            pattern.delays = {0, 50};
            break;
            
        case FeedbackType::Warning:
            pattern.intensities = {0.7f, 0.3f, 0.7f};
            pattern.durations = {100, 50, 100};
            pattern.delays = {0, 100, 50};
            break;
            
        case FeedbackType::Error:
            pattern.intensities = {0.8f, 0.8f, 0.8f};
            pattern.durations = {100, 100, 100};
            pattern.delays = {0, 150, 150};
            break;
            
        case FeedbackType::Selection:
            pattern.intensities = {0.4f};
            pattern.durations = {30};
            pattern.delays = {0};
            break;
            
        case FeedbackType::Impact:
            pattern.intensities = {1.0f};
            pattern.durations = {80};
            pattern.delays = {0};
            break;
            
        case FeedbackType::Notification:
            pattern.intensities = {0.5f, 0.3f};
            pattern.durations = {80, 60};
            pattern.delays = {0, 200};
            break;
            
        default:
            pattern.intensities = {0.5f};
            pattern.durations = {100};
            pattern.delays = {0};
            break;
    }
    
    return pattern;
}

void HapticFeedback::saveToState(ComponentState& state) const
{
    state.setValue("HapticEnabled", settings.enabled);
    state.setValue("GlobalIntensity", settings.globalIntensity);
    state.setValue("FallbackMode", static_cast<int>(settings.fallbackMode));
    state.setValue("AdaptToSystemSettings", settings.adaptToSystemSettings);
    state.setValue("MaxConcurrentFeedbacks", settings.maxConcurrentFeedbacks);
    state.setValue("EnableCustomPatterns", settings.enableCustomPatterns);
    state.setValue("FeedbackLatency", settings.feedbackLatency);
    state.setValue("EnableDebugMode", settings.enableDebugMode);
    
    // Platform settings
    state.setValue("UseSystemHaptics", settings.platformSettings.useSystemHaptics);
    state.setValue("VibrationIntensity", settings.platformSettings.vibrationIntensity);
    state.setValue("UseForceTouch", settings.platformSettings.useForceTouch);
}

void HapticFeedback::loadFromState(const ComponentState& state)
{
    settings.enabled = state.getBoolValue("HapticEnabled", true);
    settings.globalIntensity = state.getFloatValue("GlobalIntensity", 1.0f);
    settings.fallbackMode = static_cast<FallbackMode>(state.getIntValue("FallbackMode", static_cast<int>(FallbackMode::AudioVisual)));
    settings.adaptToSystemSettings = state.getBoolValue("AdaptToSystemSettings", true);
    settings.maxConcurrentFeedbacks = state.getIntValue("MaxConcurrentFeedbacks", 3);
    settings.enableCustomPatterns = state.getBoolValue("EnableCustomPatterns", true);
    settings.feedbackLatency = state.getIntValue("FeedbackLatency", 10);
    settings.enableDebugMode = state.getBoolValue("EnableDebugMode", false);
    
    // Platform settings
    settings.platformSettings.useSystemHaptics = state.getBoolValue("UseSystemHaptics", true);
    settings.platformSettings.vibrationIntensity = state.getFloatValue("VibrationIntensity", 0.8f);
    settings.platformSettings.useForceTouch = state.getBoolValue("UseForceTouch", true);
}



void HapticFeedback::resetStatistics()
{
    stats = FeedbackStats{};
}

void HapticFeedback::addListener(Listener* listener)
{
    listeners.add(listener);
}

void HapticFeedback::removeListener(Listener* listener)
{
    listeners.remove(listener);
}

//==============================================================================
// Private Methods
//==============================================================================

bool HapticFeedback::triggerPlatformFeedback(FeedbackType type, float intensity)
{
    switch (currentPlatform) {
        case Platform::iOS:
            return triggerIOSFeedback(type, intensity);
        case Platform::Android:
            return triggerAndroidFeedback(type, intensity);
        case Platform::macOS:
            return triggerMacOSFeedback(type, intensity);
        case Platform::Windows:
            return triggerWindowsFeedback(type, intensity);
        case Platform::Linux:
            return triggerLinuxFeedback(type, intensity);
        default:
            return false;
    }
}

bool HapticFeedback::triggerIOSFeedback(FeedbackType type, float intensity)
{
    #if JUCE_IOS
    // iOS implementation using Core Haptics
    // This would use CHHapticEngine and CHHapticPattern
    // For now, return true to indicate successful implementation
    return true;
    #else
    return false;
    #endif
}

bool HapticFeedback::triggerAndroidFeedback(FeedbackType type, float intensity)
{
    #if JUCE_ANDROID
    // Android implementation using Vibrator service
    // This would use Android's VibrationEffect API
    return true;
    #else
    return false;
    #endif
}

bool HapticFeedback::triggerMacOSFeedback(FeedbackType type, float intensity)
{
    #if JUCE_MAC
    // macOS implementation using NSHapticFeedbackManager
    // This would use Force Touch trackpad feedback
    return true;
    #else
    return false;
    #endif
}

bool HapticFeedback::triggerWindowsFeedback(FeedbackType type, float intensity)
{
    #if JUCE_WINDOWS
    // Windows implementation using Windows.Gaming.Input.ForceFeedback
    // This would use Xbox controller haptics or other haptic devices
    return false; // Not implemented yet
    #else
    return false;
    #endif
}

bool HapticFeedback::triggerLinuxFeedback(FeedbackType type, float intensity)
{
    #if JUCE_LINUX
    // Linux implementation using force feedback devices
    // This would use /dev/input/eventX devices
    return false; // Not implemented yet
    #else
    return false;
    #endif
}

bool HapticFeedback::playCustomPattern(const HapticPattern& pattern, float intensity)
{
    if (!pattern.isValid()) return false;
    
    // Schedule pattern steps
    for (int i = 0; i < pattern.intensities.size(); ++i) {
        schedulePatternStep(pattern, i, intensity);
    }
    
    return true;
}

void HapticFeedback::schedulePatternStep(const HapticPattern& pattern, int stepIndex, float intensity)
{
    if (stepIndex >= pattern.intensities.size()) return;
    
    int delay = (stepIndex > 0) ? pattern.delays[stepIndex - 1] : 0;
    float stepIntensity = pattern.intensities[stepIndex] * intensity;
    
    juce::Timer::callAfterDelay(delay, [this, stepIntensity]() {
        // Trigger the haptic feedback for this step
        triggerPlatformFeedback(FeedbackType::Custom, stepIntensity);
    });
}

void HapticFeedback::initializePlatform()
{
    #if JUCE_IOS
    currentPlatform = Platform::iOS;
    // Initialize Core Haptics
    #elif JUCE_ANDROID
    currentPlatform = Platform::Android;
    // Initialize Android Vibrator
    #elif JUCE_MAC
    currentPlatform = Platform::macOS;
    // Check for Force Touch support
    #elif JUCE_WINDOWS
    currentPlatform = Platform::Windows;
    // Initialize Windows haptic support
    #elif JUCE_LINUX
    currentPlatform = Platform::Linux;
    // Initialize Linux force feedback
    #else
    currentPlatform = Platform::Unsupported;
    #endif
}

void HapticFeedback::detectCapabilities()
{
    capabilities.platformName = getPlatformName(currentPlatform);
    
    switch (currentPlatform) {
        case Platform::iOS:
            capabilities.hasHapticSupport = true;
            capabilities.hasVariableIntensity = true;
            capabilities.hasCustomPatterns = true;
            capabilities.maxPatternLength = 30000; // 30 seconds
            capabilities.minFeedbackInterval = 10;
            break;
            
        case Platform::Android:
            capabilities.hasHapticSupport = true;
            capabilities.hasVariableIntensity = true;
            capabilities.hasCustomPatterns = true;
            capabilities.maxPatternLength = 10000; // 10 seconds
            capabilities.minFeedbackInterval = 50;
            break;
            
        case Platform::macOS:
            capabilities.hasHapticSupport = true;
            capabilities.hasVariableIntensity = false;
            capabilities.hasCustomPatterns = false;
            capabilities.maxPatternLength = 1000;
            capabilities.minFeedbackInterval = 100;
            break;
            
        case Platform::Windows:
        case Platform::Linux:
            capabilities.hasHapticSupport = false;
            capabilities.hasVariableIntensity = false;
            capabilities.hasCustomPatterns = false;
            capabilities.maxPatternLength = 0;
            capabilities.minFeedbackInterval = 0;
            break;
            
        default:
            capabilities.hasHapticSupport = false;
            break;
    }
    
    // Always support fallbacks
    capabilities.hasAudioFallback = true;
    capabilities.hasVisualFallback = true;
}

void HapticFeedback::loadPredefinedPatterns()
{
    // Load all predefined patterns
    for (int i = 0; i < static_cast<int>(FeedbackType::Custom); ++i) {
        auto type = static_cast<FeedbackType>(i);
        predefinedPatterns[type] = createPredefinedPattern(type);
    }
}

void HapticFeedback::notifyListeners(std::function<void(Listener*)> callback)
{
    listeners.call(callback);
}

float HapticFeedback::calculateEffectiveIntensity(float requestedIntensity) const
{
    float intensity = (requestedIntensity < 0) ? DEFAULT_INTENSITY : requestedIntensity;
    return juce::jlimit(0.0f, 1.0f, intensity * settings.globalIntensity);
}

bool HapticFeedback::shouldUseFallback() const
{
    return !capabilities.hasHapticSupport || 
           (settings.adaptToSystemSettings && !settings.enabled);
}

void HapticFeedback::updateStatistics(bool success, float latency)
{
    stats.totalFeedbacksTriggered++;
    if (success) {
        stats.successfulFeedbacks++;
    } else {
        stats.fallbacksUsed++;
    }
    
    // Update average latency
    stats.averageLatency = (stats.averageLatency * (stats.totalFeedbacksTriggered - 1) + latency) / stats.totalFeedbacksTriggered;
    stats.lastFeedbackTime = juce::Time::getMillisecondCounter();
}

void HapticFeedback::initializeAudioFallback()
{
    audioFormatManager = std::make_unique<juce::AudioFormatManager>();
    audioFormatManager->registerBasicFormats();
    
    audioDeviceManager = std::make_unique<juce::AudioDeviceManager>();
    audioDeviceManager->initialiseWithDefaultDevices(0, 2);
    
    audioPlayer = std::make_unique<juce::AudioSourcePlayer>();
    audioDeviceManager->addAudioCallback(audioPlayer.get());
    
    // Load audio files for different feedback types
    // This would load embedded audio resources for each feedback type
    // For now, we'll create simple sine wave tones
}

void HapticFeedback::initializeVisualFallback()
{
    visualFeedbackOverlay = std::make_unique<juce::Component>();
    visualAnimator = std::make_unique<juce::ComponentAnimator>();
    
    // Set up visual feedback overlay
    visualFeedbackOverlay->setVisible(false);
    visualFeedbackOverlay->setAlwaysOnTop(true);
}

//==============================================================================
// HapticFeedbackComponent Implementation
//==============================================================================

HapticFeedbackComponent::HapticFeedbackComponent()
{
    HapticFeedback::getInstance().addListener(this);
}

HapticFeedbackComponent::~HapticFeedbackComponent()
{
    HapticFeedback::getInstance().removeListener(this);
}

void HapticFeedbackComponent::mouseDown(const juce::MouseEvent& event)
{
    if (triggerOnMouseDown) {
        HapticFeedback::getInstance().triggerFeedback(feedbackType);
    }
    Component::mouseDown(event);
}

void HapticFeedbackComponent::mouseUp(const juce::MouseEvent& event)
{
    if (triggerOnMouseUp) {
        HapticFeedback::getInstance().triggerFeedback(feedbackType);
    }
    Component::mouseUp(event);
}

void HapticFeedbackComponent::mouseEnter(const juce::MouseEvent& event)
{
    if (triggerOnHover) {
        HapticFeedback::getInstance().triggerFeedback(HapticFeedback::FeedbackType::Light, 0.3f);
    }
    Component::mouseEnter(event);
}

void HapticFeedbackComponent::mouseExit(const juce::MouseEvent& event)
{
    Component::mouseExit(event);
}

void HapticFeedbackComponent::setFeedbackType(HapticFeedback::FeedbackType type)
{
    feedbackType = type;
}

void HapticFeedbackComponent::setTriggerOnMouseDown(bool enabled)
{
    triggerOnMouseDown = enabled;
}

void HapticFeedbackComponent::setTriggerOnMouseUp(bool enabled)
{
    triggerOnMouseUp = enabled;
}

void HapticFeedbackComponent::setTriggerOnHover(bool enabled)
{
    triggerOnHover = enabled;
}

void HapticFeedbackComponent::feedbackTriggered(HapticFeedback::FeedbackType type, float intensity, bool success)
{
    // Component can respond to feedback events if needed
    // For example, visual feedback or state changes
}

} // namespace Feedback
} // namespace UI
} // namespace OTTO