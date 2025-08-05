#pragma once

#include "JUCE8_CODING_STANDARDS.h"
#include <JuceHeader.h>
#include "../../ColorScheme.h"
#include "../../INIConfig.h"
#include "../../ComponentState.h"

/**
 * @file AudioScopeComponent.h
 * @brief Professional oscilloscope-style audio visualization component for OTTO JUCE 8 application
 * 
 * This component provides professional oscilloscope display with multiple trace modes,
 * real-time audio waveform visualization, and comprehensive measurement tools.
 * Designed for professional audio analysis and monitoring.
 * 
 * FEATURES:
 * - Professional oscilloscope display with multiple trace modes
 * - Real-time audio waveform visualization with configurable time scales
 * - Trigger modes: Auto, Normal, Single, External
 * - Multiple channel display with color-coded traces
 * - Measurement tools: Peak, RMS, Frequency, Phase
 * - Zoom and pan functionality with precise time/amplitude scaling
 * - Persistence mode for capturing transient events
 * - Export functionality for waveform analysis
 * 
 * TECHNICAL SPECIFICATIONS:
 * - High-resolution rendering at 44.1kHz/48kHz sample rates
 * - Configurable buffer sizes from 512 to 8192 samples
 * - Sub-pixel rendering for smooth waveform display
 * - Hardware-accelerated drawing using JUCE Graphics optimization
 * - Automatic gain control and DC offset removal
 * - Configurable trigger sensitivity and hysteresis
 * - Memory-efficient circular buffer implementation
 * 
 * INTEGRATION:
 * - Uses ColorScheme for consistent theming
 * - INI-driven configuration for all parameters
 * - Integrates with audio processing pipeline
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
 * @class AudioScopeComponent
 * @brief Professional oscilloscope-style audio visualization component
 * 
 * This class provides comprehensive oscilloscope functionality for real-time
 * audio analysis and visualization with professional measurement capabilities.
 */
class AudioScopeComponent : public juce::Component,
                           public juce::Timer
{
public:
    /**
     * @enum TriggerMode
     * @brief Oscilloscope trigger modes
     */
    enum class TriggerMode {
        Auto,       ///< Automatic triggering
        Normal,     ///< Normal triggering (waits for trigger condition)
        Single,     ///< Single shot triggering
        External,   ///< External trigger input
        Free        ///< Free running (no trigger)
    };

    /**
     * @enum TriggerSlope
     * @brief Trigger slope direction
     */
    enum class TriggerSlope {
        Rising,     ///< Trigger on rising edge
        Falling,    ///< Trigger on falling edge
        Both        ///< Trigger on both edges
    };

    /**
     * @enum TimeBase
     * @brief Time base settings for horizontal scale
     */
    enum class TimeBase {
        Microseconds_10,    ///< 10 μs/div
        Microseconds_25,    ///< 25 μs/div
        Microseconds_50,    ///< 50 μs/div
        Microseconds_100,   ///< 100 μs/div
        Microseconds_250,   ///< 250 μs/div
        Microseconds_500,   ///< 500 μs/div
        Milliseconds_1,     ///< 1 ms/div
        Milliseconds_2_5,   ///< 2.5 ms/div
        Milliseconds_5,     ///< 5 ms/div
        Milliseconds_10,    ///< 10 ms/div
        Milliseconds_25,    ///< 25 ms/div
        Milliseconds_50,    ///< 50 ms/div
        Milliseconds_100,   ///< 100 ms/div
        Milliseconds_250,   ///< 250 ms/div
        Milliseconds_500,   ///< 500 ms/div
        Seconds_1           ///< 1 s/div
    };

    /**
     * @enum VerticalScale
     * @brief Vertical scale settings for amplitude
     */
    enum class VerticalScale {
        Millivolts_1,       ///< 1 mV/div
        Millivolts_2,       ///< 2 mV/div
        Millivolts_5,       ///< 5 mV/div
        Millivolts_10,      ///< 10 mV/div
        Millivolts_20,      ///< 20 mV/div
        Millivolts_50,      ///< 50 mV/div
        Millivolts_100,     ///< 100 mV/div
        Millivolts_200,     ///< 200 mV/div
        Millivolts_500,     ///< 500 mV/div
        Volts_1,            ///< 1 V/div
        Volts_2,            ///< 2 V/div
        Volts_5             ///< 5 V/div
    };

    /**
     * @enum DisplayMode
     * @brief Waveform display modes
     */
    enum class DisplayMode {
        Normal,     ///< Normal waveform display
        Envelope,   ///< Envelope display (peak detection)
        XY,         ///< X-Y mode (channel 1 vs channel 2)
        FFT,        ///< FFT spectrum display
        Persistence ///< Persistence mode (trace accumulation)
    };

    /**
     * @struct ScopeSettings
     * @brief Configuration settings for the oscilloscope
     */
    struct ScopeSettings {
        TriggerMode triggerMode = TriggerMode::Auto;
        TriggerSlope triggerSlope = TriggerSlope::Rising;
        TimeBase timeBase = TimeBase::Milliseconds_1;
        VerticalScale verticalScale = VerticalScale::Volts_1;
        DisplayMode displayMode = DisplayMode::Normal;
        
        float triggerLevel = 0.0f;          ///< Trigger level (-1.0 to 1.0)
        float triggerHysteresis = 0.01f;    ///< Trigger hysteresis
        float horizontalPosition = 0.5f;    ///< Horizontal position (0.0 to 1.0)
        float verticalPosition = 0.5f;      ///< Vertical position (0.0 to 1.0)
        
        int bufferSize = 2048;              ///< Sample buffer size
        int numChannels = 2;                ///< Number of input channels
        float sampleRate = 48000.0f;        ///< Sample rate in Hz
        
        bool showGrid = true;               ///< Show oscilloscope grid
        bool showMeasurements = true;       ///< Show measurement overlay
        bool showCursors = false;           ///< Show measurement cursors
        bool autoScale = false;             ///< Automatic vertical scaling
        bool dcCoupling = true;             ///< DC coupling (false = AC coupling)
        
        float persistenceDecay = 0.95f;     ///< Persistence decay factor
        int refreshRate = 60;               ///< Display refresh rate in Hz
        
        juce::Colour backgroundColor = juce::Colours::black;
        juce::Colour gridColor = juce::Colour(0x40ffffff);
        juce::Colour traceColor1 = juce::Colours::yellow;
        juce::Colour traceColor2 = juce::Colours::cyan;
        juce::Colour triggerColor = juce::Colours::red;
        juce::Colour cursorColor = juce::Colours::white;
        juce::Colour textColor = juce::Colours::white;
    };

    /**
     * @struct ChannelData
     * @brief Audio data for a single channel
     */
    struct ChannelData {
        std::vector<float> buffer;          ///< Circular audio buffer
        std::vector<float> displayBuffer;   ///< Processed display buffer
        std::vector<float> persistenceBuffer; ///< Persistence accumulation buffer
        
        int writeIndex = 0;                 ///< Current write position
        int triggerIndex = 0;               ///< Trigger position
        bool triggered = false;             ///< Trigger state
        
        float dcOffset = 0.0f;              ///< DC offset for AC coupling
        float gain = 1.0f;                  ///< Channel gain
        float peakLevel = 0.0f;             ///< Peak level
        float rmsLevel = 0.0f;              ///< RMS level
        float frequency = 0.0f;             ///< Estimated frequency
        
        bool enabled = true;                ///< Channel enabled state
        juce::Colour color = juce::Colours::yellow;
    };

    /**
     * @struct MeasurementData
     * @brief Measurement results
     */
    struct MeasurementData {
        float peakToPeak = 0.0f;            ///< Peak-to-peak amplitude
        float rms = 0.0f;                   ///< RMS value
        float frequency = 0.0f;             ///< Fundamental frequency
        float period = 0.0f;                ///< Period
        float dutyCycle = 0.0f;             ///< Duty cycle (for square waves)
        float phase = 0.0f;                 ///< Phase (for multi-channel)
        float dcLevel = 0.0f;               ///< DC level
        float thd = 0.0f;                   ///< Total harmonic distortion
        
        bool valid = false;                 ///< Measurement validity
    };

    /**
     * @struct CursorData
     * @brief Measurement cursor data
     */
    struct CursorData {
        float timeA = 0.0f;                 ///< Time cursor A position
        float timeB = 0.0f;                 ///< Time cursor B position
        float voltageA = 0.0f;              ///< Voltage cursor A position
        float voltageB = 0.0f;              ///< Voltage cursor B position
        
        bool timeEnabled = false;           ///< Time cursors enabled
        bool voltageEnabled = false;        ///< Voltage cursors enabled
    };

    // Constructor and Destructor
    AudioScopeComponent();
    explicit AudioScopeComponent(const ScopeSettings& settings);
    ~AudioScopeComponent() override;

    // Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;

    // Timer override
    void timerCallback() override;

    // Audio Input
    /**
     * @brief Process audio buffer and update scope display
     * @param buffer The audio buffer to analyze
     */
    void processAudioBuffer(const juce::AudioBuffer<float>& buffer);

    /**
     * @brief Add audio sample to specific channel
     * @param channel Channel index
     * @param sample Audio sample value
     */
    void addSample(int channel, float sample);

    /**
     * @brief Add multiple samples to specific channel
     * @param channel Channel index
     * @param samples Array of sample values
     * @param numSamples Number of samples
     */
    void addSamples(int channel, const float* samples, int numSamples);

    /**
     * @brief Clear all audio buffers
     */
    void clearBuffers();

    /**
     * @brief Force trigger event
     */
    void forceTrigger();

    // Configuration
    /**
     * @brief Set scope settings
     * @param newSettings The new settings to apply
     */
    void setSettings(const ScopeSettings& newSettings);

    /**
     * @brief Get current scope settings
     * @return Current settings
     */
    const ScopeSettings& getSettings() const { return settings; }

    /**
     * @brief Set trigger mode
     * @param mode The trigger mode
     */
    void setTriggerMode(TriggerMode mode);

    /**
     * @brief Set trigger level
     * @param level Trigger level (-1.0 to 1.0)
     */
    void setTriggerLevel(float level);

    /**
     * @brief Set trigger slope
     * @param slope The trigger slope
     */
    void setTriggerSlope(TriggerSlope slope);

    /**
     * @brief Set time base
     * @param timeBase The time base setting
     */
    void setTimeBase(TimeBase timeBase);

    /**
     * @brief Set vertical scale
     * @param scale The vertical scale setting
     */
    void setVerticalScale(VerticalScale scale);

    /**
     * @brief Set display mode
     * @param mode The display mode
     */
    void setDisplayMode(DisplayMode mode);

    /**
     * @brief Set horizontal position
     * @param position Position (0.0 to 1.0)
     */
    void setHorizontalPosition(float position);

    /**
     * @brief Set vertical position
     * @param position Position (0.0 to 1.0)
     */
    void setVerticalPosition(float position);

    /**
     * @brief Set number of channels
     * @param numChannels Number of channels (1-8)
     */
    void setNumChannels(int numChannels);

    /**
     * @brief Set sample rate
     * @param sampleRate Sample rate in Hz
     */
    void setSampleRate(float sampleRate);

    /**
     * @brief Set buffer size
     * @param bufferSize Buffer size in samples
     */
    void setBufferSize(int bufferSize);

    // Channel Configuration
    /**
     * @brief Enable/disable channel
     * @param channel Channel index
     * @param enabled Whether channel is enabled
     */
    void setChannelEnabled(int channel, bool enabled);

    /**
     * @brief Set channel color
     * @param channel Channel index
     * @param color Channel trace color
     */
    void setChannelColor(int channel, const juce::Colour& color);

    /**
     * @brief Set channel gain
     * @param channel Channel index
     * @param gain Channel gain multiplier
     */
    void setChannelGain(int channel, float gain);

    /**
     * @brief Set DC coupling mode
     * @param dcCoupling True for DC coupling, false for AC coupling
     */
    void setDCCoupling(bool dcCoupling);

    // Visual Configuration
    /**
     * @brief Set scope colors
     * @param backgroundColor Background color
     * @param gridColor Grid line color
     * @param textColor Text color
     */
    void setColors(const juce::Colour& backgroundColor,
                   const juce::Colour& gridColor,
                   const juce::Colour& textColor);

    /**
     * @brief Enable/disable grid display
     * @param showGrid Whether to show grid
     */
    void setShowGrid(bool showGrid);

    /**
     * @brief Enable/disable measurement display
     * @param showMeasurements Whether to show measurements
     */
    void setShowMeasurements(bool showMeasurements);

    /**
     * @brief Enable/disable cursor display
     * @param showCursors Whether to show cursors
     */
    void setShowCursors(bool showCursors);

    /**
     * @brief Enable/disable auto scaling
     * @param autoScale Whether to enable auto scaling
     */
    void setAutoScale(bool autoScale);

    // Measurement Functions
    /**
     * @brief Get measurement data for channel
     * @param channel Channel index
     * @return Measurement data
     */
    MeasurementData getMeasurements(int channel) const;

    /**
     * @brief Get cursor data
     * @return Cursor measurement data
     */
    CursorData getCursorData() const { return cursorData; }

    /**
     * @brief Set cursor positions
     * @param timeA Time cursor A position
     * @param timeB Time cursor B position
     * @param voltageA Voltage cursor A position
     * @param voltageB Voltage cursor B position
     */
    void setCursorPositions(float timeA, float timeB, float voltageA, float voltageB);

    // Export Functions
    /**
     * @brief Export current waveform as image
     * @param file Output file
     * @param width Image width
     * @param height Image height
     * @return Success status
     */
    bool exportWaveform(const juce::File& file, int width = 1920, int height = 1080);

    /**
     * @brief Export waveform data as CSV
     * @param file Output file
     * @param channel Channel to export (-1 for all channels)
     * @return Success status
     */
    bool exportData(const juce::File& file, int channel = -1);

    /**
     * @brief Start recording waveform data
     * @param outputFile Output file for recording
     * @param duration Recording duration in seconds
     * @return Success status
     */
    bool startRecording(const juce::File& outputFile, float duration = 10.0f);

    /**
     * @brief Stop recording waveform data
     */
    void stopRecording();

    // State Management
    /**
     * @brief Save scope settings to component state
     * @param state The component state to save to
     */
    void saveToState(ComponentState& state) const;

    /**
     * @brief Load scope settings from component state
     * @param state The component state to load from
     */
    void loadFromState(const ComponentState& state);

    // Utility Functions
    /**
     * @brief Convert time base to seconds per division
     * @param timeBase The time base setting
     * @return Seconds per division
     */
    static float timeBaseToSeconds(TimeBase timeBase);

    /**
     * @brief Convert vertical scale to volts per division
     * @param scale The vertical scale setting
     * @return Volts per division
     */
    static float verticalScaleToVolts(VerticalScale scale);

    /**
     * @brief Get time base name
     * @param timeBase The time base setting
     * @return Human-readable time base name
     */
    static juce::String getTimeBaseName(TimeBase timeBase);

    /**
     * @brief Get vertical scale name
     * @param scale The vertical scale setting
     * @return Human-readable scale name
     */
    static juce::String getVerticalScaleName(VerticalScale scale);

    /**
     * @brief Convert screen coordinates to time/voltage
     * @param screenPoint Screen coordinates
     * @return Time (x) and voltage (y) values
     */
    juce::Point<float> screenToScope(const juce::Point<int>& screenPoint);

    /**
     * @brief Convert time/voltage to screen coordinates
     * @param scopePoint Time (x) and voltage (y) values
     * @return Screen coordinates
     */
    juce::Point<int> scopeToScreen(const juce::Point<float>& scopePoint);

    // Listeners
    /**
     * @class Listener
     * @brief Interface for scope event notifications
     */
    class Listener {
    public:
        virtual ~Listener() = default;
        
        /**
         * @brief Called when trigger event occurs
         * @param channel Channel that triggered
         * @param level Trigger level
         */
        virtual void scopeTriggered(int channel, float level) {}
        
        /**
         * @brief Called when measurements are updated
         * @param channel Channel index
         * @param measurements New measurement data
         */
        virtual void measurementsUpdated(int channel, const MeasurementData& measurements) {}
        
        /**
         * @brief Called when scope settings change
         * @param newSettings The new settings
         */
        virtual void scopeSettingsChanged(const ScopeSettings& newSettings) {}
        
        /**
         * @brief Called when cursor positions change
         * @param cursorData New cursor data
         */
        virtual void cursorsChanged(const CursorData& cursorData) {}
    };

    /**
     * @brief Add a listener for scope events
     * @param listener The listener to add
     */
    void addListener(Listener* listener);

    /**
     * @brief Remove a listener
     * @param listener The listener to remove
     */
    void removeListener(Listener* listener);

private:
    // Rendering Methods
    void paintBackground(juce::Graphics& g, const juce::Rectangle<int>& bounds);
    void paintGrid(juce::Graphics& g, const juce::Rectangle<int>& bounds);
    void paintWaveforms(juce::Graphics& g, const juce::Rectangle<int>& bounds);
    void paintTriggerLevel(juce::Graphics& g, const juce::Rectangle<int>& bounds);
    void paintCursors(juce::Graphics& g, const juce::Rectangle<int>& bounds);
    void paintMeasurements(juce::Graphics& g, const juce::Rectangle<int>& bounds);
    void paintChannelWaveform(juce::Graphics& g, const juce::Rectangle<int>& bounds, int channel);
    void paintFFTSpectrum(juce::Graphics& g, const juce::Rectangle<int>& bounds, int channel);
    void paintXYMode(juce::Graphics& g, const juce::Rectangle<int>& bounds);
    
    // Processing Methods
    void updateTrigger();
    void updateMeasurements();
    void updateDisplayBuffers();
    void processChannel(int channel);
    void applyDCCoupling(int channel);
    void detectTrigger(int channel);
    void calculateMeasurements(int channel);
    void updatePersistence();
    
    // Interaction Handling
    void handleCursorDrag(const juce::MouseEvent& event);
    void handlePositionDrag(const juce::MouseEvent& event);
    void handleTriggerDrag(const juce::MouseEvent& event);
    
    // Utility Methods
    void initializeBuffers();
    void resizeBuffers();
    void notifyListeners(std::function<void(Listener*)> callback);
    float interpolateSample(const std::vector<float>& buffer, float index) const;
    int findTriggerPoint(const std::vector<float>& buffer, int startIndex) const;
    void performFFT(int channel);
    
    // Recording Methods
    void recordSample(int channel, float sample);
    void saveRecordingData();

    // Member Variables
    ScopeSettings settings;
    std::vector<ChannelData> channelData;
    std::vector<MeasurementData> measurements;
    CursorData cursorData;
    
    // Display State
    juce::Rectangle<int> displayArea;
    float timePerPixel = 0.0f;
    float voltsPerPixel = 0.0f;
    int gridDivisions = 10;
    bool needsUpdate = true;
    
    // Trigger State
    bool triggerArmed = true;
    bool triggerHoldoff = false;
    int triggerHoldoffSamples = 0;
    int triggerHoldoffCounter = 0;
    
    // Interaction State
    bool isDragging = false;
    bool isDraggingCursor = false;
    bool isDraggingTrigger = false;
    juce::Point<int> lastMousePosition;
    int selectedCursor = -1; // -1: none, 0: timeA, 1: timeB, 2: voltageA, 3: voltageB
    
    // Recording State
    bool isRecording = false;
    juce::File recordingFile;
    float recordingDuration = 0.0f;
    float recordingTime = 0.0f;
    std::vector<std::vector<float>> recordingData;
    
    // Performance Monitoring
    float lastFrameTime = 0.0f;
    int frameCount = 0;
    float averageFPS = 60.0f;
    
    // FFT Processing
    std::unique_ptr<juce::dsp::FFT> fftProcessor;
    std::vector<std::vector<float>> fftData;
    std::vector<std::vector<float>> fftMagnitudes;
    
    // Listeners
    juce::ListenerList<Listener> listeners;
    
    // Constants
    static constexpr int MAX_CHANNELS = 8;
    static constexpr int MIN_BUFFER_SIZE = 512;
    static constexpr int MAX_BUFFER_SIZE = 8192;
    static constexpr float MIN_TRIGGER_HYSTERESIS = 0.001f;
    static constexpr float MAX_TRIGGER_HYSTERESIS = 0.1f;
    static constexpr int TRIGGER_HOLDOFF_SAMPLES = 100;
    static constexpr float DC_FILTER_CUTOFF = 1.0f; // Hz
    static constexpr int FFT_SIZE = 2048;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioScopeComponent)
};

} // namespace Visualizations
} // namespace UI
} // namespace OTTO