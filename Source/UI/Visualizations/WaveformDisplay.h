#pragma once

#include "JUCE8_CODING_STANDARDS.h"
#include <JuceHeader.h>
#include "ComponentState.h"
#include "INIConfig.h"
#include "ColorScheme.h"

/**
 * @class WaveformDisplay
 * @brief High-performance audio waveform visualization for OTTO Phase 9D
 * 
 * Provides professional waveform display with:
 * - Multi-resolution waveform rendering with automatic LOD
 * - Real-time audio scrubbing and editing capabilities
 * - Zoom and pan functionality with smooth animations
 * - Multiple display modes (Peak, RMS, Filled, Line)
 * - Selection and loop region visualization
 * - GPU-accelerated rendering for large audio files
 * - Integration with OTTO's theme and animation systems
 * 
 * Optimized for audio editing workflows with sample-accurate positioning.
 */
class WaveformDisplay : public juce::Component,
                       public juce::Timer,
                       public juce::ChangeListener {
public:
    /**
     * @enum DisplayMode
     * @brief Different waveform display modes
     */
    enum class DisplayMode {
        Peak,        // Peak waveform (traditional)
        RMS,         // RMS envelope
        Filled,      // Filled waveform
        Line,        // Center line only
        Stereo,      // Separate left/right channels
        MidSide      // Mid/Side representation
    };

    /**
     * @enum ZoomMode
     * @brief Zoom behavior modes
     */
    enum class ZoomMode {
        Horizontal,  // Horizontal zoom only
        Vertical,    // Vertical zoom only
        Both,        // Both horizontal and vertical
        Selection    // Zoom to selection
    };

    /**
     * @struct WaveformSettings
     * @brief Configuration for waveform display
     */
    struct WaveformSettings {
        DisplayMode displayMode = DisplayMode::Peak;
        ZoomMode zoomMode = ZoomMode::Horizontal;
        
        bool showRuler = true;              // Show time ruler
        bool showGrid = true;               // Show time grid
        bool showZeroCrossings = false;     // Highlight zero crossings
        bool showPeakMarkers = false;       // Show peak level markers
        bool showSelection = true;          // Show selection region
        bool showPlayhead = true;           // Show playback position
        bool showLoopRegion = true;         // Show loop region
        
        float verticalZoom = 1.0f;          // Vertical zoom factor
        double horizontalZoom = 1.0;        // Horizontal zoom factor
        double viewStart = 0.0;             // View start time in seconds
        double viewEnd = 10.0;              // View end time in seconds
        
        int pixelsPerSecond = 100;          // Pixels per second at 1x zoom
        int minPixelsPerSample = 1;         // Minimum pixels per sample
        int maxPixelsPerSample = 10;        // Maximum pixels per sample
        
        juce::Colour waveformColor = juce::Colour(0xff00ff00);
        juce::Colour backgroundColor = juce::Colour(0xff000000);
        juce::Colour gridColor = juce::Colour(0x40ffffff);
        juce::Colour selectionColor = juce::Colour(0x4000ff00);
        juce::Colour playheadColor = juce::Colour(0xffff0000);
        juce::Colour loopColor = juce::Colour(0x40ffff00);
        
        WaveformSettings() = default;
    };

    /**
     * @struct SelectionRegion
     * @brief Audio selection region
     */
    struct SelectionRegion {
        double startTime = 0.0;
        double endTime = 0.0;
        bool active = false;
        
        SelectionRegion() = default;
        SelectionRegion(double start, double end) : startTime(start), endTime(end), active(true) {}
        
        double getLength() const { return endTime - startTime; }
        bool contains(double time) const { return active && time >= startTime && time <= endTime; }
        void clear() { active = false; startTime = endTime = 0.0; }
    };

    /**
     * @struct LoopRegion
     * @brief Audio loop region
     */
    struct LoopRegion {
        double startTime = 0.0;
        double endTime = 0.0;
        bool enabled = false;
        
        LoopRegion() = default;
        LoopRegion(double start, double end) : startTime(start), endTime(end), enabled(true) {}
        
        double getLength() const { return endTime - startTime; }
        bool contains(double time) const { return enabled && time >= startTime && time <= endTime; }
        void clear() { enabled = false; startTime = endTime = 0.0; }
    };

    WaveformDisplay(ColorScheme& colorScheme);
    ~WaveformDisplay() override;

    // Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    void mouseMove(const juce::MouseEvent& e) override;
    void mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel) override;
    void mouseDoubleClick(const juce::MouseEvent& e) override;

    // Audio data management
    void setAudioBuffer(const juce::AudioBuffer<float>& buffer, double sampleRate);
    void setAudioFile(const juce::File& file);
    void clearAudioData();
    bool hasAudioData() const { return audioBuffer.getNumSamples() > 0; }
    
    double getAudioLength() const;
    int getNumChannels() const { return audioBuffer.getNumChannels(); }
    double getSampleRate() const { return sampleRate; }

    // Playback position
    void setPlayheadPosition(double timeInSeconds);
    double getPlayheadPosition() const { return playheadPosition; }
    void setPlayheadVisible(bool visible);
    bool isPlayheadVisible() const { return settings.showPlayhead; }

    // View control
    void setViewRange(double startTime, double endTime);
    void getViewRange(double& startTime, double& endTime) const;
    void zoomToFit();
    void zoomToSelection();
    void zoomIn(double centerTime = -1.0);
    void zoomOut(double centerTime = -1.0);
    void setZoomLevel(double zoomLevel);
    double getZoomLevel() const { return settings.horizontalZoom; }
    
    void setVerticalZoom(float zoom);
    float getVerticalZoom() const { return settings.verticalZoom; }

    // Selection management
    void setSelection(double startTime, double endTime);
    void setSelection(const SelectionRegion& selection);
    SelectionRegion getSelection() const { return selection; }
    void clearSelection();
    bool hasSelection() const { return selection.active; }

    // Loop region management
    void setLoopRegion(double startTime, double endTime);
    void setLoopRegion(const LoopRegion& loop);
    LoopRegion getLoopRegion() const { return loopRegion; }
    void clearLoopRegion();
    bool hasLoopRegion() const { return loopRegion.enabled; }

    // Display settings
    void setWaveformSettings(const WaveformSettings& newSettings);
    WaveformSettings getWaveformSettings() const { return settings; }
    
    void setDisplayMode(DisplayMode mode);
    DisplayMode getDisplayMode() const { return settings.displayMode; }
    
    void setShowRuler(bool show);
    void setShowGrid(bool show);
    void setShowZeroCrossings(bool show);

    // Color customization
    void setWaveformColor(const juce::Colour& color);
    void setSelectionColor(const juce::Colour& color);
    void setPlayheadColor(const juce::Colour& color);
    void updateColorsFromTheme();

    // Coordinate transformations
    double pixelToTime(int pixel) const;
    int timeToPixel(double time) const;
    float sampleToY(float sample, int channel = 0) const;
    float yToSample(float y, int channel = 0) const;

    // Audio analysis
    struct WaveformStats {
        float peakLevel = 0.0f;
        float rmsLevel = 0.0f;
        double duration = 0.0;
        int zeroCrossings = 0;
        float dynamicRange = 0.0f;
        
        void reset() {
            peakLevel = rmsLevel = dynamicRange = 0.0f;
            duration = 0.0;
            zeroCrossings = 0;
        }
    };
    
    WaveformStats analyzeWaveform() const;
    WaveformStats analyzeSelection() const;
    juce::Array<double> findZeroCrossings() const;
    juce::Array<double> findPeaks(float threshold = 0.5f) const;

    // State management
    void saveState(ComponentState& state) const;
    void loadState(const ComponentState& state);

    // Timer override for smooth animations
    void timerCallback() override;

    // ChangeListener override
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    // Listener interface for waveform events
    class Listener {
    public:
        virtual ~Listener() = default;
        virtual void waveformSelectionChanged(const SelectionRegion& newSelection) {}
        virtual void waveformPlayheadMoved(double newPosition) {}
        virtual void waveformZoomChanged(double newZoom) {}
        virtual void waveformViewRangeChanged(double startTime, double endTime) {}
        virtual void waveformLoopRegionChanged(const LoopRegion& newLoop) {}
    };
    
    void addListener(Listener* listener);
    void removeListener(Listener* listener);

private:
    ColorScheme& colorScheme;
    WaveformSettings settings;
    
    // Audio data
    juce::AudioBuffer<float> audioBuffer;
    double sampleRate = 44100.0;
    
    // Playback state
    double playheadPosition = 0.0;
    bool playheadVisible = true;
    
    // View state
    SelectionRegion selection;
    LoopRegion loopRegion;
    
    // Mouse interaction state
    enum class MouseMode {
        None,
        Selecting,
        DraggingSelection,
        DraggingPlayhead,
        DraggingLoopStart,
        DraggingLoopEnd,
        Panning,
        Zooming
    };
    
    MouseMode currentMouseMode = MouseMode::None;
    juce::Point<int> mouseDownPosition;
    double mouseDownTime = 0.0;
    SelectionRegion originalSelection;
    LoopRegion originalLoop;
    double originalViewStart = 0.0;
    double originalViewEnd = 0.0;
    
    // Waveform cache for different zoom levels
    struct WaveformCache {
        int samplesPerPixel = 1;
        std::vector<float> minValues;
        std::vector<float> maxValues;
        std::vector<float> rmsValues;
        bool valid = false;
        
        void clear() {
            minValues.clear();
            maxValues.clear();
            rmsValues.clear();
            valid = false;
        }
    };
    
    std::vector<WaveformCache> waveformCaches;
    int currentCacheLevel = 0;
    
    // Animation state
    struct AnimationState {
        bool animating = false;
        double targetViewStart = 0.0;
        double targetViewEnd = 0.0;
        double startViewStart = 0.0;
        double startViewEnd = 0.0;
        juce::Time animationStartTime;
        double animationDuration = 0.3; // seconds
        
        void reset() {
            animating = false;
            targetViewStart = targetViewEnd = 0.0;
            startViewStart = startViewEnd = 0.0;
        }
    };
    
    AnimationState animationState;
    
    // Listeners
    juce::ListenerList<Listener> listeners;
    
    // Thread safety
    juce::CriticalSection audioDataLock;
    
    // Initialization
    void initializeWaveformCaches();
    void updateWaveformCache(int cacheLevel);
    
    // Drawing methods
    void drawWaveform(juce::Graphics& g);
    void drawWaveformPeak(juce::Graphics& g, int channel, const juce::Rectangle<int>& area);
    void drawWaveformRMS(juce::Graphics& g, int channel, const juce::Rectangle<int>& area);
    void drawWaveformFilled(juce::Graphics& g, int channel, const juce::Rectangle<int>& area);
    void drawWaveformLine(juce::Graphics& g, int channel, const juce::Rectangle<int>& area);
    void drawWaveformStereo(juce::Graphics& g);
    void drawWaveformMidSide(juce::Graphics& g);
    
    void drawRuler(juce::Graphics& g);
    void drawGrid(juce::Graphics& g);
    void drawSelection(juce::Graphics& g);
    void drawPlayhead(juce::Graphics& g);
    void drawLoopRegion(juce::Graphics& g);
    void drawZeroCrossings(juce::Graphics& g);
    void drawPeakMarkers(juce::Graphics& g);
    
    // Cache management
    int getBestCacheLevel() const;
    void invalidateCache();
    void updateCacheIfNeeded();
    
    // Mouse handling helpers
    MouseMode getMouseModeForPosition(const juce::Point<int>& position);
    void handleSelectionDrag(const juce::MouseEvent& e);
    void handlePlayheadDrag(const juce::MouseEvent& e);
    void handleLoopDrag(const juce::MouseEvent& e);
    void handlePanning(const juce::MouseEvent& e);
    void handleZooming(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel);
    
    // Animation helpers
    void startViewAnimation(double targetStart, double targetEnd);
    void updateAnimation();
    double easeInOutCubic(double t) const;
    
    // Utility methods
    void constrainViewRange();
    void updatePixelsPerSecond();
    juce::String formatTime(double timeInSeconds) const;
    juce::Colour getWaveformColorForChannel(int channel) const;
    
    // Validation helpers
    bool isValidTimeRange(double startTime, double endTime) const;
    bool isValidZoomLevel(double zoom) const;
    
    // Notification helpers
    void notifySelectionChanged();
    void notifyPlayheadMoved();
    void notifyZoomChanged();
    void notifyViewRangeChanged();
    void notifyLoopRegionChanged();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformDisplay)
};