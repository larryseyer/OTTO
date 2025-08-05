#include "GestureRecognizer.h"
#include "JUCE8_CODING_STANDARDS.h"
#include "INIDataManager.h"
#include <algorithm>
#include <cmath>

//==============================================================================
// GestureRecognizer Implementation
//==============================================================================

GestureRecognizer::GestureRecognizer() {
    // Enable common gestures by default
    enabledGestures = {
        GestureType::Tap,
        GestureType::DoubleTap,
        GestureType::LongPress,
        GestureType::Pan,
        GestureType::Pinch,
        GestureType::Rotate,
        GestureType::Swipe
    };
    
    // Load settings from INI
    ComponentState state;
    if (INIDataManager::loadComponentState("GestureRecognizer", state)) {
        loadState(state);
    }
    
    #if JUCE_IOS || JUCE_ANDROID
    // Initialize haptic feedback on mobile platforms
    if (settings.enableHapticFeedback) {
        hapticFeedback = std::make_unique<juce::HapticFeedback>();
    }
    #endif
}

GestureRecognizer::~GestureRecognizer() {
    // Save current state
    ComponentState state;
    saveState(state);
    INIDataManager::saveComponentState("GestureRecognizer", state);
    
    // Detach from all components
    for (auto* component : attachedComponents) {
        if (component != nullptr) {
            component->removeMouseListener(this);
        }
    }
    
    attachedComponents.clear();
}

//==============================================================================
// Configuration
//==============================================================================

void GestureRecognizer::setGestureSettings(const GestureSettings& newSettings) {
    settings = newSettings;
    
    // Update haptic feedback if needed
    #if JUCE_IOS || JUCE_ANDROID
    if (settings.enableHapticFeedback && !hapticFeedback) {
        hapticFeedback = std::make_unique<juce::HapticFeedback>();
    } else if (!settings.enableHapticFeedback && hapticFeedback) {
        hapticFeedback.reset();
    }
    #endif
}

void GestureRecognizer::setEnabledGestures(const std::set<GestureType>& gestures) {
    enabledGestures = gestures;
    
    // Cancel current gesture if it's no longer enabled
    if (currentGesture.type != GestureType::None && 
        enabledGestures.find(currentGesture.type) == enabledGestures.end()) {
        cancelCurrentGesture();
    }
}

void GestureRecognizer::enableGesture(GestureType gesture, bool enabled) {
    if (enabled) {
        enabledGestures.insert(gesture);
    } else {
        enabledGestures.erase(gesture);
        
        // Cancel current gesture if it's being disabled
        if (currentGesture.type == gesture) {
            cancelCurrentGesture();
        }
    }
}

bool GestureRecognizer::isGestureEnabled(GestureType gesture) const {
    return enabledGestures.find(gesture) != enabledGestures.end();
}

//==============================================================================
// Component Attachment
//==============================================================================

void GestureRecognizer::attachToComponent(juce::Component* component) {
    if (component == nullptr || attachedComponents.contains(component)) {
        return;
    }
    
    component->addMouseListener(this, true);
    attachedComponents.add(component);
}

void GestureRecognizer::detachFromComponent(juce::Component* component) {
    if (component == nullptr) {
        return;
    }
    
    component->removeMouseListener(this);
    attachedComponents.removeAllInstancesOf(component);
    
    // Cancel gesture if it was on this component
    if (currentGesture.targetComponent == component) {
        cancelCurrentGesture();
    }
}

juce::Array<juce::Component*> GestureRecognizer::getAttachedComponents() const {
    return attachedComponents;
}

//==============================================================================
// Gesture Recognition
//==============================================================================

void GestureRecognizer::processMouseEvent(const juce::MouseEvent& e, juce::Component* component) {
    // Convert mouse event to touch event for unified processing
    int touchId = 0; // Mouse is always touch ID 0
    
    if (e.source.isTouch()) {
        if (e.mouseWasClicked()) {
            addTouchPoint(touchId, e.position, component);
        } else if (e.mouseWasDraggedSinceMouseDown()) {
            updateTouchPoint(touchId, e.position);
        } else if (e.mouseWasClicked()) { // Use mouseWasClicked for release detection
            removeTouchPoint(touchId);
        }
    }
    
    updateGestureRecognition();
}

void GestureRecognizer::processTouchEvent(const juce::MouseEvent& e, juce::Component* component) {
    // In JUCE 8, multitouch is handled through separate MouseEvent instances
    // Each touch generates its own MouseEvent with a unique MouseInputSource
    int touchId = e.source.getIndex(); // Use source index as touch ID
    
    if (e.mouseWasClicked()) {
        addTouchPoint(touchId, e.position, component);
    } else if (e.mouseWasDraggedSinceMouseDown()) {
        updateTouchPoint(touchId, e.position);
    } else if (e.mouseWasClicked()) { // Mouse up
        removeTouchPoint(touchId);
    }
    
    updateGestureRecognition();
}

//==============================================================================
// Touch Point Management
//==============================================================================

int GestureRecognizer::getActiveTouchCount() const {
    return static_cast<int>(activeTouches.size());
}

std::vector<GestureRecognizer::TouchPoint> GestureRecognizer::getActiveTouchPoints() const {
    std::vector<TouchPoint> points;
    for (const auto& pair : activeTouches) {
        points.push_back(pair.second);
    }
    return points;
}

GestureRecognizer::TouchPoint GestureRecognizer::getTouchPoint(int touchId) const {
    auto it = activeTouches.find(touchId);
    return it != activeTouches.end() ? it->second : TouchPoint();
}

bool GestureRecognizer::hasTouchPoint(int touchId) const {
    return activeTouches.find(touchId) != activeTouches.end();
}

//==============================================================================
// Gesture Prediction
//==============================================================================

GestureRecognizer::GestureType GestureRecognizer::predictGesture() const {
    if (activeTouches.empty()) {
        return GestureType::None;
    }
    
    int touchCount = getActiveTouchCount();
    
    // Single touch predictions
    if (touchCount == 1) {
        const auto& touch = activeTouches.begin()->second;
        double duration = touch.getDuration();
        float distance = touch.getDistance();
        
        if (duration > settings.longPressMinDuration && distance < settings.longPressMaxMovement) {
            return GestureType::LongPress;
        } else if (distance > settings.panMinDistance) {
            auto velocity = calculateVelocity(touch.touchId);
            if (velocity.getDistanceFromOrigin() > settings.swipeMinVelocity) {
                return GestureType::Swipe;
            } else {
                return GestureType::Pan;
            }
        } else if (duration < settings.tapMaxDuration && distance < settings.tapMaxMovement) {
            return GestureType::Tap;
        }
    }
    // Two touch predictions
    else if (touchCount == 2) {
        if (isValidPinch()) {
            return GestureType::Pinch;
        } else if (isValidRotation()) {
            return GestureType::Rotate;
        } else {
            return GestureType::TwoFingerPan;
        }
    }
    // Multi-touch predictions
    else if (touchCount >= 3) {
        if (touchCount == 3) {
            return GestureType::ThreeFingerTap;
        } else if (touchCount == 4) {
            return GestureType::FourFingerTap;
        }
    }
    
    return GestureType::None;
}

float GestureRecognizer::getGestureConfidence(GestureType gesture) const {
    // Simple confidence calculation based on gesture characteristics
    if (activeTouches.empty()) {
        return 0.0f;
    }
    
    switch (gesture) {
        case GestureType::Tap:
            if (getActiveTouchCount() == 1) {
                const auto& touch = activeTouches.begin()->second;
                float distanceScore = 1.0f - juce::jmin(1.0f, touch.getDistance() / settings.tapMaxMovement);
                float durationScore = 1.0f - juce::jmin(1.0f, static_cast<float>(touch.getDuration() / settings.tapMaxDuration));
                return (distanceScore + durationScore) * 0.5f;
            }
            break;
            
        case GestureType::Pan:
            if (getActiveTouchCount() == 1) {
                const auto& touch = activeTouches.begin()->second;
                float distance = touch.getDistance();
                return juce::jmin(1.0f, distance / (settings.panMinDistance * 2.0f));
            }
            break;
            
        case GestureType::Pinch:
            if (getActiveTouchCount() == 2 && isValidPinch()) {
                return 0.8f; // High confidence for valid pinch
            }
            break;
            
        default:
            break;
    }
    
    return 0.0f;
}

//==============================================================================
// Custom Gesture Support
//==============================================================================

void GestureRecognizer::addCustomGesture(const CustomGesturePattern& pattern) {
    // Remove existing gesture with same name
    removeCustomGesture(pattern.name);
    
    customGestures.push_back(pattern);
}

void GestureRecognizer::removeCustomGesture(const juce::String& name) {
    customGestures.erase(
        std::remove_if(customGestures.begin(), customGestures.end(),
                      [&name](const CustomGesturePattern& pattern) {
                          return pattern.name == name;
                      }),
        customGestures.end()
    );
}

juce::StringArray GestureRecognizer::getCustomGestureNames() const {
    juce::StringArray names;
    for (const auto& pattern : customGestures) {
        names.add(pattern.name);
    }
    return names;
}

//==============================================================================
// MouseListener Overrides
//==============================================================================

void GestureRecognizer::mouseDown(const juce::MouseEvent& e) {
    processMouseEvent(e, e.eventComponent);
}

void GestureRecognizer::mouseDrag(const juce::MouseEvent& e) {
    processMouseEvent(e, e.eventComponent);
}

void GestureRecognizer::mouseUp(const juce::MouseEvent& e) {
    processMouseEvent(e, e.eventComponent);
}

void GestureRecognizer::mouseMove(const juce::MouseEvent& e) {
    // Mouse move events don't affect gesture recognition
}

void GestureRecognizer::mouseDoubleClick(const juce::MouseEvent& e) {
    // Handle double-click as a special case
    if (isGestureEnabled(GestureType::DoubleTap)) {
        currentGesture.reset();
        currentGesture.type = GestureType::DoubleTap;
        currentGesture.state = GestureState::Began;
        currentGesture.position = e.position;
        currentGesture.startPosition = e.position;
        currentGesture.targetComponent = e.eventComponent;
        currentGesture.touchCount = 1;
        
        notifyGestureStarted();
        
        currentGesture.state = GestureState::Ended;
        notifyGestureEnded();
        
        triggerHapticFeedback(GestureType::DoubleTap);
        performanceStats.gesturesRecognized++;
    }
}

//==============================================================================
// State Management
//==============================================================================

void GestureRecognizer::saveState(ComponentState& state) const {
    state.setValue("tapMaxDuration", settings.tapMaxDuration);
    state.setValue("tapMaxMovement", settings.tapMaxMovement);
    state.setValue("longPressMinDuration", settings.longPressMinDuration);
    state.setValue("panMinDistance", settings.panMinDistance);
    state.setValue("swipeMinVelocity", settings.swipeMinVelocity);
    state.setValue("enableHapticFeedback", settings.enableHapticFeedback);
    state.setValue("hapticIntensity", settings.hapticIntensity);
}

void GestureRecognizer::loadState(const ComponentState& state) {
    settings.tapMaxDuration = state.getValue("tapMaxDuration", 0.3);
    settings.tapMaxMovement = state.getValue("tapMaxMovement", 10.0f);
    settings.longPressMinDuration = state.getValue("longPressMinDuration", 0.8);
    settings.panMinDistance = state.getValue("panMinDistance", 10.0f);
    settings.swipeMinVelocity = state.getValue("swipeMinVelocity", 200.0f);
    settings.enableHapticFeedback = state.getValue("enableHapticFeedback", true);
    settings.hapticIntensity = state.getValue("hapticIntensity", 0.5f);
}

void GestureRecognizer::reset() {
    activeTouches.clear();
    touchHistory.clear();
    currentGesture.reset();
    previousGesture.reset();
    tapCount = 0;
    lastTapTime = juce::Time();
    lastTapPosition = juce::Point<float>();
}

void GestureRecognizer::cancelCurrentGesture() {
    if (currentGesture.state != GestureState::Idle) {
        currentGesture.state = GestureState::Cancelled;
        notifyGestureCancelled();
        currentGesture.reset();
    }
}

//==============================================================================
// Listener Management
//==============================================================================

void GestureRecognizer::addListener(Listener* listener) {
    listeners.add(listener);
}

void GestureRecognizer::removeListener(Listener* listener) {
    listeners.remove(listener);
}

//==============================================================================
// Private Implementation
//==============================================================================

void GestureRecognizer::updateGestureRecognition() {
    recognitionStartTime = juce::Time::getCurrentTime();
    
    // Clean up inactive touches
    cleanupInactiveTouches();
    
    // Update current gesture info
    currentGesture.touchCount = getActiveTouchCount();
    currentGesture.touchPoints = getActiveTouchPoints();
    currentGesture.lastUpdateTime = juce::Time::getCurrentTime();
    
    if (!activeTouches.empty()) {
        currentGesture.position = calculateCentroid();
        currentGesture.duration = (currentGesture.lastUpdateTime - currentGesture.startTime).inSeconds();
    }
    
    // Recognize different gesture types
    if (isGestureEnabled(GestureType::Tap) || isGestureEnabled(GestureType::DoubleTap)) {
        recognizeTapGesture();
    }
    
    if (isGestureEnabled(GestureType::LongPress)) {
        recognizeLongPressGesture();
    }
    
    if (isGestureEnabled(GestureType::Pan)) {
        recognizePanGesture();
    }
    
    if (isGestureEnabled(GestureType::Pinch)) {
        recognizePinchGesture();
    }
    
    if (isGestureEnabled(GestureType::Rotate)) {
        recognizeRotationGesture();
    }
    
    if (isGestureEnabled(GestureType::Swipe)) {
        recognizeSwipeGesture();
    }
    
    recognizeMultiTouchGestures();
    recognizeCustomGestures();
    
    updatePerformanceStats();
}

void GestureRecognizer::recognizeTapGesture() {
    if (getActiveTouchCount() != 1) {
        return;
    }
    
    const auto& touch = activeTouches.begin()->second;
    
    if (currentGesture.type == GestureType::None && isValidTap(touch)) {
        if (touch.getDuration() < settings.tapMaxDuration) {
            // Check for double tap
            auto timeSinceLastTap = (touch.startTime - lastTapTime).inSeconds();
            auto distanceFromLastTap = touch.startPosition.getDistanceFrom(lastTapPosition);
            
            if (timeSinceLastTap < settings.doubleTapMaxInterval && 
                distanceFromLastTap < settings.doubleTapMaxDistance &&
                isGestureEnabled(GestureType::DoubleTap)) {
                
                startGesture(GestureType::DoubleTap);
                triggerHapticFeedback(GestureType::DoubleTap);
            } else {
                startGesture(GestureType::Tap);
                triggerHapticFeedback(GestureType::Tap);
            }
            
            lastTapTime = touch.startTime;
            lastTapPosition = touch.startPosition;
        }
    }
    
    // End tap when touch is released
    if (currentGesture.type == GestureType::Tap || currentGesture.type == GestureType::DoubleTap) {
        if (activeTouches.empty()) {
            endGesture();
            performanceStats.gesturesRecognized++;
        }
    }
}

void GestureRecognizer::recognizeDoubleTapGesture() {
    // Double tap is handled in recognizeTapGesture()
}

void GestureRecognizer::recognizeLongPressGesture() {
    if (getActiveTouchCount() != 1) {
        return;
    }
    
    const auto& touch = activeTouches.begin()->second;
    
    if (currentGesture.type == GestureType::None && isValidLongPress(touch)) {
        if (touch.getDuration() >= settings.longPressMinDuration) {
            startGesture(GestureType::LongPress);
            triggerHapticFeedback(GestureType::LongPress);
            performanceStats.gesturesRecognized++;
        }
    }
    
    // End long press when touch is released
    if (currentGesture.type == GestureType::LongPress && activeTouches.empty()) {
        endGesture();
    }
}

void GestureRecognizer::recognizePanGesture() {
    if (getActiveTouchCount() != 1) {
        return;
    }
    
    const auto& touch = activeTouches.begin()->second;
    
    if (currentGesture.type == GestureType::None && isValidPan(touch)) {
        startGesture(GestureType::Pan);
        currentGesture.translation = touch.getDelta();
        currentGesture.velocity = calculateVelocity(touch.touchId);
        triggerHapticFeedback(GestureType::Pan);
    } else if (currentGesture.type == GestureType::Pan) {
        currentGesture.translation = touch.getDelta();
        currentGesture.velocity = calculateVelocity(touch.touchId);
        updateGesture();
    }
    
    // End pan when touch is released
    if (currentGesture.type == GestureType::Pan && activeTouches.empty()) {
        endGesture();
        performanceStats.gesturesRecognized++;
    }
}

void GestureRecognizer::recognizePinchGesture() {
    if (getActiveTouchCount() != 2) {
        return;
    }
    
    if (currentGesture.type == GestureType::None && isValidPinch()) {
        startGesture(GestureType::Pinch);
        currentGesture.scale = calculateScale();
        triggerHapticFeedback(GestureType::Pinch);
    } else if (currentGesture.type == GestureType::Pinch) {
        float newScale = calculateScale();
        if (std::abs(newScale - currentGesture.scale) > settings.pinchThreshold) {
            currentGesture.scale = newScale;
            updateGesture();
        }
    }
    
    // End pinch when not exactly 2 touches
    if (currentGesture.type == GestureType::Pinch && getActiveTouchCount() != 2) {
        endGesture();
        performanceStats.gesturesRecognized++;
    }
}

void GestureRecognizer::recognizeRotationGesture() {
    if (getActiveTouchCount() != 2) {
        return;
    }
    
    if (currentGesture.type == GestureType::None && isValidRotation()) {
        startGesture(GestureType::Rotate);
        currentGesture.rotation = calculateRotation();
        triggerHapticFeedback(GestureType::Rotate);
    } else if (currentGesture.type == GestureType::Rotate) {
        float newRotation = calculateRotation();
        if (std::abs(newRotation - currentGesture.rotation) > settings.rotationThreshold) {
            currentGesture.rotation = newRotation;
            updateGesture();
        }
    }
    
    // End rotation when not exactly 2 touches
    if (currentGesture.type == GestureType::Rotate && getActiveTouchCount() != 2) {
        endGesture();
        performanceStats.gesturesRecognized++;
    }
}

void GestureRecognizer::recognizeSwipeGesture() {
    if (getActiveTouchCount() != 1) {
        return;
    }
    
    const auto& touch = activeTouches.begin()->second;
    
    if (currentGesture.type == GestureType::None && isValidSwipe(touch)) {
        startGesture(GestureType::Swipe);
        currentGesture.swipeDirection = calculateSwipeDirection(touch.getDelta());
        currentGesture.swipeVelocity = calculateVelocity(touch.touchId).getDistanceFromOrigin();
        triggerHapticFeedback(GestureType::Swipe);
        
        // Swipe is typically a quick gesture, end it immediately
        endGesture();
        performanceStats.gesturesRecognized++;
    }
}

void GestureRecognizer::recognizeMultiTouchGestures() {
    int touchCount = getActiveTouchCount();
    
    if (touchCount == 2 && currentGesture.type == GestureType::None) {
        if (isGestureEnabled(GestureType::TwoFingerTap)) {
            // Check if both touches are tap-like
            bool allTapsValid = true;
            for (const auto& pair : activeTouches) {
                if (!isValidTap(pair.second)) {
                    allTapsValid = false;
                    break;
                }
            }
            
            if (allTapsValid) {
                startGesture(GestureType::TwoFingerTap);
                triggerHapticFeedback(GestureType::TwoFingerTap);
            }
        }
    } else if (touchCount == 3 && currentGesture.type == GestureType::None) {
        if (isGestureEnabled(GestureType::ThreeFingerTap)) {
            startGesture(GestureType::ThreeFingerTap);
            triggerHapticFeedback(GestureType::ThreeFingerTap);
        }
    } else if (touchCount == 4 && currentGesture.type == GestureType::None) {
        if (isGestureEnabled(GestureType::FourFingerTap)) {
            startGesture(GestureType::FourFingerTap);
            triggerHapticFeedback(GestureType::FourFingerTap);
        }
    }
    
    // End multi-touch taps when touch count changes
    if ((currentGesture.type == GestureType::TwoFingerTap && touchCount != 2) ||
        (currentGesture.type == GestureType::ThreeFingerTap && touchCount != 3) ||
        (currentGesture.type == GestureType::FourFingerTap && touchCount != 4)) {
        
        if (touchCount == 0) {
            endGesture();
            performanceStats.gesturesRecognized++;
        } else {
            cancelGesture();
        }
    }
}

void GestureRecognizer::recognizeCustomGestures() {
    if (currentGesture.type != GestureType::None || customGestures.empty()) {
        return;
    }
    
    // Check each custom gesture pattern
    for (const auto& pattern : customGestures) {
        int touchCount = getActiveTouchCount();
        
        if (touchCount >= pattern.minTouchPoints && touchCount <= pattern.maxTouchPoints) {
            float matchScore = matchCustomGesture(pattern);
            
            if (matchScore > 0.8f) { // 80% match threshold
                startGesture(GestureType::Custom);
                notifyCustomGestureRecognized(pattern.name);
                endGesture();
                performanceStats.gesturesRecognized++;
                break;
            }
        }
    }
}

//==============================================================================
// Touch Point Management
//==============================================================================

void GestureRecognizer::addTouchPoint(int touchId, const juce::Point<float>& position, juce::Component* component) {
    TouchPoint touch(touchId, position);
    touch.active = true;
    
    activeTouches[touchId] = touch;
    touchHistory.push_back(touch);
    
    // Initialize gesture if this is the first touch
    if (currentGesture.type == GestureType::None) {
        currentGesture.startPosition = position;
        currentGesture.position = position;
        currentGesture.startTime = juce::Time::getCurrentTime();
        currentGesture.targetComponent = component;
    }
}

void GestureRecognizer::updateTouchPoint(int touchId, const juce::Point<float>& position) {
    auto it = activeTouches.find(touchId);
    if (it != activeTouches.end()) {
        it->second.position = position;
        it->second.lastUpdateTime = juce::Time::getCurrentTime();
        
        // Calculate velocity
        it->second.velocity = calculateVelocity(touchId);
        
        // Add to history
        touchHistory.push_back(it->second);
        
        // Limit history size
        if (touchHistory.size() > static_cast<size_t>(settings.velocityHistorySize * settings.maxTouchPoints)) {
            touchHistory.erase(touchHistory.begin());
        }
    }
}

void GestureRecognizer::removeTouchPoint(int touchId) {
    auto it = activeTouches.find(touchId);
    if (it != activeTouches.end()) {
        it->second.active = false;
        touchHistory.push_back(it->second);
        activeTouches.erase(it);
    }
}

void GestureRecognizer::cleanupInactiveTouches() {
    auto currentTime = juce::Time::getCurrentTime();
    
    // Remove old touch history
    touchHistory.erase(
        std::remove_if(touchHistory.begin(), touchHistory.end(),
                      [&](const TouchPoint& touch) {
                          return (currentTime - touch.lastUpdateTime).inSeconds() > settings.touchTimeoutDuration;
                      }),
        touchHistory.end()
    );
    
    // Remove timed-out active touches
    for (auto it = activeTouches.begin(); it != activeTouches.end();) {
        if ((currentTime - it->second.lastUpdateTime).inSeconds() > settings.touchTimeoutDuration) {
            it = activeTouches.erase(it);
        } else {
            ++it;
        }
    }
}

//==============================================================================
// Gesture State Management
//==============================================================================

void GestureRecognizer::startGesture(GestureType type) {
    if (currentGesture.type != GestureType::None) {
        cancelGesture();
    }
    
    currentGesture.type = type;
    currentGesture.state = GestureState::Began;
    currentGesture.startTime = juce::Time::getCurrentTime();
    
    notifyGestureStarted();
}

void GestureRecognizer::updateGesture() {
    if (currentGesture.type != GestureType::None) {
        currentGesture.state = GestureState::Changed;
        notifyGestureChanged();
    }
}

void GestureRecognizer::endGesture() {
    if (currentGesture.type != GestureType::None) {
        currentGesture.state = GestureState::Ended;
        notifyGestureEnded();
        
        previousGesture = currentGesture;
        currentGesture.reset();
    }
}

void GestureRecognizer::cancelGesture() {
    if (currentGesture.type != GestureType::None) {
        currentGesture.state = GestureState::Cancelled;
        notifyGestureCancelled();
        currentGesture.reset();
    }
}

void GestureRecognizer::failGesture() {
    if (currentGesture.type != GestureType::None) {
        currentGesture.state = GestureState::Failed;
        performanceStats.gesturesFailed++;
        currentGesture.reset();
    }
}

//==============================================================================
// Calculation Helpers
//==============================================================================

juce::Point<float> GestureRecognizer::calculateCentroid() const {
    if (activeTouches.empty()) {
        return juce::Point<float>();
    }
    
    juce::Point<float> centroid;
    for (const auto& pair : activeTouches) {
        centroid += pair.second.position;
    }
    
    return centroid / static_cast<float>(activeTouches.size());
}

float GestureRecognizer::calculateAverageDistance() const {
    if (activeTouches.size() < 2) {
        return 0.0f;
    }
    
    auto centroid = calculateCentroid();
    float totalDistance = 0.0f;
    
    for (const auto& pair : activeTouches) {
        totalDistance += centroid.getDistanceFrom(pair.second.position);
    }
    
    return totalDistance / static_cast<float>(activeTouches.size());
}

float GestureRecognizer::calculateScale() const {
    if (activeTouches.size() != 2) {
        return 1.0f;
    }
    
    auto it = activeTouches.begin();
    const auto& touch1 = it->second;
    ++it;
    const auto& touch2 = it->second;
    
    float currentDistance = touch1.position.getDistanceFrom(touch2.position);
    float initialDistance = touch1.startPosition.getDistanceFrom(touch2.startPosition);
    
    if (initialDistance > 0.0f) {
        return currentDistance / initialDistance;
    }
    
    return 1.0f;
}

float GestureRecognizer::calculateRotation() const {
    if (activeTouches.size() != 2) {
        return 0.0f;
    }
    
    auto it = activeTouches.begin();
    const auto& touch1 = it->second;
    ++it;
    const auto& touch2 = it->second;
    
    // Calculate initial angle
    auto initialDelta = touch2.startPosition - touch1.startPosition;
    float initialAngle = std::atan2(initialDelta.y, initialDelta.x);
    
    // Calculate current angle
    auto currentDelta = touch2.position - touch1.position;
    float currentAngle = std::atan2(currentDelta.y, currentDelta.x);
    
    return currentAngle - initialAngle;
}

juce::Point<float> GestureRecognizer::calculateVelocity(int touchId) const {
    auto it = activeTouches.find(touchId);
    if (it == activeTouches.end()) {
        return juce::Point<float>();
    }
    
    const auto& touch = it->second;
    
    // Find recent history for this touch
    std::vector<TouchPoint> recentHistory;
    auto cutoffTime = juce::Time::getCurrentTime() - juce::RelativeTime::seconds(settings.velocityTimeWindow);
    
    for (const auto& historyTouch : touchHistory) {
        if (historyTouch.touchId == touchId && historyTouch.lastUpdateTime > cutoffTime) {
            recentHistory.push_back(historyTouch);
        }
    }
    
    if (recentHistory.size() < 2) {
        return juce::Point<float>();
    }
    
    // Calculate velocity from recent history
    const auto& oldest = recentHistory.front();
    const auto& newest = recentHistory.back();
    
    auto deltaPosition = newest.position - oldest.position;
    auto deltaTime = (newest.lastUpdateTime - oldest.lastUpdateTime).inSeconds();
    
    if (deltaTime > 0.0) {
        return deltaPosition / static_cast<float>(deltaTime);
    }
    
    return juce::Point<float>();
}

GestureRecognizer::SwipeDirection GestureRecognizer::calculateSwipeDirection(const juce::Point<float>& delta) const {
    if (delta.getDistanceFromOrigin() < settings.swipeMinDistance) {
        return SwipeDirection::None;
    }
    
    float angle = std::atan2(delta.y, delta.x);
    float degrees = angle * 180.0f / juce::MathConstants<float>::pi;
    
    // Normalize to 0-360 degrees
    if (degrees < 0) {
        degrees += 360.0f;
    }
    
    // Determine direction based on angle
    if (degrees >= 337.5f || degrees < 22.5f) {
        return SwipeDirection::Right;
    } else if (degrees >= 22.5f && degrees < 67.5f) {
        return SwipeDirection::DownRight;
    } else if (degrees >= 67.5f && degrees < 112.5f) {
        return SwipeDirection::Down;
    } else if (degrees >= 112.5f && degrees < 157.5f) {
        return SwipeDirection::DownLeft;
    } else if (degrees >= 157.5f && degrees < 202.5f) {
        return SwipeDirection::Left;
    } else if (degrees >= 202.5f && degrees < 247.5f) {
        return SwipeDirection::UpLeft;
    } else if (degrees >= 247.5f && degrees < 292.5f) {
        return SwipeDirection::Up;
    } else if (degrees >= 292.5f && degrees < 337.5f) {
        return SwipeDirection::UpRight;
    }
    
    return SwipeDirection::None;
}

//==============================================================================
// Validation Helpers
//==============================================================================

bool GestureRecognizer::isValidTap(const TouchPoint& touch) const {
    return touch.getDuration() <= settings.tapMaxDuration && 
           touch.getDistance() <= settings.tapMaxMovement;
}

bool GestureRecognizer::isValidLongPress(const TouchPoint& touch) const {
    return touch.getDuration() >= settings.longPressMinDuration && 
           touch.getDistance() <= settings.longPressMaxMovement;
}

bool GestureRecognizer::isValidPan(const TouchPoint& touch) const {
    return touch.getDistance() >= settings.panMinDistance;
}

bool GestureRecognizer::isValidSwipe(const TouchPoint& touch) const {
    auto velocity = calculateVelocity(touch.touchId);
    return touch.getDistance() >= settings.swipeMinDistance &&
           velocity.getDistanceFromOrigin() >= settings.swipeMinVelocity &&
           touch.getDuration() <= settings.swipeMaxDuration;
}

bool GestureRecognizer::isValidPinch() const {
    if (activeTouches.size() != 2) {
        return false;
    }
    
    auto it = activeTouches.begin();
    const auto& touch1 = it->second;
    ++it;
    const auto& touch2 = it->second;
    
    float distance = touch1.position.getDistanceFrom(touch2.position);
    return distance >= settings.pinchMinDistance;
}

bool GestureRecognizer::isValidRotation() const {
    if (activeTouches.size() != 2) {
        return false;
    }
    
    float rotation = std::abs(calculateRotation());
    return rotation >= settings.rotationMinAngle;
}

//==============================================================================
// Custom Gesture Matching
//==============================================================================

float GestureRecognizer::matchCustomGesture(const CustomGesturePattern& pattern) const {
    // Simple pattern matching implementation
    // In a real implementation, you might use more sophisticated algorithms
    // like Dynamic Time Warping (DTW) or template matching
    
    if (activeTouches.size() < static_cast<size_t>(pattern.minTouchPoints) || 
        activeTouches.size() > static_cast<size_t>(pattern.maxTouchPoints)) {
        return 0.0f;
    }
    
    // For now, return a simple match based on touch count
    if (static_cast<int>(activeTouches.size()) == pattern.minTouchPoints) {
        return 0.5f; // Basic match
    }
    
    return 0.0f;
}

std::vector<juce::Point<float>> GestureRecognizer::normalizeGesturePath(const std::vector<juce::Point<float>>& path) const {
    if (path.empty()) {
        return path;
    }
    
    // Find bounding box
    auto minX = path[0].x, maxX = path[0].x;
    auto minY = path[0].y, maxY = path[0].y;
    
    for (const auto& point : path) {
        minX = juce::jmin(minX, point.x);
        maxX = juce::jmax(maxX, point.x);
        minY = juce::jmin(minY, point.y);
        maxY = juce::jmax(maxY, point.y);
    }
    
    // Normalize to unit square
    std::vector<juce::Point<float>> normalized;
    float width = maxX - minX;
    float height = maxY - minY;
    
    if (width > 0 && height > 0) {
        for (const auto& point : path) {
            normalized.emplace_back((point.x - minX) / width, (point.y - minY) / height);
        }
    }
    
    return normalized;
}

//==============================================================================
// Haptic Feedback
//==============================================================================

void GestureRecognizer::triggerHapticFeedback(GestureType gesture) {
    #if JUCE_IOS || JUCE_ANDROID
    if (hapticFeedback && settings.enableHapticFeedback) {
        // Different haptic patterns for different gestures
        switch (gesture) {
            case GestureType::Tap:
                hapticFeedback->triggerHapticFeedback(juce::HapticFeedback::light);
                break;
                
            case GestureType::DoubleTap:
                hapticFeedback->triggerHapticFeedback(juce::HapticFeedback::medium);
                break;
                
            case GestureType::LongPress:
                hapticFeedback->triggerHapticFeedback(juce::HapticFeedback::heavy);
                break;
                
            case GestureType::Swipe:
                hapticFeedback->triggerHapticFeedback(juce::HapticFeedback::light);
                break;
                
            default:
                hapticFeedback->triggerHapticFeedback(juce::HapticFeedback::light);
                break;
        }
    }
    #else
    // Desktop platforms don't typically have haptic feedback
    juce::ignoreUnused(gesture);
    #endif
}

//==============================================================================
// Notification Helpers
//==============================================================================

void GestureRecognizer::notifyGestureStarted() {
    listeners.call([this](Listener& l) { l.gestureStarted(currentGesture); });
}

void GestureRecognizer::notifyGestureChanged() {
    listeners.call([this](Listener& l) { l.gestureChanged(currentGesture); });
}

void GestureRecognizer::notifyGestureEnded() {
    listeners.call([this](Listener& l) { l.gestureEnded(currentGesture); });
}

void GestureRecognizer::notifyGestureCancelled() {
    listeners.call([this](Listener& l) { l.gestureCancelled(currentGesture); });
}

void GestureRecognizer::notifyCustomGestureRecognized(const juce::String& gestureName) {
    listeners.call([&](Listener& l) { l.customGestureRecognized(gestureName, currentGesture); });
}

//==============================================================================
// Performance Tracking
//==============================================================================

void GestureRecognizer::updatePerformanceStats() {
    auto currentTime = juce::Time::getCurrentTime();
    auto recognitionTime = (currentTime - recognitionStartTime).inMilliseconds();
    
    // Update average recognition time
    if (performanceStats.gesturesRecognized > 0) {
        performanceStats.averageRecognitionTime = 
            (performanceStats.averageRecognitionTime * (performanceStats.gesturesRecognized - 1) + recognitionTime) / 
            performanceStats.gesturesRecognized;
    } else {
        performanceStats.averageRecognitionTime = recognitionTime;
    }
    
    // Update max recognition time
    if (recognitionTime > performanceStats.maxRecognitionTime) {
        performanceStats.maxRecognitionTime = recognitionTime;
    }
}

//==============================================================================
// Utility Methods
//==============================================================================

juce::String GestureRecognizer::gestureTypeToString(GestureType type) const {
    switch (type) {
        case GestureType::None: return "None";
        case GestureType::Tap: return "Tap";
        case GestureType::DoubleTap: return "DoubleTap";
        case GestureType::LongPress: return "LongPress";
        case GestureType::Pan: return "Pan";
        case GestureType::Pinch: return "Pinch";
        case GestureType::Rotate: return "Rotate";
        case GestureType::Swipe: return "Swipe";
        case GestureType::TwoFingerTap: return "TwoFingerTap";
        case GestureType::ThreeFingerTap: return "ThreeFingerTap";
        case GestureType::FourFingerTap: return "FourFingerTap";
        case GestureType::TwoFingerPan: return "TwoFingerPan";
        case GestureType::ThreeFingerPan: return "ThreeFingerPan";
        case GestureType::EdgeSwipe: return "EdgeSwipe";
        case GestureType::Custom: return "Custom";
        default: return "Unknown";
    }
}

juce::String GestureRecognizer::gestureStateToString(GestureState state) const {
    switch (state) {
        case GestureState::Idle: return "Idle";
        case GestureState::Possible: return "Possible";
        case GestureState::Began: return "Began";
        case GestureState::Changed: return "Changed";
        case GestureState::Ended: return "Ended";
        case GestureState::Cancelled: return "Cancelled";
        case GestureState::Failed: return "Failed";
        default: return "Unknown";
    }
}

juce::String GestureRecognizer::swipeDirectionToString(SwipeDirection direction) const {
    switch (direction) {
        case SwipeDirection::None: return "None";
        case SwipeDirection::Up: return "Up";
        case SwipeDirection::Down: return "Down";
        case SwipeDirection::Left: return "Left";
        case SwipeDirection::Right: return "Right";
        case SwipeDirection::UpLeft: return "UpLeft";
        case SwipeDirection::UpRight: return "UpRight";
        case SwipeDirection::DownLeft: return "DownLeft";
        case SwipeDirection::DownRight: return "DownRight";
        default: return "Unknown";
    }
}