#pragma once

#include "JUCE8_CODING_STANDARDS.h"
#include <JuceHeader.h>
#include "../../ColorScheme.h"
#include "../../INIConfig.h"
#include "../../ComponentState.h"

/**
 * @file TouchOptimizedControls.h
 * @brief Touch-friendly UI controls optimized for mobile and tablet interfaces
 * 
 * This component library provides touch-optimized controls designed for
 * professional audio applications on mobile devices. All controls follow
 * accessibility guidelines and provide haptic feedback integration.
 * 
 * FEATURES:
 * - Touch-friendly sizing (44px minimum touch targets)
 * - Gesture recognition for advanced interactions
 * - Haptic feedback integration for tactile response
 * - Adaptive sizing based on screen density
 * - Multi-touch support for complex gestures
 * - Accessibility compliance (WCAG 2.1 AA)
 * - Platform-specific optimizations
 * 
 * TECHNICAL SPECIFICATIONS:
 * - Minimum touch target: 44x44 pixels (iOS/Android guidelines)
 * - Touch tolerance: 8px margin around controls
 * - Gesture recognition: Tap, long press, swipe, pinch, rotate
 * - Haptic patterns: Light, medium, heavy feedback
 * - Screen density adaptation: 1x, 2x, 3x scaling
 * - Multi-touch: Up to 10 simultaneous touch points
 * 
 * INTEGRATION:
 * - Uses ColorScheme for consistent theming
 * - INI-driven configuration for all parameters
 * - Integrates with HapticFeedback system
 * - Supports ComponentState for persistence
 * 
 * @author OTTO Development Team
 * @version 1.0
 * @date 2025
 */

namespace OTTO {
namespace UI {
namespace Controls {

/**
 * @class TouchOptimizedButton
 * @brief Touch-friendly button with haptic feedback and gesture support
 */
class TouchOptimizedButton : public juce::Button
{
public:
    /**
     * @enum HapticStyle
     * @brief Haptic feedback intensity levels
     */
    enum class HapticStyle {
        None,       ///< No haptic feedback
        Light,      ///< Light haptic feedback
        Medium,     ///< Medium haptic feedback
        Heavy       ///< Heavy haptic feedback
    };

    /**
     * @enum TouchState
     * @brief Touch interaction states
     */
    enum class TouchState {
        Normal,     ///< Normal state
        Hover,      ///< Hover/proximity state
        Pressed,    ///< Pressed state
        Disabled    ///< Disabled state
    };

    TouchOptimizedButton(const juce::String& buttonName = {});
    ~TouchOptimizedButton() override;

    // Button overrides
    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;

    // Touch Configuration
    void setMinimumTouchTarget(int size);
    int getMinimumTouchTarget() const { return minimumTouchTarget; }
    
    void setTouchTolerance(int tolerance);
    int getTouchTolerance() const { return touchTolerance; }
    
    void setHapticStyle(HapticStyle style);
    HapticStyle getHapticStyle() const { return hapticStyle; }
    
    void setLongPressEnabled(bool enabled);
    bool isLongPressEnabled() const { return longPressEnabled; }
    
    void setLongPressDelay(int milliseconds);
    int getLongPressDelay() const { return longPressDelay; }

    // Visual Configuration
    void setCornerRadius(float radius);
    float getCornerRadius() const { return cornerRadius; }
    
    void setGradientEnabled(bool enabled);
    bool isGradientEnabled() const { return gradientEnabled; }
    
    void setShadowEnabled(bool enabled);
    bool isShadowEnabled() const { return shadowEnabled; }

    // State Management
    TouchState getCurrentTouchState() const { return currentTouchState; }
    bool isTouchActive() const { return touchActive; }

    // Callbacks
    std::function<void()> onLongPress;
    std::function<void(const juce::Point<float>&)> onTouchMove;
    std::function<void()> onTouchCancel;

private:
    void updateTouchState(TouchState newState);
    void triggerHapticFeedback();
    void startLongPressTimer();
    void stopLongPressTimer();
    juce::Rectangle<int> getExpandedBounds() const;

    // Touch Properties
    int minimumTouchTarget = 44;
    int touchTolerance = 8;
    HapticStyle hapticStyle = HapticStyle::Light;
    bool longPressEnabled = false;
    int longPressDelay = 500;

    // Visual Properties
    float cornerRadius = 8.0f;
    bool gradientEnabled = true;
    bool shadowEnabled = true;

    // State
    TouchState currentTouchState = TouchState::Normal;
    bool touchActive = false;
    juce::Point<int> touchStartPosition;
    
    // Timing
    std::unique_ptr<juce::Timer> longPressTimer;
    juce::uint32 touchStartTime = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TouchOptimizedButton)
};

/**
 * @class TouchOptimizedSlider
 * @brief Touch-friendly slider with gesture support and haptic feedback
 */
class TouchOptimizedSlider : public juce::Slider
{
public:
    /**
     * @enum GestureMode
     * @brief Gesture interaction modes
     */
    enum class GestureMode {
        Standard,   ///< Standard drag interaction
        Circular,   ///< Circular/rotary gesture
        Vertical,   ///< Vertical-only gesture
        Horizontal  ///< Horizontal-only gesture
    };

    TouchOptimizedSlider(SliderStyle style = LinearHorizontal, TextEntryBoxPosition textBoxPosition = NoTextBox);
    ~TouchOptimizedSlider() override;

    // Slider overrides
    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;

    // Touch Configuration
    void setGestureMode(GestureMode mode);
    GestureMode getGestureMode() const { return gestureMode; }
    
    void setTouchSensitivity(float sensitivity);
    float getTouchSensitivity() const { return touchSensitivity; }
    
    void setSnapToValues(bool enabled);
    bool isSnapToValuesEnabled() const { return snapToValues; }
    
    void setHapticFeedbackEnabled(bool enabled);
    bool isHapticFeedbackEnabled() const { return hapticFeedbackEnabled; }

    // Visual Configuration
    void setThumbSize(float size);
    float getThumbSize() const { return thumbSize; }
    
    void setTrackThickness(float thickness);
    float getTrackThickness() const { return trackThickness; }
    
    void setShowValueTooltip(bool show);
    bool isShowValueTooltip() const { return showValueTooltip; }

    // Gesture Recognition
    void enablePinchZoom(bool enabled);
    bool isPinchZoomEnabled() const { return pinchZoomEnabled; }
    
    void enableRotaryGesture(bool enabled);
    bool isRotaryGestureEnabled() const { return rotaryGestureEnabled; }

private:
    void updateFromGesture(const juce::MouseEvent& event);
    void handleCircularGesture(const juce::MouseEvent& event);
    void handleLinearGesture(const juce::MouseEvent& event);
    void showValueTooltip(const juce::Point<int>& position);
    void hideValueTooltip();
    void triggerHapticFeedback(float intensity);

    // Gesture Properties
    GestureMode gestureMode = GestureMode::Standard;
    float touchSensitivity = 1.0f;
    bool snapToValues = false;
    bool hapticFeedbackEnabled = true;

    // Visual Properties
    float thumbSize = 20.0f;
    float trackThickness = 4.0f;
    bool showValueTooltip = true;

    // Gesture Recognition
    bool pinchZoomEnabled = false;
    bool rotaryGestureEnabled = false;
    
    // State
    juce::Point<float> lastTouchPosition;
    float gestureStartValue = 0.0f;
    float gestureStartAngle = 0.0f;
    bool isDragging = false;
    
    // Tooltip
    std::unique_ptr<juce::Component> valueTooltip;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TouchOptimizedSlider)
};

/**
 * @class TouchOptimizedKnob
 * @brief Touch-friendly rotary knob with circular gesture support
 */
class TouchOptimizedKnob : public juce::Component
{
public:
    TouchOptimizedKnob();
    ~TouchOptimizedKnob() override;

    // Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;

    // Value Management
    void setValue(double newValue, juce::NotificationType notification = juce::sendNotificationAsync);
    double getValue() const { return currentValue; }
    
    void setRange(double minimum, double maximum, double interval = 0.0);
    juce::Range<double> getRange() const { return valueRange; }
    
    void setDefaultValue(double defaultVal);
    double getDefaultValue() const { return defaultValue; }

    // Visual Configuration
    void setKnobStyle(int style);
    int getKnobStyle() const { return knobStyle; }
    
    void setArcThickness(float thickness);
    float getArcThickness() const { return arcThickness; }
    
    void setShowValueLabel(bool show);
    bool isShowValueLabel() const { return showValueLabel; }

    // Touch Configuration
    void setRotationSensitivity(float sensitivity);
    float getRotationSensitivity() const { return rotationSensitivity; }
    
    void setSnapToCenter(bool enabled);
    bool isSnapToCenter() const { return snapToCenter; }

    // Callbacks
    std::function<void(double)> onValueChange;
    std::function<juce::String(double)> valueToStringFunction;

private:
    void updateValueFromAngle(float angle);
    float valueToAngle(double value) const;
    double angleToValue(float angle) const;
    void triggerValueChanged();
    juce::Point<float> getKnobCenter() const;
    float getKnobRadius() const;

    // Value Properties
    double currentValue = 0.0;
    juce::Range<double> valueRange{0.0, 1.0};
    double interval = 0.0;
    double defaultValue = 0.0;

    // Visual Properties
    int knobStyle = 0;
    float arcThickness = 3.0f;
    bool showValueLabel = true;
    
    // Touch Properties
    float rotationSensitivity = 1.0f;
    bool snapToCenter = false;
    
    // State
    bool isDragging = false;
    juce::Point<float> dragStartPosition;
    double dragStartValue = 0.0;
    float startAngle = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TouchOptimizedKnob)
};

/**
 * @class TouchOptimizedToggle
 * @brief Touch-friendly toggle switch with smooth animations
 */
class TouchOptimizedToggle : public juce::ToggleButton
{
public:
    /**
     * @enum ToggleStyle
     * @brief Visual styles for toggle switches
     */
    enum class ToggleStyle {
        Switch,     ///< iOS-style switch
        Checkbox,   ///< Traditional checkbox
        Button,     ///< Button-style toggle
        Slider      ///< Slider-style toggle
    };

    TouchOptimizedToggle(const juce::String& buttonText = {});
    ~TouchOptimizedToggle() override;

    // ToggleButton overrides
    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;

    // Configuration
    void setToggleStyle(ToggleStyle style);
    ToggleStyle getToggleStyle() const { return toggleStyle; }
    
    void setAnimationDuration(int milliseconds);
    int getAnimationDuration() const { return animationDuration; }
    
    void setSwipeToToggle(bool enabled);
    bool isSwipeToToggle() const { return swipeToToggle; }

private:
    void startAnimation();
    void updateAnimation();
    float getAnimationProgress() const;

    // Style Properties
    ToggleStyle toggleStyle = ToggleStyle::Switch;
    int animationDuration = 200;
    bool swipeToToggle = true;
    
    // Animation State
    bool isAnimating = false;
    juce::uint32 animationStartTime = 0;
    bool animationTargetState = false;
    
    // Interaction State
    juce::Point<int> dragStartPosition;
    bool isDragToggle = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TouchOptimizedToggle)
};

/**
 * @class TouchGestureRecognizer
 * @brief Advanced gesture recognition for complex touch interactions
 */
class TouchGestureRecognizer : public juce::Component
{
public:
    /**
     * @enum GestureType
     * @brief Types of recognized gestures
     */
    enum class GestureType {
        None,
        Tap,
        DoubleTap,
        LongPress,
        Swipe,
        Pinch,
        Rotate,
        Pan
    };

    /**
     * @enum SwipeDirection
     * @brief Swipe gesture directions
     */
    enum class SwipeDirection {
        None,
        Up,
        Down,
        Left,
        Right
    };

    /**
     * @struct GestureData
     * @brief Data associated with a recognized gesture
     */
    struct GestureData {
        GestureType type = GestureType::None;
        juce::Point<float> position;
        juce::Point<float> velocity;
        float scale = 1.0f;
        float rotation = 0.0f;
        SwipeDirection swipeDirection = SwipeDirection::None;
        int touchCount = 0;
        juce::uint32 timestamp = 0;
    };

    TouchGestureRecognizer();
    ~TouchGestureRecognizer() override;

    // Component overrides
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;

    // Configuration
    void setTapThreshold(float threshold);
    float getTapThreshold() const { return tapThreshold; }
    
    void setSwipeThreshold(float threshold);
    float getSwipeThreshold() const { return swipeThreshold; }
    
    void setLongPressDelay(int milliseconds);
    int getLongPressDelay() const { return longPressDelay; }
    
    void setPinchThreshold(float threshold);
    float getPinchThreshold() const { return pinchThreshold; }

    // Gesture Callbacks
    std::function<void(const GestureData&)> onGestureRecognized;
    std::function<void(const GestureData&)> onGestureUpdate;
    std::function<void(const GestureData&)> onGestureEnd;

private:
    void processGesture();
    void recognizeTap();
    void recognizeSwipe();
    void recognizePinch();
    void recognizeRotation();
    void startLongPressTimer();
    void stopLongPressTimer();
    float calculateDistance(const juce::Point<float>& p1, const juce::Point<float>& p2) const;
    float calculateAngle(const juce::Point<float>& p1, const juce::Point<float>& p2) const;

    // Thresholds
    float tapThreshold = 10.0f;
    float swipeThreshold = 50.0f;
    int longPressDelay = 500;
    float pinchThreshold = 20.0f;
    
    // State
    std::vector<juce::Point<float>> touchPoints;
    std::vector<juce::uint32> touchTimes;
    juce::Point<float> initialTouchPosition;
    juce::uint32 initialTouchTime = 0;
    bool isTracking = false;
    
    // Gesture Recognition
    GestureData currentGesture;
    std::unique_ptr<juce::Timer> longPressTimer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TouchGestureRecognizer)
};

/**
 * @class TouchOptimizedControlsManager
 * @brief Manager class for coordinating touch-optimized controls
 */
class TouchOptimizedControlsManager
{
public:
    /**
     * @struct TouchSettings
     * @brief Global touch interaction settings
     */
    struct TouchSettings {
        int minimumTouchTarget = 44;        ///< Minimum touch target size
        int touchTolerance = 8;             ///< Touch tolerance margin
        float touchSensitivity = 1.0f;      ///< Global touch sensitivity
        bool hapticFeedbackEnabled = true;  ///< Enable haptic feedback
        bool gestureRecognitionEnabled = true; ///< Enable gesture recognition
        int longPressDelay = 500;           ///< Long press delay in ms
        float swipeThreshold = 50.0f;       ///< Swipe distance threshold
        bool adaptToScreenDensity = true;   ///< Adapt to screen DPI
    };

    static TouchOptimizedControlsManager& getInstance();
    
    // Configuration
    void setTouchSettings(const TouchSettings& settings);
    const TouchSettings& getTouchSettings() const { return touchSettings; }
    
    void setScreenDensity(float density);
    float getScreenDensity() const { return screenDensity; }
    
    void setHapticFeedbackEnabled(bool enabled);
    bool isHapticFeedbackEnabled() const { return touchSettings.hapticFeedbackEnabled; }

    // Utility Functions
    int getScaledTouchTarget(int baseSize) const;
    float getScaledSensitivity(float baseSensitivity) const;
    juce::Rectangle<int> expandBoundsForTouch(const juce::Rectangle<int>& bounds) const;
    
    // Platform Detection
    bool isTouchDevice() const;
    bool isHighDensityScreen() const;
    juce::String getPlatformName() const;

    // State Management
    void saveToState(ComponentState& state) const;
    void loadFromState(const ComponentState& state);

private:
    TouchOptimizedControlsManager() = default;
    
    TouchSettings touchSettings;
    float screenDensity = 1.0f;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TouchOptimizedControlsManager)
};

} // namespace Controls
} // namespace UI
} // namespace OTTO