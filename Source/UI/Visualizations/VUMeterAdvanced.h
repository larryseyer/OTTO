#pragma once

#include "JUCE8_CODING_STANDARDS.h"
#include <JuceHeader.h>
#include "../../ColorScheme.h"
#include "../../INIConfig.h"

/**
 * @file VUMeterAdvanced.h
 * @brief Professional VU meter component for OTTO JUCE 8 application
 * 
 * This component provides professional-grade VU meters with multiple display modes,
 * peak hold, RMS averaging, and customizable ballistics. Designed for studio-quality
 * audio monitoring with precise level indication.
 * 
 * FEATURES:
 * - Multiple meter types (VU, PPM, Peak, RMS, LUFS)
 * - Horizontal and vertical orientations
 * - Peak hold with configurable decay
 * - Customizable ballistics and response times
 * - Professional scale markings and calibration
 * - Stereo and multi-channel support
 * - GPU-accelerated rendering for smooth animation
 * 
 * INTEGRATION:
 * - Uses ColorScheme for consistent theming
 * - INI-driven configuration for all parameters
 * - Integrates with audio processing pipeline
 * - Supports real-time audio analysis
 * 
 * @author OTTO Development Team
 * @version 1.0
 * @date 2025
 */

namespace OTTO {
namespace UI {
namespace Visualizations {

/**
 * @class VUMeterAdvanced
 * @brief Professional VU meter with advanced features and customization
 * 
 * This class provides a comprehensive VU meter implementation suitable for
 * professional audio applications. It supports multiple meter types, orientations,
 * and display modes with precise audio level indication.
 */
class VUMeterAdvanced : public juce::Component,
                       public juce::Timer
{
public:
    /**
     * @enum MeterType
     * @brief Types of meter display
     */
    enum class MeterType {
        VU,         ///< Classic VU meter with ballistics
        PPM,        ///< Peak Programme Meter
        Peak,       ///< Digital peak meter
        RMS,        ///< RMS level meter
        LUFS,       ///< Loudness Units relative to Full Scale
        Combined    ///< Combined peak and RMS display
    };

    /**
     * @enum Orientation
     * @brief Meter orientation
     */
    enum class Orientation {
        Horizontal, ///< Horizontal meter
        Vertical    ///< Vertical meter
    };

    /**
     * @enum ChannelMode
     * @brief Channel display mode
     */
    enum class ChannelMode {
        Mono,       ///< Single channel
        Stereo,     ///< Left/Right channels
        MultiChannel ///< Multiple channels
    };

    /**
     * @enum ScaleType
     * @brief Scale marking type
     */
    enum class ScaleType {
        Linear,     ///< Linear scale
        Logarithmic, ///< Logarithmic scale (dB)
        Custom      ///< Custom scale markings
    };

    /**
     * @struct MeterSettings
     * @brief Configuration settings for the meter
     */
    struct MeterSettings {
        MeterType type = MeterType::Peak;
        Orientation orientation = Orientation::Vertical;
        ChannelMode channelMode = ChannelMode::Stereo;
        ScaleType scaleType = ScaleType::Logarithmic;
        
        float minLevel = -60.0f;        ///< Minimum level in dB
        float maxLevel = 6.0f;          ///< Maximum level in dB
        float referenceLevel = 0.0f;    ///< Reference level (0 VU)
        
        float attackTime = 10.0f;       ///< Attack time in ms
        float releaseTime = 300.0f;     ///< Release time in ms
        float peakHoldTime = 1500.0f;   ///< Peak hold time in ms
        float peakDecayRate = 20.0f;    ///< Peak decay rate in dB/s
        
        bool showPeakHold = true;       ///< Show peak hold indicators
        bool showScale = true;          ///< Show scale markings
        bool showNumbers = true;        ///< Show numerical values
        bool showClipIndicator = true;  ///< Show clipping indicator
        
        int refreshRate = 30;           ///< Refresh rate in Hz
        int numChannels = 2;            ///< Number of channels
        
        juce::Colour meterColor = juce::Colours::green;
        juce::Colour peakColor = juce::Colours::red;
        juce::Colour backgroundColor = juce::Colours::black;
        juce::Colour scaleColor = juce::Colours::white;
    };

    /**
     * @struct ChannelData
     * @brief Audio data for a single channel
     */
    struct ChannelData {
        float currentLevel = -std::numeric_limits<float>::infinity();
        float peakLevel = -std::numeric_limits<float>::infinity();
        float rmsLevel = -std::numeric_limits<float>::infinity();
        float peakHoldLevel = -std::numeric_limits<float>::infinity();
        
        juce::Time lastPeakTime;
        bool isClipping = false;
        
        // Ballistics smoothing
        float smoothedLevel = -std::numeric_limits<float>::infinity();
        float smoothedRMS = -std::numeric_limits<float>::infinity();
    };

    // Constructor and Destructor
    VUMeterAdvanced();
    explicit VUMeterAdvanced(const MeterSettings& settings);
    ~VUMeterAdvanced() override;

    // Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDoubleClick(const juce::MouseEvent& event) override;

    // Timer override
    void timerCallback() override;

    // Audio Input
    /**
     * @brief Process audio buffer and update meter levels
     * @param buffer The audio buffer to analyze
     */
    void processAudioBuffer(const juce::AudioBuffer<float>& buffer);

    /**
     * @brief Set level for specific channel
     * @param channel Channel index
     * @param level Level in dB
     */
    void setLevel(int channel, float level);

    /**
     * @brief Set peak level for specific channel
     * @param channel Channel index
     * @param peakLevel Peak level in dB
     */
    void setPeakLevel(int channel, float peakLevel);

    /**
     * @brief Set RMS level for specific channel
     * @param channel Channel index
     * @param rmsLevel RMS level in dB
     */
    void setRMSLevel(int channel, float rmsLevel);

    /**
     * @brief Reset all meter levels
     */
    void resetLevels();

    /**
     * @brief Reset peak hold for all channels
     */
    void resetPeakHold();

    // Configuration
    /**
     * @brief Set meter settings
     * @param newSettings The new settings to apply
     */
    void setSettings(const MeterSettings& newSettings);

    /**
     * @brief Get current meter settings
     * @return Current settings
     */
    const MeterSettings& getSettings() const { return settings; }

    /**
     * @brief Set meter type
     * @param type The meter type
     */
    void setMeterType(MeterType type);

    /**
     * @brief Set meter orientation
     * @param orientation The orientation
     */
    void setOrientation(Orientation orientation);

    /**
     * @brief Set channel mode
     * @param mode The channel mode
     */
    void setChannelMode(ChannelMode mode);

    /**
     * @brief Set number of channels
     * @param numChannels Number of channels to display
     */
    void setNumChannels(int numChannels);

    /**
     * @brief Set level range
     * @param minLevel Minimum level in dB
     * @param maxLevel Maximum level in dB
     */
    void setLevelRange(float minLevel, float maxLevel);

    /**
     * @brief Set ballistics parameters
     * @param attackTime Attack time in ms
     * @param releaseTime Release time in ms
     */
    void setBallistics(float attackTime, float releaseTime);

    /**
     * @brief Set peak hold parameters
     * @param holdTime Hold time in ms
     * @param decayRate Decay rate in dB/s
     */
    void setPeakHold(float holdTime, float decayRate);

    // Visual Configuration
    /**
     * @brief Set meter colors
     * @param meterColor Main meter color
     * @param peakColor Peak indicator color
     * @param backgroundColor Background color
     */
    void setColors(const juce::Colour& meterColor, 
                   const juce::Colour& peakColor,
                   const juce::Colour& backgroundColor);

    /**
     * @brief Enable/disable scale display
     * @param showScale Whether to show scale markings
     */
    void setShowScale(bool showScale);

    /**
     * @brief Enable/disable numerical display
     * @param showNumbers Whether to show numerical values
     */
    void setShowNumbers(bool showNumbers);

    /**
     * @brief Enable/disable peak hold display
     * @param showPeakHold Whether to show peak hold indicators
     */
    void setShowPeakHold(bool showPeakHold);

    /**
     * @brief Enable/disable clip indicator
     * @param showClipIndicator Whether to show clipping indicator
     */
    void setShowClipIndicator(bool showClipIndicator);

    // Calibration
    /**
     * @brief Set reference level (0 VU point)
     * @param referenceLevel Reference level in dB
     */
    void setReferenceLevel(float referenceLevel);

    /**
     * @brief Add custom scale marking
     * @param level Level in dB
     * @param label Text label for the marking
     */
    void addScaleMarking(float level, const juce::String& label);

    /**
     * @brief Clear all custom scale markings
     */
    void clearScaleMarkings();

    // State Management
    /**
     * @brief Save meter settings to component state
     * @param state The component state to save to
     */
    void saveToState(ComponentState& state) const;

    /**
     * @brief Load meter settings from component state
     * @param state The component state to load from
     */
    void loadFromState(const ComponentState& state);

    // Utility Functions
    /**
     * @brief Convert linear amplitude to dB
     * @param amplitude Linear amplitude (0.0 to 1.0)
     * @return Level in dB
     */
    static float amplitudeToDb(float amplitude);

    /**
     * @brief Convert dB to linear amplitude
     * @param db Level in dB
     * @return Linear amplitude (0.0 to 1.0)
     */
    static float dbToAmplitude(float db);

    /**
     * @brief Get meter type name
     * @param type The meter type
     * @return Human-readable type name
     */
    static juce::String getMeterTypeName(MeterType type);

    /**
     * @brief Get orientation name
     * @param orientation The orientation
     * @return Human-readable orientation name
     */
    static juce::String getOrientationName(Orientation orientation);

    // Listeners
    /**
     * @class Listener
     * @brief Interface for meter event notifications
     */
    class Listener {
    public:
        virtual ~Listener() = default;
        
        /**
         * @brief Called when clipping is detected
         * @param channel Channel that is clipping
         */
        virtual void meterClippingDetected(int channel) {}
        
        /**
         * @brief Called when peak level changes significantly
         * @param channel Channel index
         * @param peakLevel New peak level in dB
         */
        virtual void meterPeakChanged(int channel, float peakLevel) {}
        
        /**
         * @brief Called when meter settings change
         * @param newSettings The new settings
         */
        virtual void meterSettingsChanged(const MeterSettings& newSettings) {}
    };

    /**
     * @brief Add a listener for meter events
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
    void paintVerticalMeter(juce::Graphics& g, const juce::Rectangle<int>& bounds);
    void paintHorizontalMeter(juce::Graphics& g, const juce::Rectangle<int>& bounds);
    void paintChannelMeter(juce::Graphics& g, const juce::Rectangle<int>& bounds, int channel);
    void paintScale(juce::Graphics& g, const juce::Rectangle<int>& bounds);
    void paintNumbers(juce::Graphics& g, const juce::Rectangle<int>& bounds);
    void paintClipIndicator(juce::Graphics& g, const juce::Rectangle<int>& bounds, int channel);
    
    // Level Calculation
    float calculateDisplayLevel(float inputLevel, MeterType type) const;
    float applyBallistics(float currentLevel, float targetLevel, float deltaTime) const;
    void updatePeakHold(int channel, float currentLevel);
    void updateSmoothing(int channel, float deltaTime);
    
    // Coordinate Conversion
    float levelToPosition(float level) const;
    float positionToLevel(float position) const;
    juce::Rectangle<int> getChannelBounds(int channel, const juce::Rectangle<int>& totalBounds) const;
    juce::Rectangle<int> getScaleBounds(const juce::Rectangle<int>& totalBounds) const;
    
    // Color Calculation
    juce::Colour getLevelColor(float level) const;
    juce::Colour getChannelColor(int channel, float level) const;
    
    // Scale Generation
    void generateScaleMarkings();
    struct ScaleMarking {
        float level;
        juce::String label;
        bool isMajor;
    };
    juce::Array<ScaleMarking> scaleMarkings;
    
    // Audio Analysis
    void analyzeBuffer(const juce::AudioBuffer<float>& buffer);
    float calculateRMS(const float* samples, int numSamples) const;
    float calculatePeak(const float* samples, int numSamples) const;
    
    // Notification
    void notifyListeners(std::function<void(Listener*)> notification);
    
    // Member Variables
    MeterSettings settings;
    juce::Array<ChannelData> channelData;
    juce::ListenerList<Listener> listeners;
    
    // Timing
    juce::Time lastUpdateTime;
    float deltaTime = 0.0f;
    
    // Visual state
    bool needsRepaint = true;
    juce::Rectangle<int> lastBounds;
    
    // Performance optimization
    juce::Image cachedBackground;
    bool backgroundNeedsUpdate = true;
    
    // Constants
    static constexpr float SILENCE_THRESHOLD = -100.0f;  ///< Silence threshold in dB
    static constexpr float CLIP_THRESHOLD = -0.1f;       ///< Clipping threshold in dB
    static constexpr float MIN_PEAK_HOLD_TIME = 100.0f;  ///< Minimum peak hold time in ms
    static constexpr float MAX_PEAK_HOLD_TIME = 10000.0f; ///< Maximum peak hold time in ms
    static constexpr int MAX_CHANNELS = 32;              ///< Maximum supported channels
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VUMeterAdvanced)
};

} // namespace Visualizations
} // namespace UI
} // namespace OTTO