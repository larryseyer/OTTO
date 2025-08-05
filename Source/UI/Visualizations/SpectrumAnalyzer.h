#pragma once

#include "JUCE8_CODING_STANDARDS.h"
#include <JuceHeader.h>
#include "ComponentState.h"
#include "INIConfig.h"
#include "ColorScheme.h"

/**
 * @class SpectrumAnalyzer
 * @brief Real-time frequency spectrum visualization for OTTO Phase 9D
 * 
 * Provides professional-grade spectrum analysis with:
 * - Real-time FFT processing with configurable window sizes
 * - Multiple display modes (Linear, Logarithmic, Mel scale)
 * - Customizable frequency ranges and resolution
 * - Peak hold and averaging options
 * - GPU-accelerated rendering for smooth 60fps performance
 * - Integration with OTTO's theme system
 * 
 * Optimized for audio production environments with accurate frequency representation.
 */
class SpectrumAnalyzer : public juce::Component,
                        public juce::Timer,
                        public juce::AudioProcessorValueTreeState::Listener {
public:
    /**
     * @enum DisplayMode
     * @brief Different spectrum display modes
     */
    enum class DisplayMode {
        Linear,      // Linear frequency scale
        Logarithmic, // Logarithmic frequency scale
        MelScale,    // Mel scale (perceptually uniform)
        Octave,      // Octave-based display
        ThirdOctave  // Third-octave analysis
    };

    /**
     * @enum WindowType
     * @brief FFT window functions
     */
    enum class WindowType {
        Rectangular,
        Hanning,
        Hamming,
        Blackman,
        BlackmanHarris,
        Kaiser
    };

    /**
     * @enum AveragingMode
     * @brief Spectrum averaging modes
     */
    enum class AveragingMode {
        None,        // No averaging
        Exponential, // Exponential moving average
        Linear,      // Linear moving average
        PeakHold     // Peak hold with decay
    };

    /**
     * @struct AnalyzerSettings
     * @brief Configuration for spectrum analysis
     */
    struct AnalyzerSettings {
        int fftSize = 2048;                    // FFT size (512, 1024, 2048, 4096, 8192)
        int overlapFactor = 4;                 // Overlap factor for smoother updates
        DisplayMode displayMode = DisplayMode::Logarithmic;
        WindowType windowType = WindowType::Hanning;
        AveragingMode averagingMode = AveragingMode::Exponential;
        
        float minFrequency = 20.0f;            // Minimum frequency to display (Hz)
        float maxFrequency = 20000.0f;         // Maximum frequency to display (Hz)
        float minDecibels = -80.0f;            // Minimum dB level to display
        float maxDecibels = 0.0f;              // Maximum dB level to display
        
        float averagingFactor = 0.8f;          // Averaging factor (0.0 - 1.0)
        float peakHoldTime = 2.0f;             // Peak hold time in seconds
        float peakDecayRate = 12.0f;           // Peak decay rate in dB/second
        
        bool showPeakHold = true;              // Show peak hold lines
        bool showGrid = true;                  // Show frequency/amplitude grid
        bool showLabels = true;                // Show frequency/amplitude labels
        bool showCursor = true;                // Show frequency cursor
        
        juce::Colour spectrumColor = juce::Colour(0xff00ff00);
        juce::Colour peakHoldColor = juce::Colour(0xffff0000);
        juce::Colour gridColor = juce::Colour(0x40ffffff);
        juce::Colour backgroundColor = juce::Colour(0xff000000);
        
        AnalyzerSettings() = default;
    };

    /**
     * @struct FrequencyBin
     * @brief Information about a frequency bin
     */
    struct FrequencyBin {
        float frequency;
        float magnitude;
        float phase;
        float peakHold;
        juce::Time peakTime;
        
        FrequencyBin() : frequency(0), magnitude(-80.0f), phase(0), peakHold(-80.0f) {}
    };

    SpectrumAnalyzer(ColorScheme& colorScheme);
    ~SpectrumAnalyzer() override;

    // Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseMove(const juce::MouseEvent& e) override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;

    // Audio processing
    void processAudioBlock(const juce::AudioBuffer<float>& buffer);
    void setSampleRate(double newSampleRate);
    double getSampleRate() const { return sampleRate; }

    // Settings management
    void setAnalyzerSettings(const AnalyzerSettings& settings);
    AnalyzerSettings getAnalyzerSettings() const { return settings; }
    
    void setFFTSize(int size);
    int getFFTSize() const { return settings.fftSize; }
    
    void setDisplayMode(DisplayMode mode);
    DisplayMode getDisplayMode() const { return settings.displayMode; }
    
    void setFrequencyRange(float minFreq, float maxFreq);
    void setAmplitudeRange(float minDb, float maxDb);
    
    void setAveragingMode(AveragingMode mode);
    AveragingMode getAveragingMode() const { return settings.averagingMode; }
    
    void setAveragingFactor(float factor);
    float getAveragingFactor() const { return settings.averagingFactor; }

    // Display options
    void setShowPeakHold(bool show);
    bool getShowPeakHold() const { return settings.showPeakHold; }
    
    void setShowGrid(bool show);
    bool getShowGrid() const { return settings.showGrid; }
    
    void setShowLabels(bool show);
    bool getShowLabels() const { return settings.showLabels; }

    // Color customization
    void setSpectrumColor(const juce::Colour& color);
    void setPeakHoldColor(const juce::Colour& color);
    void setGridColor(const juce::Colour& color);
    void updateColorsFromTheme();

    // Frequency analysis
    float getFrequencyAtX(int x) const;
    int getXForFrequency(float frequency) const;
    float getMagnitudeAtFrequency(float frequency) const;
    juce::Array<FrequencyBin> getFrequencyBins() const;

    // Peak detection
    struct Peak {
        float frequency;
        float magnitude;
        float bandwidth;
        
        Peak(float freq, float mag, float bw = 0) : frequency(freq), magnitude(mag), bandwidth(bw) {}
    };
    
    juce::Array<Peak> detectPeaks(float threshold = -40.0f, int minPeakDistance = 5) const;
    Peak getFundamentalFrequency() const;

    // State management
    void saveState(ComponentState& state) const;
    void loadState(const ComponentState& state);

    // Timer override for updates
    void timerCallback() override;

    // AudioProcessorValueTreeState::Listener override
    void parameterChanged(const juce::String& parameterID, float newValue) override;

    // Performance monitoring
    struct PerformanceStats {
        double averageProcessingTime = 0.0;
        double maxProcessingTime = 0.0;
        int droppedFrames = 0;
        double cpuUsage = 0.0;
        
        void reset() {
            averageProcessingTime = 0.0;
            maxProcessingTime = 0.0;
            droppedFrames = 0;
            cpuUsage = 0.0;
        }
    };
    
    PerformanceStats getPerformanceStats() const { return performanceStats; }
    void resetPerformanceStats() { performanceStats.reset(); }

private:
    ColorScheme& colorScheme;
    AnalyzerSettings settings;
    
    // Audio processing
    double sampleRate = 44100.0;
    std::unique_ptr<juce::dsp::FFT> fft;
    std::unique_ptr<juce::dsp::WindowingFunction<float>> window;
    
    // FFT buffers
    juce::AudioBuffer<float> fftBuffer;
    std::vector<float> fftData;
    std::vector<float> magnitudeData;
    std::vector<float> phaseData;
    std::vector<float> averagedMagnitudes;
    std::vector<float> peakHoldData;
    std::vector<juce::Time> peakHoldTimes;
    
    // Circular buffer for audio input
    juce::AudioBuffer<float> circularBuffer;
    int circularBufferWritePos = 0;
    bool bufferReady = false;
    
    // Display data
    std::vector<juce::Point<float>> spectrumPath;
    std::vector<juce::Point<float>> peakHoldPath;
    juce::Path gridPath;
    
    // Mouse interaction
    bool showFrequencyCursor = false;
    float cursorFrequency = 1000.0f;
    juce::Point<int> lastMousePosition;
    
    // Performance monitoring
    PerformanceStats performanceStats;
    juce::Time lastProcessTime;
    std::vector<double> processingTimes;
    
    // Thread safety
    juce::CriticalSection bufferLock;
    juce::CriticalSection displayLock;
    
    // Initialization
    void initializeFFT();
    void initializeBuffers();
    void initializeWindow();
    
    // FFT processing
    void performFFT();
    void applyWindow();
    void calculateMagnitudes();
    void updateAveraging();
    void updatePeakHold();
    
    // Display calculations
    void updateSpectrumPath();
    void updatePeakHoldPath();
    void updateGridPath();
    
    // Coordinate transformations
    float frequencyToX(float frequency) const;
    float xToFrequency(float x) const;
    float magnitudeToY(float magnitude) const;
    float yToMagnitude(float y) const;
    
    // Drawing helpers
    void drawSpectrum(juce::Graphics& g);
    void drawPeakHold(juce::Graphics& g);
    void drawGrid(juce::Graphics& g);
    void drawLabels(juce::Graphics& g);
    void drawFrequencyCursor(juce::Graphics& g);
    
    // Grid calculation
    std::vector<float> calculateFrequencyGridLines() const;
    std::vector<float> calculateAmplitudeGridLines() const;
    juce::String formatFrequency(float frequency) const;
    juce::String formatAmplitude(float amplitude) const;
    
    // Color management
    void updateColors();
    juce::Colour getSpectrumColorAtFrequency(float frequency) const;
    
    // Performance optimization
    void optimizeForPerformance();
    bool shouldSkipFrame() const;
    void updatePerformanceStats(double processingTime);
    
    // Validation helpers
    bool isValidFFTSize(int size) const;
    bool isValidFrequencyRange(float minFreq, float maxFreq) const;
    bool isValidAmplitudeRange(float minDb, float maxDb) const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrumAnalyzer)
};