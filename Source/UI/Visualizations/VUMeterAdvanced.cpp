#include "VUMeterAdvanced.h"
#include "JUCE8_CODING_STANDARDS.h"
#include "../../INIDataManager.h"

namespace OTTO {
namespace UI {
namespace Visualizations {

//==============================================================================
// Constructor and Destructor
//==============================================================================

VUMeterAdvanced::VUMeterAdvanced()
    : VUMeterAdvanced(MeterSettings{})
{
}

VUMeterAdvanced::VUMeterAdvanced(const MeterSettings& initialSettings)
    : settings(initialSettings)
    , lastUpdateTime(juce::Time::getCurrentTime())
{
    // Initialize channel data
    channelData.resize(settings.numChannels);
    for (auto& channel : channelData) {
        channel.currentLevel = SILENCE_THRESHOLD;
        channel.peakLevel = SILENCE_THRESHOLD;
        channel.rmsLevel = SILENCE_THRESHOLD;
        channel.peakHoldLevel = SILENCE_THRESHOLD;
        channel.smoothedLevel = SILENCE_THRESHOLD;
        channel.smoothedRMS = SILENCE_THRESHOLD;
        channel.lastPeakTime = juce::Time::getCurrentTime();
        channel.isClipping = false;
    }
    
    // Generate scale markings
    generateScaleMarkings();
    
    // Start timer for updates
    startTimer(1000 / settings.refreshRate);
    
    // Set initial size
    setSize(static_cast<int>(getWidth() * INIConfig::LayoutConstants::Row5::patternMatrixWidthPercent / 100.0f),
            static_cast<int>(getHeight() * INIConfig::LayoutConstants::Row5::patternMatrixHeightPercent / 100.0f));
}

VUMeterAdvanced::~VUMeterAdvanced()
{
    stopTimer();
}

//==============================================================================
// Component Overrides
//==============================================================================

void VUMeterAdvanced::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    // Fill background
    g.fillAll(settings.backgroundColor);
    
    // Paint meter based on orientation
    if (settings.orientation == Orientation::Vertical) {
        paintVerticalMeter(g, bounds);
    } else {
        paintHorizontalMeter(g, bounds);
    }
    
    // Paint scale if enabled
    if (settings.showScale) {
        paintScale(g, bounds);
    }
    
    // Paint numbers if enabled
    if (settings.showNumbers) {
        paintNumbers(g, bounds);
    }
    
    needsRepaint = false;
}

void VUMeterAdvanced::resized()
{
    backgroundNeedsUpdate = true;
    lastBounds = getLocalBounds();
    
    // Update timer interval based on size (larger meters can refresh slower)
    auto area = getWidth() * getHeight();
    auto refreshRate = area > 10000 ? settings.refreshRate / 2 : settings.refreshRate;
    startTimer(1000 / refreshRate);
}

void VUMeterAdvanced::mouseDown(const juce::MouseEvent& event)
{
    if (event.mods.isRightButtonDown()) {
        // Right-click to reset peak hold
        resetPeakHold();
    } else if (event.getNumberOfClicks() == 1) {
        // Single click to reset levels
        resetLevels();
    }
}

void VUMeterAdvanced::mouseDoubleClick(const juce::MouseEvent& event)
{
    // Double-click to open settings (could trigger a callback)
    notifyListeners([&](Listener* l) { l->meterSettingsChanged(settings); });
}

//==============================================================================
// Timer Override
//==============================================================================

void VUMeterAdvanced::timerCallback()
{
    auto currentTime = juce::Time::getCurrentTime();
    deltaTime = static_cast<float>((currentTime - lastUpdateTime).inMilliseconds());
    lastUpdateTime = currentTime;
    
    bool shouldRepaint = false;
    
    // Update smoothing and peak hold for each channel
    for (int i = 0; i < channelData.size(); ++i) {
        updateSmoothing(i, deltaTime);
        updatePeakHold(i, channelData[i].currentLevel);
        
        // Check if any values changed significantly
        if (std::abs(channelData[i].smoothedLevel - channelData[i].currentLevel) > 0.1f) {
            shouldRepaint = true;
        }
    }
    
    if (shouldRepaint || needsRepaint) {
        repaint();
    }
}

//==============================================================================
// Audio Input
//==============================================================================

void VUMeterAdvanced::processAudioBuffer(const juce::AudioBuffer<float>& buffer)
{
    analyzeBuffer(buffer);
    needsRepaint = true;
}

void VUMeterAdvanced::setLevel(int channel, float level)
{
    if (channel >= 0 && channel < channelData.size()) {
        channelData[channel].currentLevel = level;
        
        // Check for clipping
        if (level > CLIP_THRESHOLD && !channelData[channel].isClipping) {
            channelData[channel].isClipping = true;
            notifyListeners([=](Listener* l) { l->meterClippingDetected(channel); });
        } else if (level <= CLIP_THRESHOLD) {
            channelData[channel].isClipping = false;
        }
        
        needsRepaint = true;
    }
}

void VUMeterAdvanced::setPeakLevel(int channel, float peakLevel)
{
    if (channel >= 0 && channel < channelData.size()) {
        channelData[channel].peakLevel = peakLevel;
        updatePeakHold(channel, peakLevel);
        needsRepaint = true;
    }
}

void VUMeterAdvanced::setRMSLevel(int channel, float rmsLevel)
{
    if (channel >= 0 && channel < channelData.size()) {
        channelData[channel].rmsLevel = rmsLevel;
        needsRepaint = true;
    }
}

void VUMeterAdvanced::resetLevels()
{
    for (auto& channel : channelData) {
        channel.currentLevel = SILENCE_THRESHOLD;
        channel.peakLevel = SILENCE_THRESHOLD;
        channel.rmsLevel = SILENCE_THRESHOLD;
        channel.smoothedLevel = SILENCE_THRESHOLD;
        channel.smoothedRMS = SILENCE_THRESHOLD;
        channel.isClipping = false;
    }
    needsRepaint = true;
}

void VUMeterAdvanced::resetPeakHold()
{
    for (auto& channel : channelData) {
        channel.peakHoldLevel = SILENCE_THRESHOLD;
        channel.lastPeakTime = juce::Time::getCurrentTime();
    }
    needsRepaint = true;
}

//==============================================================================
// Configuration
//==============================================================================

void VUMeterAdvanced::setSettings(const MeterSettings& newSettings)
{
    settings = newSettings;
    
    // Resize channel data if needed
    if (channelData.size() != settings.numChannels) {
        channelData.resize(settings.numChannels);
        for (int i = 0; i < settings.numChannels; ++i) {
            if (i >= channelData.size()) {
                ChannelData newChannel;
                newChannel.currentLevel = SILENCE_THRESHOLD;
                newChannel.peakLevel = SILENCE_THRESHOLD;
                newChannel.rmsLevel = SILENCE_THRESHOLD;
                newChannel.peakHoldLevel = SILENCE_THRESHOLD;
                newChannel.smoothedLevel = SILENCE_THRESHOLD;
                newChannel.smoothedRMS = SILENCE_THRESHOLD;
                newChannel.lastPeakTime = juce::Time::getCurrentTime();
                newChannel.isClipping = false;
                channelData.add(newChannel);
            }
        }
    }
    
    // Update timer
    startTimer(1000 / settings.refreshRate);
    
    // Regenerate scale markings
    generateScaleMarkings();
    
    backgroundNeedsUpdate = true;
    needsRepaint = true;
    
    notifyListeners([&](Listener* l) { l->meterSettingsChanged(settings); });
}

void VUMeterAdvanced::setMeterType(MeterType type)
{
    settings.type = type;
    generateScaleMarkings();
    needsRepaint = true;
}

void VUMeterAdvanced::setOrientation(Orientation orientation)
{
    settings.orientation = orientation;
    backgroundNeedsUpdate = true;
    needsRepaint = true;
}

void VUMeterAdvanced::setChannelMode(ChannelMode mode)
{
    settings.channelMode = mode;
    
    // Adjust number of channels based on mode
    switch (mode) {
        case ChannelMode::Mono:
            setNumChannels(1);
            break;
        case ChannelMode::Stereo:
            setNumChannels(2);
            break;
        case ChannelMode::MultiChannel:
            // Keep current number of channels
            break;
    }
    
    needsRepaint = true;
}

void VUMeterAdvanced::setNumChannels(int numChannels)
{
    numChannels = juce::jlimit(1, MAX_CHANNELS, numChannels);
    settings.numChannels = numChannels;
    
    // Resize channel data
    channelData.resize(numChannels);
    for (int i = 0; i < numChannels; ++i) {
        if (i >= channelData.size()) {
            ChannelData newChannel;
            newChannel.currentLevel = SILENCE_THRESHOLD;
            newChannel.peakLevel = SILENCE_THRESHOLD;
            newChannel.rmsLevel = SILENCE_THRESHOLD;
            newChannel.peakHoldLevel = SILENCE_THRESHOLD;
            newChannel.smoothedLevel = SILENCE_THRESHOLD;
            newChannel.smoothedRMS = SILENCE_THRESHOLD;
            newChannel.lastPeakTime = juce::Time::getCurrentTime();
            newChannel.isClipping = false;
            channelData.add(newChannel);
        }
    }
    
    needsRepaint = true;
}

void VUMeterAdvanced::setLevelRange(float minLevel, float maxLevel)
{
    settings.minLevel = minLevel;
    settings.maxLevel = maxLevel;
    generateScaleMarkings();
    needsRepaint = true;
}

void VUMeterAdvanced::setBallistics(float attackTime, float releaseTime)
{
    settings.attackTime = juce::jmax(1.0f, attackTime);
    settings.releaseTime = juce::jmax(1.0f, releaseTime);
}

void VUMeterAdvanced::setPeakHold(float holdTime, float decayRate)
{
    settings.peakHoldTime = juce::jlimit(MIN_PEAK_HOLD_TIME, MAX_PEAK_HOLD_TIME, holdTime);
    settings.peakDecayRate = juce::jmax(1.0f, decayRate);
}

//==============================================================================
// Visual Configuration
//==============================================================================

void VUMeterAdvanced::setColors(const juce::Colour& meterColor, 
                                const juce::Colour& peakColor,
                                const juce::Colour& backgroundColor)
{
    settings.meterColor = meterColor;
    settings.peakColor = peakColor;
    settings.backgroundColor = backgroundColor;
    backgroundNeedsUpdate = true;
    needsRepaint = true;
}

void VUMeterAdvanced::setShowScale(bool showScale)
{
    settings.showScale = showScale;
    backgroundNeedsUpdate = true;
    needsRepaint = true;
}

void VUMeterAdvanced::setShowNumbers(bool showNumbers)
{
    settings.showNumbers = showNumbers;
    backgroundNeedsUpdate = true;
    needsRepaint = true;
}

void VUMeterAdvanced::setShowPeakHold(bool showPeakHold)
{
    settings.showPeakHold = showPeakHold;
    needsRepaint = true;
}

void VUMeterAdvanced::setShowClipIndicator(bool showClipIndicator)
{
    settings.showClipIndicator = showClipIndicator;
    needsRepaint = true;
}

//==============================================================================
// Calibration
//==============================================================================

void VUMeterAdvanced::setReferenceLevel(float referenceLevel)
{
    settings.referenceLevel = referenceLevel;
    generateScaleMarkings();
    needsRepaint = true;
}

void VUMeterAdvanced::addScaleMarking(float level, const juce::String& label)
{
    ScaleMarking marking;
    marking.level = level;
    marking.label = label;
    marking.isMajor = true;
    scaleMarkings.add(marking);
    
    // Sort by level
    std::sort(scaleMarkings.begin(), scaleMarkings.end(), 
              [](const ScaleMarking& a, const ScaleMarking& b) {
                  return a.level < b.level;
              });
    
    needsRepaint = true;
}

void VUMeterAdvanced::clearScaleMarkings()
{
    scaleMarkings.clear();
    generateScaleMarkings();
    needsRepaint = true;
}

//==============================================================================
// State Management
//==============================================================================

void VUMeterAdvanced::saveToState(ComponentState& state) const
{
    state.setValue("MeterType", static_cast<int>(settings.type));
    state.setValue("Orientation", static_cast<int>(settings.orientation));
    state.setValue("ChannelMode", static_cast<int>(settings.channelMode));
    state.setValue("ScaleType", static_cast<int>(settings.scaleType));
    
    state.setValue("MinLevel", settings.minLevel);
    state.setValue("MaxLevel", settings.maxLevel);
    state.setValue("ReferenceLevel", settings.referenceLevel);
    
    state.setValue("AttackTime", settings.attackTime);
    state.setValue("ReleaseTime", settings.releaseTime);
    state.setValue("PeakHoldTime", settings.peakHoldTime);
    state.setValue("PeakDecayRate", settings.peakDecayRate);
    
    state.setValue("ShowPeakHold", settings.showPeakHold);
    state.setValue("ShowScale", settings.showScale);
    state.setValue("ShowNumbers", settings.showNumbers);
    state.setValue("ShowClipIndicator", settings.showClipIndicator);
    
    state.setValue("RefreshRate", settings.refreshRate);
    state.setValue("NumChannels", settings.numChannels);
    
    state.setValue("MeterColor", settings.meterColor.toString());
    state.setValue("PeakColor", settings.peakColor.toString());
    state.setValue("BackgroundColor", settings.backgroundColor.toString());
    state.setValue("ScaleColor", settings.scaleColor.toString());
}

void VUMeterAdvanced::loadFromState(const ComponentState& state)
{
    MeterSettings newSettings;
    
    newSettings.type = static_cast<MeterType>(state.getIntValue("MeterType", static_cast<int>(MeterType::Peak)));
    newSettings.orientation = static_cast<Orientation>(state.getIntValue("Orientation", static_cast<int>(Orientation::Vertical)));
    newSettings.channelMode = static_cast<ChannelMode>(state.getIntValue("ChannelMode", static_cast<int>(ChannelMode::Stereo)));
    newSettings.scaleType = static_cast<ScaleType>(state.getIntValue("ScaleType", static_cast<int>(ScaleType::Logarithmic)));
    
    newSettings.minLevel = static_cast<float>(state.getDoubleValue("MinLevel", -60.0));
    newSettings.maxLevel = static_cast<float>(state.getDoubleValue("MaxLevel", 6.0));
    newSettings.referenceLevel = static_cast<float>(state.getDoubleValue("ReferenceLevel", 0.0));
    
    newSettings.attackTime = static_cast<float>(state.getDoubleValue("AttackTime", 10.0));
    newSettings.releaseTime = static_cast<float>(state.getDoubleValue("ReleaseTime", 300.0));
    newSettings.peakHoldTime = static_cast<float>(state.getDoubleValue("PeakHoldTime", 1500.0));
    newSettings.peakDecayRate = static_cast<float>(state.getDoubleValue("PeakDecayRate", 20.0));
    
    newSettings.showPeakHold = state.getBoolValue("ShowPeakHold", true);
    newSettings.showScale = state.getBoolValue("ShowScale", true);
    newSettings.showNumbers = state.getBoolValue("ShowNumbers", true);
    newSettings.showClipIndicator = state.getBoolValue("ShowClipIndicator", true);
    
    newSettings.refreshRate = state.getIntValue("RefreshRate", 30);
    newSettings.numChannels = state.getIntValue("NumChannels", 2);
    
    newSettings.meterColor = juce::Colour::fromString(state.getStringValue("MeterColor", "FF00FF00"));
    newSettings.peakColor = juce::Colour::fromString(state.getStringValue("PeakColor", "FFFF0000"));
    newSettings.backgroundColor = juce::Colour::fromString(state.getStringValue("BackgroundColor", "FF000000"));
    newSettings.scaleColor = juce::Colour::fromString(state.getStringValue("ScaleColor", "FFFFFFFF"));
    
    setSettings(newSettings);
}

//==============================================================================
// Utility Functions
//==============================================================================

float VUMeterAdvanced::amplitudeToDb(float amplitude)
{
    if (amplitude <= 0.0f) {
        return SILENCE_THRESHOLD;
    }
    
    return 20.0f * std::log10(amplitude);
}

float VUMeterAdvanced::dbToAmplitude(float db)
{
    if (db <= SILENCE_THRESHOLD) {
        return 0.0f;
    }
    
    return std::pow(10.0f, db / 20.0f);
}

juce::String VUMeterAdvanced::getMeterTypeName(MeterType type)
{
    switch (type) {
        case MeterType::VU:
            return "VU";
        case MeterType::PPM:
            return "PPM";
        case MeterType::Peak:
            return "Peak";
        case MeterType::RMS:
            return "RMS";
        case MeterType::LUFS:
            return "LUFS";
        case MeterType::Combined:
            return "Combined";
        default:
            return "Unknown";
    }
}

juce::String VUMeterAdvanced::getOrientationName(Orientation orientation)
{
    switch (orientation) {
        case Orientation::Horizontal:
            return "Horizontal";
        case Orientation::Vertical:
            return "Vertical";
        default:
            return "Unknown";
    }
}

//==============================================================================
// Listeners
//==============================================================================

void VUMeterAdvanced::addListener(Listener* listener)
{
    listeners.add(listener);
}

void VUMeterAdvanced::removeListener(Listener* listener)
{
    listeners.remove(listener);
}

//==============================================================================
// Private Methods - Rendering
//==============================================================================

void VUMeterAdvanced::paintVerticalMeter(juce::Graphics& g, const juce::Rectangle<int>& bounds)
{
    auto meterBounds = bounds;
    
    // Reserve space for scale if enabled
    if (settings.showScale) {
        auto scaleBounds = getScaleBounds(bounds);
        meterBounds = bounds.withTrimmedRight(scaleBounds.getWidth());
    }
    
    // Paint each channel
    for (int i = 0; i < channelData.size(); ++i) {
        auto channelBounds = getChannelBounds(i, meterBounds);
        paintChannelMeter(g, channelBounds, i);
        
        if (settings.showClipIndicator) {
            paintClipIndicator(g, channelBounds, i);
        }
    }
}

void VUMeterAdvanced::paintHorizontalMeter(juce::Graphics& g, const juce::Rectangle<int>& bounds)
{
    auto meterBounds = bounds;
    
    // Reserve space for scale if enabled
    if (settings.showScale) {
        auto scaleBounds = getScaleBounds(bounds);
        meterBounds = bounds.withTrimmedBottom(scaleBounds.getHeight());
    }
    
    // Paint each channel
    for (int i = 0; i < channelData.size(); ++i) {
        auto channelBounds = getChannelBounds(i, meterBounds);
        paintChannelMeter(g, channelBounds, i);
        
        if (settings.showClipIndicator) {
            paintClipIndicator(g, channelBounds, i);
        }
    }
}

void VUMeterAdvanced::paintChannelMeter(juce::Graphics& g, const juce::Rectangle<int>& bounds, int channel)
{
    if (channel >= channelData.size()) {
        return;
    }
    
    const auto& data = channelData[channel];
    
    // Calculate display level based on meter type
    float displayLevel = calculateDisplayLevel(data.smoothedLevel, settings.type);
    
    // Convert level to position
    float levelPosition = levelToPosition(displayLevel);
    
    // Draw meter bar
    if (settings.orientation == Orientation::Vertical) {
        auto meterHeight = static_cast<int>(bounds.getHeight() * levelPosition);
        auto meterRect = bounds.withTop(bounds.getBottom() - meterHeight);
        
        // Create gradient from bottom to top
        juce::ColourGradient gradient(getLevelColor(settings.minLevel), 
                                    meterRect.getBottomLeft().toFloat(),
                                    getLevelColor(settings.maxLevel), 
                                    meterRect.getTopLeft().toFloat(),
                                    false);
        
        // Add color stops for different level ranges
        gradient.addColour(0.7, settings.meterColor);
        gradient.addColour(0.9, juce::Colours::yellow);
        gradient.addColour(1.0, settings.peakColor);
        
        g.setGradientFill(gradient);
        g.fillRect(meterRect);
        
        // Draw peak hold if enabled
        if (settings.showPeakHold && data.peakHoldLevel > SILENCE_THRESHOLD) {
            float peakPosition = levelToPosition(data.peakHoldLevel);
            auto peakY = bounds.getBottom() - static_cast<int>(bounds.getHeight() * peakPosition);
            
            g.setColour(settings.peakColor);
            g.fillRect(bounds.getX(), peakY - 1, bounds.getWidth(), 2);
        }
    } else {
        auto meterWidth = static_cast<int>(bounds.getWidth() * levelPosition);
        auto meterRect = bounds.withWidth(meterWidth);
        
        // Create gradient from left to right
        juce::ColourGradient gradient(getLevelColor(settings.minLevel), 
                                    meterRect.getBottomLeft().toFloat(),
                                    getLevelColor(settings.maxLevel), 
                                    meterRect.getBottomRight().toFloat(),
                                    false);
        
        // Add color stops for different level ranges
        gradient.addColour(0.7, settings.meterColor);
        gradient.addColour(0.9, juce::Colours::yellow);
        gradient.addColour(1.0, settings.peakColor);
        
        g.setGradientFill(gradient);
        g.fillRect(meterRect);
        
        // Draw peak hold if enabled
        if (settings.showPeakHold && data.peakHoldLevel > SILENCE_THRESHOLD) {
            float peakPosition = levelToPosition(data.peakHoldLevel);
            auto peakX = bounds.getX() + static_cast<int>(bounds.getWidth() * peakPosition);
            
            g.setColour(settings.peakColor);
            g.fillRect(peakX - 1, bounds.getY(), 2, bounds.getHeight());
        }
    }
    
    // Draw border
    g.setColour(settings.scaleColor.withAlpha(0.3f));
    g.drawRect(bounds, 1);
}

void VUMeterAdvanced::paintScale(juce::Graphics& g, const juce::Rectangle<int>& bounds)
{
    auto scaleBounds = getScaleBounds(bounds);
    
    g.setColour(settings.scaleColor);
    g.setFont(JUCE8_FONT(10.0f));
    
    for (const auto& marking : scaleMarkings) {
        float position = levelToPosition(marking.level);
        
        if (settings.orientation == Orientation::Vertical) {
            auto y = scaleBounds.getBottom() - static_cast<int>(scaleBounds.getHeight() * position);
            
            // Draw tick mark
            auto tickLength = marking.isMajor ? 8 : 4;
            g.drawLine(scaleBounds.getX(), y, scaleBounds.getX() + tickLength, y);
            
            // Draw label
            if (marking.isMajor && settings.showNumbers) {
                g.drawText(marking.label, 
                          scaleBounds.getX() + tickLength + 2, y - 6,
                          scaleBounds.getWidth() - tickLength - 2, 12,
                          juce::Justification::centredLeft);
            }
        } else {
            auto x = scaleBounds.getX() + static_cast<int>(scaleBounds.getWidth() * position);
            
            // Draw tick mark
            auto tickLength = marking.isMajor ? 8 : 4;
            g.drawLine(x, scaleBounds.getBottom() - tickLength, x, scaleBounds.getBottom());
            
            // Draw label
            if (marking.isMajor && settings.showNumbers) {
                g.drawText(marking.label, 
                          x - 15, scaleBounds.getY(),
                          30, scaleBounds.getHeight() - tickLength - 2,
                          juce::Justification::centredTop);
            }
        }
    }
}

void VUMeterAdvanced::paintNumbers(juce::Graphics& g, const juce::Rectangle<int>& bounds)
{
    // Numbers are painted as part of the scale
    // This method could be used for additional numerical displays
}

void VUMeterAdvanced::paintClipIndicator(juce::Graphics& g, const juce::Rectangle<int>& bounds, int channel)
{
    if (channel >= channelData.size()) {
        return;
    }
    
    const auto& data = channelData[channel];
    
    if (data.isClipping) {
        // Flash red indicator
        auto flashAlpha = 0.5f + 0.5f * std::sin(juce::Time::getCurrentTime().toMilliseconds() * 0.01f);
        g.setColour(settings.peakColor.withAlpha(flashAlpha));
        
        if (settings.orientation == Orientation::Vertical) {
            g.fillRect(bounds.getX(), bounds.getY(), bounds.getWidth(), 4);
        } else {
            g.fillRect(bounds.getRight() - 4, bounds.getY(), 4, bounds.getHeight());
        }
    }
}

//==============================================================================
// Private Methods - Level Calculation
//==============================================================================

float VUMeterAdvanced::calculateDisplayLevel(float inputLevel, MeterType type) const
{
    switch (type) {
        case MeterType::VU:
            // VU meters have specific ballistics and reference level
            return inputLevel - settings.referenceLevel;
            
        case MeterType::PPM:
            // PPM meters have fast attack, slow release
            return inputLevel;
            
        case MeterType::Peak:
            // Peak meters show instantaneous peak
            return inputLevel;
            
        case MeterType::RMS:
            // RMS meters show average level
            return inputLevel;
            
        case MeterType::LUFS:
            // LUFS meters show loudness units
            return inputLevel;
            
        case MeterType::Combined:
            // Combined meters show both peak and RMS
            return inputLevel;
            
        default:
            return inputLevel;
    }
}

float VUMeterAdvanced::applyBallistics(float currentLevel, float targetLevel, float deltaTime) const
{
    if (targetLevel > currentLevel) {
        // Attack
        auto attackCoeff = 1.0f - std::exp(-deltaTime / settings.attackTime);
        return currentLevel + (targetLevel - currentLevel) * attackCoeff;
    } else {
        // Release
        auto releaseCoeff = 1.0f - std::exp(-deltaTime / settings.releaseTime);
        return currentLevel + (targetLevel - currentLevel) * releaseCoeff;
    }
}

void VUMeterAdvanced::updatePeakHold(int channel, float currentLevel)
{
    if (channel >= channelData.size()) {
        return;
    }
    
    auto& data = channelData[channel];
    auto currentTime = juce::Time::getCurrentTime();
    
    // Update peak hold level
    if (currentLevel > data.peakHoldLevel) {
        data.peakHoldLevel = currentLevel;
        data.lastPeakTime = currentTime;
    } else {
        // Decay peak hold
        auto timeSincePeak = (currentTime - data.lastPeakTime).inMilliseconds();
        if (timeSincePeak > settings.peakHoldTime) {
            auto decayAmount = settings.peakDecayRate * (timeSincePeak - settings.peakHoldTime) / 1000.0f;
            data.peakHoldLevel = juce::jmax(currentLevel, data.peakHoldLevel - decayAmount);
        }
    }
}

void VUMeterAdvanced::updateSmoothing(int channel, float deltaTime)
{
    if (channel >= channelData.size()) {
        return;
    }
    
    auto& data = channelData[channel];
    
    // Apply ballistics to current level
    data.smoothedLevel = applyBallistics(data.smoothedLevel, data.currentLevel, deltaTime);
    
    // Apply ballistics to RMS level
    data.smoothedRMS = applyBallistics(data.smoothedRMS, data.rmsLevel, deltaTime);
}

//==============================================================================
// Private Methods - Coordinate Conversion
//==============================================================================

float VUMeterAdvanced::levelToPosition(float level) const
{
    if (settings.scaleType == ScaleType::Logarithmic) {
        // Logarithmic scale (dB)
        auto normalizedLevel = (level - settings.minLevel) / (settings.maxLevel - settings.minLevel);
        return juce::jlimit(0.0f, 1.0f, normalizedLevel);
    } else {
        // Linear scale
        auto amplitude = dbToAmplitude(level);
        auto maxAmplitude = dbToAmplitude(settings.maxLevel);
        return juce::jlimit(0.0f, 1.0f, amplitude / maxAmplitude);
    }
}

float VUMeterAdvanced::positionToLevel(float position) const
{
    if (settings.scaleType == ScaleType::Logarithmic) {
        // Logarithmic scale (dB)
        return settings.minLevel + position * (settings.maxLevel - settings.minLevel);
    } else {
        // Linear scale
        auto maxAmplitude = dbToAmplitude(settings.maxLevel);
        auto amplitude = position * maxAmplitude;
        return amplitudeToDb(amplitude);
    }
}

juce::Rectangle<int> VUMeterAdvanced::getChannelBounds(int channel, const juce::Rectangle<int>& totalBounds) const
{
    if (settings.orientation == Orientation::Vertical) {
        auto channelWidth = totalBounds.getWidth() / settings.numChannels;
        return totalBounds.withX(totalBounds.getX() + channel * channelWidth)
                         .withWidth(channelWidth - 2);  // 2px spacing between channels
    } else {
        auto channelHeight = totalBounds.getHeight() / settings.numChannels;
        return totalBounds.withY(totalBounds.getY() + channel * channelHeight)
                         .withHeight(channelHeight - 2);  // 2px spacing between channels
    }
}

juce::Rectangle<int> VUMeterAdvanced::getScaleBounds(const juce::Rectangle<int>& totalBounds) const
{
    if (settings.orientation == Orientation::Vertical) {
        return totalBounds.withLeft(totalBounds.getRight() - 40)
                         .withWidth(40);
    } else {
        return totalBounds.withTop(totalBounds.getBottom() - 20)
                         .withHeight(20);
    }
}

//==============================================================================
// Private Methods - Color Calculation
//==============================================================================

juce::Colour VUMeterAdvanced::getLevelColor(float level) const
{
    // Color based on level
    if (level > -6.0f) {
        return settings.peakColor;  // Red for hot levels
    } else if (level > -18.0f) {
        return juce::Colours::yellow;  // Yellow for medium levels
    } else {
        return settings.meterColor;  // Green for low levels
    }
}

juce::Colour VUMeterAdvanced::getChannelColor(int channel, float level) const
{
    auto baseColor = getLevelColor(level);
    
    // Slightly different hue for each channel
    if (settings.numChannels > 1) {
        auto hueShift = (channel * 10.0f) / 360.0f;
        return baseColor.withRotatedHue(hueShift);
    }
    
    return baseColor;
}

//==============================================================================
// Private Methods - Scale Generation
//==============================================================================

void VUMeterAdvanced::generateScaleMarkings()
{
    scaleMarkings.clear();
    
    if (settings.scaleType == ScaleType::Logarithmic) {
        // Generate dB scale markings
        for (float level = settings.minLevel; level <= settings.maxLevel; level += 6.0f) {
            ScaleMarking marking;
            marking.level = level;
            marking.label = juce::String(static_cast<int>(level));
            marking.isMajor = (static_cast<int>(level) % 12 == 0);
            scaleMarkings.add(marking);
        }
        
        // Add reference level marking
        if (settings.referenceLevel >= settings.minLevel && settings.referenceLevel <= settings.maxLevel) {
            ScaleMarking refMarking;
            refMarking.level = settings.referenceLevel;
            refMarking.label = "0";
            refMarking.isMajor = true;
            scaleMarkings.add(refMarking);
        }
    } else {
        // Generate linear scale markings
        for (int i = 0; i <= 10; ++i) {
            float position = i / 10.0f;
            float level = positionToLevel(position);
            
            ScaleMarking marking;
            marking.level = level;
            marking.label = juce::String(static_cast<int>(level));
            marking.isMajor = (i % 2 == 0);
            scaleMarkings.add(marking);
        }
    }
    
    // Sort by level
    std::sort(scaleMarkings.begin(), scaleMarkings.end(), 
              [](const ScaleMarking& a, const ScaleMarking& b) {
                  return a.level < b.level;
              });
}

//==============================================================================
// Private Methods - Audio Analysis
//==============================================================================

void VUMeterAdvanced::analyzeBuffer(const juce::AudioBuffer<float>& buffer)
{
    auto numChannels = juce::jmin(buffer.getNumChannels(), channelData.size());
    auto numSamples = buffer.getNumSamples();
    
    for (int channel = 0; channel < numChannels; ++channel) {
        const float* samples = buffer.getReadPointer(channel);
        
        // Calculate peak level
        float peak = calculatePeak(samples, numSamples);
        float peakDb = amplitudeToDb(peak);
        
        // Calculate RMS level
        float rms = calculateRMS(samples, numSamples);
        float rmsDb = amplitudeToDb(rms);
        
        // Update channel data
        setLevel(channel, peakDb);
        setPeakLevel(channel, peakDb);
        setRMSLevel(channel, rmsDb);
    }
}

float VUMeterAdvanced::calculateRMS(const float* samples, int numSamples) const
{
    if (numSamples == 0) {
        return 0.0f;
    }
    
    float sum = 0.0f;
    for (int i = 0; i < numSamples; ++i) {
        sum += samples[i] * samples[i];
    }
    
    return std::sqrt(sum / numSamples);
}

float VUMeterAdvanced::calculatePeak(const float* samples, int numSamples) const
{
    if (numSamples == 0) {
        return 0.0f;
    }
    
    float peak = 0.0f;
    for (int i = 0; i < numSamples; ++i) {
        peak = juce::jmax(peak, std::abs(samples[i]));
    }
    
    return peak;
}

//==============================================================================
// Private Methods - Notification
//==============================================================================

void VUMeterAdvanced::notifyListeners(std::function<void(Listener*)> notification)
{
    listeners.call(notification);
}

} // namespace Visualizations
} // namespace UI
} // namespace OTTO