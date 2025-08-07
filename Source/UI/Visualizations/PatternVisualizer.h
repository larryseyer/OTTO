#pragma once

#include "JUCE8_CODING_STANDARDS.h"
#include <JuceHeader.h>
#include "../../ColorScheme.h"
#include "../../INIConfig.h"
#include "../../ComponentState.h"
#include "../../PatternManager.h"

/**
 * @file PatternVisualizer.h
 * @brief 3D Pattern visualization component for OTTO JUCE 8 application
 * 
 * This component provides professional 3D visualization of MIDI patterns with
 * real-time playback indication, multiple view modes, and interactive editing
 * capabilities. Designed for intuitive pattern creation and editing.
 * 
 * FEATURES:
 * - 3D grid visualization with depth and perspective
 * - Real-time pattern playback with beat highlighting
 * - Multiple view modes: Top-down, Isometric, Side view, Free camera
 * - Interactive pattern editing with 3D manipulation
 * - GPU-accelerated rendering for smooth 60fps performance
 * - Pattern layer visualization (velocity, timing, probability)
 * - Export capabilities for screenshots and animations
 * - Integration with existing pattern matrix system
 * 
 * TECHNICAL SPECIFICATIONS:
 * - Uses JUCE OpenGL context for 3D rendering
 * - LOD (Level of Detail) system for performance optimization
 * - Supports up to 64 steps per pattern with 16 drum channels
 * - Real-time audio synchronization with visual beat indicators
 * - Touch gesture support for 3D navigation
 * - Memory-efficient vertex buffer management
 * - Shader-based lighting and material system
 * 
 * INTEGRATION:
 * - Uses ColorScheme for consistent theming
 * - INI-driven configuration for all parameters
 * - Integrates with PatternManager for data access
 * - Supports ComponentState for persistence
 * 
 * @author OTTO Development Team
 * @version 1.0
 * @date 2025
 */

namespace OTTO {
namespace UI {
namespace Visualizations {

/**
 * @class PatternVisualizer
 * @brief 3D pattern visualization component with interactive editing
 * 
 * This class provides comprehensive 3D visualization of drum patterns with
 * real-time playback indication and interactive editing capabilities.
 */
class PatternVisualizer : public juce::Component,
                         public juce::OpenGLRenderer,
                         public juce::Timer
{
public:
    /**
     * @enum ViewMode
     * @brief 3D camera view modes
     */
    enum class ViewMode {
        TopDown,    ///< Top-down orthographic view
        Isometric,  ///< Isometric 3D view
        Side,       ///< Side view for timing visualization
        Free        ///< Free camera movement
    };

    /**
     * @enum RenderQuality
     * @brief Rendering quality levels for performance optimization
     */
    enum class RenderQuality {
        Low,        ///< Low quality for mobile/low-end devices
        Medium,     ///< Medium quality for standard use
        High,       ///< High quality for professional use
        Ultra       ///< Ultra quality for high-end systems
    };

    /**
     * @enum PatternLayer
     * @brief Pattern data layers for visualization
     */
    enum class PatternLayer {
        Notes,      ///< Basic note on/off visualization
        Velocity,   ///< Velocity intensity visualization
        Timing,     ///< Timing offset visualization
        Probability ///< Note probability visualization
    };

    /**
     * @struct VisualizerSettings
     * @brief Configuration settings for the visualizer
     */
    struct VisualizerSettings {
        ViewMode viewMode = ViewMode::Isometric;
        RenderQuality quality = RenderQuality::Medium;
        PatternLayer activeLayer = PatternLayer::Notes;
        
        int maxSteps = 64;              ///< Maximum pattern steps
        int maxChannels = 16;           ///< Maximum drum channels
        float gridSpacing = 1.0f;       ///< 3D grid spacing
        float noteHeight = 0.8f;        ///< Height of note cubes
        
        bool showGrid = true;           ///< Show 3D grid lines
        bool showPlayhead = true;       ///< Show playback position
        bool showVelocity = true;       ///< Show velocity as height/color
        bool showTiming = false;        ///< Show timing offsets
        bool enableLighting = true;     ///< Enable 3D lighting
        bool enableShadows = false;     ///< Enable shadow rendering
        
        float cameraDistance = 10.0f;   ///< Camera distance from center
        float cameraAngleX = 30.0f;     ///< Camera X rotation angle
        float cameraAngleY = 45.0f;     ///< Camera Y rotation angle
        
        int refreshRate = 60;           ///< Refresh rate in Hz
        bool vsyncEnabled = true;       ///< Enable vertical sync
        
        juce::Colour gridColor = juce::Colours::grey;
        juce::Colour noteColor = juce::Colours::white;
        juce::Colour playheadColor = juce::Colours::red;
        juce::Colour backgroundColor = juce::Colours::black;
    };

    /**
     * @struct PatternNote
     * @brief Represents a single note in the pattern
     */
    struct PatternNote {
        int step = 0;                   ///< Step position (0-63)
        int channel = 0;                ///< Drum channel (0-15)
        float velocity = 1.0f;          ///< Note velocity (0.0-1.0)
        float timing = 0.0f;            ///< Timing offset (-0.5 to 0.5)
        float probability = 1.0f;       ///< Note probability (0.0-1.0)
        bool active = false;            ///< Whether note is active
        
        juce::Colour color = juce::Colours::white;
    };

    /**
     * @struct Camera3D
     * @brief 3D camera state and controls
     */
    struct Camera3D {
        juce::Vector3D<float> position{0.0f, 5.0f, 10.0f};
        juce::Vector3D<float> target{0.0f, 0.0f, 0.0f};
        juce::Vector3D<float> up{0.0f, 1.0f, 0.0f};
        
        float fov = 45.0f;              ///< Field of view in degrees
        float nearPlane = 0.1f;         ///< Near clipping plane
        float farPlane = 100.0f;        ///< Far clipping plane
        
        // View matrices
        juce::Matrix3D<float> viewMatrix;
        juce::Matrix3D<float> projectionMatrix;
        juce::Matrix3D<float> modelMatrix;
    };

    // Constructor and Destructor
    PatternVisualizer();
    explicit PatternVisualizer(const VisualizerSettings& settings);
    ~PatternVisualizer() override;

    // Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;

    // OpenGL Renderer overrides
    void newOpenGLContextCreated() override;
    void renderOpenGL() override;
    void openGLContextClosing() override;

    // Timer override
    void timerCallback() override;

    // Pattern Data Management
    /**
     * @brief Set pattern data for visualization
     * @param pattern The MIDI pattern to visualize
     */
    void setPattern(const juce::MidiMessageSequence& pattern);

    /**
     * @brief Add a note to the pattern
     * @param step Step position
     * @param channel Drum channel
     * @param velocity Note velocity
     */
    void addNote(int step, int channel, float velocity = 1.0f);

    /**
     * @brief Remove a note from the pattern
     * @param step Step position
     * @param channel Drum channel
     */
    void removeNote(int step, int channel);

    /**
     * @brief Toggle a note in the pattern
     * @param step Step position
     * @param channel Drum channel
     * @param velocity Note velocity if adding
     */
    void toggleNote(int step, int channel, float velocity = 1.0f);

    /**
     * @brief Clear all notes from the pattern
     */
    void clearPattern();

    /**
     * @brief Get current pattern as MIDI sequence
     * @return The current pattern
     */
    juce::MidiMessageSequence getCurrentPattern() const;

    // Playback Control
    /**
     * @brief Set current playback position
     * @param position Position in steps (0.0 to pattern length)
     */
    void setPlaybackPosition(float position);

    /**
     * @brief Get current playback position
     * @return Current position in steps
     */
    float getPlaybackPosition() const { return playbackPosition; }

    /**
     * @brief Set whether pattern is playing
     * @param playing Whether pattern is playing
     */
    void setPlaying(bool playing);

    /**
     * @brief Get whether pattern is playing
     * @return Whether pattern is playing
     */
    bool isPlaying() const { return isPatternPlaying; }

    // Configuration
    /**
     * @brief Set visualizer settings
     * @param newSettings The new settings to apply
     */
    void setSettings(const VisualizerSettings& newSettings);

    /**
     * @brief Get current visualizer settings
     * @return Current settings
     */
    const VisualizerSettings& getSettings() const { return settings; }

    /**
     * @brief Set view mode
     * @param mode The view mode
     */
    void setViewMode(ViewMode mode);

    /**
     * @brief Set render quality
     * @param quality The render quality
     */
    void setRenderQuality(RenderQuality quality);

    /**
     * @brief Set active pattern layer
     * @param layer The pattern layer to visualize
     */
    void setActiveLayer(PatternLayer layer);

    /**
     * @brief Set pattern dimensions
     * @param steps Number of steps
     * @param channels Number of channels
     */
    void setPatternDimensions(int steps, int channels);

    // Camera Control
    /**
     * @brief Reset camera to default position
     */
    void resetCamera();

    /**
     * @brief Set camera position
     * @param position Camera position
     * @param target Camera target
     */
    void setCameraPosition(const juce::Vector3D<float>& position, 
                          const juce::Vector3D<float>& target);

    /**
     * @brief Rotate camera around target
     * @param deltaX X rotation delta
     * @param deltaY Y rotation delta
     */
    void rotateCamera(float deltaX, float deltaY);

    /**
     * @brief Zoom camera in/out
     * @param delta Zoom delta
     */
    void zoomCamera(float delta);

    // Visual Configuration
    /**
     * @brief Set visualization colors
     * @param gridColor Grid line color
     * @param noteColor Note cube color
     * @param playheadColor Playhead indicator color
     * @param backgroundColor Background color
     */
    void setColors(const juce::Colour& gridColor,
                   const juce::Colour& noteColor,
                   const juce::Colour& playheadColor,
                   const juce::Colour& backgroundColor);

    /**
     * @brief Enable/disable grid display
     * @param showGrid Whether to show grid
     */
    void setShowGrid(bool showGrid);

    /**
     * @brief Enable/disable playhead display
     * @param showPlayhead Whether to show playhead
     */
    void setShowPlayhead(bool showPlayhead);

    /**
     * @brief Enable/disable velocity visualization
     * @param showVelocity Whether to show velocity
     */
    void setShowVelocity(bool showVelocity);

    /**
     * @brief Enable/disable lighting
     * @param enableLighting Whether to enable lighting
     */
    void setEnableLighting(bool enableLighting);

    // Export Functions
    /**
     * @brief Export current view as image
     * @param file Output file
     * @param width Image width
     * @param height Image height
     * @return Success status
     */
    bool exportImage(const juce::File& file, int width = 1920, int height = 1080);

    /**
     * @brief Start recording animation
     * @param outputFolder Folder for animation frames
     * @param duration Animation duration in seconds
     * @param fps Frames per second
     * @return Success status
     */
    bool startAnimationRecording(const juce::File& outputFolder, 
                                float duration = 10.0f, 
                                int fps = 30);

    /**
     * @brief Stop recording animation
     */
    void stopAnimationRecording();

    // State Management
    /**
     * @brief Save visualizer settings to component state
     * @param state The component state to save to
     */
    void saveToState(ComponentState& state) const;

    /**
     * @brief Load visualizer settings from component state
     * @param state The component state to load from
     */
    void loadFromState(const ComponentState& state);

    // Utility Functions
    /**
     * @brief Convert screen coordinates to 3D world coordinates
     * @param screenPoint Screen coordinates
     * @return 3D world coordinates
     */
    juce::Vector3D<float> screenToWorld(const juce::Point<int>& screenPoint);

    /**
     * @brief Convert 3D world coordinates to screen coordinates
     * @param worldPoint 3D world coordinates
     * @return Screen coordinates
     */
    juce::Point<int> worldToScreen(const juce::Vector3D<float>& worldPoint);

    /**
     * @brief Get note at screen position
     * @param screenPoint Screen coordinates
     * @return Note at position (nullptr if none)
     */
    PatternNote* getNoteAtPosition(const juce::Point<int>& screenPoint);

    // Listeners
    /**
     * @class Listener
     * @brief Interface for pattern visualizer event notifications
     */
    class Listener {
    public:
        virtual ~Listener() = default;
        
        /**
         * @brief Called when a note is added/removed/modified
         * @param step Step position
         * @param channel Channel index
         * @param note The note data (nullptr if removed)
         */
        virtual void patternNoteChanged(int step, int channel, const PatternNote* note) {}
        
        /**
         * @brief Called when pattern is cleared
         */
        virtual void patternCleared() {}
        
        /**
         * @brief Called when view mode changes
         * @param newMode The new view mode
         */
        virtual void viewModeChanged(ViewMode newMode) {}
        
        /**
         * @brief Called when camera position changes
         * @param position New camera position
         * @param target New camera target
         */
        virtual void cameraChanged(const juce::Vector3D<float>& position,
                                 const juce::Vector3D<float>& target) {}
    };

    /**
     * @brief Add a listener for visualizer events
     * @param listener The listener to add
     */
    void addListener(Listener* listener);

    /**
     * @brief Remove a listener
     * @param listener The listener to remove
     */
    void removeListener(Listener* listener);

private:
    // OpenGL Resources
    struct OpenGLResources {
        std::unique_ptr<juce::OpenGLShaderProgram> shaderProgram;
        std::unique_ptr<juce::OpenGLShaderProgram> gridShaderProgram;
        
        GLuint vertexBuffer = 0;
        GLuint indexBuffer = 0;
        GLuint gridVertexBuffer = 0;
        GLuint gridIndexBuffer = 0;
        
        GLuint vertexArrayObject = 0;
        GLuint gridVertexArrayObject = 0;
        
        bool initialized = false;
    };

    // Rendering Methods
    void initializeOpenGL();
    void createShaders();
    void createBuffers();
    void updateVertexData();
    void renderGrid(const juce::Matrix3D<float>& mvpMatrix);
    void renderNotes(const juce::Matrix3D<float>& mvpMatrix);
    void renderPlayhead(const juce::Matrix3D<float>& mvpMatrix);
    void updateCamera();
    void updateMatrices();
    
    // Pattern Management
    void updatePatternFromMidi(const juce::MidiMessageSequence& pattern);
    void notifyPatternChanged(int step, int channel, const PatternNote* note);
    void notifyListeners(std::function<void(Listener&)> callback);
    
    // Interaction Handling
    void handleMouseInteraction(const juce::MouseEvent& event);
    void handleCameraControl(const juce::MouseEvent& event);
    void handleNoteEditing(const juce::MouseEvent& event);
    
    // Performance Optimization
    void updateLOD();
    void optimizeForQuality();
    bool shouldRenderNote(const PatternNote& note) const;
    
    // Animation Recording
    void recordFrame();
    void saveAnimationFrame(const juce::Image& frame, int frameNumber);

    // Member Variables
    VisualizerSettings settings;
    Camera3D camera;
    OpenGLResources glResources;
    
    // Pattern Data
    std::vector<std::vector<PatternNote>> patternGrid;
    int currentSteps = 16;
    int currentChannels = 8;
    float playbackPosition = 0.0f;
    bool isPatternPlaying = false;
    
    // Interaction State
    bool isDragging = false;
    bool isCameraControl = false;
    juce::Point<int> lastMousePosition;
    PatternNote* selectedNote = nullptr;
    
    // Animation Recording
    bool isRecordingAnimation = false;
    juce::File animationOutputFolder;
    float animationDuration = 0.0f;
    float animationTime = 0.0f;
    int animationFPS = 30;
    int animationFrameCount = 0;
    
    // Performance Monitoring
    float lastFrameTime = 0.0f;
    int frameCount = 0;
    float averageFPS = 60.0f;
    
    // OpenGL Context
    juce::OpenGLContext openGLContext;
    
    // Listeners
    juce::ListenerList<Listener> listeners;
    
    // Constants
    static constexpr int MAX_STEPS = 64;
    static constexpr int MAX_CHANNELS = 16;
    static constexpr float MIN_CAMERA_DISTANCE = 2.0f;
    static constexpr float MAX_CAMERA_DISTANCE = 50.0f;
    static constexpr float CAMERA_ROTATION_SPEED = 0.5f;
    static constexpr float CAMERA_ZOOM_SPEED = 0.1f;
    static constexpr float NOTE_SELECTION_THRESHOLD = 0.5f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatternVisualizer)
};

} // namespace Visualizations
} // namespace UI
} // namespace OTTO