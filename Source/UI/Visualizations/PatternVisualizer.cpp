#include "PatternVisualizer.h"
#include "JUCE8_CODING_STANDARDS.h"
#include "../../INIDataManager.h"

#if JUCE_OPENGL
    #if JUCE_MAC
        #include <OpenGL/gl.h>
        #include <OpenGL/glext.h>
    #elif JUCE_WINDOWS
        #include <GL/gl.h>
        #include <GL/glext.h>
    #elif JUCE_LINUX
        #include <GL/gl.h>
        #include <GL/glext.h>
    #endif
#endif

namespace OTTO {
namespace UI {
namespace Visualizations {

//==============================================================================
// Shader Source Code
//==============================================================================

static const char* vertexShader = R"(
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 color;

uniform mat4 mvpMatrix;
uniform mat4 modelMatrix;
uniform mat4 normalMatrix;
uniform vec3 lightDirection;
uniform bool enableLighting;

out vec4 vertexColor;
out vec3 worldNormal;
out vec3 lightDir;

void main()
{
    gl_Position = mvpMatrix * vec4(position, 1.0);
    
    if (enableLighting) {
        worldNormal = normalize((normalMatrix * vec4(normal, 0.0)).xyz);
        lightDir = normalize(-lightDirection);
        
        // Basic diffuse lighting
        float diffuse = max(dot(worldNormal, lightDir), 0.2);
        vertexColor = vec4(color.rgb * diffuse, color.a);
    } else {
        vertexColor = color;
    }
}
)";

static const char* fragmentShader = R"(
#version 330 core

in vec4 vertexColor;
out vec4 fragColor;

void main()
{
    fragColor = vertexColor;
}
)";

static const char* gridVertexShader = R"(
#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 mvpMatrix;
uniform vec4 gridColor;

out vec4 vertexColor;

void main()
{
    gl_Position = mvpMatrix * vec4(position, 1.0);
    vertexColor = gridColor;
}
)";

static const char* gridFragmentShader = R"(
#version 330 core

in vec4 vertexColor;
out vec4 fragColor;

void main()
{
    fragColor = vertexColor;
}
)";

//==============================================================================
// Constructor and Destructor
//==============================================================================

PatternVisualizer::PatternVisualizer()
    : PatternVisualizer(VisualizerSettings{})
{
}

PatternVisualizer::PatternVisualizer(const VisualizerSettings& initialSettings)
    : settings(initialSettings)
{
    // Initialize pattern grid
    patternGrid.resize(MAX_CHANNELS);
    for (auto& channel : patternGrid) {
        channel.resize(MAX_STEPS);
    }
    
    // Set up OpenGL context
    openGLContext.setRenderer(this);
    openGLContext.attachTo(*this);
    openGLContext.setContinuousRepainting(true);
    
    // Initialize camera
    resetCamera();
    
    // Start timer for updates
    startTimer(1000 / settings.refreshRate);
    
    // Set initial size using INI constants
    setSize(static_cast<int>(getWidth() * 80.0f / 100.0f),  // Default 80% width
            static_cast<int>(getHeight() * 60.0f / 100.0f)); // Default 60% height
}

PatternVisualizer::~PatternVisualizer()
{
    stopTimer();
    openGLContext.detach();
}

//==============================================================================
// Component Overrides
//==============================================================================

void PatternVisualizer::paint(juce::Graphics& g)
{
    // OpenGL rendering is handled in renderOpenGL()
    // This is just for fallback when OpenGL is not available
    if (!openGLContext.isAttached()) {
        g.fillAll(settings.backgroundColor);
        g.setColour(juce::Colours::white);
        g.setFont(JUCE8_FONT(16.0f));
        g.drawText("3D Pattern Visualizer\n(OpenGL not available)", 
                   getLocalBounds(), juce::Justification::centred);
    }
}

void PatternVisualizer::resized()
{
    // Update projection matrix when component is resized
    if (glResources.initialized) {
        updateMatrices();
    }
    
    // Update timer interval based on size (larger visualizers can refresh slower for performance)
    auto area = getWidth() * getHeight();
    auto refreshRate = area > 500000 ? settings.refreshRate / 2 : settings.refreshRate;
    startTimer(1000 / refreshRate);
}

void PatternVisualizer::mouseDown(const juce::MouseEvent& event)
{
    isDragging = true;
    lastMousePosition = event.getPosition();
    
    if (event.mods.isRightButtonDown() || event.mods.isCommandDown()) {
        // Camera control mode
        isCameraControl = true;
    } else {
        // Note editing mode
        isCameraControl = false;
        handleNoteEditing(event);
    }
}

void PatternVisualizer::mouseDrag(const juce::MouseEvent& event)
{
    if (!isDragging) return;
    
    auto currentPosition = event.getPosition();
    auto delta = currentPosition - lastMousePosition;
    
    if (isCameraControl) {
        handleCameraControl(event);
    } else {
        // Continue note editing if dragging
        handleNoteEditing(event);
    }
    
    lastMousePosition = currentPosition;
}

void PatternVisualizer::mouseUp(const juce::MouseEvent& event)
{
    isDragging = false;
    isCameraControl = false;
    selectedNote = nullptr;
}

void PatternVisualizer::mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel)
{
    // Zoom camera with mouse wheel
    zoomCamera(wheel.deltaY * CAMERA_ZOOM_SPEED);
}

//==============================================================================
// OpenGL Renderer Overrides
//==============================================================================

void PatternVisualizer::newOpenGLContextCreated()
{
    initializeOpenGL();
}

void PatternVisualizer::renderOpenGL()
{
    if (!glResources.initialized) return;
    
    auto startTime = juce::Time::getMillisecondCounterHiRes();
    
    // Clear the screen
    juce::OpenGLHelpers::clear(settings.backgroundColor);
    
    // Enable depth testing and blending using JUCE's OpenGL context
    auto* context = juce::OpenGLContext::getCurrentContext();
    if (context != nullptr) {
        juce::gl::glEnable(juce::gl::GL_DEPTH_TEST);
        juce::gl::glDepthFunc(juce::gl::GL_LESS);
        juce::gl::glEnable(juce::gl::GL_BLEND);
        juce::gl::glBlendFunc(juce::gl::GL_SRC_ALPHA, juce::gl::GL_ONE_MINUS_SRC_ALPHA);
    }
    
    // Update matrices
    updateMatrices();
    auto mvpMatrix = camera.projectionMatrix * camera.viewMatrix * camera.modelMatrix;
    
    // Render grid if enabled
    if (settings.showGrid) {
        renderGrid(mvpMatrix);
    }
    
    // Render notes
    renderNotes(mvpMatrix);
    
    // Render playhead if enabled and playing
    if (settings.showPlayhead && isPatternPlaying) {
        renderPlayhead(mvpMatrix);
    }
    
    // Disable depth testing and blending
    juce::gl::glDisable(juce::gl::GL_DEPTH_TEST);
    juce::gl::glDisable(juce::gl::GL_BLEND);
    
    // Update performance metrics
    auto endTime = juce::Time::getMillisecondCounterHiRes();
    lastFrameTime = static_cast<float>(endTime - startTime);
    frameCount++;
    
    if (frameCount >= 60) {
        averageFPS = 60000.0f / (lastFrameTime * 60);
        frameCount = 0;
    }
    
    // Record frame if animation recording is active
    if (isRecordingAnimation) {
        recordFrame();
    }
}

void PatternVisualizer::openGLContextClosing()
{
    // Clean up OpenGL resources
    if (glResources.vertexBuffer != 0) {
        juce::gl::glDeleteBuffers(1, &glResources.vertexBuffer);
        glResources.vertexBuffer = 0;
    }
    
    if (glResources.indexBuffer != 0) {
        juce::gl::glDeleteBuffers(1, &glResources.indexBuffer);
        glResources.indexBuffer = 0;
    }
    
    if (glResources.gridVertexBuffer != 0) {
        juce::gl::glDeleteBuffers(1, &glResources.gridVertexBuffer);
        glResources.gridVertexBuffer = 0;
    }
    
    if (glResources.gridIndexBuffer != 0) {
        juce::gl::glDeleteBuffers(1, &glResources.gridIndexBuffer);
        glResources.gridIndexBuffer = 0;
    }
    
    if (glResources.vertexArrayObject != 0) {
        juce::gl::glDeleteVertexArrays(1, &glResources.vertexArrayObject);
        glResources.vertexArrayObject = 0;
    }
    
    if (glResources.gridVertexArrayObject != 0) {
        juce::gl::glDeleteVertexArrays(1, &glResources.gridVertexArrayObject);
        glResources.gridVertexArrayObject = 0;
    }
    
    glResources.shaderProgram.reset();
    glResources.gridShaderProgram.reset();
    glResources.initialized = false;
}

//==============================================================================
// Timer Override
//==============================================================================

void PatternVisualizer::timerCallback()
{
    // Update animation time if recording
    if (isRecordingAnimation) {
        animationTime += 1.0f / settings.refreshRate;
        if (animationTime >= animationDuration) {
            stopAnimationRecording();
        }
    }
    
    // Update LOD based on performance
    updateLOD();
    
    // Trigger repaint
    openGLContext.triggerRepaint();
}

//==============================================================================
// Pattern Data Management
//==============================================================================

void PatternVisualizer::setPattern(const juce::MidiMessageSequence& pattern)
{
    updatePatternFromMidi(pattern);
    updateVertexData();
}

void PatternVisualizer::addNote(int step, int channel, float velocity)
{
    if (step >= 0 && step < currentSteps && channel >= 0 && channel < currentChannels) {
        auto& note = patternGrid[channel][step];
        note.step = step;
        note.channel = channel;
        note.velocity = velocity;
        note.active = true;
        note.color = settings.noteColor;
        
        updateVertexData();
        notifyPatternChanged(step, channel, &note);
    }
}

void PatternVisualizer::removeNote(int step, int channel)
{
    if (step >= 0 && step < currentSteps && channel >= 0 && channel < currentChannels) {
        auto& note = patternGrid[channel][step];
        note.active = false;
        
        updateVertexData();
        notifyPatternChanged(step, channel, nullptr);
    }
}

void PatternVisualizer::toggleNote(int step, int channel, float velocity)
{
    if (step >= 0 && step < currentSteps && channel >= 0 && channel < currentChannels) {
        auto& note = patternGrid[channel][step];
        if (note.active) {
            removeNote(step, channel);
        } else {
            addNote(step, channel, velocity);
        }
    }
}

void PatternVisualizer::clearPattern()
{
    for (auto& channel : patternGrid) {
        for (auto& note : channel) {
            note.active = false;
        }
    }
    
    updateVertexData();
    notifyListeners([](Listener* l) { l->patternCleared(); });
}

juce::MidiMessageSequence PatternVisualizer::getCurrentPattern() const
{
    juce::MidiMessageSequence sequence;
    
    for (int channel = 0; channel < currentChannels; ++channel) {
        for (int step = 0; step < currentSteps; ++step) {
            const auto& note = patternGrid[channel][step];
            if (note.active) {
                auto timestamp = step * (1.0 / 4.0); // Quarter note per step
                auto noteOn = juce::MidiMessage::noteOn(channel + 1, 60 + channel, 
                                                       static_cast<juce::uint8>(note.velocity * 127));
                auto noteOff = juce::MidiMessage::noteOff(channel + 1, 60 + channel);
                
                sequence.addEvent(noteOn, timestamp);
                sequence.addEvent(noteOff, timestamp + 0.1); // Short note duration
            }
        }
    }
    
    return sequence;
}

//==============================================================================
// Playback Control
//==============================================================================

void PatternVisualizer::setPlaybackPosition(float position)
{
    playbackPosition = juce::jlimit(0.0f, static_cast<float>(currentSteps), position);
}

void PatternVisualizer::setPlaying(bool playing)
{
    isPatternPlaying = playing;
}

//==============================================================================
// Configuration
//==============================================================================

void PatternVisualizer::setSettings(const VisualizerSettings& newSettings)
{
    settings = newSettings;
    
    // Update timer
    startTimer(1000 / settings.refreshRate);
    
    // Update camera if view mode changed
    if (settings.viewMode != newSettings.viewMode) {
        resetCamera();
    }
    
    // Update OpenGL resources if quality changed
    if (settings.quality != newSettings.quality) {
        optimizeForQuality();
    }
    
    // Update vertex data if visual settings changed
    updateVertexData();
}

void PatternVisualizer::setViewMode(ViewMode mode)
{
    settings.viewMode = mode;
    resetCamera();
    notifyListeners([mode](Listener* l) { l->viewModeChanged(mode); });
}

void PatternVisualizer::setRenderQuality(RenderQuality quality)
{
    settings.quality = quality;
    optimizeForQuality();
}

void PatternVisualizer::setActiveLayer(PatternLayer layer)
{
    settings.activeLayer = layer;
    updateVertexData();
}

void PatternVisualizer::setPatternDimensions(int steps, int channels)
{
    currentSteps = juce::jlimit(1, MAX_STEPS, steps);
    currentChannels = juce::jlimit(1, MAX_CHANNELS, channels);
    
    // Clear pattern data beyond new dimensions
    for (int c = 0; c < MAX_CHANNELS; ++c) {
        for (int s = 0; s < MAX_STEPS; ++s) {
            if (c >= currentChannels || s >= currentSteps) {
                patternGrid[c][s].active = false;
            }
        }
    }
    
    updateVertexData();
}

//==============================================================================
// Camera Control
//==============================================================================

void PatternVisualizer::resetCamera()
{
    switch (settings.viewMode) {
        case ViewMode::TopDown:
            camera.position = {0.0f, 15.0f, 0.0f};
            camera.target = {0.0f, 0.0f, 0.0f};
            break;
            
        case ViewMode::Isometric:
            camera.position = {10.0f, 10.0f, 10.0f};
            camera.target = {0.0f, 0.0f, 0.0f};
            break;
            
        case ViewMode::Side:
            camera.position = {0.0f, 5.0f, 15.0f};
            camera.target = {0.0f, 0.0f, 0.0f};
            break;
            
        case ViewMode::Free:
            camera.position = {settings.cameraDistance * std::cos(settings.cameraAngleY) * std::cos(settings.cameraAngleX),
                              settings.cameraDistance * std::sin(settings.cameraAngleX),
                              settings.cameraDistance * std::sin(settings.cameraAngleY) * std::cos(settings.cameraAngleX)};
            camera.target = {0.0f, 0.0f, 0.0f};
            break;
    }
    
    camera.up = {0.0f, 1.0f, 0.0f};
    updateCamera();
}

void PatternVisualizer::setCameraPosition(const juce::Vector3D<float>& position, 
                                         const juce::Vector3D<float>& target)
{
    camera.position = position;
    camera.target = target;
    updateCamera();
    
    notifyListeners([position, target](Listener* l) { 
        l->cameraChanged(position, target); 
    });
}

void PatternVisualizer::rotateCamera(float deltaX, float deltaY)
{
    if (settings.viewMode != ViewMode::Free) return;
    
    settings.cameraAngleX += deltaY * CAMERA_ROTATION_SPEED;
    settings.cameraAngleY += deltaX * CAMERA_ROTATION_SPEED;
    
    // Clamp X angle to prevent flipping
    settings.cameraAngleX = juce::jlimit(-89.0f, 89.0f, settings.cameraAngleX);
    
    // Update camera position
    camera.position = {settings.cameraDistance * std::cos(juce::degreesToRadians(settings.cameraAngleY)) * std::cos(juce::degreesToRadians(settings.cameraAngleX)),
                      settings.cameraDistance * std::sin(juce::degreesToRadians(settings.cameraAngleX)),
                      settings.cameraDistance * std::sin(juce::degreesToRadians(settings.cameraAngleY)) * std::cos(juce::degreesToRadians(settings.cameraAngleX))};
    
    updateCamera();
}

void PatternVisualizer::zoomCamera(float delta)
{
    settings.cameraDistance = juce::jlimit(MIN_CAMERA_DISTANCE, MAX_CAMERA_DISTANCE, 
                                          settings.cameraDistance + delta);
    
    if (settings.viewMode == ViewMode::Free) {
        // Update camera position for free mode
        camera.position = {settings.cameraDistance * std::cos(juce::degreesToRadians(settings.cameraAngleY)) * std::cos(juce::degreesToRadians(settings.cameraAngleX)),
                          settings.cameraDistance * std::sin(juce::degreesToRadians(settings.cameraAngleX)),
                          settings.cameraDistance * std::sin(juce::degreesToRadians(settings.cameraAngleY)) * std::cos(juce::degreesToRadians(settings.cameraAngleX))};
    } else {
        // Scale position for other modes
        auto direction = camera.position - camera.target;
        direction = direction.normalised() * settings.cameraDistance;
        camera.position = camera.target + direction;
    }
    
    updateCamera();
}

//==============================================================================
// Visual Configuration
//==============================================================================

void PatternVisualizer::setColors(const juce::Colour& gridColor,
                                  const juce::Colour& noteColor,
                                  const juce::Colour& playheadColor,
                                  const juce::Colour& backgroundColor)
{
    settings.gridColor = gridColor;
    settings.noteColor = noteColor;
    settings.playheadColor = playheadColor;
    settings.backgroundColor = backgroundColor;
    
    updateVertexData();
}

void PatternVisualizer::setShowGrid(bool showGrid)
{
    settings.showGrid = showGrid;
}

void PatternVisualizer::setShowPlayhead(bool showPlayhead)
{
    settings.showPlayhead = showPlayhead;
}

void PatternVisualizer::setShowVelocity(bool showVelocity)
{
    settings.showVelocity = showVelocity;
    updateVertexData();
}

void PatternVisualizer::setEnableLighting(bool enableLighting)
{
    settings.enableLighting = enableLighting;
}

//==============================================================================
// Export Functions
//==============================================================================

bool PatternVisualizer::exportImage(const juce::File& file, int width, int height)
{
    // Create offscreen framebuffer for rendering
    juce::Image image(juce::Image::ARGB, width, height, true);
    
    // TODO: Implement offscreen rendering
    // This would require creating a separate OpenGL context and framebuffer
    
    // Use JUCE 8 API for saving images
    auto format = juce::ImageFileFormat::findImageFormatForFileExtension(file);
    if (format != nullptr) {
        juce::FileOutputStream stream(file);
        if (stream.openedOk()) {
            return format->writeImageToStream(image, stream);
        }
    }
    return false;
}

bool PatternVisualizer::startAnimationRecording(const juce::File& outputFolder, 
                                                float duration, 
                                                int fps)
{
    if (!outputFolder.exists()) {
        outputFolder.createDirectory();
    }
    
    animationOutputFolder = outputFolder;
    animationDuration = duration;
    animationFPS = fps;
    animationTime = 0.0f;
    animationFrameCount = 0;
    isRecordingAnimation = true;
    
    return true;
}

void PatternVisualizer::stopAnimationRecording()
{
    isRecordingAnimation = false;
}

//==============================================================================
// State Management
//==============================================================================

void PatternVisualizer::saveToState(ComponentState& state) const
{
    state.setValue("ViewMode", static_cast<int>(settings.viewMode));
    state.setValue("RenderQuality", static_cast<int>(settings.quality));
    state.setValue("ActiveLayer", static_cast<int>(settings.activeLayer));
    
    state.setValue("MaxSteps", settings.maxSteps);
    state.setValue("MaxChannels", settings.maxChannels);
    state.setValue("GridSpacing", settings.gridSpacing);
    state.setValue("NoteHeight", settings.noteHeight);
    
    state.setValue("ShowGrid", settings.showGrid);
    state.setValue("ShowPlayhead", settings.showPlayhead);
    state.setValue("ShowVelocity", settings.showVelocity);
    state.setValue("ShowTiming", settings.showTiming);
    state.setValue("EnableLighting", settings.enableLighting);
    state.setValue("EnableShadows", settings.enableShadows);
    
    state.setValue("CameraDistance", settings.cameraDistance);
    state.setValue("CameraAngleX", settings.cameraAngleX);
    state.setValue("CameraAngleY", settings.cameraAngleY);
    
    state.setValue("RefreshRate", settings.refreshRate);
    state.setValue("VsyncEnabled", settings.vsyncEnabled);
    
    // Save pattern data
    juce::String patternData;
    for (int c = 0; c < currentChannels; ++c) {
        for (int s = 0; s < currentSteps; ++s) {
            const auto& note = patternGrid[c][s];
            if (note.active) {
                patternData += juce::String(c) + "," + juce::String(s) + "," + 
                              juce::String(note.velocity) + "," + juce::String(note.timing) + 
                              "," + juce::String(note.probability) + ";";
            }
        }
    }
    state.setValue("PatternData", patternData);
}

void PatternVisualizer::loadFromState(const ComponentState& state)
{
    VisualizerSettings newSettings = settings;
    
    newSettings.viewMode = static_cast<ViewMode>(state.getIntValue("ViewMode", static_cast<int>(ViewMode::Isometric)));
    newSettings.quality = static_cast<RenderQuality>(state.getIntValue("RenderQuality", static_cast<int>(RenderQuality::Medium)));
    newSettings.activeLayer = static_cast<PatternLayer>(state.getIntValue("ActiveLayer", static_cast<int>(PatternLayer::Notes)));
    
    newSettings.maxSteps = state.getIntValue("MaxSteps", 64);
    newSettings.maxChannels = state.getIntValue("MaxChannels", 16);
    newSettings.gridSpacing = state.getFloatValue("GridSpacing", 1.0f);
    newSettings.noteHeight = state.getFloatValue("NoteHeight", 0.8f);
    
    newSettings.showGrid = state.getBoolValue("ShowGrid", true);
    newSettings.showPlayhead = state.getBoolValue("ShowPlayhead", true);
    newSettings.showVelocity = state.getBoolValue("ShowVelocity", true);
    newSettings.showTiming = state.getBoolValue("ShowTiming", false);
    newSettings.enableLighting = state.getBoolValue("EnableLighting", true);
    newSettings.enableShadows = state.getBoolValue("EnableShadows", false);
    
    newSettings.cameraDistance = state.getFloatValue("CameraDistance", 10.0f);
    newSettings.cameraAngleX = state.getFloatValue("CameraAngleX", 30.0f);
    newSettings.cameraAngleY = state.getFloatValue("CameraAngleY", 45.0f);
    
    newSettings.refreshRate = state.getIntValue("RefreshRate", 60);
    newSettings.vsyncEnabled = state.getBoolValue("VsyncEnabled", true);
    
    setSettings(newSettings);
    
    // Load pattern data
    auto patternData = state.getStringValue("PatternData", "");
    if (patternData.isNotEmpty()) {
        clearPattern();
        auto tokens = juce::StringArray::fromTokens(patternData, ";", "");
        for (const auto& token : tokens) {
            auto parts = juce::StringArray::fromTokens(token, ",", "");
            if (parts.size() >= 5) {
                int channel = parts[0].getIntValue();
                int step = parts[1].getIntValue();
                float velocity = parts[2].getFloatValue();
                float timing = parts[3].getFloatValue();
                float probability = parts[4].getFloatValue();
                
                if (channel >= 0 && channel < currentChannels && step >= 0 && step < currentSteps) {
                    auto& note = patternGrid[channel][step];
                    note.step = step;
                    note.channel = channel;
                    note.velocity = velocity;
                    note.timing = timing;
                    note.probability = probability;
                    note.active = true;
                    note.color = settings.noteColor;
                }
            }
        }
        updateVertexData();
    }
}

//==============================================================================
// Utility Functions
//==============================================================================

juce::Vector3D<float> PatternVisualizer::screenToWorld(const juce::Point<int>& screenPoint)
{
    // Convert screen coordinates to normalized device coordinates
    float x = (2.0f * screenPoint.x) / getWidth() - 1.0f;
    float y = 1.0f - (2.0f * screenPoint.y) / getHeight();
    
    // Simplified approach: use inverse MVP matrix for screen-to-world conversion
    // For now, return a simplified grid-based coordinate system
    // This is a temporary solution until proper matrix inversion is implemented
    
    // Map screen coordinates directly to grid coordinates
    float worldX = (x + 1.0f) * currentSteps * 0.5f;
    float worldZ = (y + 1.0f) * currentChannels * 0.5f;
    
    return juce::Vector3D<float>(worldX, 0.0f, worldZ);
}

juce::Point<int> PatternVisualizer::worldToScreen(const juce::Vector3D<float>& worldPoint)
{
    // Simplified approach: reverse the mapping from screenToWorld
    // This is a temporary solution until proper matrix transformation is implemented
    
    float normalizedX = (worldPoint.x / currentSteps) * 2.0f - 1.0f;
    float normalizedY = (worldPoint.z / currentChannels) * 2.0f - 1.0f;
    
    // Convert from normalized device coordinates to screen coordinates
    int x = static_cast<int>((normalizedX + 1.0f) * getWidth() / 2.0f);
    int y = static_cast<int>((1.0f - normalizedY) * getHeight() / 2.0f);
    
    return {x, y};
}

PatternVisualizer::PatternNote* PatternVisualizer::getNoteAtPosition(const juce::Point<int>& screenPoint)
{
    auto worldPoint = screenToWorld(screenPoint);
    
    // Convert world coordinates to grid coordinates
    int step = static_cast<int>(std::round(worldPoint.x / settings.gridSpacing));
    int channel = static_cast<int>(std::round(worldPoint.z / settings.gridSpacing));
    
    if (step >= 0 && step < currentSteps && channel >= 0 && channel < currentChannels) {
        auto& note = patternGrid[channel][step];
        if (note.active) {
            return &note;
        }
    }
    
    return nullptr;
}

//==============================================================================
// Listeners
//==============================================================================

void PatternVisualizer::addListener(Listener* listener)
{
    listeners.add(listener);
}

void PatternVisualizer::removeListener(Listener* listener)
{
    listeners.remove(listener);
}

//==============================================================================
// Private Methods
//==============================================================================

void PatternVisualizer::initializeOpenGL()
{
    // Create shaders
    createShaders();
    
    // Create buffers
    createBuffers();
    
    // Set initial OpenGL state
    juce::gl::glEnable(juce::gl::GL_DEPTH_TEST);
    juce::gl::glEnable(juce::gl::GL_BLEND);
    juce::gl::glBlendFunc(juce::gl::GL_SRC_ALPHA, juce::gl::GL_ONE_MINUS_SRC_ALPHA);
    
    glResources.initialized = true;
    
    // Initial vertex data update
    updateVertexData();
}

void PatternVisualizer::createShaders()
{
    // Create main shader program
    glResources.shaderProgram = std::make_unique<juce::OpenGLShaderProgram>(openGLContext);
    if (!glResources.shaderProgram->addVertexShader(vertexShader) ||
        !glResources.shaderProgram->addFragmentShader(fragmentShader) ||
        !glResources.shaderProgram->link()) {
        jassertfalse; // Shader compilation failed
    }
    
    // Create grid shader program
    glResources.gridShaderProgram = std::make_unique<juce::OpenGLShaderProgram>(openGLContext);
    if (!glResources.gridShaderProgram->addVertexShader(gridVertexShader) ||
        !glResources.gridShaderProgram->addFragmentShader(gridFragmentShader) ||
        !glResources.gridShaderProgram->link()) {
        jassertfalse; // Grid shader compilation failed
    }
}

void PatternVisualizer::createBuffers()
{
    // Generate vertex array objects
    juce::gl::glGenVertexArrays(1, &glResources.vertexArrayObject);
    juce::gl::glGenVertexArrays(1, &glResources.gridVertexArrayObject);
    
    // Generate buffers
    juce::gl::glGenBuffers(1, &glResources.vertexBuffer);
    juce::gl::glGenBuffers(1, &glResources.indexBuffer);
    juce::gl::glGenBuffers(1, &glResources.gridVertexBuffer);
    juce::gl::glGenBuffers(1, &glResources.gridIndexBuffer);
}

void PatternVisualizer::updateVertexData()
{
    if (!glResources.initialized) return;
    
    // Update note vertex data
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    
    unsigned int indexOffset = 0;
    
    for (int channel = 0; channel < currentChannels; ++channel) {
        for (int step = 0; step < currentSteps; ++step) {
            const auto& note = patternGrid[channel][step];
            if (!shouldRenderNote(note)) continue;
            
            // Calculate note position
            float x = step * settings.gridSpacing - (currentSteps * settings.gridSpacing) / 2.0f;
            float y = settings.showVelocity ? note.velocity * settings.noteHeight : settings.noteHeight / 2.0f;
            float z = channel * settings.gridSpacing - (currentChannels * settings.gridSpacing) / 2.0f;
            
            // Calculate note color based on layer
            juce::Colour noteColor = settings.noteColor;
            switch (settings.activeLayer) {
                case PatternLayer::Velocity:
                    noteColor = juce::Colour::fromHSV(0.3f * (1.0f - note.velocity), 1.0f, 1.0f, 1.0f);
                    break;
                case PatternLayer::Timing:
                    noteColor = juce::Colour::fromHSV(0.6f + 0.3f * note.timing, 1.0f, 1.0f, 1.0f);
                    break;
                case PatternLayer::Probability:
                    noteColor = juce::Colour::fromHSV(0.8f, note.probability, 1.0f, 1.0f);
                    break;
                default:
                    noteColor = note.color;
                    break;
            }
            
            // Create cube vertices (simplified - just front face for now)
            float size = 0.4f;
            
            // Front face vertices
            vertices.insert(vertices.end(), {
                x - size, y - size, z + size,  0.0f, 0.0f, 1.0f,  noteColor.getFloatRed(), noteColor.getFloatGreen(), noteColor.getFloatBlue(), noteColor.getFloatAlpha(),
                x + size, y - size, z + size,  0.0f, 0.0f, 1.0f,  noteColor.getFloatRed(), noteColor.getFloatGreen(), noteColor.getFloatBlue(), noteColor.getFloatAlpha(),
                x + size, y + size, z + size,  0.0f, 0.0f, 1.0f,  noteColor.getFloatRed(), noteColor.getFloatGreen(), noteColor.getFloatBlue(), noteColor.getFloatAlpha(),
                x - size, y + size, z + size,  0.0f, 0.0f, 1.0f,  noteColor.getFloatRed(), noteColor.getFloatGreen(), noteColor.getFloatBlue(), noteColor.getFloatAlpha()
            });
            
            // Front face indices
            indices.insert(indices.end(), {
                indexOffset, indexOffset + 1, indexOffset + 2,
                indexOffset, indexOffset + 2, indexOffset + 3
            });
            
            indexOffset += 4;
        }
    }
    
    // Update vertex buffer
    juce::gl::glBindVertexArray(glResources.vertexArrayObject);
    juce::gl::glBindBuffer(juce::gl::GL_ARRAY_BUFFER, glResources.vertexBuffer);
    juce::gl::glBufferData(juce::gl::GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), juce::gl::GL_DYNAMIC_DRAW);
    
    // Set vertex attributes
    juce::gl::glVertexAttribPointer(0, 3, juce::gl::GL_FLOAT, juce::gl::GL_FALSE, 10 * sizeof(float), (void*)0);
    juce::gl::glEnableVertexAttribArray(0);
    juce::gl::glVertexAttribPointer(1, 3, juce::gl::GL_FLOAT, juce::gl::GL_FALSE, 10 * sizeof(float), (void*)(3 * sizeof(float)));
    juce::gl::glEnableVertexAttribArray(1);
    juce::gl::glVertexAttribPointer(2, 4, juce::gl::GL_FLOAT, juce::gl::GL_FALSE, 10 * sizeof(float), (void*)(6 * sizeof(float)));
    juce::gl::glEnableVertexAttribArray(2);
    
    // Update index buffer
    juce::gl::glBindBuffer(juce::gl::GL_ELEMENT_ARRAY_BUFFER, glResources.indexBuffer);
    juce::gl::glBufferData(juce::gl::GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), juce::gl::GL_DYNAMIC_DRAW);
    
    juce::gl::glBindVertexArray(0);
    
    // Update grid vertex data
    std::vector<float> gridVertices;
    std::vector<unsigned int> gridIndices;
    
    // Create grid lines
    float gridSize = std::max(currentSteps, currentChannels) * settings.gridSpacing;
    float halfGrid = gridSize / 2.0f;
    
    unsigned int gridIndexOffset = 0;
    
    // Horizontal lines (along X axis)
    for (int i = 0; i <= currentChannels; ++i) {
        float z = i * settings.gridSpacing - (currentChannels * settings.gridSpacing) / 2.0f;
        gridVertices.insert(gridVertices.end(), {
            -halfGrid, 0.0f, z,
            halfGrid, 0.0f, z
        });
        gridIndices.insert(gridIndices.end(), {
            gridIndexOffset, gridIndexOffset + 1
        });
        gridIndexOffset += 2;
    }
    
    // Vertical lines (along Z axis)
    for (int i = 0; i <= currentSteps; ++i) {
        float x = i * settings.gridSpacing - (currentSteps * settings.gridSpacing) / 2.0f;
        gridVertices.insert(gridVertices.end(), {
            x, 0.0f, -halfGrid,
            x, 0.0f, halfGrid
        });
        gridIndices.insert(gridIndices.end(), {
            gridIndexOffset, gridIndexOffset + 1
        });
        gridIndexOffset += 2;
    }
    
    // Update grid buffers
    juce::gl::glBindVertexArray(glResources.gridVertexArrayObject);
    juce::gl::glBindBuffer(juce::gl::GL_ARRAY_BUFFER, glResources.gridVertexBuffer);
    juce::gl::glBufferData(juce::gl::GL_ARRAY_BUFFER, gridVertices.size() * sizeof(float), gridVertices.data(), juce::gl::GL_STATIC_DRAW);
    
    juce::gl::glVertexAttribPointer(0, 3, juce::gl::GL_FLOAT, juce::gl::GL_FALSE, 3 * sizeof(float), (void*)0);
    juce::gl::glEnableVertexAttribArray(0);
    
    juce::gl::glBindBuffer(juce::gl::GL_ELEMENT_ARRAY_BUFFER, glResources.gridIndexBuffer);
    juce::gl::glBufferData(juce::gl::GL_ELEMENT_ARRAY_BUFFER, gridIndices.size() * sizeof(unsigned int), gridIndices.data(), juce::gl::GL_STATIC_DRAW);
    
    juce::gl::glBindVertexArray(0);
}

void PatternVisualizer::renderGrid(const juce::Matrix3D<float>& mvpMatrix)
{
    if (!glResources.gridShaderProgram || !settings.showGrid) return;
    
    glResources.gridShaderProgram->use();
    
    // Set uniforms using JUCE 8 API
    glResources.gridShaderProgram->setUniformMat4("mvpMatrix", mvpMatrix.mat, 1, false);
    glResources.gridShaderProgram->setUniform("gridColor", 
                                             settings.gridColor.getFloatRed(),
                                             settings.gridColor.getFloatGreen(),
                                             settings.gridColor.getFloatBlue(),
                                             settings.gridColor.getFloatAlpha());
    
    // Render grid
    juce::gl::glBindVertexArray(glResources.gridVertexArrayObject);
    juce::gl::glDrawElements(juce::gl::GL_LINES, (currentSteps + 1 + currentChannels + 1) * 2, juce::gl::GL_UNSIGNED_INT, 0);
    juce::gl::glBindVertexArray(0);
}

void PatternVisualizer::renderNotes(const juce::Matrix3D<float>& mvpMatrix)
{
    if (!glResources.shaderProgram) return;
    
    glResources.shaderProgram->use();
    
    // Set uniforms using JUCE 8 API
    glResources.shaderProgram->setUniformMat4("mvpMatrix", mvpMatrix.mat, 1, false);
    glResources.shaderProgram->setUniformMat4("modelMatrix", camera.modelMatrix.mat, 1, false);
    glResources.shaderProgram->setUniformMat4("normalMatrix", camera.modelMatrix.mat, 1, false);
    glResources.shaderProgram->setUniform("lightDirection", 0.5f, -1.0f, 0.3f);
    glResources.shaderProgram->setUniform("enableLighting", settings.enableLighting ? 1 : 0);
    
    // Count active notes for rendering
    int activeNotes = 0;
    for (int channel = 0; channel < currentChannels; ++channel) {
        for (int step = 0; step < currentSteps; ++step) {
            if (shouldRenderNote(patternGrid[channel][step])) {
                activeNotes++;
            }
        }
    }
    
    // Render notes
    juce::gl::glBindVertexArray(glResources.vertexArrayObject);
    juce::gl::glDrawElements(juce::gl::GL_TRIANGLES, activeNotes * 6, juce::gl::GL_UNSIGNED_INT, 0);
    juce::gl::glBindVertexArray(0);
}

void PatternVisualizer::renderPlayhead(const juce::Matrix3D<float>& mvpMatrix)
{
    // TODO: Implement playhead rendering
    // This would render a vertical line or plane at the current playback position
}

void PatternVisualizer::updateCamera()
{
    // Simplified view matrix - JUCE 8 Matrix3D doesn't have fromLookAt
    // For now, use a simple translation matrix based on camera position
    camera.viewMatrix = juce::Matrix3D<float>::fromTranslation(juce::Vector3D<float>(-camera.position.x, -camera.position.y, -camera.position.z));
    
    notifyListeners([this](Listener* l) { 
        l->cameraChanged(camera.position, camera.target); 
    });
}

void PatternVisualizer::updateMatrices()
{
    // Update projection matrix
    float aspect = static_cast<float>(getWidth()) / static_cast<float>(getHeight());
    camera.projectionMatrix = juce::Matrix3D<float>::fromFrustum(-aspect, aspect, -1.0f, 1.0f, 
                                                                 camera.nearPlane, camera.farPlane);
    
    // Update model matrix (identity for now)
    camera.modelMatrix = juce::Matrix3D<float>();
}

void PatternVisualizer::updatePatternFromMidi(const juce::MidiMessageSequence& pattern)
{
    clearPattern();
    
    for (int i = 0; i < pattern.getNumEvents(); ++i) {
        auto* event = pattern.getEventPointer(i);
        if (event->message.isNoteOn()) {
            int channel = event->message.getChannel() - 1;
            int step = static_cast<int>(event->message.getTimeStamp() * 4.0); // Assuming quarter note steps
            float velocity = event->message.getVelocity() / 127.0f;
            
            if (channel >= 0 && channel < currentChannels && step >= 0 && step < currentSteps) {
                addNote(step, channel, velocity);
            }
        }
    }
}

void PatternVisualizer::notifyPatternChanged(int step, int channel, const PatternNote* note)
{
    notifyListeners([step, channel, note](Listener* l) { 
        l->patternNoteChanged(step, channel, note); 
    });
}

void PatternVisualizer::notifyListeners(std::function<void(Listener*)> callback)
{
    listeners.call(callback);
}

void PatternVisualizer::handleMouseInteraction(const juce::MouseEvent& event)
{
    if (isCameraControl) {
        handleCameraControl(event);
    } else {
        handleNoteEditing(event);
    }
}

void PatternVisualizer::handleCameraControl(const juce::MouseEvent& event)
{
    if (!isDragging) return;
    
    auto delta = event.getPosition() - lastMousePosition;
    rotateCamera(static_cast<float>(delta.x), static_cast<float>(delta.y));
}

void PatternVisualizer::handleNoteEditing(const juce::MouseEvent& event)
{
    auto* note = getNoteAtPosition(event.getPosition());
    
    if (event.mouseWasClicked()) {
        if (note) {
            selectedNote = note;
        } else {
            // Add new note at position
            auto worldPoint = screenToWorld(event.getPosition());
            int step = static_cast<int>(std::round(worldPoint.x / settings.gridSpacing + (currentSteps / 2.0f)));
            int channel = static_cast<int>(std::round(worldPoint.z / settings.gridSpacing + (currentChannels / 2.0f)));
            
            if (step >= 0 && step < currentSteps && channel >= 0 && channel < currentChannels) {
                toggleNote(step, channel);
            }
        }
    }
}

void PatternVisualizer::updateLOD()
{
    // Adjust quality based on performance
    if (averageFPS < 30.0f && settings.quality > RenderQuality::Low) {
        setRenderQuality(static_cast<RenderQuality>(static_cast<int>(settings.quality) - 1));
    } else if (averageFPS > 55.0f && settings.quality < RenderQuality::Ultra) {
        setRenderQuality(static_cast<RenderQuality>(static_cast<int>(settings.quality) + 1));
    }
}

void PatternVisualizer::optimizeForQuality()
{
    switch (settings.quality) {
        case RenderQuality::Low:
            settings.enableLighting = false;
            settings.enableShadows = false;
            settings.refreshRate = 30;
            break;
            
        case RenderQuality::Medium:
            settings.enableLighting = true;
            settings.enableShadows = false;
            settings.refreshRate = 60;
            break;
            
        case RenderQuality::High:
            settings.enableLighting = true;
            settings.enableShadows = true;
            settings.refreshRate = 60;
            break;
            
        case RenderQuality::Ultra:
            settings.enableLighting = true;
            settings.enableShadows = true;
            settings.refreshRate = 120;
            break;
    }
    
    startTimer(1000 / settings.refreshRate);
}

bool PatternVisualizer::shouldRenderNote(const PatternNote& note) const
{
    if (!note.active) return false;
    
    // LOD: Skip notes that are too small to see
    if (settings.quality == RenderQuality::Low && note.velocity < 0.1f) {
        return false;
    }
    
    return true;
}

void PatternVisualizer::recordFrame()
{
    if (!isRecordingAnimation) return;
    
    // TODO: Implement frame recording
    // This would capture the current OpenGL framebuffer and save it as an image
    
    animationFrameCount++;
}

void PatternVisualizer::saveAnimationFrame(const juce::Image& frame, int frameNumber)
{
    auto filename = juce::String::formatted("frame_%06d.png", frameNumber);
    auto file = animationOutputFolder.getChildFile(filename);
    
    // Use JUCE 8 API for saving images
    auto format = juce::ImageFileFormat::findImageFormatForFileExtension(file);
    if (format != nullptr) {
        juce::FileOutputStream stream(file);
        if (stream.openedOk()) {
            format->writeImageToStream(frame, stream);
        }
    }
}

} // namespace Visualizations
} // namespace UI
} // namespace OTTO