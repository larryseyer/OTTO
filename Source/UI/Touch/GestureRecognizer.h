#pragma once

#include "JUCE8_CODING_STANDARDS.h"
#include <JuceHeader.h>
#include "ComponentState.h"
#include "INIConfig.h"

/**
 * @class GestureRecognizer
 * @brief Advanced multi-touch gesture recognition for OTTO Phase 9D
 * 
 * Provides comprehensive gesture recognition including:
 * - Multi-touch gesture detection (pinch, rotate, swipe, tap)
 * - Customizable gesture parameters and thresholds
 * - Real-time gesture state tracking and prediction
 * - Platform-specific optimizations (iOS, Android, Windows Touch)
 * - Integration with OTTO's animation and UI systems
 * - Haptic feedback coordination
 * 
 * Designed for professional audio production workflows with precise control.
 */
class GestureRecognizer : public juce::MouseListener {
public:
    /**
     * @enum GestureType
     * @brief Types of recognized gestures
     */
    enum class GestureType {
        None,
        Tap,           // Single tap
        DoubleTap,     // Double tap
        LongPress,     // Long press/hold
        Pan,           // Single finger pan/drag
        Pinch,         // Two finger pinch (zoom)
        Rotate,        // Two finger rotation
        Swipe,         // Fast directional movement
        TwoFingerTap,  // Two finger tap
        ThreeFingerTap,// Three finger tap
        FourFingerTap, // Four finger tap
        TwoFingerPan,  // Two finger pan
        ThreeFingerPan,// Three finger pan
        EdgeSwipe,     // Swipe from screen edge
        Custom         // User-defined gesture
    };

    /**
     * @enum GestureState
     * @brief Current state of gesture recognition
     */
    enum class GestureState {
        Idle,          // No gesture in progress
        Possible,      // Gesture might be starting
        Began,         // Gesture has begun
        Changed,       // Gesture is continuing/changing
        Ended,         // Gesture has ended successfully
        Cancelled,     // Gesture was cancelled
        Failed         // Gesture recognition failed
    };

    /**
     * @enum SwipeDirection
     * @brief Direction of swipe gestures
     */
    enum class SwipeDirection {
        None,
        Up,
        Down,
        Left,
        Right,
        UpLeft,
        UpRight,
        DownLeft,
        DownRight
    };

    /**
     * @struct TouchPoint
     * @brief Information about a single touch point
     */
    struct TouchPoint {
        int touchId = -1;
        juce::Point<float> position;
        juce::Point<float> startPosition;
        juce::Point<float> velocity;
        juce::Time startTime;
        juce::Time lastUpdateTime;
        float pressure = 1.0f;
        float radius = 10.0f;
        bool active = false;
        
        TouchPoint() = default;
        TouchPoint(int id, const juce::Point<float>& pos) 
            : touchId(id), position(pos), startPosition(pos), 
              startTime(juce::Time::getCurrentTime()), 
              lastUpdateTime(juce::Time::getCurrentTime()), active(true) {}
        
        float getDistance() const { return startPosition.getDistanceFrom(position); }
        double getDuration() const { return (juce::Time::getCurrentTime() - startTime).inSeconds(); }
        juce::Point<float> getDelta() const { return position - startPosition; }
    };

    /**
     * @struct GestureInfo
     * @brief Complete information about a recognized gesture
     */
    struct GestureInfo {
        GestureType type = GestureType::None;
        GestureState state = GestureState::Idle;
        
        juce::Point<float> position;        // Current gesture position
        juce::Point<float> startPosition;   // Starting position
        juce::Point<float> velocity;        // Current velocity
        juce::Point<float> translation;     // Total translation
        
        float scale = 1.0f;                 // Pinch scale factor
        float rotation = 0.0f;              // Rotation angle in radians
        float pressure = 1.0f;              // Average pressure
        
        SwipeDirection swipeDirection = SwipeDirection::None;
        float swipeVelocity = 0.0f;
        
        int touchCount = 0;                 // Number of active touches
        std::vector<TouchPoint> touchPoints;
        
        juce::Time startTime;
        juce::Time lastUpdateTime;
        double duration = 0.0;
        
        juce::Component* targetComponent = nullptr;
        
        GestureInfo() : startTime(juce::Time::getCurrentTime()), lastUpdateTime(juce::Time::getCurrentTime()) {}
        
        void reset() {
            type = GestureType::None;
            state = GestureState::Idle;
            position = startPosition = velocity = translation = juce::Point<float>();
            scale = 1.0f;
            rotation = pressure = 0.0f;
            swipeDirection = SwipeDirection::None;
            swipeVelocity = 0.0f;
            touchCount = 0;
            touchPoints.clear();
            startTime = lastUpdateTime = juce::Time::getCurrentTime();
            duration = 0.0;
            targetComponent = nullptr;
        }
    };

    /**
     * @struct GestureSettings
     * @brief Configuration for gesture recognition
     */
    struct GestureSettings {
        // Tap settings
        double tapMaxDuration = 0.3;           // Maximum tap duration (seconds)
        float tapMaxMovement = 10.0f;          // Maximum movement for tap (pixels)
        double doubleTapMaxInterval = 0.5;     // Maximum interval between taps
        float doubleTapMaxDistance = 50.0f;    // Maximum distance between taps
        
        // Long press settings
        double longPressMinDuration = 0.8;     // Minimum long press duration
        float longPressMaxMovement = 10.0f;    // Maximum movement for long press
        
        // Pan settings
        float panMinDistance = 10.0f;          // Minimum distance to start pan
        float panVelocityThreshold = 50.0f;    // Minimum velocity for pan
        
        // Pinch settings
        float pinchMinScale = 0.1f;            // Minimum pinch scale
        float pinchMaxScale = 10.0f;           // Maximum pinch scale
        float pinchMinDistance = 20.0f;        // Minimum distance between fingers
        float pinchThreshold = 5.0f;           // Minimum scale change to trigger
        
        // Rotation settings
        float rotationMinAngle = 0.1f;         // Minimum rotation angle (radians)
        float rotationThreshold = 0.05f;       // Minimum angle change to trigger
        
        // Swipe settings
        float swipeMinVelocity = 200.0f;       // Minimum swipe velocity (pixels/second)
        float swipeMaxDuration = 1.0;          // Maximum swipe duration
        float swipeMinDistance = 50.0f;        // Minimum swipe distance
        float swipeDirectionTolerance = 0.5f;  // Direction tolerance (radians)
        
        // Edge swipe settings
        float edgeSwipeMargin = 20.0f;         // Edge margin for edge swipes
        
        // Multi-touch settings
        int maxTouchPoints = 10;               // Maximum simultaneous touch points
        double touchTimeoutDuration = 5.0;     // Touch point timeout
        
        // Velocity calculation
        int velocityHistorySize = 5;           // Number of points for velocity calculation
        double velocityTimeWindow = 0.1;       // Time window for velocity calculation
        
        // Platform-specific settings
        bool enableHapticFeedback = true;      // Enable haptic feedback
        float hapticIntensity = 0.5f;          // Haptic feedback intensity
        
        GestureSettings() = default;
    };

    GestureRecognizer();
    ~GestureRecognizer() override;

    // Configuration
    void setGestureSettings(const GestureSettings& settings);
    GestureSettings getGestureSettings() const { return settings; }
    
    void setEnabledGestures(const std::set<GestureType>& gestures);
    std::set<GestureType> getEnabledGestures() const { return enabledGestures; }
    
    void enableGesture(GestureType gesture, bool enabled = true);
    bool isGestureEnabled(GestureType gesture) const;

    // Component attachment
    void attachToComponent(juce::Component* component);
    void detachFromComponent(juce::Component* component);
    juce::Array<juce::Component*> getAttachedComponents() const;

    // Gesture recognition
    void processMouseEvent(const juce::MouseEvent& e, juce::Component* component);
    void processTouchEvent(const juce::MouseEvent& e, juce::Component* component);
    
    // Current gesture state
    GestureInfo getCurrentGesture() const { return currentGesture; }
    GestureType getCurrentGestureType() const { return currentGesture.type; }
    GestureState getCurrentGestureState() const { return currentGesture.state; }
    bool isGestureActive() const { return currentGesture.state != GestureState::Idle; }

    // Touch point management
    int getActiveTouchCount() const;
    std::vector<TouchPoint> getActiveTouchPoints() const;
    TouchPoint getTouchPoint(int touchId) const;
    bool hasTouchPoint(int touchId) const;

    // Gesture prediction
    GestureType predictGesture() const;
    float getGestureConfidence(GestureType gesture) const;
    
    // Custom gesture support
    struct CustomGesturePattern {
        juce::String name;
        std::vector<juce::Point<float>> pattern;
        float tolerance = 50.0f;
        double maxDuration = 2.0;
        int minTouchPoints = 1;
        int maxTouchPoints = 1;
        
        CustomGesturePattern(const juce::String& gestureName) : name(gestureName) {}
    };
    
    void addCustomGesture(const CustomGesturePattern& pattern);
    void removeCustomGesture(const juce::String& name);
    juce::StringArray getCustomGestureNames() const;

    // MouseListener overrides
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    void mouseMove(const juce::MouseEvent& e) override;
    void mouseDoubleClick(const juce::MouseEvent& e) override;

    // State management
    void saveState(ComponentState& state) const;
    void loadState(const ComponentState& state);
    
    void reset();
    void cancelCurrentGesture();

    // Listener interface
    class Listener {
    public:
        virtual ~Listener() = default;
        virtual void gestureStarted(const GestureInfo& gesture) {}
        virtual void gestureChanged(const GestureInfo& gesture) {}
        virtual void gestureEnded(const GestureInfo& gesture) {}
        virtual void gestureCancelled(const GestureInfo& gesture) {}
        virtual void customGestureRecognized(const juce::String& gestureName, const GestureInfo& gesture) {}
    };
    
    void addListener(Listener* listener);
    void removeListener(Listener* listener);

    // Performance monitoring
    struct PerformanceStats {
        int gesturesRecognized = 0;
        int gesturesFailed = 0;
        double averageRecognitionTime = 0.0;
        double maxRecognitionTime = 0.0;
        
        void reset() {
            gesturesRecognized = gesturesFailed = 0;
            averageRecognitionTime = maxRecognitionTime = 0.0;
        }
    };
    
    PerformanceStats getPerformanceStats() const { return performanceStats; }
    void resetPerformanceStats() { performanceStats.reset(); }

private:
    GestureSettings settings;
    std::set<GestureType> enabledGestures;
    
    // Current gesture state
    GestureInfo currentGesture;
    GestureInfo previousGesture;
    
    // Touch tracking
    std::unordered_map<int, TouchPoint> activeTouches;
    std::vector<TouchPoint> touchHistory;
    
    // Gesture recognition state
    juce::Time lastTapTime;
    juce::Point<float> lastTapPosition;
    int tapCount = 0;
    
    // Custom gestures
    std::vector<CustomGesturePattern> customGestures;
    
    // Component tracking
    juce::Array<juce::Component*> attachedComponents;
    
    // Listeners
    juce::ListenerList<Listener> listeners;
    
    // Performance monitoring
    PerformanceStats performanceStats;
    juce::Time recognitionStartTime;
    
    // Platform-specific
    #if JUCE_IOS || JUCE_ANDROID
    std::unique_ptr<juce::HapticFeedback> hapticFeedback;
    #endif
    
    // Gesture recognition methods
    void updateGestureRecognition();
    void recognizeTapGesture();
    void recognizeDoubleTapGesture();
    void recognizeLongPressGesture();
    void recognizePanGesture();
    void recognizePinchGesture();
    void recognizeRotationGesture();
    void recognizeSwipeGesture();
    void recognizeMultiTouchGestures();
    void recognizeCustomGestures();
    
    // Touch point management
    void addTouchPoint(int touchId, const juce::Point<float>& position, juce::Component* component);
    void updateTouchPoint(int touchId, const juce::Point<float>& position);
    void removeTouchPoint(int touchId);
    void cleanupInactiveTouches();
    
    // Gesture state management
    void startGesture(GestureType type);
    void updateGesture();
    void endGesture();
    void cancelGesture();
    void failGesture();
    
    // Calculation helpers
    juce::Point<float> calculateCentroid() const;
    float calculateAverageDistance() const;
    float calculateScale() const;
    float calculateRotation() const;
    juce::Point<float> calculateVelocity(int touchId) const;
    SwipeDirection calculateSwipeDirection(const juce::Point<float>& delta) const;
    
    // Validation helpers
    bool isValidTap(const TouchPoint& touch) const;
    bool isValidLongPress(const TouchPoint& touch) const;
    bool isValidPan(const TouchPoint& touch) const;
    bool isValidSwipe(const TouchPoint& touch) const;
    bool isValidPinch() const;
    bool isValidRotation() const;
    
    // Custom gesture matching
    float matchCustomGesture(const CustomGesturePattern& pattern) const;
    std::vector<juce::Point<float>> normalizeGesturePath(const std::vector<juce::Point<float>>& path) const;
    
    // Haptic feedback
    void triggerHapticFeedback(GestureType gesture);
    
    // Notification helpers
    void notifyGestureStarted();
    void notifyGestureChanged();
    void notifyGestureEnded();
    void notifyGestureCancelled();
    void notifyCustomGestureRecognized(const juce::String& gestureName);
    
    // Performance tracking
    void updatePerformanceStats();
    
    // Utility methods
    juce::String gestureTypeToString(GestureType type) const;
    juce::String gestureStateToString(GestureState state) const;
    juce::String swipeDirectionToString(SwipeDirection direction) const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GestureRecognizer)
};