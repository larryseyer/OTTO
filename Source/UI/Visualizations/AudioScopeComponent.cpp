#include "AudioScopeComponent.h"
#include "JUCE8_CODING_STANDARDS.h"
#include "../../INIDataManager.h"

namespace OTTO {
namespace UI {
namespace Visualizations {

//==============================================================================
// Constructor and Destructor
//==============================================================================

AudioScopeComponent::AudioScopeComponent()
    : AudioScopeComponent(ScopeSettings{})
{
}

AudioScopeComponent::AudioScopeComponent(const ScopeSettings& initialSettings)
    : settings(initialSettings)
{
    // Initialize channel data
    channelData.resize(settings.numChannels);
    measurements.resize(settings.numChannels);
    
    for (int i = 0; i < settings.numChannels; ++i) {
        channelData[i].color = (i == 0) ? settings.traceColor1 : settings.traceColor2;
        channelData[i].enabled = true;
    }
    
    // Initialize FFT processor
    fftProcessor = std::make_unique<juce::dsp::FFT>(static_cast<int>(std::log2(FFT_SIZE)));
    fftData.resize(settings.numChannels);
    fftMagnitudes.resize(settings.numChannels);
    
    for (int i = 0; i < settings.numChannels; ++i) {
        fftData[i].resize(FFT_SIZE * 2, 0.0f);
        fftMagnitudes[i].resize(FFT_SIZE / 2, 0.0f);
    }
    
    // Initialize buffers
    initializeBuffers();
    
    // Start timer for updates
    startTimer(1000 / settings.refreshRate);
    
    // Set initial size using default percentages
    setSize(static_cast<int>(getWidth() * 80.0f / 100.0f),  // Default 80% width
            static_cast<int>(getHeight() * 60.0f / 100.0f)); // Default 60% height
}

AudioScopeComponent::~AudioScopeComponent()
{
    stopTimer();
    stopRecording();
}

//==============================================================================
// Component Overrides
//==============================================================================

void AudioScopeComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    displayArea = bounds.reduced(20); // Leave margin for labels
    
    // Calculate scaling factors
    timePerPixel = timeBaseToSeconds(settings.timeBase) * gridDivisions / displayArea.getWidth();
    voltsPerPixel = verticalScaleToVolts(settings.verticalScale) * gridDivisions / displayArea.getHeight();
    
    // Paint background
    paintBackground(g, bounds);
    
    // Paint grid if enabled
    if (settings.showGrid) {
        paintGrid(g, displayArea);
    }
    
    // Paint waveforms based on display mode
    switch (settings.displayMode) {
        case DisplayMode::Normal:
        case DisplayMode::Envelope:
        case DisplayMode::Persistence:
            paintWaveforms(g, displayArea);
            break;
            
        case DisplayMode::XY:
            paintXYMode(g, displayArea);
            break;
            
        case DisplayMode::FFT:
            for (int i = 0; i < settings.numChannels; ++i) {
                if (channelData[i].enabled) {
                    paintFFTSpectrum(g, displayArea, i);
                }
            }
            break;
    }
    
    // Paint trigger level indicator
    paintTriggerLevel(g, displayArea);
    
    // Paint cursors if enabled
    if (settings.showCursors) {
        paintCursors(g, displayArea);
    }
    
    // Paint measurements if enabled
    if (settings.showMeasurements) {
        paintMeasurements(g, bounds);
    }
    
    needsUpdate = false;
}

void AudioScopeComponent::resized()
{
    displayArea = getLocalBounds().reduced(20);
    needsUpdate = true;
    
    // Update timer interval based on size (larger scopes can refresh slower for performance)
    auto area = getWidth() * getHeight();
    auto refreshRate = area > 500000 ? settings.refreshRate / 2 : settings.refreshRate;
    startTimer(1000 / refreshRate);
}

void AudioScopeComponent::mouseDown(const juce::MouseEvent& event)
{
    isDragging = true;
    lastMousePosition = event.getPosition();
    
    if (settings.showCursors) {
        // Check if clicking on a cursor
        auto scopePoint = screenToScope(event.getPosition());
        
        // Check time cursors
        if (std::abs(scopePoint.x - cursorData.timeA) < timePerPixel * 5) {
            selectedCursor = 0;
            isDraggingCursor = true;
            return;
        }
        if (std::abs(scopePoint.x - cursorData.timeB) < timePerPixel * 5) {
            selectedCursor = 1;
            isDraggingCursor = true;
            return;
        }
        
        // Check voltage cursors
        if (std::abs(scopePoint.y - cursorData.voltageA) < voltsPerPixel * 5) {
            selectedCursor = 2;
            isDraggingCursor = true;
            return;
        }
        if (std::abs(scopePoint.y - cursorData.voltageB) < voltsPerPixel * 5) {
            selectedCursor = 3;
            isDraggingCursor = true;
            return;
        }
    }
    
    // Check if clicking on trigger level
    auto triggerY = displayArea.getCentreY() - (settings.triggerLevel / voltsPerPixel);
    if (std::abs(event.getPosition().y - triggerY) < 5) {
        isDraggingTrigger = true;
        return;
    }
    
    // Default to position dragging
    isDraggingCursor = false;
    isDraggingTrigger = false;
}

void AudioScopeComponent::mouseDrag(const juce::MouseEvent& event)
{
    if (!isDragging) return;
    
    if (isDraggingCursor) {
        handleCursorDrag(event);
    } else if (isDraggingTrigger) {
        handleTriggerDrag(event);
    } else {
        handlePositionDrag(event);
    }
    
    lastMousePosition = event.getPosition();
    needsUpdate = true;
    repaint();
}

void AudioScopeComponent::mouseUp(const juce::MouseEvent& event)
{
    isDragging = false;
    isDraggingCursor = false;
    isDraggingTrigger = false;
    selectedCursor = -1;
}

void AudioScopeComponent::mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel)
{
    if (event.mods.isShiftDown()) {
        // Horizontal zoom (time base)
        int currentTimeBase = static_cast<int>(settings.timeBase);
        if (wheel.deltaY > 0 && currentTimeBase > 0) {
            setTimeBase(static_cast<TimeBase>(currentTimeBase - 1));
        } else if (wheel.deltaY < 0 && currentTimeBase < static_cast<int>(TimeBase::Seconds_1)) {
            setTimeBase(static_cast<TimeBase>(currentTimeBase + 1));
        }
    } else {
        // Vertical zoom (voltage scale)
        int currentScale = static_cast<int>(settings.verticalScale);
        if (wheel.deltaY > 0 && currentScale > 0) {
            setVerticalScale(static_cast<VerticalScale>(currentScale - 1));
        } else if (wheel.deltaY < 0 && currentScale < static_cast<int>(VerticalScale::Volts_5)) {
            setVerticalScale(static_cast<VerticalScale>(currentScale + 1));
        }
    }
}

//==============================================================================
// Timer Override
//==============================================================================

void AudioScopeComponent::timerCallback()
{
    auto startTime = juce::Time::getMillisecondCounterHiRes();
    
    // Update trigger detection
    updateTrigger();
    
    // Update display buffers
    updateDisplayBuffers();
    
    // Update measurements
    if (settings.showMeasurements) {
        updateMeasurements();
    }
    
    // Update persistence if enabled
    if (settings.displayMode == DisplayMode::Persistence) {
        updatePersistence();
    }
    
    // Update recording if active
    if (isRecording) {
        recordingTime += 1.0f / settings.refreshRate;
        if (recordingTime >= recordingDuration) {
            stopRecording();
        }
    }
    
    // Trigger repaint if needed
    if (needsUpdate) {
        repaint();
    }
    
    // Update performance metrics
    auto endTime = juce::Time::getMillisecondCounterHiRes();
    lastFrameTime = static_cast<float>(endTime - startTime);
    frameCount++;
    
    if (frameCount >= 60) {
        averageFPS = 60000.0f / (lastFrameTime * 60);
        frameCount = 0;
    }
}

//==============================================================================
// Audio Input
//==============================================================================

void AudioScopeComponent::processAudioBuffer(const juce::AudioBuffer<float>& buffer)
{
    int numChannels = juce::jmin(buffer.getNumChannels(), settings.numChannels);
    int numSamples = buffer.getNumSamples();
    
    for (int channel = 0; channel < numChannels; ++channel) {
        const float* channelData = buffer.getReadPointer(channel);
        addSamples(channel, channelData, numSamples);
    }
    
    needsUpdate = true;
}

void AudioScopeComponent::addSample(int channel, float sample)
{
    if (channel < 0 || channel >= settings.numChannels) return;
    
    auto& data = channelData[channel];
    
    // Apply DC coupling filter if AC coupling is enabled
    if (!settings.dcCoupling) {
        // Simple high-pass filter for AC coupling
        float alpha = 1.0f - std::exp(-2.0f * juce::MathConstants<float>::pi * DC_FILTER_CUTOFF / settings.sampleRate);
        data.dcOffset = alpha * sample + (1.0f - alpha) * data.dcOffset;
        sample -= data.dcOffset;
    }
    
    // Apply channel gain
    sample *= data.gain;
    
    // Store sample in circular buffer
    data.buffer[data.writeIndex] = sample;
    data.writeIndex = (data.writeIndex + 1) % data.buffer.size();
    
    // Record sample if recording is active
    if (isRecording) {
        recordSample(channel, sample);
    }
    
    // Update trigger detection
    if (channel == 0) { // Use channel 0 for triggering
        detectTrigger(channel);
    }
}

void AudioScopeComponent::addSamples(int channel, const float* samples, int numSamples)
{
    for (int i = 0; i < numSamples; ++i) {
        addSample(channel, samples[i]);
    }
}

void AudioScopeComponent::clearBuffers()
{
    for (auto& data : channelData) {
        std::fill(data.buffer.begin(), data.buffer.end(), 0.0f);
        std::fill(data.displayBuffer.begin(), data.displayBuffer.end(), 0.0f);
        std::fill(data.persistenceBuffer.begin(), data.persistenceBuffer.end(), 0.0f);
        data.writeIndex = 0;
        data.triggerIndex = 0;
        data.triggered = false;
    }
    
    needsUpdate = true;
}

void AudioScopeComponent::forceTrigger()
{
    for (auto& data : channelData) {
        data.triggered = true;
        data.triggerIndex = data.writeIndex;
    }
    
    triggerArmed = false;
    triggerHoldoff = true;
    triggerHoldoffCounter = TRIGGER_HOLDOFF_SAMPLES;
    
    notifyListeners([](Listener& l) { l.scopeTriggered(0, 0.0f); });
}

//==============================================================================
// Configuration
//==============================================================================

void AudioScopeComponent::setSettings(const ScopeSettings& newSettings)
{
    bool bufferSizeChanged = (settings.bufferSize != newSettings.bufferSize || 
                             settings.numChannels != newSettings.numChannels);
    
    settings = newSettings;
    
    if (bufferSizeChanged) {
        resizeBuffers();
    }
    
    // Update timer
    startTimer(1000 / settings.refreshRate);
    
    needsUpdate = true;
    notifyListeners([this](Listener& l) { l.scopeSettingsChanged(settings); });
}

void AudioScopeComponent::setTriggerMode(TriggerMode mode)
{
    settings.triggerMode = mode;
    triggerArmed = (mode != TriggerMode::Free);
    needsUpdate = true;
}

void AudioScopeComponent::setTriggerLevel(float level)
{
    settings.triggerLevel = juce::jlimit(-1.0f, 1.0f, level);
    needsUpdate = true;
}

void AudioScopeComponent::setTriggerSlope(TriggerSlope slope)
{
    settings.triggerSlope = slope;
    needsUpdate = true;
}

void AudioScopeComponent::setTimeBase(TimeBase timeBase)
{
    settings.timeBase = timeBase;
    needsUpdate = true;
}

void AudioScopeComponent::setVerticalScale(VerticalScale scale)
{
    settings.verticalScale = scale;
    needsUpdate = true;
}

void AudioScopeComponent::setDisplayMode(DisplayMode mode)
{
    settings.displayMode = mode;
    
    // Perform FFT if switching to FFT mode
    if (mode == DisplayMode::FFT) {
        for (int i = 0; i < settings.numChannels; ++i) {
            performFFT(i);
        }
    }
    
    needsUpdate = true;
}

void AudioScopeComponent::setHorizontalPosition(float position)
{
    settings.horizontalPosition = juce::jlimit(0.0f, 1.0f, position);
    needsUpdate = true;
}

void AudioScopeComponent::setVerticalPosition(float position)
{
    settings.verticalPosition = juce::jlimit(0.0f, 1.0f, position);
    needsUpdate = true;
}

void AudioScopeComponent::setNumChannels(int numChannels)
{
    numChannels = juce::jlimit(1, MAX_CHANNELS, numChannels);
    settings.numChannels = numChannels;
    
    channelData.resize(numChannels);
    measurements.resize(numChannels);
    fftData.resize(numChannels);
    fftMagnitudes.resize(numChannels);
    
    for (int i = 0; i < numChannels; ++i) {
        if (i >= channelData.size()) {
            channelData[i].color = (i == 0) ? settings.traceColor1 : settings.traceColor2;
            channelData[i].enabled = true;
            fftData[i].resize(FFT_SIZE * 2, 0.0f);
            fftMagnitudes[i].resize(FFT_SIZE / 2, 0.0f);
        }
    }
    
    resizeBuffers();
    needsUpdate = true;
}

void AudioScopeComponent::setSampleRate(float sampleRate)
{
    settings.sampleRate = sampleRate;
    needsUpdate = true;
}

void AudioScopeComponent::setBufferSize(int bufferSize)
{
    settings.bufferSize = juce::jlimit(MIN_BUFFER_SIZE, MAX_BUFFER_SIZE, bufferSize);
    resizeBuffers();
    needsUpdate = true;
}

//==============================================================================
// Channel Configuration
//==============================================================================

void AudioScopeComponent::setChannelEnabled(int channel, bool enabled)
{
    if (channel >= 0 && channel < settings.numChannels) {
        channelData[channel].enabled = enabled;
        needsUpdate = true;
    }
}

void AudioScopeComponent::setChannelColor(int channel, const juce::Colour& color)
{
    if (channel >= 0 && channel < settings.numChannels) {
        channelData[channel].color = color;
        needsUpdate = true;
    }
}

void AudioScopeComponent::setChannelGain(int channel, float gain)
{
    if (channel >= 0 && channel < settings.numChannels) {
        channelData[channel].gain = juce::jmax(0.1f, gain);
        needsUpdate = true;
    }
}

void AudioScopeComponent::setDCCoupling(bool dcCoupling)
{
    settings.dcCoupling = dcCoupling;
    
    // Reset DC offset filters
    for (auto& data : channelData) {
        data.dcOffset = 0.0f;
    }
    
    needsUpdate = true;
}

//==============================================================================
// Visual Configuration
//==============================================================================

void AudioScopeComponent::setColors(const juce::Colour& backgroundColor,
                                    const juce::Colour& gridColor,
                                    const juce::Colour& textColor)
{
    settings.backgroundColor = backgroundColor;
    settings.gridColor = gridColor;
    settings.textColor = textColor;
    needsUpdate = true;
}

void AudioScopeComponent::setShowGrid(bool showGrid)
{
    settings.showGrid = showGrid;
    needsUpdate = true;
}

void AudioScopeComponent::setShowMeasurements(bool showMeasurements)
{
    settings.showMeasurements = showMeasurements;
    needsUpdate = true;
}

void AudioScopeComponent::setShowCursors(bool showCursors)
{
    settings.showCursors = showCursors;
    cursorData.timeEnabled = showCursors;
    cursorData.voltageEnabled = showCursors;
    needsUpdate = true;
}

void AudioScopeComponent::setAutoScale(bool autoScale)
{
    settings.autoScale = autoScale;
    needsUpdate = true;
}

//==============================================================================
// Measurement Functions
//==============================================================================

AudioScopeComponent::MeasurementData AudioScopeComponent::getMeasurements(int channel) const
{
    if (channel >= 0 && channel < settings.numChannels) {
        return measurements[channel];
    }
    return MeasurementData{};
}

void AudioScopeComponent::setCursorPositions(float timeA, float timeB, float voltageA, float voltageB)
{
    cursorData.timeA = timeA;
    cursorData.timeB = timeB;
    cursorData.voltageA = voltageA;
    cursorData.voltageB = voltageB;
    
    needsUpdate = true;
    notifyListeners([this](Listener& l) { l.cursorsChanged(cursorData); });
}

//==============================================================================
// Export Functions
//==============================================================================

bool AudioScopeComponent::exportWaveform(const juce::File& file, int width, int height)
{
    juce::Image image(juce::Image::ARGB, width, height, true);
    juce::Graphics g(image);
    
    // Scale the component to fit the export size
    auto originalBounds = getBounds();
    setBounds(0, 0, width, height);
    
    // Paint the component
    paint(g);
    
    // Restore original bounds
    setBounds(originalBounds);
    
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

bool AudioScopeComponent::exportData(const juce::File& file, int channel)
{
    juce::FileOutputStream stream(file);
    if (!stream.openedOk()) return false;
    
    // Write CSV header
    if (channel == -1) {
        stream.writeText("Time", false, false, nullptr);
        for (int i = 0; i < settings.numChannels; ++i) {
            stream.writeText(",Channel" + juce::String(i + 1), false, false, nullptr);
        }
        stream.writeText("\n", false, false, nullptr);
        
        // Write data for all channels
        int maxSamples = 0;
        for (const auto& data : channelData) {
            maxSamples = juce::jmax(maxSamples, static_cast<int>(data.displayBuffer.size()));
        }
        
        for (int sample = 0; sample < maxSamples; ++sample) {
            float time = sample / settings.sampleRate;
            stream.writeText(juce::String(time, 6), false, false, nullptr);
            
            for (int ch = 0; ch < settings.numChannels; ++ch) {
                float value = (sample < channelData[ch].displayBuffer.size()) ? 
                             channelData[ch].displayBuffer[sample] : 0.0f;
                stream.writeText("," + juce::String(value, 6), false, false, nullptr);
            }
            stream.writeText("\n", false, false, nullptr);
        }
    } else if (channel >= 0 && channel < settings.numChannels) {
        // Write single channel data
        stream.writeText("Time,Amplitude\n", false, false, nullptr);
        const auto& data = channelData[channel];
        
        for (int sample = 0; sample < data.displayBuffer.size(); ++sample) {
            float time = sample / settings.sampleRate;
            stream.writeText(juce::String(time, 6) + "," + 
                           juce::String(data.displayBuffer[sample], 6) + "\n", false, false, nullptr);
        }
    }
    
    return true;
}

bool AudioScopeComponent::startRecording(const juce::File& outputFile, float duration)
{
    if (isRecording) return false;
    
    recordingFile = outputFile;
    recordingDuration = duration;
    recordingTime = 0.0f;
    isRecording = true;
    
    // Initialize recording data
    recordingData.clear();
    recordingData.resize(settings.numChannels);
    
    return true;
}

void AudioScopeComponent::stopRecording()
{
    if (!isRecording) return;
    
    isRecording = false;
    saveRecordingData();
}

//==============================================================================
// State Management
//==============================================================================

void AudioScopeComponent::saveToState(ComponentState& state) const
{
    state.setValue("TriggerMode", static_cast<int>(settings.triggerMode));
    state.setValue("TriggerSlope", static_cast<int>(settings.triggerSlope));
    state.setValue("TimeBase", static_cast<int>(settings.timeBase));
    state.setValue("VerticalScale", static_cast<int>(settings.verticalScale));
    state.setValue("DisplayMode", static_cast<int>(settings.displayMode));
    
    state.setValue("TriggerLevel", settings.triggerLevel);
    state.setValue("TriggerHysteresis", settings.triggerHysteresis);
    state.setValue("HorizontalPosition", settings.horizontalPosition);
    state.setValue("VerticalPosition", settings.verticalPosition);
    
    state.setValue("BufferSize", settings.bufferSize);
    state.setValue("NumChannels", settings.numChannels);
    state.setValue("SampleRate", settings.sampleRate);
    
    state.setValue("ShowGrid", settings.showGrid);
    state.setValue("ShowMeasurements", settings.showMeasurements);
    state.setValue("ShowCursors", settings.showCursors);
    state.setValue("AutoScale", settings.autoScale);
    state.setValue("DCCoupling", settings.dcCoupling);
    
    state.setValue("PersistenceDecay", settings.persistenceDecay);
    state.setValue("RefreshRate", settings.refreshRate);
    
    // Save cursor positions
    state.setValue("CursorTimeA", cursorData.timeA);
    state.setValue("CursorTimeB", cursorData.timeB);
    state.setValue("CursorVoltageA", cursorData.voltageA);
    state.setValue("CursorVoltageB", cursorData.voltageB);
    state.setValue("CursorTimeEnabled", cursorData.timeEnabled);
    state.setValue("CursorVoltageEnabled", cursorData.voltageEnabled);
}

void AudioScopeComponent::loadFromState(const ComponentState& state)
{
    ScopeSettings newSettings = settings;
    
    newSettings.triggerMode = static_cast<TriggerMode>(state.getIntValue("TriggerMode", static_cast<int>(TriggerMode::Auto)));
    newSettings.triggerSlope = static_cast<TriggerSlope>(state.getIntValue("TriggerSlope", static_cast<int>(TriggerSlope::Rising)));
    newSettings.timeBase = static_cast<TimeBase>(state.getIntValue("TimeBase", static_cast<int>(TimeBase::Milliseconds_1)));
    newSettings.verticalScale = static_cast<VerticalScale>(state.getIntValue("VerticalScale", static_cast<int>(VerticalScale::Volts_1)));
    newSettings.displayMode = static_cast<DisplayMode>(state.getIntValue("DisplayMode", static_cast<int>(DisplayMode::Normal)));
    
    newSettings.triggerLevel = state.getFloatValue("TriggerLevel", 0.0f);
    newSettings.triggerHysteresis = state.getFloatValue("TriggerHysteresis", 0.01f);
    newSettings.horizontalPosition = state.getFloatValue("HorizontalPosition", 0.5f);
    newSettings.verticalPosition = state.getFloatValue("VerticalPosition", 0.5f);
    
    newSettings.bufferSize = state.getIntValue("BufferSize", 2048);
    newSettings.numChannels = state.getIntValue("NumChannels", 2);
    newSettings.sampleRate = state.getFloatValue("SampleRate", 48000.0f);
    
    newSettings.showGrid = state.getBoolValue("ShowGrid", true);
    newSettings.showMeasurements = state.getBoolValue("ShowMeasurements", true);
    newSettings.showCursors = state.getBoolValue("ShowCursors", false);
    newSettings.autoScale = state.getBoolValue("AutoScale", false);
    newSettings.dcCoupling = state.getBoolValue("DCCoupling", true);
    
    newSettings.persistenceDecay = state.getFloatValue("PersistenceDecay", 0.95f);
    newSettings.refreshRate = state.getIntValue("RefreshRate", 60);
    
    setSettings(newSettings);
    
    // Load cursor positions
    cursorData.timeA = state.getFloatValue("CursorTimeA", 0.0f);
    cursorData.timeB = state.getFloatValue("CursorTimeB", 0.0f);
    cursorData.voltageA = state.getFloatValue("CursorVoltageA", 0.0f);
    cursorData.voltageB = state.getFloatValue("CursorVoltageB", 0.0f);
    cursorData.timeEnabled = state.getBoolValue("CursorTimeEnabled", false);
    cursorData.voltageEnabled = state.getBoolValue("CursorVoltageEnabled", false);
}

//==============================================================================
// Utility Functions
//==============================================================================

float AudioScopeComponent::timeBaseToSeconds(TimeBase timeBase)
{
    switch (timeBase) {
        case TimeBase::Microseconds_10:    return 10e-6f;
        case TimeBase::Microseconds_25:    return 25e-6f;
        case TimeBase::Microseconds_50:    return 50e-6f;
        case TimeBase::Microseconds_100:   return 100e-6f;
        case TimeBase::Microseconds_250:   return 250e-6f;
        case TimeBase::Microseconds_500:   return 500e-6f;
        case TimeBase::Milliseconds_1:     return 1e-3f;
        case TimeBase::Milliseconds_2_5:   return 2.5e-3f;
        case TimeBase::Milliseconds_5:     return 5e-3f;
        case TimeBase::Milliseconds_10:    return 10e-3f;
        case TimeBase::Milliseconds_25:    return 25e-3f;
        case TimeBase::Milliseconds_50:    return 50e-3f;
        case TimeBase::Milliseconds_100:   return 100e-3f;
        case TimeBase::Milliseconds_250:   return 250e-3f;
        case TimeBase::Milliseconds_500:   return 500e-3f;
        case TimeBase::Seconds_1:          return 1.0f;
        default:                           return 1e-3f;
    }
}

float AudioScopeComponent::verticalScaleToVolts(VerticalScale scale)
{
    switch (scale) {
        case VerticalScale::Millivolts_1:   return 1e-3f;
        case VerticalScale::Millivolts_2:   return 2e-3f;
        case VerticalScale::Millivolts_5:   return 5e-3f;
        case VerticalScale::Millivolts_10:  return 10e-3f;
        case VerticalScale::Millivolts_20:  return 20e-3f;
        case VerticalScale::Millivolts_50:  return 50e-3f;
        case VerticalScale::Millivolts_100: return 100e-3f;
        case VerticalScale::Millivolts_200: return 200e-3f;
        case VerticalScale::Millivolts_500: return 500e-3f;
        case VerticalScale::Volts_1:        return 1.0f;
        case VerticalScale::Volts_2:        return 2.0f;
        case VerticalScale::Volts_5:        return 5.0f;
        default:                            return 1.0f;
    }
}

juce::String AudioScopeComponent::getTimeBaseName(TimeBase timeBase)
{
    switch (timeBase) {
        case TimeBase::Microseconds_10:    return "10 μs/div";
        case TimeBase::Microseconds_25:    return "25 μs/div";
        case TimeBase::Microseconds_50:    return "50 μs/div";
        case TimeBase::Microseconds_100:   return "100 μs/div";
        case TimeBase::Microseconds_250:   return "250 μs/div";
        case TimeBase::Microseconds_500:   return "500 μs/div";
        case TimeBase::Milliseconds_1:     return "1 ms/div";
        case TimeBase::Milliseconds_2_5:   return "2.5 ms/div";
        case TimeBase::Milliseconds_5:     return "5 ms/div";
        case TimeBase::Milliseconds_10:    return "10 ms/div";
        case TimeBase::Milliseconds_25:    return "25 ms/div";
        case TimeBase::Milliseconds_50:    return "50 ms/div";
        case TimeBase::Milliseconds_100:   return "100 ms/div";
        case TimeBase::Milliseconds_250:   return "250 ms/div";
        case TimeBase::Milliseconds_500:   return "500 ms/div";
        case TimeBase::Seconds_1:          return "1 s/div";
        default:                           return "1 ms/div";
    }
}

juce::String AudioScopeComponent::getVerticalScaleName(VerticalScale scale)
{
    switch (scale) {
        case VerticalScale::Millivolts_1:   return "1 mV/div";
        case VerticalScale::Millivolts_2:   return "2 mV/div";
        case VerticalScale::Millivolts_5:   return "5 mV/div";
        case VerticalScale::Millivolts_10:  return "10 mV/div";
        case VerticalScale::Millivolts_20:  return "20 mV/div";
        case VerticalScale::Millivolts_50:  return "50 mV/div";
        case VerticalScale::Millivolts_100: return "100 mV/div";
        case VerticalScale::Millivolts_200: return "200 mV/div";
        case VerticalScale::Millivolts_500: return "500 mV/div";
        case VerticalScale::Volts_1:        return "1 V/div";
        case VerticalScale::Volts_2:        return "2 V/div";
        case VerticalScale::Volts_5:        return "5 V/div";
        default:                            return "1 V/div";
    }
}

juce::Point<float> AudioScopeComponent::screenToScope(const juce::Point<int>& screenPoint)
{
    float time = (screenPoint.x - displayArea.getX()) * timePerPixel;
    float voltage = (displayArea.getCentreY() - screenPoint.y) * voltsPerPixel;
    return {time, voltage};
}

juce::Point<int> AudioScopeComponent::scopeToScreen(const juce::Point<float>& scopePoint)
{
    int x = displayArea.getX() + static_cast<int>(scopePoint.x / timePerPixel);
    int y = displayArea.getCentreY() - static_cast<int>(scopePoint.y / voltsPerPixel);
    return {x, y};
}

//==============================================================================
// Listeners
//==============================================================================

void AudioScopeComponent::addListener(Listener* listener)
{
    listeners.add(listener);
}

void AudioScopeComponent::removeListener(Listener* listener)
{
    listeners.remove(listener);
}

//==============================================================================
// Private Methods
//==============================================================================

void AudioScopeComponent::paintBackground(juce::Graphics& g, const juce::Rectangle<int>& bounds)
{
    g.fillAll(settings.backgroundColor);
    
    // Draw border
    g.setColour(settings.gridColor.withAlpha(0.5f));
    g.drawRect(displayArea, 1);
}

void AudioScopeComponent::paintGrid(juce::Graphics& g, const juce::Rectangle<int>& bounds)
{
    g.setColour(settings.gridColor);
    
    // Draw vertical grid lines
    for (int i = 1; i < gridDivisions; ++i) {
        int x = bounds.getX() + (bounds.getWidth() * i) / gridDivisions;
        g.drawVerticalLine(x, static_cast<float>(bounds.getY()), static_cast<float>(bounds.getBottom()));
    }
    
    // Draw horizontal grid lines
    for (int i = 1; i < gridDivisions; ++i) {
        int y = bounds.getY() + (bounds.getHeight() * i) / gridDivisions;
        g.drawHorizontalLine(y, static_cast<float>(bounds.getX()), static_cast<float>(bounds.getRight()));
    }
    
    // Draw center lines with different opacity
    g.setColour(settings.gridColor.withAlpha(0.8f));
    g.drawVerticalLine(bounds.getCentreX(), static_cast<float>(bounds.getY()), static_cast<float>(bounds.getBottom()));
    g.drawHorizontalLine(bounds.getCentreY(), static_cast<float>(bounds.getX()), static_cast<float>(bounds.getRight()));
}

void AudioScopeComponent::paintWaveforms(juce::Graphics& g, const juce::Rectangle<int>& bounds)
{
    for (int channel = 0; channel < settings.numChannels; ++channel) {
        if (channelData[channel].enabled) {
            paintChannelWaveform(g, bounds, channel);
        }
    }
}

void AudioScopeComponent::paintTriggerLevel(juce::Graphics& g, const juce::Rectangle<int>& bounds)
{
    if (settings.triggerMode == TriggerMode::Free) return;
    
    g.setColour(settings.triggerColor);
    int triggerY = bounds.getCentreY() - static_cast<int>(settings.triggerLevel / voltsPerPixel);
    
    if (triggerY >= bounds.getY() && triggerY <= bounds.getBottom()) {
        g.drawHorizontalLine(triggerY, static_cast<float>(bounds.getX()), static_cast<float>(bounds.getRight()));
        
        // Draw trigger arrow
        juce::Path arrow;
        arrow.addTriangle(static_cast<float>(bounds.getX() - 10), static_cast<float>(triggerY),
                         static_cast<float>(bounds.getX() - 5), static_cast<float>(triggerY - 5),
                         static_cast<float>(bounds.getX() - 5), static_cast<float>(triggerY + 5));
        g.fillPath(arrow);
    }
}

void AudioScopeComponent::paintCursors(juce::Graphics& g, const juce::Rectangle<int>& bounds)
{
    g.setColour(settings.cursorColor);
    
    // Draw time cursors
    if (cursorData.timeEnabled) {
        int xA = bounds.getX() + static_cast<int>(cursorData.timeA / timePerPixel);
        int xB = bounds.getX() + static_cast<int>(cursorData.timeB / timePerPixel);
        
        if (xA >= bounds.getX() && xA <= bounds.getRight()) {
            g.drawVerticalLine(xA, static_cast<float>(bounds.getY()), static_cast<float>(bounds.getBottom()));
        }
        if (xB >= bounds.getX() && xB <= bounds.getRight()) {
            g.drawVerticalLine(xB, static_cast<float>(bounds.getY()), static_cast<float>(bounds.getBottom()));
        }
    }
    
    // Draw voltage cursors
    if (cursorData.voltageEnabled) {
        int yA = bounds.getCentreY() - static_cast<int>(cursorData.voltageA / voltsPerPixel);
        int yB = bounds.getCentreY() - static_cast<int>(cursorData.voltageB / voltsPerPixel);
        
        if (yA >= bounds.getY() && yA <= bounds.getBottom()) {
            g.drawHorizontalLine(yA, static_cast<float>(bounds.getX()), static_cast<float>(bounds.getRight()));
        }
        if (yB >= bounds.getY() && yB <= bounds.getBottom()) {
            g.drawHorizontalLine(yB, static_cast<float>(bounds.getX()), static_cast<float>(bounds.getRight()));
        }
    }
}

void AudioScopeComponent::paintMeasurements(juce::Graphics& g, const juce::Rectangle<int>& bounds)
{
    g.setColour(settings.textColor);
    g.setFont(JUCE8_FONT(12.0f));
    
    int yOffset = 10;
    for (int channel = 0; channel < settings.numChannels; ++channel) {
        if (!channelData[channel].enabled) continue;
        
        const auto& measurement = measurements[channel];
        if (!measurement.valid) continue;
        
        juce::String text = juce::String::formatted("CH%d: %.3fV p-p, %.3fV RMS, %.1fHz", 
                                                   channel + 1,
                                                   measurement.peakToPeak,
                                                   measurement.rms,
                                                   measurement.frequency);
        
        g.setColour(channelData[channel].color);
        g.drawText(text, 10, yOffset, 300, 15, juce::Justification::left);
        yOffset += 20;
    }
    
    // Draw cursor measurements
    if (settings.showCursors && (cursorData.timeEnabled || cursorData.voltageEnabled)) {
        g.setColour(settings.textColor);
        
        if (cursorData.timeEnabled) {
            float deltaTime = std::abs(cursorData.timeB - cursorData.timeA);
            float frequency = (deltaTime > 0) ? 1.0f / deltaTime : 0.0f;
            juce::String timeText = juce::String::formatted("ΔT: %.3fms, F: %.1fHz", 
                                                           deltaTime * 1000.0f, frequency);
            g.drawText(timeText, bounds.getRight() - 200, 10, 190, 15, juce::Justification::right);
        }
        
        if (cursorData.voltageEnabled) {
            float deltaVoltage = std::abs(cursorData.voltageB - cursorData.voltageA);
            juce::String voltageText = juce::String::formatted("ΔV: %.3fV", deltaVoltage);
            g.drawText(voltageText, bounds.getRight() - 200, 30, 190, 15, juce::Justification::right);
        }
    }
}

void AudioScopeComponent::paintChannelWaveform(juce::Graphics& g, const juce::Rectangle<int>& bounds, int channel)
{
    const auto& data = channelData[channel];
    if (data.displayBuffer.empty()) return;
    
    g.setColour(data.color);
    
    juce::Path waveformPath;
    bool firstPoint = true;
    
    for (int x = 0; x < bounds.getWidth(); ++x) {
        float sampleIndex = (x * data.displayBuffer.size()) / static_cast<float>(bounds.getWidth());
        float sample = interpolateSample(data.displayBuffer, sampleIndex);
        
        // Apply vertical position and scale
        int y = bounds.getCentreY() - static_cast<int>((sample + settings.verticalPosition - 0.5f) / voltsPerPixel);
        y = juce::jlimit(bounds.getY(), bounds.getBottom(), y);
        
        if (firstPoint) {
            waveformPath.startNewSubPath(static_cast<float>(bounds.getX() + x), static_cast<float>(y));
            firstPoint = false;
        } else {
            waveformPath.lineTo(static_cast<float>(bounds.getX() + x), static_cast<float>(y));
        }
    }
    
    // Draw waveform based on display mode
    switch (settings.displayMode) {
        case DisplayMode::Normal:
            g.strokePath(waveformPath, juce::PathStrokeType(1.0f));
            break;
            
        case DisplayMode::Envelope:
            // TODO: Implement envelope display
            g.strokePath(waveformPath, juce::PathStrokeType(1.0f));
            break;
            
        case DisplayMode::Persistence:
            // Draw with reduced opacity for persistence effect
            g.setColour(data.color.withAlpha(0.3f));
            g.strokePath(waveformPath, juce::PathStrokeType(1.0f));
            break;
            
        default:
            g.strokePath(waveformPath, juce::PathStrokeType(1.0f));
            break;
    }
}

void AudioScopeComponent::paintFFTSpectrum(juce::Graphics& g, const juce::Rectangle<int>& bounds, int channel)
{
    const auto& magnitudes = fftMagnitudes[channel];
    if (magnitudes.empty()) return;
    
    g.setColour(channelData[channel].color);
    
    juce::Path spectrumPath;
    bool firstPoint = true;
    
    for (int x = 0; x < bounds.getWidth(); ++x) {
        float binIndex = (x * magnitudes.size()) / static_cast<float>(bounds.getWidth());
        float magnitude = interpolateSample(magnitudes, binIndex);
        
        // Convert to dB and scale
        float dB = 20.0f * std::log10(magnitude + 1e-10f);
        int y = bounds.getBottom() - static_cast<int>((dB + 60.0f) * bounds.getHeight() / 60.0f);
        y = juce::jlimit(bounds.getY(), bounds.getBottom(), y);
        
        if (firstPoint) {
            spectrumPath.startNewSubPath(static_cast<float>(bounds.getX() + x), static_cast<float>(y));
            firstPoint = false;
        } else {
            spectrumPath.lineTo(static_cast<float>(bounds.getX() + x), static_cast<float>(y));
        }
    }
    
    g.strokePath(spectrumPath, juce::PathStrokeType(1.0f));
}

void AudioScopeComponent::paintXYMode(juce::Graphics& g, const juce::Rectangle<int>& bounds)
{
    if (settings.numChannels < 2 || !channelData[0].enabled || !channelData[1].enabled) return;
    
    const auto& dataX = channelData[0].displayBuffer;
    const auto& dataY = channelData[1].displayBuffer;
    
    if (dataX.empty() || dataY.empty()) return;
    
    g.setColour(settings.traceColor1);
    
    juce::Path xyPath;
    bool firstPoint = true;
    
    int minSize = juce::jmin(static_cast<int>(dataX.size()), static_cast<int>(dataY.size()));
    
    for (int i = 0; i < minSize; ++i) {
        int x = bounds.getCentreX() + static_cast<int>(dataX[i] / voltsPerPixel);
        int y = bounds.getCentreY() - static_cast<int>(dataY[i] / voltsPerPixel);
        
        x = juce::jlimit(bounds.getX(), bounds.getRight(), x);
        y = juce::jlimit(bounds.getY(), bounds.getBottom(), y);
        
        if (firstPoint) {
            xyPath.startNewSubPath(static_cast<float>(x), static_cast<float>(y));
            firstPoint = false;
        } else {
            xyPath.lineTo(static_cast<float>(x), static_cast<float>(y));
        }
    }
    
    g.strokePath(xyPath, juce::PathStrokeType(1.0f));
}

void AudioScopeComponent::updateTrigger()
{
    // Handle trigger holdoff
    if (triggerHoldoff) {
        triggerHoldoffCounter--;
        if (triggerHoldoffCounter <= 0) {
            triggerHoldoff = false;
            triggerArmed = true;
        }
    }
    
    // Re-arm trigger for auto mode
    if (settings.triggerMode == TriggerMode::Auto && !triggerArmed) {
        triggerArmed = true;
    }
}

void AudioScopeComponent::updateMeasurements()
{
    for (int channel = 0; channel < settings.numChannels; ++channel) {
        if (channelData[channel].enabled) {
            calculateMeasurements(channel);
        }
    }
}

void AudioScopeComponent::updateDisplayBuffers()
{
    for (int channel = 0; channel < settings.numChannels; ++channel) {
        processChannel(channel);
    }
    
    needsUpdate = true;
}

void AudioScopeComponent::processChannel(int channel)
{
    auto& data = channelData[channel];
    
    // Copy from circular buffer to display buffer based on trigger
    int startIndex = data.triggered ? data.triggerIndex : data.writeIndex;
    int displaySize = juce::jmin(static_cast<int>(data.buffer.size()), settings.bufferSize);
    
    data.displayBuffer.resize(displaySize);
    
    for (int i = 0; i < displaySize; ++i) {
        int bufferIndex = (startIndex + i) % data.buffer.size();
        data.displayBuffer[i] = data.buffer[bufferIndex];
    }
}

void AudioScopeComponent::applyDCCoupling(int channel)
{
    // DC coupling filter is applied in addSample()
}

void AudioScopeComponent::detectTrigger(int channel)
{
    if (!triggerArmed || settings.triggerMode == TriggerMode::Free) return;
    
    auto& data = channelData[channel];
    int triggerIndex = findTriggerPoint(data.buffer, data.writeIndex);
    
    if (triggerIndex >= 0) {
        data.triggered = true;
        data.triggerIndex = triggerIndex;
        triggerArmed = false;
        triggerHoldoff = true;
        triggerHoldoffCounter = TRIGGER_HOLDOFF_SAMPLES;
        
        notifyListeners([channel, this](Listener& l) { 
            l.scopeTriggered(channel, settings.triggerLevel); 
        });
    }
}

void AudioScopeComponent::calculateMeasurements(int channel)
{
    const auto& buffer = channelData[channel].displayBuffer;
    if (buffer.empty()) return;
    
    auto& measurement = measurements[channel];
    
    // Calculate peak-to-peak
    auto minMax = std::minmax_element(buffer.begin(), buffer.end());
    measurement.peakToPeak = *minMax.second - *minMax.first;
    
    // Calculate RMS
    float sumSquares = 0.0f;
    for (float sample : buffer) {
        sumSquares += sample * sample;
    }
    measurement.rms = std::sqrt(sumSquares / buffer.size());
    
    // Calculate DC level (average)
    float sum = std::accumulate(buffer.begin(), buffer.end(), 0.0f);
    measurement.dcLevel = sum / buffer.size();
    
    // Simple frequency estimation using zero crossings
    int zeroCrossings = 0;
    for (int i = 1; i < buffer.size(); ++i) {
        if ((buffer[i-1] >= 0.0f && buffer[i] < 0.0f) ||
            (buffer[i-1] < 0.0f && buffer[i] >= 0.0f)) {
            zeroCrossings++;
        }
    }
    
    float timeSpan = buffer.size() / settings.sampleRate;
    measurement.frequency = (zeroCrossings > 0) ? (zeroCrossings / 2.0f) / timeSpan : 0.0f;
    measurement.period = (measurement.frequency > 0) ? 1.0f / measurement.frequency : 0.0f;
    
    measurement.valid = true;
    
    notifyListeners([channel, &measurement](Listener& l) { 
        l.measurementsUpdated(channel, measurement); 
    });
}

void AudioScopeComponent::updatePersistence()
{
    for (auto& data : channelData) {
        if (data.persistenceBuffer.size() != data.displayBuffer.size()) {
            data.persistenceBuffer.resize(data.displayBuffer.size(), 0.0f);
        }
        
        // Apply persistence decay and accumulate new data
        for (int i = 0; i < data.persistenceBuffer.size(); ++i) {
            data.persistenceBuffer[i] *= settings.persistenceDecay;
            data.persistenceBuffer[i] = juce::jmax(data.persistenceBuffer[i], 
                                                  std::abs(data.displayBuffer[i]));
        }
    }
}

void AudioScopeComponent::handleCursorDrag(const juce::MouseEvent& event)
{
    auto scopePoint = screenToScope(event.getPosition());
    
    switch (selectedCursor) {
        case 0: cursorData.timeA = scopePoint.x; break;
        case 1: cursorData.timeB = scopePoint.x; break;
        case 2: cursorData.voltageA = scopePoint.y; break;
        case 3: cursorData.voltageB = scopePoint.y; break;
    }
    
    notifyListeners([this](Listener& l) { l.cursorsChanged(cursorData); });
}

void AudioScopeComponent::handlePositionDrag(const juce::MouseEvent& event)
{
    auto delta = event.getPosition() - lastMousePosition;
    
    // Adjust horizontal and vertical positions
    settings.horizontalPosition += delta.x * timePerPixel / (timeBaseToSeconds(settings.timeBase) * gridDivisions);
    settings.verticalPosition += delta.y * voltsPerPixel / (verticalScaleToVolts(settings.verticalScale) * gridDivisions);
    
    settings.horizontalPosition = juce::jlimit(0.0f, 1.0f, settings.horizontalPosition);
    settings.verticalPosition = juce::jlimit(0.0f, 1.0f, settings.verticalPosition);
}

void AudioScopeComponent::handleTriggerDrag(const juce::MouseEvent& event)
{
    auto scopePoint = screenToScope(event.getPosition());
    setTriggerLevel(scopePoint.y);
}

void AudioScopeComponent::initializeBuffers()
{
    for (auto& data : channelData) {
        data.buffer.resize(settings.bufferSize, 0.0f);
        data.displayBuffer.resize(settings.bufferSize, 0.0f);
        data.persistenceBuffer.resize(settings.bufferSize, 0.0f);
        data.writeIndex = 0;
        data.triggerIndex = 0;
        data.triggered = false;
        data.dcOffset = 0.0f;
        data.gain = 1.0f;
    }
}

void AudioScopeComponent::resizeBuffers()
{
    channelData.resize(settings.numChannels);
    measurements.resize(settings.numChannels);
    
    for (int i = 0; i < settings.numChannels; ++i) {
        auto& data = channelData[i];
        data.buffer.resize(settings.bufferSize, 0.0f);
        data.displayBuffer.resize(settings.bufferSize, 0.0f);
        data.persistenceBuffer.resize(settings.bufferSize, 0.0f);
        
        if (i >= channelData.size()) {
            data.color = (i == 0) ? settings.traceColor1 : settings.traceColor2;
            data.enabled = true;
            data.writeIndex = 0;
            data.triggerIndex = 0;
            data.triggered = false;
            data.dcOffset = 0.0f;
            data.gain = 1.0f;
        }
    }
}

void AudioScopeComponent::notifyListeners(std::function<void(Listener&)> callback)
{
    listeners.call(callback);
}

float AudioScopeComponent::interpolateSample(const std::vector<float>& buffer, float index) const
{
    if (buffer.empty()) return 0.0f;
    
    int i0 = static_cast<int>(index);
    int i1 = i0 + 1;
    
    if (i0 < 0) return buffer[0];
    if (i1 >= buffer.size()) return buffer.back();
    
    float fraction = index - i0;
    return buffer[i0] * (1.0f - fraction) + buffer[i1] * fraction;
}

int AudioScopeComponent::findTriggerPoint(const std::vector<float>& buffer, int startIndex) const
{
    if (buffer.size() < 2) return -1;
    
    for (int i = 1; i < buffer.size(); ++i) {
        int index = (startIndex + i) % buffer.size();
        int prevIndex = (startIndex + i - 1) % buffer.size();
        
        float current = buffer[index];
        float previous = buffer[prevIndex];
        
        bool triggered = false;
        
        switch (settings.triggerSlope) {
            case TriggerSlope::Rising:
                triggered = (previous < settings.triggerLevel && current >= settings.triggerLevel);
                break;
                
            case TriggerSlope::Falling:
                triggered = (previous > settings.triggerLevel && current <= settings.triggerLevel);
                break;
                
            case TriggerSlope::Both:
                triggered = ((previous < settings.triggerLevel && current >= settings.triggerLevel) ||
                           (previous > settings.triggerLevel && current <= settings.triggerLevel));
                break;
        }
        
        if (triggered && std::abs(current - previous) > settings.triggerHysteresis) {
            return index;
        }
    }
    
    return -1;
}

void AudioScopeComponent::performFFT(int channel)
{
    if (channel >= fftData.size()) return;
    
    const auto& buffer = channelData[channel].displayBuffer;
    auto& fftBuffer = fftData[channel];
    auto& magnitudes = fftMagnitudes[channel];
    
    // Copy data to FFT buffer (zero-padded if necessary)
    std::fill(fftBuffer.begin(), fftBuffer.end(), 0.0f);
    int copySize = juce::jmin(static_cast<int>(buffer.size()), FFT_SIZE);
    
    for (int i = 0; i < copySize; ++i) {
        fftBuffer[i * 2] = buffer[i]; // Real part
        fftBuffer[i * 2 + 1] = 0.0f;  // Imaginary part
    }
    
    // Perform FFT
    fftProcessor->performFrequencyOnlyForwardTransform(fftBuffer.data());
    
    // Calculate magnitudes
    for (int i = 0; i < FFT_SIZE / 2; ++i) {
        float real = fftBuffer[i * 2];
        float imag = fftBuffer[i * 2 + 1];
        magnitudes[i] = std::sqrt(real * real + imag * imag);
    }
}

void AudioScopeComponent::recordSample(int channel, float sample)
{
    if (channel >= recordingData.size()) return;
    
    recordingData[channel].push_back(sample);
}

void AudioScopeComponent::saveRecordingData()
{
    if (recordingData.empty() || recordingFile == juce::File{}) return;
    
    juce::FileOutputStream stream(recordingFile);
    if (!stream.openedOk()) return;
    
    // Write CSV header
    stream.writeText("Time", false, false, nullptr);
    for (int i = 0; i < recordingData.size(); ++i) {
        stream.writeText(",Channel" + juce::String(i + 1), false, false, nullptr);
    }
    stream.writeText("\n", false, false, nullptr);
    
    // Find maximum number of samples
    int maxSamples = 0;
    for (const auto& channelData : recordingData) {
        maxSamples = juce::jmax(maxSamples, static_cast<int>(channelData.size()));
    }
    
    // Write data
    for (int sample = 0; sample < maxSamples; ++sample) {
        float time = sample / settings.sampleRate;
        stream.writeText(juce::String(time, 6), false, false, nullptr);
        
        for (int channel = 0; channel < recordingData.size(); ++channel) {
            float value = (sample < recordingData[channel].size()) ? 
                         recordingData[channel][sample] : 0.0f;
            stream.writeText("," + juce::String(value, 6), false, false, nullptr);
        }
        stream.writeText("\n", false, false, nullptr);
    }
    
    recordingData.clear();
}

} // namespace Visualizations
} // namespace UI
} // namespace OTTO