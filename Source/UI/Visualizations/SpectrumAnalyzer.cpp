#include "SpectrumAnalyzer.h"
#include "JUCE8_CODING_STANDARDS.h"
#include <algorithm>
#include <cmath>

//==============================================================================
// SpectrumAnalyzer Implementation
//==============================================================================

SpectrumAnalyzer::SpectrumAnalyzer(ColorScheme& colorScheme)
    : colorScheme(colorScheme)
{
    // Initialize FFT and buffers
    initializeFFT();
    initializeBuffers();
    initializeWindow();
    
    // Setup timer for display updates (60 FPS)
    startTimer(16); // ~60 FPS
    
    // Load settings from INI
    ComponentState state;
    if (INIDataManager::loadComponentState("SpectrumAnalyzer", state)) {
        loadState(state);
    }
    
    // Update colors from theme
    updateColorsFromTheme();
    
    // Enable mouse tracking
    setMouseClickGrabsKeyboardFocus(false);
}

SpectrumAnalyzer::~SpectrumAnalyzer() {
    // Save current state
    ComponentState state;
    saveState(state);
    INIDataManager::saveComponentState("SpectrumAnalyzer", state);
    
    stopTimer();
}

//==============================================================================
// Component Overrides
//==============================================================================

void SpectrumAnalyzer::paint(juce::Graphics& g) {
    // Clear background
    g.fillAll(settings.backgroundColor);
    
    // Early return if no data
    if (magnitudeData.empty()) {
        g.setColour(colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
        g.setFont(JUCE8_FONT(14.0f));
        g.drawText("No audio signal", getLocalBounds(), juce::Justification::centred);
        return;
    }
    
    // Enable antialiasing for smooth rendering
    g.setImageResamplingQuality(juce::Graphics::highResamplingQuality);
    
    // Draw components in order
    if (settings.showGrid) {
        drawGrid(g);
    }
    
    drawSpectrum(g);
    
    if (settings.showPeakHold) {
        drawPeakHold(g);
    }
    
    if (settings.showLabels) {
        drawLabels(g);
    }
    
    if (showFrequencyCursor) {
        drawFrequencyCursor(g);
    }
}

void SpectrumAnalyzer::resized() {
    // Recalculate display paths when size changes
    juce::ScopedLock lock(displayLock);
    
    updateSpectrumPath();
    updatePeakHoldPath();
    updateGridPath();
    
    // Optimize for current size
    optimizeForPerformance();
}

void SpectrumAnalyzer::mouseMove(const juce::MouseEvent& e) {
    lastMousePosition = e.getPosition();
    cursorFrequency = xToFrequency(static_cast<float>(e.x));
    
    if (settings.showCursor) {
        showFrequencyCursor = true;
        repaint();
    }
}

void SpectrumAnalyzer::mouseDown(const juce::MouseEvent& e) {
    if (e.mods.isRightButtonDown()) {
        // Right-click for context menu (future implementation)
        return;
    }
    
    // Left-click to focus on frequency
    cursorFrequency = xToFrequency(static_cast<float>(e.x));
    showFrequencyCursor = true;
    repaint();
}

void SpectrumAnalyzer::mouseUp(const juce::MouseEvent& e) {
    // Handle mouse up events if needed
}

void SpectrumAnalyzer::mouseDrag(const juce::MouseEvent& e) {
    // Update cursor position during drag
    cursorFrequency = xToFrequency(static_cast<float>(e.x));
    repaint();
}

//==============================================================================
// Audio Processing
//==============================================================================

void SpectrumAnalyzer::processAudioBlock(const juce::AudioBuffer<float>& buffer) {
    if (shouldSkipFrame()) {
        return;
    }
    
    auto startTime = juce::Time::getMillisecondCounterHiRes();
    
    juce::ScopedLock lock(bufferLock);
    
    // Mix down to mono if stereo
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    
    for (int sample = 0; sample < numSamples; ++sample) {
        float monoSample = 0.0f;
        
        // Mix all channels to mono
        for (int channel = 0; channel < numChannels; ++channel) {
            monoSample += buffer.getSample(channel, sample);
        }
        
        if (numChannels > 1) {
            monoSample /= static_cast<float>(numChannels);
        }
        
        // Write to circular buffer
        circularBuffer.setSample(0, circularBufferWritePos, monoSample);
        circularBufferWritePos = (circularBufferWritePos + 1) % circularBuffer.getNumSamples();
        
        // Check if we have enough data for FFT
        if (circularBufferWritePos % (settings.fftSize / settings.overlapFactor) == 0) {
            bufferReady = true;
        }
    }
    
    // Update performance stats
    auto processingTime = juce::Time::getMillisecondCounterHiRes() - startTime;
    updatePerformanceStats(processingTime);
}

void SpectrumAnalyzer::setSampleRate(double newSampleRate) {
    if (newSampleRate > 0 && newSampleRate != sampleRate) {
        sampleRate = newSampleRate;
        
        // Reinitialize buffers for new sample rate
        initializeBuffers();
        
        // Update frequency range if needed
        if (settings.maxFrequency > sampleRate / 2.0f) {
            settings.maxFrequency = static_cast<float>(sampleRate / 2.0f);
        }
    }
}

//==============================================================================
// Settings Management
//==============================================================================

void SpectrumAnalyzer::setAnalyzerSettings(const AnalyzerSettings& newSettings) {
    settings = newSettings;
    
    // Validate and clamp settings
    settings.fftSize = juce::jlimit(512, 8192, settings.fftSize);
    settings.overlapFactor = juce::jlimit(1, 8, settings.overlapFactor);
    settings.minFrequency = juce::jlimit(1.0f, static_cast<float>(sampleRate / 2.0f), settings.minFrequency);
    settings.maxFrequency = juce::jlimit(settings.minFrequency, static_cast<float>(sampleRate / 2.0f), settings.maxFrequency);
    settings.averagingFactor = juce::jlimit(0.0f, 1.0f, settings.averagingFactor);
    
    // Reinitialize if FFT size changed
    if (fft->getSize() != settings.fftSize) {
        initializeFFT();
        initializeBuffers();
        initializeWindow();
    }
    
    updateColors();
    repaint();
}

void SpectrumAnalyzer::setFFTSize(int size) {
    if (isValidFFTSize(size) && size != settings.fftSize) {
        settings.fftSize = size;
        initializeFFT();
        initializeBuffers();
        initializeWindow();
    }
}

void SpectrumAnalyzer::setDisplayMode(DisplayMode mode) {
    if (mode != settings.displayMode) {
        settings.displayMode = mode;
        updateSpectrumPath();
        updatePeakHoldPath();
        updateGridPath();
        repaint();
    }
}

void SpectrumAnalyzer::setFrequencyRange(float minFreq, float maxFreq) {
    if (isValidFrequencyRange(minFreq, maxFreq)) {
        settings.minFrequency = minFreq;
        settings.maxFrequency = maxFreq;
        updateSpectrumPath();
        updatePeakHoldPath();
        updateGridPath();
        repaint();
    }
}

void SpectrumAnalyzer::setAmplitudeRange(float minDb, float maxDb) {
    if (isValidAmplitudeRange(minDb, maxDb)) {
        settings.minDecibels = minDb;
        settings.maxDecibels = maxDb;
        updateSpectrumPath();
        updatePeakHoldPath();
        repaint();
    }
}

void SpectrumAnalyzer::setAveragingMode(AveragingMode mode) {
    settings.averagingMode = mode;
    
    // Reset averaging data when mode changes
    if (!averagedMagnitudes.empty()) {
        std::fill(averagedMagnitudes.begin(), averagedMagnitudes.end(), settings.minDecibels);
    }
}

void SpectrumAnalyzer::setAveragingFactor(float factor) {
    settings.averagingFactor = juce::jlimit(0.0f, 1.0f, factor);
}

//==============================================================================
// Display Options
//==============================================================================

void SpectrumAnalyzer::setShowPeakHold(bool show) {
    settings.showPeakHold = show;
    repaint();
}

void SpectrumAnalyzer::setShowGrid(bool show) {
    settings.showGrid = show;
    repaint();
}

void SpectrumAnalyzer::setShowLabels(bool show) {
    settings.showLabels = show;
    repaint();
}

//==============================================================================
// Color Customization
//==============================================================================

void SpectrumAnalyzer::setSpectrumColor(const juce::Colour& color) {
    settings.spectrumColor = color;
    repaint();
}

void SpectrumAnalyzer::setPeakHoldColor(const juce::Colour& color) {
    settings.peakHoldColor = color;
    repaint();
}

void SpectrumAnalyzer::setGridColor(const juce::Colour& color) {
    settings.gridColor = color;
    repaint();
}

void SpectrumAnalyzer::updateColorsFromTheme() {
    settings.backgroundColor = colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground);
    settings.spectrumColor = colorScheme.getColor(ColorScheme::ColorRole::Accent);
    settings.peakHoldColor = colorScheme.getColor(ColorScheme::ColorRole::MeterHigh);
    settings.gridColor = colorScheme.getColor(ColorScheme::ColorRole::GridLine);
    
    updateColors();
    repaint();
}

//==============================================================================
// Frequency Analysis
//==============================================================================

float SpectrumAnalyzer::getFrequencyAtX(int x) const {
    return xToFrequency(static_cast<float>(x));
}

int SpectrumAnalyzer::getXForFrequency(float frequency) const {
    return static_cast<int>(frequencyToX(frequency));
}

float SpectrumAnalyzer::getMagnitudeAtFrequency(float frequency) const {
    if (magnitudeData.empty()) {
        return settings.minDecibels;
    }
    
    // Find the bin index for this frequency
    float binWidth = static_cast<float>(sampleRate) / static_cast<float>(settings.fftSize);
    int binIndex = static_cast<int>(frequency / binWidth);
    
    if (binIndex >= 0 && binIndex < static_cast<int>(magnitudeData.size())) {
        return magnitudeData[binIndex];
    }
    
    return settings.minDecibels;
}

juce::Array<SpectrumAnalyzer::FrequencyBin> SpectrumAnalyzer::getFrequencyBins() const {
    juce::Array<FrequencyBin> bins;
    
    if (magnitudeData.empty()) {
        return bins;
    }
    
    float binWidth = static_cast<float>(sampleRate) / static_cast<float>(settings.fftSize);
    
    for (int i = 0; i < static_cast<int>(magnitudeData.size()); ++i) {
        FrequencyBin bin;
        bin.frequency = i * binWidth;
        bin.magnitude = magnitudeData[i];
        bin.phase = (i < static_cast<int>(phaseData.size())) ? phaseData[i] : 0.0f;
        bin.peakHold = (i < static_cast<int>(peakHoldData.size())) ? peakHoldData[i] : settings.minDecibels;
        
        if (i < static_cast<int>(peakHoldTimes.size())) {
            bin.peakTime = peakHoldTimes[i];
        }
        
        bins.add(bin);
    }
    
    return bins;
}

//==============================================================================
// Peak Detection
//==============================================================================

juce::Array<SpectrumAnalyzer::Peak> SpectrumAnalyzer::detectPeaks(float threshold, int minPeakDistance) const {
    juce::Array<Peak> peaks;
    
    if (magnitudeData.empty() || magnitudeData.size() < 3) {
        return peaks;
    }
    
    float binWidth = static_cast<float>(sampleRate) / static_cast<float>(settings.fftSize);
    
    for (int i = minPeakDistance; i < static_cast<int>(magnitudeData.size()) - minPeakDistance; ++i) {
        float magnitude = magnitudeData[i];
        
        // Check if this is above threshold
        if (magnitude < threshold) {
            continue;
        }
        
        // Check if this is a local maximum
        bool isPeak = true;
        for (int j = i - minPeakDistance; j <= i + minPeakDistance; ++j) {
            if (j != i && magnitudeData[j] >= magnitude) {
                isPeak = false;
                break;
            }
        }
        
        if (isPeak) {
            float frequency = i * binWidth;
            
            // Calculate bandwidth (simple estimation)
            float bandwidth = 0.0f;
            int leftEdge = i, rightEdge = i;
            float halfPower = magnitude - 3.0f; // -3dB point
            
            // Find left edge
            for (int j = i - 1; j >= 0; --j) {
                if (magnitudeData[j] < halfPower) {
                    leftEdge = j;
                    break;
                }
            }
            
            // Find right edge
            for (int j = i + 1; j < static_cast<int>(magnitudeData.size()); ++j) {
                if (magnitudeData[j] < halfPower) {
                    rightEdge = j;
                    break;
                }
            }
            
            bandwidth = (rightEdge - leftEdge) * binWidth;
            
            peaks.add(Peak(frequency, magnitude, bandwidth));
        }
    }
    
    return peaks;
}

SpectrumAnalyzer::Peak SpectrumAnalyzer::getFundamentalFrequency() const {
    auto peaks = detectPeaks(-40.0f, 3);
    
    if (peaks.isEmpty()) {
        return Peak(0.0f, settings.minDecibels);
    }
    
    // Find the strongest peak in the fundamental range (typically 80-800 Hz)
    Peak fundamental(0.0f, settings.minDecibels);
    
    for (const auto& peak : peaks) {
        if (peak.frequency >= 80.0f && peak.frequency <= 800.0f) {
            if (peak.magnitude > fundamental.magnitude) {
                fundamental = peak;
            }
        }
    }
    
    return fundamental;
}

//==============================================================================
// State Management
//==============================================================================

void SpectrumAnalyzer::saveState(ComponentState& state) const {
    state.setValue("fftSize", settings.fftSize);
    state.setValue("displayMode", static_cast<int>(settings.displayMode));
    state.setValue("windowType", static_cast<int>(settings.windowType));
    state.setValue("averagingMode", static_cast<int>(settings.averagingMode));
    state.setValue("minFrequency", settings.minFrequency);
    state.setValue("maxFrequency", settings.maxFrequency);
    state.setValue("minDecibels", settings.minDecibels);
    state.setValue("maxDecibels", settings.maxDecibels);
    state.setValue("averagingFactor", settings.averagingFactor);
    state.setValue("showPeakHold", settings.showPeakHold);
    state.setValue("showGrid", settings.showGrid);
    state.setValue("showLabels", settings.showLabels);
}

void SpectrumAnalyzer::loadState(const ComponentState& state) {
    settings.fftSize = state.getValue("fftSize", 2048);
    settings.displayMode = static_cast<DisplayMode>(state.getValue("displayMode", static_cast<int>(DisplayMode::Logarithmic)));
    settings.windowType = static_cast<WindowType>(state.getValue("windowType", static_cast<int>(WindowType::Hanning)));
    settings.averagingMode = static_cast<AveragingMode>(state.getValue("averagingMode", static_cast<int>(AveragingMode::Exponential)));
    settings.minFrequency = state.getValue("minFrequency", 20.0f);
    settings.maxFrequency = state.getValue("maxFrequency", 20000.0f);
    settings.minDecibels = state.getValue("minDecibels", -80.0f);
    settings.maxDecibels = state.getValue("maxDecibels", 0.0f);
    settings.averagingFactor = state.getValue("averagingFactor", 0.8f);
    settings.showPeakHold = state.getValue("showPeakHold", true);
    settings.showGrid = state.getValue("showGrid", true);
    settings.showLabels = state.getValue("showLabels", true);
    
    // Apply loaded settings
    setAnalyzerSettings(settings);
}

//==============================================================================
// Timer Callback
//==============================================================================

void SpectrumAnalyzer::timerCallback() {
    if (bufferReady) {
        performFFT();
        bufferReady = false;
        repaint();
    }
}

//==============================================================================
// AudioProcessorValueTreeState::Listener
//==============================================================================

void SpectrumAnalyzer::parameterChanged(const juce::String& parameterID, float newValue) {
    // Handle parameter changes from host if needed
    // This would be used when integrated with an audio plugin
}

//==============================================================================
// Private Implementation
//==============================================================================

void SpectrumAnalyzer::initializeFFT() {
    fft = std::make_unique<juce::dsp::FFT>(static_cast<int>(std::log2(settings.fftSize)));
}

void SpectrumAnalyzer::initializeBuffers() {
    // Initialize circular buffer (4x FFT size for overlap)
    int circularBufferSize = settings.fftSize * 4;
    circularBuffer.setSize(1, circularBufferSize);
    circularBuffer.clear();
    circularBufferWritePos = 0;
    
    // Initialize FFT buffer
    fftBuffer.setSize(1, settings.fftSize);
    fftBuffer.clear();
    
    // Initialize data vectors
    fftData.resize(settings.fftSize * 2, 0.0f); // Complex data (real + imaginary)
    magnitudeData.resize(settings.fftSize / 2, settings.minDecibels);
    phaseData.resize(settings.fftSize / 2, 0.0f);
    averagedMagnitudes.resize(settings.fftSize / 2, settings.minDecibels);
    peakHoldData.resize(settings.fftSize / 2, settings.minDecibels);
    peakHoldTimes.resize(settings.fftSize / 2);
    
    // Initialize peak hold times
    auto currentTime = juce::Time::getCurrentTime();
    for (auto& time : peakHoldTimes) {
        time = currentTime;
    }
}

void SpectrumAnalyzer::initializeWindow() {
    juce::dsp::WindowingFunction<float>::WindowingMethod windowMethod;
    
    switch (settings.windowType) {
        case WindowType::Rectangular:
            windowMethod = juce::dsp::WindowingFunction<float>::rectangular;
            break;
        case WindowType::Hanning:
            windowMethod = juce::dsp::WindowingFunction<float>::hann;
            break;
        case WindowType::Hamming:
            windowMethod = juce::dsp::WindowingFunction<float>::hamming;
            break;
        case WindowType::Blackman:
            windowMethod = juce::dsp::WindowingFunction<float>::blackman;
            break;
        case WindowType::BlackmanHarris:
            windowMethod = juce::dsp::WindowingFunction<float>::blackmanHarris;
            break;
        case WindowType::Kaiser:
            windowMethod = juce::dsp::WindowingFunction<float>::kaiser;
            break;
        default:
            windowMethod = juce::dsp::WindowingFunction<float>::hann;
            break;
    }
    
    window = std::make_unique<juce::dsp::WindowingFunction<float>>(settings.fftSize, windowMethod);
}

void SpectrumAnalyzer::performFFT() {
    juce::ScopedLock lock(bufferLock);
    
    // Copy data from circular buffer to FFT buffer
    int readPos = (circularBufferWritePos - settings.fftSize + circularBuffer.getNumSamples()) % circularBuffer.getNumSamples();
    
    for (int i = 0; i < settings.fftSize; ++i) {
        int pos = (readPos + i) % circularBuffer.getNumSamples();
        fftBuffer.setSample(0, i, circularBuffer.getSample(0, pos));
    }
    
    // Apply window function
    applyWindow();
    
    // Copy to FFT data array (interleaved real/imaginary)
    for (int i = 0; i < settings.fftSize; ++i) {
        fftData[i * 2] = fftBuffer.getSample(0, i);     // Real part
        fftData[i * 2 + 1] = 0.0f;                      // Imaginary part
    }
    
    // Perform FFT
    fft->performFrequencyOnlyForwardTransform(fftData.data());
    
    // Calculate magnitudes and phases
    calculateMagnitudes();
    
    // Apply averaging
    updateAveraging();
    
    // Update peak hold
    updatePeakHold();
    
    // Update display paths
    {
        juce::ScopedLock displayLock(this->displayLock);
        updateSpectrumPath();
        updatePeakHoldPath();
    }
}

void SpectrumAnalyzer::applyWindow() {
    window->multiplyWithWindowingTable(fftBuffer.getWritePointer(0), settings.fftSize);
}

void SpectrumAnalyzer::calculateMagnitudes() {
    for (int i = 0; i < settings.fftSize / 2; ++i) {
        float real = fftData[i * 2];
        float imag = fftData[i * 2 + 1];
        
        // Calculate magnitude
        float magnitude = std::sqrt(real * real + imag * imag);
        
        // Convert to dB
        if (magnitude > 0.0f) {
            magnitudeData[i] = 20.0f * std::log10(magnitude);
        } else {
            magnitudeData[i] = settings.minDecibels;
        }
        
        // Calculate phase
        phaseData[i] = std::atan2(imag, real);
    }
}

void SpectrumAnalyzer::updateAveraging() {
    if (settings.averagingMode == AveragingMode::None) {
        return;
    }
    
    for (int i = 0; i < static_cast<int>(magnitudeData.size()); ++i) {
        switch (settings.averagingMode) {
            case AveragingMode::Exponential:
                averagedMagnitudes[i] = averagedMagnitudes[i] * settings.averagingFactor + 
                                       magnitudeData[i] * (1.0f - settings.averagingFactor);
                break;
                
            case AveragingMode::Linear:
                // Simple linear averaging (could be improved with a proper moving average)
                averagedMagnitudes[i] = (averagedMagnitudes[i] + magnitudeData[i]) * 0.5f;
                break;
                
            case AveragingMode::PeakHold:
                // Peak hold is handled separately
                averagedMagnitudes[i] = magnitudeData[i];
                break;
                
            default:
                averagedMagnitudes[i] = magnitudeData[i];
                break;
        }
    }
    
    // Use averaged data for display
    if (settings.averagingMode != AveragingMode::None) {
        magnitudeData = averagedMagnitudes;
    }
}

void SpectrumAnalyzer::updatePeakHold() {
    if (!settings.showPeakHold) {
        return;
    }
    
    auto currentTime = juce::Time::getCurrentTime();
    
    for (int i = 0; i < static_cast<int>(magnitudeData.size()); ++i) {
        // Update peak if current magnitude is higher
        if (magnitudeData[i] > peakHoldData[i]) {
            peakHoldData[i] = magnitudeData[i];
            peakHoldTimes[i] = currentTime;
        } else {
            // Apply decay
            auto timeSinceLastPeak = currentTime - peakHoldTimes[i];
            auto decayTime = timeSinceLastPeak.inSeconds();
            
            if (decayTime > settings.peakHoldTime) {
                float decay = settings.peakDecayRate * static_cast<float>(decayTime - settings.peakHoldTime);
                peakHoldData[i] = juce::jmax(peakHoldData[i] - decay, settings.minDecibels);
            }
        }
    }
}

void SpectrumAnalyzer::updateSpectrumPath() {
    spectrumPath.clear();
    
    if (magnitudeData.empty() || getWidth() <= 0 || getHeight() <= 0) {
        return;
    }
    
    float binWidth = static_cast<float>(sampleRate) / static_cast<float>(settings.fftSize);
    
    for (int i = 0; i < static_cast<int>(magnitudeData.size()); ++i) {
        float frequency = i * binWidth;
        
        // Skip frequencies outside our display range
        if (frequency < settings.minFrequency || frequency > settings.maxFrequency) {
            continue;
        }
        
        float x = frequencyToX(frequency);
        float y = magnitudeToY(magnitudeData[i]);
        
        spectrumPath.emplace_back(x, y);
    }
}

void SpectrumAnalyzer::updatePeakHoldPath() {
    peakHoldPath.clear();
    
    if (peakHoldData.empty() || !settings.showPeakHold || getWidth() <= 0 || getHeight() <= 0) {
        return;
    }
    
    float binWidth = static_cast<float>(sampleRate) / static_cast<float>(settings.fftSize);
    
    for (int i = 0; i < static_cast<int>(peakHoldData.size()); ++i) {
        float frequency = i * binWidth;
        
        // Skip frequencies outside our display range
        if (frequency < settings.minFrequency || frequency > settings.maxFrequency) {
            continue;
        }
        
        float x = frequencyToX(frequency);
        float y = magnitudeToY(peakHoldData[i]);
        
        peakHoldPath.emplace_back(x, y);
    }
}

void SpectrumAnalyzer::updateGridPath() {
    gridPath.clear();
    
    if (getWidth() <= 0 || getHeight() <= 0) {
        return;
    }
    
    // Frequency grid lines
    auto freqLines = calculateFrequencyGridLines();
    for (float freq : freqLines) {
        float x = frequencyToX(freq);
        if (x >= 0 && x <= getWidth()) {
            gridPath.startNewSubPath(x, 0);
            gridPath.lineTo(x, static_cast<float>(getHeight()));
        }
    }
    
    // Amplitude grid lines
    auto ampLines = calculateAmplitudeGridLines();
    for (float amp : ampLines) {
        float y = magnitudeToY(amp);
        if (y >= 0 && y <= getHeight()) {
            gridPath.startNewSubPath(0, y);
            gridPath.lineTo(static_cast<float>(getWidth()), y);
        }
    }
}

//==============================================================================
// Coordinate Transformations
//==============================================================================

float SpectrumAnalyzer::frequencyToX(float frequency) const {
    if (getWidth() <= 0) return 0.0f;
    
    switch (settings.displayMode) {
        case DisplayMode::Linear:
            return (frequency - settings.minFrequency) / (settings.maxFrequency - settings.minFrequency) * getWidth();
            
        case DisplayMode::Logarithmic:
            if (frequency <= 0 || settings.minFrequency <= 0) return 0.0f;
            return std::log10(frequency / settings.minFrequency) / 
                   std::log10(settings.maxFrequency / settings.minFrequency) * getWidth();
            
        case DisplayMode::MelScale:
            // Mel scale conversion: mel = 2595 * log10(1 + freq/700)
            {
                float melFreq = 2595.0f * std::log10(1.0f + frequency / 700.0f);
                float melMin = 2595.0f * std::log10(1.0f + settings.minFrequency / 700.0f);
                float melMax = 2595.0f * std::log10(1.0f + settings.maxFrequency / 700.0f);
                return (melFreq - melMin) / (melMax - melMin) * getWidth();
            }
            
        default:
            return frequencyToX(frequency); // Fallback to logarithmic
    }
}

float SpectrumAnalyzer::xToFrequency(float x) const {
    if (getWidth() <= 0) return settings.minFrequency;
    
    float ratio = x / getWidth();
    
    switch (settings.displayMode) {
        case DisplayMode::Linear:
            return settings.minFrequency + ratio * (settings.maxFrequency - settings.minFrequency);
            
        case DisplayMode::Logarithmic:
            if (settings.minFrequency <= 0) return settings.minFrequency;
            return settings.minFrequency * std::pow(settings.maxFrequency / settings.minFrequency, ratio);
            
        case DisplayMode::MelScale:
            // Inverse mel scale conversion
            {
                float melMin = 2595.0f * std::log10(1.0f + settings.minFrequency / 700.0f);
                float melMax = 2595.0f * std::log10(1.0f + settings.maxFrequency / 700.0f);
                float mel = melMin + ratio * (melMax - melMin);
                return 700.0f * (std::pow(10.0f, mel / 2595.0f) - 1.0f);
            }
            
        default:
            return xToFrequency(x); // Fallback to logarithmic
    }
}

float SpectrumAnalyzer::magnitudeToY(float magnitude) const {
    if (getHeight() <= 0) return 0.0f;
    
    float ratio = (magnitude - settings.minDecibels) / (settings.maxDecibels - settings.minDecibels);
    return getHeight() - (ratio * getHeight()); // Invert Y axis (0 at top)
}

float SpectrumAnalyzer::yToMagnitude(float y) const {
    if (getHeight() <= 0) return settings.minDecibels;
    
    float ratio = (getHeight() - y) / getHeight(); // Invert Y axis
    return settings.minDecibels + ratio * (settings.maxDecibels - settings.minDecibels);
}

//==============================================================================
// Drawing Helpers
//==============================================================================

void SpectrumAnalyzer::drawSpectrum(juce::Graphics& g) {
    if (spectrumPath.empty()) {
        return;
    }
    
    // Create path from spectrum points
    juce::Path path;
    
    if (!spectrumPath.empty()) {
        path.startNewSubPath(spectrumPath[0]);
        
        for (size_t i = 1; i < spectrumPath.size(); ++i) {
            path.lineTo(spectrumPath[i]);
        }
        
        // Close path to bottom for filled display
        if (!spectrumPath.empty()) {
            path.lineTo(spectrumPath.back().x, static_cast<float>(getHeight()));
            path.lineTo(spectrumPath.front().x, static_cast<float>(getHeight()));
            path.closeSubPath();
        }
    }
    
    // Draw filled spectrum
    g.setColour(settings.spectrumColor.withAlpha(0.3f));
    g.fillPath(path);
    
    // Draw spectrum outline
    g.setColour(settings.spectrumColor);
    g.strokePath(path, juce::PathStrokeType(1.0f));
}

void SpectrumAnalyzer::drawPeakHold(juce::Graphics& g) {
    if (peakHoldPath.empty()) {
        return;
    }
    
    g.setColour(settings.peakHoldColor);
    
    // Draw peak hold as individual lines
    for (size_t i = 0; i < peakHoldPath.size(); ++i) {
        const auto& point = peakHoldPath[i];
        g.drawLine(point.x - 1, point.y, point.x + 1, point.y, 1.0f);
    }
}

void SpectrumAnalyzer::drawGrid(juce::Graphics& g) {
    g.setColour(settings.gridColor);
    g.strokePath(gridPath, juce::PathStrokeType(0.5f));
}

void SpectrumAnalyzer::drawLabels(juce::Graphics& g) {
    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
    g.setFont(JUCE8_FONT(10.0f));
    
    // Draw frequency labels
    auto freqLines = calculateFrequencyGridLines();
    for (float freq : freqLines) {
        float x = frequencyToX(freq);
        if (x >= 0 && x <= getWidth()) {
            auto label = formatFrequency(freq);
            g.drawText(label, static_cast<int>(x - 20), getHeight() - 15, 40, 12, juce::Justification::centred);
        }
    }
    
    // Draw amplitude labels
    auto ampLines = calculateAmplitudeGridLines();
    for (float amp : ampLines) {
        float y = magnitudeToY(amp);
        if (y >= 0 && y <= getHeight()) {
            auto label = formatAmplitude(amp);
            g.drawText(label, 2, static_cast<int>(y - 6), 40, 12, juce::Justification::left);
        }
    }
}

void SpectrumAnalyzer::drawFrequencyCursor(juce::Graphics& g) {
    if (!showFrequencyCursor) {
        return;
    }
    
    float x = frequencyToX(cursorFrequency);
    
    g.setColour(juce::Colours::white.withAlpha(0.8f));
    g.drawLine(x, 0, x, static_cast<float>(getHeight()), 1.0f);
    
    // Draw frequency value
    auto freqText = formatFrequency(cursorFrequency);
    auto magnitude = getMagnitudeAtFrequency(cursorFrequency);
    auto magText = formatAmplitude(magnitude);
    
    g.setFont(JUCE8_FONT(12.0f));
    g.drawText(freqText + " / " + magText, 
               static_cast<int>(x + 5), 5, 100, 15, juce::Justification::left);
}

//==============================================================================
// Grid Calculation
//==============================================================================

std::vector<float> SpectrumAnalyzer::calculateFrequencyGridLines() const {
    std::vector<float> lines;
    
    switch (settings.displayMode) {
        case DisplayMode::Linear:
            {
                float step = (settings.maxFrequency - settings.minFrequency) / 10.0f;
                for (int i = 0; i <= 10; ++i) {
                    lines.push_back(settings.minFrequency + i * step);
                }
            }
            break;
            
        case DisplayMode::Logarithmic:
            {
                // Standard frequencies: 20, 50, 100, 200, 500, 1k, 2k, 5k, 10k, 20k
                std::vector<float> standardFreqs = {20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000};
                for (float freq : standardFreqs) {
                    if (freq >= settings.minFrequency && freq <= settings.maxFrequency) {
                        lines.push_back(freq);
                    }
                }
            }
            break;
            
        default:
            // Use logarithmic as default
            return calculateFrequencyGridLines();
    }
    
    return lines;
}

std::vector<float> SpectrumAnalyzer::calculateAmplitudeGridLines() const {
    std::vector<float> lines;
    
    // Standard dB lines: every 10 dB
    float step = 10.0f;
    float start = std::ceil(settings.minDecibels / step) * step;
    
    for (float db = start; db <= settings.maxDecibels; db += step) {
        lines.push_back(db);
    }
    
    return lines;
}

juce::String SpectrumAnalyzer::formatFrequency(float frequency) const {
    if (frequency >= 1000.0f) {
        return juce::String(frequency / 1000.0f, 1) + "k";
    } else {
        return juce::String(static_cast<int>(frequency));
    }
}

juce::String SpectrumAnalyzer::formatAmplitude(float amplitude) const {
    return juce::String(amplitude, 1) + "dB";
}

//==============================================================================
// Color Management
//==============================================================================

void SpectrumAnalyzer::updateColors() {
    // Update colors based on current theme
    // This could include gradient colors based on frequency
}

juce::Colour SpectrumAnalyzer::getSpectrumColorAtFrequency(float frequency) const {
    // Could implement frequency-based coloring here
    return settings.spectrumColor;
}

//==============================================================================
// Performance Optimization
//==============================================================================

void SpectrumAnalyzer::optimizeForPerformance() {
    // Adjust update rate based on component size
    int updateRate = 16; // Default 60 FPS
    
    if (getWidth() * getHeight() > 500000) { // Large display
        updateRate = 33; // 30 FPS
    } else if (getWidth() * getHeight() < 50000) { // Small display
        updateRate = 8; // 120 FPS
    }
    
    startTimer(updateRate);
}

bool SpectrumAnalyzer::shouldSkipFrame() const {
    // Skip frames if processing is taking too long
    return performanceStats.averageProcessingTime > 10.0; // 10ms threshold
}

void SpectrumAnalyzer::updatePerformanceStats(double processingTime) {
    processingTimes.push_back(processingTime);
    
    // Keep only last 100 measurements
    if (processingTimes.size() > 100) {
        processingTimes.erase(processingTimes.begin());
    }
    
    // Calculate average
    double sum = 0.0;
    for (double time : processingTimes) {
        sum += time;
    }
    performanceStats.averageProcessingTime = sum / processingTimes.size();
    
    // Update max
    performanceStats.maxProcessingTime = *std::max_element(processingTimes.begin(), processingTimes.end());
    
    // Simple CPU usage estimation
    performanceStats.cpuUsage = (performanceStats.averageProcessingTime / 16.67) * 100.0; // Assume 60 FPS target
}

//==============================================================================
// Validation Helpers
//==============================================================================

bool SpectrumAnalyzer::isValidFFTSize(int size) const {
    return size >= 512 && size <= 8192 && (size & (size - 1)) == 0; // Power of 2
}

bool SpectrumAnalyzer::isValidFrequencyRange(float minFreq, float maxFreq) const {
    return minFreq > 0 && maxFreq > minFreq && maxFreq <= sampleRate / 2.0;
}

bool SpectrumAnalyzer::isValidAmplitudeRange(float minDb, float maxDb) const {
    return minDb < maxDb && minDb >= -120.0f && maxDb <= 20.0f;
}