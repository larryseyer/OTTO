#include "TouchOptimizedControls.h"
#include "JUCE8_CODING_STANDARDS.h"
#include "../../INIDataManager.h"

//==============================================================================
// LongPressTimer Implementation
//==============================================================================

void LongPressTimer::timerCallback()
{
    stopTimer();
    if (owner.onLongPress) {
        owner.onLongPress();
    }
    owner.triggerHapticFeedback(); // Different haptic for long press
}

namespace OTTO {
namespace UI {
namespace Controls {

//==============================================================================
// TouchOptimizedButton Implementation
//==============================================================================

TouchOptimizedButton::TouchOptimizedButton(const juce::String& buttonName)
    : juce::Button(buttonName)
{
    // Set minimum size based on touch guidelines
    setSize(minimumTouchTarget, minimumTouchTarget);
    
    // Enable mouse events
    setMouseClickGrabsKeyboardFocus(false);
    setWantsKeyboardFocus(true);
    
    // Initialize long press timer
    longPressTimer = std::make_unique<juce::Timer>();
}

TouchOptimizedButton::~TouchOptimizedButton()
{
    stopLongPressTimer();
}

void TouchOptimizedButton::paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto bounds = getLocalBounds().toFloat();
    
    // Determine colors based on state
    juce::Colour baseColor, textColor;
    
    switch (currentTouchState) {
        case TouchState::Normal:
            baseColor = findColour(juce::TextButton::buttonColourId);
            textColor = findColour(juce::TextButton::textColourOffId);
            break;
            
        case TouchState::Hover:
            baseColor = findColour(juce::TextButton::buttonOnColourId).withAlpha(0.7f);
            textColor = findColour(juce::TextButton::textColourOnId);
            break;
            
        case TouchState::Pressed:
            baseColor = findColour(juce::TextButton::buttonOnColourId);
            textColor = findColour(juce::TextButton::textColourOnId);
            break;
            
        case TouchState::Disabled:
            baseColor = findColour(juce::TextButton::buttonColourId).withAlpha(0.3f);
            textColor = findColour(juce::TextButton::textColourOffId).withAlpha(0.3f);
            break;
    }
    
    // Draw shadow if enabled
    if (shadowEnabled && currentTouchState != TouchState::Disabled) {
        auto shadowBounds = bounds.translated(0, 2);
        g.setColour(juce::Colours::black.withAlpha(0.2f));
        g.fillRoundedRectangle(shadowBounds, cornerRadius);
    }
    
    // Draw gradient background if enabled
    if (gradientEnabled) {
        juce::ColourGradient gradient(baseColor.brighter(0.1f), bounds.getTopLeft(),
                                     baseColor.darker(0.1f), bounds.getBottomLeft(), false);
        g.setGradientFill(gradient);
    } else {
        g.setColour(baseColor);
    }
    
    g.fillRoundedRectangle(bounds, cornerRadius);
    
    // Draw border
    g.setColour(baseColor.contrasting(0.3f));
    g.drawRoundedRectangle(bounds, cornerRadius, 1.0f);
    
    // Draw button text
    g.setColour(textColor);
    g.setFont(JUCE8_FONT(14.0f));
    g.drawText(getButtonText(), bounds.toNearestInt(), juce::Justification::centred);
    
    // Draw touch indicator for active touches
    if (touchActive) {
        g.setColour(juce::Colours::white.withAlpha(0.3f));
        g.fillEllipse(bounds.getCentreX() - 5, bounds.getCentreY() - 5, 10, 10);
    }
}

void TouchOptimizedButton::mouseDown(const juce::MouseEvent& event)
{
    touchActive = true;
    touchStartPosition = event.getPosition();
    touchStartTime = juce::Time::getMillisecondCounter();
    
    updateTouchState(TouchState::Pressed);
    triggerHapticFeedback();
    
    if (longPressEnabled) {
        startLongPressTimer();
    }
    
    Button::mouseDown(event);
}

void TouchOptimizedButton::mouseUp(const juce::MouseEvent& event)
{
    touchActive = false;
    stopLongPressTimer();
    
    // Check if touch ended within bounds (including tolerance)
    auto expandedBounds = getExpandedBounds();
    bool withinBounds = expandedBounds.contains(event.getPosition());
    
    updateTouchState(withinBounds ? TouchState::Hover : TouchState::Normal);
    
    if (onTouchCancel && !withinBounds) {
        onTouchCancel();
    }
    
    Button::mouseUp(event);
}

void TouchOptimizedButton::mouseEnter(const juce::MouseEvent& event)
{
    if (!touchActive) {
        updateTouchState(TouchState::Hover);
    }
    Button::mouseEnter(event);
}

void TouchOptimizedButton::mouseExit(const juce::MouseEvent& event)
{
    if (!touchActive) {
        updateTouchState(TouchState::Normal);
    }
    Button::mouseExit(event);
}

void TouchOptimizedButton::mouseDrag(const juce::MouseEvent& event)
{
    if (onTouchMove) {
        onTouchMove(event.getPosition().toFloat());
    }
    
    // Check if drag is still within expanded bounds
    auto expandedBounds = getExpandedBounds();
    bool withinBounds = expandedBounds.contains(event.getPosition());
    
    updateTouchState(withinBounds ? TouchState::Pressed : TouchState::Normal);
    
    Button::mouseDrag(event);
}

void TouchOptimizedButton::setMinimumTouchTarget(int size)
{
    minimumTouchTarget = juce::jmax(44, size); // Enforce minimum 44px
    setSize(juce::jmax(getWidth(), minimumTouchTarget), 
            juce::jmax(getHeight(), minimumTouchTarget));
}

void TouchOptimizedButton::setTouchTolerance(int tolerance)
{
    touchTolerance = juce::jmax(0, tolerance);
}

void TouchOptimizedButton::setHapticStyle(HapticStyle style)
{
    hapticStyle = style;
}

void TouchOptimizedButton::setLongPressEnabled(bool enabled)
{
    longPressEnabled = enabled;
    if (!enabled) {
        stopLongPressTimer();
    }
}

void TouchOptimizedButton::setLongPressDelay(int milliseconds)
{
    longPressDelay = juce::jmax(100, milliseconds);
}

void TouchOptimizedButton::setCornerRadius(float radius)
{
    cornerRadius = juce::jmax(0.0f, radius);
    repaint();
}

void TouchOptimizedButton::setGradientEnabled(bool enabled)
{
    gradientEnabled = enabled;
    repaint();
}

void TouchOptimizedButton::setShadowEnabled(bool enabled)
{
    shadowEnabled = enabled;
    repaint();
}

void TouchOptimizedButton::updateTouchState(TouchState newState)
{
    if (currentTouchState != newState) {
        currentTouchState = newState;
        repaint();
    }
}

void TouchOptimizedButton::triggerHapticFeedback()
{
    if (hapticStyle == HapticStyle::None) return;
    
    // Platform-specific haptic feedback implementation would go here
    // For now, we'll use a simple visual feedback
    auto& manager = TouchOptimizedControlsManager::getInstance();
    if (manager.isHapticFeedbackEnabled()) {
        // Trigger haptic feedback through system
        // This would be implemented per platform
    }
}

void TouchOptimizedButton::startLongPressTimer()
{
    if (!longPressTimer) {
        longPressTimer = std::make_unique<LongPressTimer>(*this);
    }
    
    longPressTimer->startTimer(longPressDelay);
}

void TouchOptimizedButton::stopLongPressTimer()
{
    if (longPressTimer) {
        longPressTimer->stopTimer();
    }
}

juce::Rectangle<int> TouchOptimizedButton::getExpandedBounds() const
{
    return getLocalBounds().expanded(touchTolerance);
}

//==============================================================================
// TouchOptimizedSlider Implementation
//==============================================================================

TouchOptimizedSlider::TouchOptimizedSlider(SliderStyle style, TextEntryBoxPosition textBoxPosition)
    : juce::Slider()
{
    // Set slider style and text box position
    setSliderStyle(style);
    setTextBoxStyle(textBoxPosition, false, 80, 20);
    
    // Set minimum size for touch interaction
    setSize(100, 44); // Minimum touch-friendly size
    
    // Configure for touch interaction
    setMouseDragSensitivity(100); // Reduce sensitivity for touch
    setScrollWheelEnabled(true);
    setPopupDisplayEnabled(true, true, this);
}

TouchOptimizedSlider::~TouchOptimizedSlider()
{
    hideValueTooltip();
}

void TouchOptimizedSlider::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    auto trackBounds = bounds.reduced(thumbSize / 2);
    
    // Draw track
    g.setColour(findColour(juce::Slider::trackColourId));
    
    if (getSliderStyle() == LinearHorizontal) {
        auto trackY = bounds.getCentreY() - trackThickness / 2;
        g.fillRoundedRectangle(trackBounds.getX(), trackY, 
                              trackBounds.getWidth(), trackThickness, trackThickness / 2);
        
        // Draw filled portion
        auto fillWidth = (getValue() - getMinimum()) / (getMaximum() - getMinimum()) * trackBounds.getWidth();
        g.setColour(findColour(juce::Slider::thumbColourId));
        g.fillRoundedRectangle(trackBounds.getX(), trackY, 
                              fillWidth, trackThickness, trackThickness / 2);
        
        // Draw thumb
        auto thumbX = trackBounds.getX() + fillWidth - thumbSize / 2;
        auto thumbY = bounds.getCentreY() - thumbSize / 2;
        
        g.setColour(findColour(juce::Slider::thumbColourId));
        g.fillEllipse(thumbX, thumbY, thumbSize, thumbSize);
        
        // Draw thumb border
        g.setColour(findColour(juce::Slider::thumbColourId).contrasting(0.3f));
        g.drawEllipse(thumbX, thumbY, thumbSize, thumbSize, 1.0f);
        
    } else if (getSliderStyle() == LinearVertical) {
        auto trackX = bounds.getCentreX() - trackThickness / 2;
        g.fillRoundedRectangle(trackX, trackBounds.getY(), 
                              trackThickness, trackBounds.getHeight(), trackThickness / 2);
        
        // Draw filled portion
        auto fillHeight = (getValue() - getMinimum()) / (getMaximum() - getMinimum()) * trackBounds.getHeight();
        g.setColour(findColour(juce::Slider::thumbColourId));
        g.fillRoundedRectangle(trackX, trackBounds.getBottom() - fillHeight, 
                              trackThickness, fillHeight, trackThickness / 2);
        
        // Draw thumb
        auto thumbX = bounds.getCentreX() - thumbSize / 2;
        auto thumbY = trackBounds.getBottom() - fillHeight - thumbSize / 2;
        
        g.setColour(findColour(juce::Slider::thumbColourId));
        g.fillEllipse(thumbX, thumbY, thumbSize, thumbSize);
        
        // Draw thumb border
        g.setColour(findColour(juce::Slider::thumbColourId).contrasting(0.3f));
        g.drawEllipse(thumbX, thumbY, thumbSize, thumbSize, 1.0f);
    }
}

void TouchOptimizedSlider::mouseDown(const juce::MouseEvent& event)
{
    isDragging = true;
    lastTouchPosition = event.getPosition().toFloat();
    gestureStartValue = getValue();
    
    if (gestureMode == GestureMode::Circular) {
        auto center = getLocalBounds().getCentre().toFloat();
        gestureStartAngle = std::atan2(event.getY() - center.y, event.getX() - center.x);
    }
    
    if (hapticFeedbackEnabled) {
        triggerHapticFeedback(0.3f);
    }
    
    if (showValueTooltip) {
        showValueTooltip(event.getPosition());
    }
    
    Slider::mouseDown(event);
}

void TouchOptimizedSlider::mouseDrag(const juce::MouseEvent& event)
{
    if (!isDragging) return;
    
    updateFromGesture(event);
    
    if (showValueTooltip) {
        showValueTooltip(event.getPosition());
    }
    
    lastTouchPosition = event.getPosition().toFloat();
}

void TouchOptimizedSlider::mouseUp(const juce::MouseEvent& event)
{
    isDragging = false;
    
    if (snapToValues) {
        // Snap to nearest increment
        auto snappedValue = std::round(getValue() / getInterval()) * getInterval();
        setValue(snappedValue, juce::sendNotificationSync);
    }
    
    hideValueTooltip();
    
    Slider::mouseUp(event);
}

void TouchOptimizedSlider::mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel)
{
    if (isEnabled()) {
        auto delta = wheel.deltaY * touchSensitivity * (getMaximum() - getMinimum()) * 0.1;
        setValue(getValue() + delta, juce::sendNotificationSync);
        
        if (hapticFeedbackEnabled) {
            triggerHapticFeedback(0.2f);
        }
    }
}

void TouchOptimizedSlider::setGestureMode(GestureMode mode)
{
    gestureMode = mode;
}

void TouchOptimizedSlider::setTouchSensitivity(float sensitivity)
{
    touchSensitivity = juce::jlimit(0.1f, 5.0f, sensitivity);
}

void TouchOptimizedSlider::setSnapToValues(bool enabled)
{
    snapToValues = enabled;
}

void TouchOptimizedSlider::setHapticFeedbackEnabled(bool enabled)
{
    hapticFeedbackEnabled = enabled;
}

void TouchOptimizedSlider::setThumbSize(float size)
{
    thumbSize = juce::jmax(10.0f, size);
    repaint();
}

void TouchOptimizedSlider::setTrackThickness(float thickness)
{
    trackThickness = juce::jmax(1.0f, thickness);
    repaint();
}

void TouchOptimizedSlider::setShowValueTooltip(bool show)
{
    showValueTooltip = show;
    if (!show) {
        hideValueTooltip();
    }
}

void TouchOptimizedSlider::enablePinchZoom(bool enabled)
{
    pinchZoomEnabled = enabled;
}

void TouchOptimizedSlider::enableRotaryGesture(bool enabled)
{
    rotaryGestureEnabled = enabled;
}

void TouchOptimizedSlider::updateFromGesture(const juce::MouseEvent& event)
{
    switch (gestureMode) {
        case GestureMode::Circular:
            handleCircularGesture(event);
            break;
            
        case GestureMode::Standard:
        case GestureMode::Vertical:
        case GestureMode::Horizontal:
            handleLinearGesture(event);
            break;
    }
}

void TouchOptimizedSlider::handleCircularGesture(const juce::MouseEvent& event)
{
    auto center = getLocalBounds().getCentre().toFloat();
    auto currentAngle = std::atan2(event.getY() - center.y, event.getX() - center.x);
    auto angleDelta = currentAngle - gestureStartAngle;
    
    // Normalize angle delta
    while (angleDelta > juce::MathConstants<float>::pi) angleDelta -= 2 * juce::MathConstants<float>::pi;
    while (angleDelta < -juce::MathConstants<float>::pi) angleDelta += 2 * juce::MathConstants<float>::pi;
    
    auto valueDelta = (angleDelta / (2 * juce::MathConstants<float>::pi)) * (getMaximum() - getMinimum()) * touchSensitivity;
    setValue(gestureStartValue + valueDelta, juce::sendNotificationSync);
}

void TouchOptimizedSlider::handleLinearGesture(const juce::MouseEvent& event)
{
    auto delta = event.getPosition().toFloat() - lastTouchPosition;
    float movement = 0.0f;
    
    switch (gestureMode) {
        case GestureMode::Horizontal:
            movement = delta.x;
            break;
        case GestureMode::Vertical:
            movement = -delta.y; // Invert Y for natural feel
            break;
        case GestureMode::Standard:
            movement = (getSliderStyle() == LinearHorizontal) ? delta.x : -delta.y;
            break;
        case GestureMode::Circular:
            return; // Handled separately
    }
    
    auto valueDelta = (movement / 100.0f) * (getMaximum() - getMinimum()) * touchSensitivity;
    setValue(getValue() + valueDelta, juce::sendNotificationSync);
}

void TouchOptimizedSlider::showValueTooltip(const juce::Point<int>& position)
{
    if (!valueTooltip) {
        valueTooltip = std::make_unique<juce::Component>();
        addAndMakeVisible(*valueTooltip);
    }
    
    // Position tooltip above the touch point
    auto tooltipBounds = juce::Rectangle<int>(position.x - 30, position.y - 40, 60, 25);
    valueTooltip->setBounds(tooltipBounds);
    valueTooltip->toFront(false);
}

void TouchOptimizedSlider::hideValueTooltip()
{
    if (valueTooltip) {
        valueTooltip->setVisible(false);
    }
}

void TouchOptimizedSlider::triggerHapticFeedback(float intensity)
{
    auto& manager = TouchOptimizedControlsManager::getInstance();
    if (manager.isHapticFeedbackEnabled()) {
        // Platform-specific haptic feedback implementation
        // Intensity would control the strength of the feedback
    }
}

//==============================================================================
// TouchOptimizedControlsManager Implementation
//==============================================================================

TouchOptimizedControlsManager& TouchOptimizedControlsManager::getInstance()
{
    static TouchOptimizedControlsManager instance;
    return instance;
}

void TouchOptimizedControlsManager::setTouchSettings(const TouchSettings& settings)
{
    touchSettings = settings;
}

void TouchOptimizedControlsManager::setScreenDensity(float density)
{
    screenDensity = juce::jmax(0.5f, density);
}

void TouchOptimizedControlsManager::setHapticFeedbackEnabled(bool enabled)
{
    touchSettings.hapticFeedbackEnabled = enabled;
}

int TouchOptimizedControlsManager::getScaledTouchTarget(int baseSize) const
{
    if (touchSettings.adaptToScreenDensity) {
        return static_cast<int>(baseSize * screenDensity);
    }
    return baseSize;
}

float TouchOptimizedControlsManager::getScaledSensitivity(float baseSensitivity) const
{
    return baseSensitivity * touchSettings.touchSensitivity;
}

juce::Rectangle<int> TouchOptimizedControlsManager::expandBoundsForTouch(const juce::Rectangle<int>& bounds) const
{
    return bounds.expanded(touchSettings.touchTolerance);
}

bool TouchOptimizedControlsManager::isTouchDevice() const
{
#if JUCE_IOS || JUCE_ANDROID
    return true;
#else
    // Check for touch capability on desktop platforms
    return juce::Desktop::getInstance().getMainMouseSource().isTouch();
#endif
}

bool TouchOptimizedControlsManager::isHighDensityScreen() const
{
    return screenDensity > 1.5f;
}

juce::String TouchOptimizedControlsManager::getPlatformName() const
{
#if JUCE_IOS
    return "iOS";
#elif JUCE_ANDROID
    return "Android";
#elif JUCE_MAC
    return "macOS";
#elif JUCE_WINDOWS
    return "Windows";
#elif JUCE_LINUX
    return "Linux";
#else
    return "Unknown";
#endif
}

void TouchOptimizedControlsManager::saveToState(ComponentState& state) const
{
    state.setValue("MinimumTouchTarget", touchSettings.minimumTouchTarget);
    state.setValue("TouchTolerance", touchSettings.touchTolerance);
    state.setValue("TouchSensitivity", touchSettings.touchSensitivity);
    state.setValue("HapticFeedbackEnabled", touchSettings.hapticFeedbackEnabled);
    state.setValue("GestureRecognitionEnabled", touchSettings.gestureRecognitionEnabled);
    state.setValue("LongPressDelay", touchSettings.longPressDelay);
    state.setValue("SwipeThreshold", touchSettings.swipeThreshold);
    state.setValue("AdaptToScreenDensity", touchSettings.adaptToScreenDensity);
    state.setValue("ScreenDensity", screenDensity);
}

void TouchOptimizedControlsManager::loadFromState(const ComponentState& state)
{
    touchSettings.minimumTouchTarget = state.getIntValue("MinimumTouchTarget", 44);
    touchSettings.touchTolerance = state.getIntValue("TouchTolerance", 8);
    touchSettings.touchSensitivity = state.getFloatValue("TouchSensitivity", 1.0f);
    touchSettings.hapticFeedbackEnabled = state.getBoolValue("HapticFeedbackEnabled", true);
    touchSettings.gestureRecognitionEnabled = state.getBoolValue("GestureRecognitionEnabled", true);
    touchSettings.longPressDelay = state.getIntValue("LongPressDelay", 500);
    touchSettings.swipeThreshold = state.getFloatValue("SwipeThreshold", 50.0f);
    touchSettings.adaptToScreenDensity = state.getBoolValue("AdaptToScreenDensity", true);
    screenDensity = state.getFloatValue("ScreenDensity", 1.0f);
}

//==============================================================================
// TouchOptimizedKnob Implementation
//==============================================================================

TouchOptimizedKnob::TouchOptimizedKnob()
{
    setSize(60, 60); // Default knob size
    setMouseDragSensitivity(100);
}

TouchOptimizedKnob::~TouchOptimizedKnob() = default;

void TouchOptimizedKnob::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(2);
    auto center = bounds.getCentre();
    auto radius = getKnobRadius();
    
    // Draw knob body
    g.setColour(findColour(juce::Slider::rotarySliderFillColourId));
    g.fillEllipse(center.x - radius, center.y - radius, radius * 2, radius * 2);
    
    // Draw knob border
    g.setColour(findColour(juce::Slider::rotarySliderOutlineColourId));
    g.drawEllipse(center.x - radius, center.y - radius, radius * 2, radius * 2, 2.0f);
    
    // Draw value arc
    auto angle = valueToAngle(currentValue);
    auto startAngle = -juce::MathConstants<float>::pi * 0.75f;
    auto endAngle = juce::MathConstants<float>::pi * 0.75f;
    
    juce::Path valueArc;
    valueArc.addCentredArc(center.x, center.y, radius - arcThickness, radius - arcThickness,
                          0.0f, startAngle, angle, true);
    
    g.setColour(findColour(juce::Slider::thumbColourId));
    g.strokePath(valueArc, juce::PathStrokeType(arcThickness, juce::PathStrokeType::curved));
    
    // Draw indicator line
    auto indicatorLength = radius * 0.7f;
    auto indicatorX = center.x + std::cos(angle) * indicatorLength;
    auto indicatorY = center.y + std::sin(angle) * indicatorLength;
    
    g.setColour(findColour(juce::Slider::thumbColourId).contrasting());
    g.drawLine(center.x, center.y, indicatorX, indicatorY, 2.0f);
    
    // Draw value label if enabled
    if (showValueLabel) {
        g.setColour(findColour(juce::Label::textColourId));
        g.setFont(JUCE8_FONT(12.0f));
        
        juce::String valueText;
        if (valueToStringFunction) {
            valueText = valueToStringFunction(currentValue);
        } else {
            valueText = juce::String(currentValue, 2);
        }
        
        auto textBounds = bounds.withHeight(15).withBottomY(bounds.getBottom() + 20);
        g.drawText(valueText, textBounds.toNearestInt(), juce::Justification::centred);
    }
}

void TouchOptimizedKnob::resized()
{
    // Ensure knob remains circular
    auto size = juce::jmin(getWidth(), getHeight());
    setSize(size, size);
}

void TouchOptimizedKnob::mouseDown(const juce::MouseEvent& event)
{
    isDragging = true;
    dragStartPosition = event.getPosition().toFloat();
    dragStartValue = currentValue;
    startAngle = valueToAngle(currentValue);
    
    // Double-click to reset to default
    if (event.getNumberOfClicks() == 2) {
        setValue(defaultValue, juce::sendNotificationSync);
    }
}

void TouchOptimizedKnob::mouseDrag(const juce::MouseEvent& event)
{
    if (!isDragging) return;
    
    auto center = getKnobCenter();
    auto currentPos = event.getPosition().toFloat();
    auto currentAngle = std::atan2(currentPos.y - center.y, currentPos.x - center.x);
    
    // Calculate angle delta
    auto angleDelta = currentAngle - std::atan2(dragStartPosition.y - center.y, 
                                               dragStartPosition.x - center.x);
    
    // Normalize angle delta
    while (angleDelta > juce::MathConstants<float>::pi) angleDelta -= 2 * juce::MathConstants<float>::pi;
    while (angleDelta < -juce::MathConstants<float>::pi) angleDelta += 2 * juce::MathConstants<float>::pi;
    
    // Convert angle delta to value delta
    auto totalAngleRange = juce::MathConstants<float>::pi * 1.5f; // 270 degrees
    auto valueDelta = (angleDelta / totalAngleRange) * (valueRange.getEnd() - valueRange.getStart()) * rotationSensitivity;
    
    setValue(dragStartValue + valueDelta, juce::sendNotificationSync);
}

void TouchOptimizedKnob::mouseUp(const juce::MouseEvent& event)
{
    isDragging = false;
    
    if (snapToCenter && std::abs(currentValue - defaultValue) < (valueRange.getEnd() - valueRange.getStart()) * 0.1) {
        setValue(defaultValue, juce::sendNotificationSync);
    }
}

void TouchOptimizedKnob::mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel)
{
    if (isEnabled()) {
        auto delta = wheel.deltaY * rotationSensitivity * (valueRange.getEnd() - valueRange.getStart()) * 0.1;
        setValue(currentValue + delta, juce::sendNotificationSync);
    }
}

void TouchOptimizedKnob::setValue(double newValue, juce::NotificationType notification)
{
    newValue = valueRange.clipValue(newValue);
    
    if (interval > 0.0) {
        newValue = std::round(newValue / interval) * interval;
    }
    
    if (currentValue != newValue) {
        currentValue = newValue;
        repaint();
        
        if (notification != juce::dontSendNotification) {
            triggerValueChanged();
        }
    }
}

void TouchOptimizedKnob::setRange(double minimum, double maximum, double newInterval)
{
    valueRange = juce::Range<double>(minimum, maximum);
    interval = newInterval;
    setValue(currentValue, juce::dontSendNotification);
}

void TouchOptimizedKnob::setDefaultValue(double defaultVal)
{
    defaultValue = valueRange.clipValue(defaultVal);
}

void TouchOptimizedKnob::setKnobStyle(int style)
{
    knobStyle = style;
    repaint();
}

void TouchOptimizedKnob::setArcThickness(float thickness)
{
    arcThickness = juce::jmax(1.0f, thickness);
    repaint();
}

void TouchOptimizedKnob::setShowValueLabel(bool show)
{
    showValueLabel = show;
    repaint();
}

void TouchOptimizedKnob::setRotationSensitivity(float sensitivity)
{
    rotationSensitivity = juce::jlimit(0.1f, 5.0f, sensitivity);
}

void TouchOptimizedKnob::setSnapToCenter(bool enabled)
{
    snapToCenter = enabled;
}

void TouchOptimizedKnob::updateValueFromAngle(float angle)
{
    auto normalizedAngle = (angle + juce::MathConstants<float>::pi * 0.75f) / (juce::MathConstants<float>::pi * 1.5f);
    normalizedAngle = juce::jlimit(0.0f, 1.0f, normalizedAngle);
    
    auto newValue = valueRange.getStart() + normalizedAngle * valueRange.getLength();
    setValue(newValue, juce::sendNotificationSync);
}

float TouchOptimizedKnob::valueToAngle(double value) const
{
    auto normalized = (value - valueRange.getStart()) / valueRange.getLength();
    return -juce::MathConstants<float>::pi * 0.75f + normalized * juce::MathConstants<float>::pi * 1.5f;
}

double TouchOptimizedKnob::angleToValue(float angle) const
{
    auto normalized = (angle + juce::MathConstants<float>::pi * 0.75f) / (juce::MathConstants<float>::pi * 1.5f);
    return valueRange.getStart() + normalized * valueRange.getLength();
}

void TouchOptimizedKnob::triggerValueChanged()
{
    if (onValueChange) {
        onValueChange(currentValue);
    }
}

juce::Point<float> TouchOptimizedKnob::getKnobCenter() const
{
    return getLocalBounds().toFloat().getCentre();
}

float TouchOptimizedKnob::getKnobRadius() const
{
    return juce::jmin(getWidth(), getHeight()) * 0.4f;
}

} // namespace Controls
} // namespace UI
} // namespace OTTO