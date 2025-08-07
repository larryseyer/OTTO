#include "SpectrumAnalyzer.h"
#include "JUCE8_CODING_STANDARDS.h"
#include "INIDataManager.h"
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
    
    // Load settings from INI
    ComponentState state;
    if (INIDataManager::loadComponentState("SpectrumAnalyzer", state)) {
        loadState(state);
    }
    
    // Update colors from theme
    updateColorsFromTheme();
    
    // Start timer for display updates (60 FPS)
    startTimer(16);
    
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
    // Update display paths when size changes
    updateSpectrumPath();
    updatePeakHoldPath();
    updateGridPath();
}

void SpectrumAnalyzer::mouseMove(const juce::MouseEvent& e) {
    lastMousePosition = e.getPosition();
    
    if (settings.showCursor) {
        cursorFrequency = xToFrequency(static_cast<float>(e.x));
        showFrequencyCursor = true;
        repaint();
    }
}

void SpectrumAnalyzer::mouseDown(const juce::MouseEvent& e) {
    // Handle mouse interactions (e.g., frequency selection)
    cursorFrequency = xToFrequency(static_cast<float>(e.x));
    showFrequencyCursor = true;
    repaint();
}

void SpectrumAnalyzer::mouseUp(const juce::MouseEvent& e) {
    // Handle mouse release
}

void SpectrumAnalyzer::mouseDrag(const juce::MouseEvent& e) {
    // Update cursor frequency during drag
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
    
    auto startTime = juce::Time::getCurrentTime();
    
    juce::ScopedLock lock(bufferLock);
    
    // Mix down to mono if stereo
    int numSamples = buffer.getNumSamples();
    int numChannels = buffer.getNumChannels();
    
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
        
        // Check if we have enough samples for FFT
        if (circularBufferWritePos % (settings.fftSize / settings.overlapFactor) == 0) {
            bufferReady = true;
        }
    }
    
    // Perform FFT if buffer is ready
    if (bufferReady) {
        performFFT();
        bufferReady = false;
    }
    
    // Update performance stats
    auto endTime = juce::Time::getCurrentTime();
    double processingTime = (endTime - startTime).inMilliseconds();
    updatePerformanceStats(processingTime);
}

void SpectrumAnalyzer::setSampleRate(double newSampleRate) {
    if (newSampleRate != sampleRate && newSampleRate > 0) {
        sampleRate = newSampleRate;
        
        // Reinitialize buffers with new sample rate
        initializeBuffers();
        
        // Update frequency range if needed
        float nyquist = static_cast<float>(sampleRate * 0.5);
        if (settings.maxFrequency > nyquist) {
            settings.maxFrequency = nyquist;
        }
        
        repaint();
    }
}

//==============================================================================
// Settings Management
//==============================================================================

void SpectrumAnalyzer::setAnalyzerSettings(const AnalyzerSettings& newSettings) {
    settings = newSettings;
    
    // Validate and apply settings
    if (isValidFFTSize(settings.fftSize)) {
        initializeFFT();
        initializeBuffers();
    }
    
    if (isValidFrequencyRange(settings.minFrequency, settings.maxFrequency)) {
        // Frequency range is valid
    } else {
        // Reset to default range
        settings.minFrequency = 20.0f;
        settings.maxFrequency = static_cast<float>(sampleRate * 0.5);
    }
    
    if (isValidAmplitudeRange(settings.minDecibels, settings.maxDecibels)) {
        // Amplitude range is valid
    } else {
        // Reset to default range
        settings.minDecibels = -80.0f;
        settings.maxDecibels = 0.0f;
    }
    
    initializeWindow();
    updateColors();
    
    // Update display
    updateSpectrumPath();
    updatePeakHoldPath();
    updateGridPath();
    
    repaint();
}

void SpectrumAnalyzer::setFFTSize(int size) {
    if (isValidFFTSize(size) && size != settings.fftSize) {
        settings.fftSize = size;
        initializeFFT();
        initializeBuffers();
        repaint();
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
    if (mode != settings.averagingMode) {
        settings.averagingMode = mode;
        
        // Reset averaging data
        std::fill(averagedMagnitudes.begin(), averagedMagnitudes.end(), settings.minDecibels);
        
        if (mode == AveragingMode::PeakHold) {
            std::fill(peakHoldData.begin(), peakHoldData.end(), settings.minDecibels);
            std::fill(peakHoldTimes.begin(), peakHoldTimes.end(), juce::Time::getCurrentTime());
        }
    }
}

void SpectrumAnalyzer::setAveragingFactor(float factor) {
    settings.averagingFactor = juce::jlimit(0.0f, 1.0f, factor);
}

//==============================================================================
// Display Options
//==============================================================================

void SpectrumAnalyzer::setShowPeakHold(bool show) {
    if (show != settings.showPeakHold) {
        settings.showPeakHold = show;
        repaint();
    }
}

void SpectrumAnalyzer::setShowGrid(bool show) {
    if (show != settings.showGrid) {
        settings.showGrid = show;
        repaint();
    }
}

void SpectrumAnalyzer::setShowLabels(bool show) {
    if (show != settings.showLabels) {
        settings.showLabels = show;
        repaint();
    }
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
    settings.peakHoldColor = colorScheme.getColor(ColorScheme::ColorRole::Error);
    settings.gridColor = colorScheme.getColor(ColorScheme::ColorRole::GridLine);
    
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
    
    // Convert frequency to bin index
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
    
    for (size_t i = 0; i < magnitudeData.size(); ++i) {
        FrequencyBin bin;
        bin.frequency = static_cast<float>(i) * binWidth;
        bin.magnitude = magnitudeData[i];
        bin.phase = (i < phaseData.size()) ? phaseData[i] : 0.0f;
        bin.peakHold = (i < peakHoldData.size()) ? peakHoldData[i] : settings.minDecibels;
        bin.peakTime = (i < peakHoldTimes.size()) ? peakHoldTimes[i] : juce::Time();
        
        bins.add(bin);
    }
    
    return bins;
}

//==============================================================================
// Peak Detection
//==============================================================================

juce::Array<SpectrumAnalyzer::Peak> SpectrumAnalyzer::detectPeaks(float threshold, int minPeakDistance) const {
    juce::Array<Peak> peaks;
    
    if (magnitudeData.empty()) {
        return peaks;
    }
    
    float binWidth = static_cast<float>(sampleRate) / static_cast<float>(settings.fftSize);
    
    for (size_t i = minPeakDistance; i < magnitudeData.size() - minPeakDistance; ++i) {
        float magnitude = magnitudeData[i];
        
        if (magnitude < threshold) {
            continue;
        }
        
        // Check if this is a local maximum
        bool isPeak = true;
        for (int j = -minPeakDistance; j <= minPeakDistance; ++j) {
            if (j == 0) continue;
            
            size_t checkIndex = i + j;
            if (checkIndex < magnitudeData.size() && magnitudeData[checkIndex] >= magnitude) {
                isPeak = false;
                break;
            }
        }
        
        if (isPeak) {
            float frequency = static_cast<float>(i) * binWidth;
            
            // Calculate bandwidth (simple estimation)
            float bandwidth = binWidth;
            
            peaks.add(Peak(frequency, magnitude, bandwidth));
        }
    }
    
    // Sort peaks by magnitude (highest first)
    std::sort(peaks.begin(), peaks.end(), [](const Peak& a, const Peak& b) {
        return a.magnitude > b.magnitude;
    });
    
    return peaks;
}

SpectrumAnalyzer::Peak SpectrumAnalyzer::getFundamentalFrequency() const {
    auto peaks = detectPeaks(-40.0f, 3);
    
    if (!peaks.isEmpty()) {
        // Return the strongest peak as fundamental
        return peaks.getFirst();
    }
    
    return Peak(0.0f, settings.minDecibels);
}

//==============================================================================
// State Management
//==============================================================================

void SpectrumAnalyzer::saveState(ComponentState& state) const {
    state.setValue("fftSize", settings.fftSize);
    state.setValue("overlapFactor", settings.overlapFactor);
    state.setValue("displayMode", static_cast<int>(settings.displayMode));
    state.setValue("windowType", static_cast<int>(settings.windowType));
    state.setValue("averagingMode", static_cast<int>(settings.averagingMode));
    state.setValue("minFrequency", settings.minFrequency);
    state.setValue("maxFrequency", settings.maxFrequency);
    state.setValue("minDecibels", settings.minDecibels);
    state.setValue("maxDecibels", settings.maxDecibels);
    state.setValue("averagingFactor", settings.averagingFactor);
    state.setValue("peakHoldTime", settings.peakHoldTime);
    state.setValue("peakDecayRate", settings.peakDecayRate);
    state.setValue("showPeakHold", settings.showPeakHold);
    state.setValue("showGrid", settings.showGrid);
    state.setValue("showLabels", settings.showLabels);
    state.setValue("showCursor", settings.showCursor);
}

void SpectrumAnalyzer::loadState(const ComponentState& state) {
    AnalyzerSettings newSettings;
    
    newSettings.fftSize = state.getValue("fftSize", 2048);
    newSettings.overlapFactor = state.getValue("overlapFactor", 4);
    newSettings.displayMode = static_cast<DisplayMode>(state.getIntValue("displayMode", static_cast<int>(DisplayMode::Logarithmic)));
    newSettings.windowType = static_cast<WindowType>(state.getIntValue("windowType", static_cast<int>(WindowType::Hanning)));
    newSettings.averagingMode = static_cast<AveragingMode>(state.getIntValue("averagingMode", static_cast<int>(AveragingMode::Exponential)));
    newSettings.minFrequency = state.getValue("minFrequency", 20.0f);
    newSettings.maxFrequency = state.getValue("maxFrequency", 20000.0f);
    newSettings.minDecibels = state.getValue("minDecibels", -80.0f);
    newSettings.maxDecibels = state.getValue("maxDecibels", 0.0f);
    newSettings.averagingFactor = state.getValue("averagingFactor", 0.8f);
    newSettings.peakHoldTime = state.getValue("peakHoldTime", 2.0f);
    newSettings.peakDecayRate = state.getValue("peakDecayRate", 12.0f);
    newSettings.showPeakHold = state.getValue("showPeakHold", true);
    newSettings.showGrid = state.getValue("showGrid", true);
    newSettings.showLabels = state.getValue("showLabels", true);
    newSettings.showCursor = state.getValue("showCursor", true);
    
    setAnalyzerSettings(newSettings);
}

//==============================================================================
// Timer Callback
//==============================================================================

void SpectrumAnalyzer::timerCallback() {
    // Update peak hold decay
    if (settings.averagingMode == AveragingMode::PeakHold) {
        updatePeakHold();
    }
    
    // Update display paths
    {
        juce::ScopedLock lock(displayLock);
        updateSpectrumPath();
        if (settings.showPeakHold) {
            updatePeakHoldPath();
        }
    }
    
    repaint();
}

//==============================================================================
// AudioProcessorValueTreeState::Listener
//==============================================================================

void SpectrumAnalyzer::parameterChanged(const juce::String& parameterID, float newValue) {
    // Handle parameter changes from host
    if (parameterID == "fftSize") {
        setFFTSize(static_cast<int>(newValue));
    } else if (parameterID == "averagingFactor") {
        setAveragingFactor(newValue);
    }
    // Add more parameter mappings as needed
}

//==============================================================================
// Private Implementation
//==============================================================================

void SpectrumAnalyzer::initializeFFT() {
    fft = std::make_unique<juce::dsp::FFT>(static_cast<int>(std::log2(settings.fftSize)));
    
    // Resize FFT data arrays
    fftData.resize(settings.fftSize * 2, 0.0f);
    magnitudeData.resize(settings.fftSize / 2, settings.minDecibels);
    phaseData.resize(settings.fftSize / 2, 0.0f);
    averagedMagnitudes.resize(settings.fftSize / 2, settings.minDecibels);
    peakHoldData.resize(settings.fftSize / 2, settings.minDecibels);
    peakHoldTimes.resize(settings.fftSize / 2, juce::Time::getCurrentTime());
}

void SpectrumAnalyzer::initializeBuffers() {
    // Initialize circular buffer
    int bufferSize = settings.fftSize * 4; // 4x FFT size for overlap
    circularBuffer.setSize(1, bufferSize);
    circularBuffer.clear();
    circularBufferWritePos = 0;
    
    // Initialize FFT buffer
    fftBuffer.setSize(1, settings.fftSize);
    fftBuffer.clear();
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
    juce::ScopedLock lock(displayLock);
    
    // Copy data from circular buffer to FFT buffer
    int readPos = (circularBufferWritePos - settings.fftSize + circularBuffer.getNumSamples()) % circularBuffer.getNumSamples();
    
    for (int i = 0; i < settings.fftSize; ++i) {
        int bufferIndex = (readPos + i) % circularBuffer.getNumSamples();
        fftBuffer.setSample(0, i, circularBuffer.getSample(0, bufferIndex));
    }
    
    // Apply windowing
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
}

void SpectrumAnalyzer::applyWindow() {
    window->multiplyWithWindowingTable(fftBuffer.getWritePointer(0), settings.fftSize);
}

void SpectrumAnalyzer::calculateMagnitudes() {
    for (size_t i = 0; i < magnitudeData.size(); ++i) {
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
        if (i < phaseData.size()) {
            phaseData[i] = std::atan2(imag, real);
        }
        
        // Clamp to display range
        magnitudeData[i] = juce::jlimit(settings.minDecibels, settings.maxDecibels, magnitudeData[i]);
    }
}

void SpectrumAnalyzer::updateAveraging() {
    switch (settings.averagingMode) {
        case AveragingMode::None:
            // No averaging, use raw data
            break;
            
        case AveragingMode::Exponential:
            // Exponential moving average
            for (size_t i = 0; i < averagedMagnitudes.size() && i < magnitudeData.size(); ++i) {
                averagedMagnitudes[i] = settings.averagingFactor * averagedMagnitudes[i] + 
                                       (1.0f - settings.averagingFactor) * magnitudeData[i];
                magnitudeData[i] = averagedMagnitudes[i];
            }
            break;
            
        case AveragingMode::Linear:
            // Simple linear averaging (not implemented in detail here)
            break;
            
        case AveragingMode::PeakHold:
            // Peak hold with decay
            for (size_t i = 0; i < peakHoldData.size() && i < magnitudeData.size(); ++i) {
                if (magnitudeData[i] > peakHoldData[i]) {
                    peakHoldData[i] = magnitudeData[i];
                    peakHoldTimes[i] = juce::Time::getCurrentTime();
                }
            }
            break;
    }
}

void SpectrumAnalyzer::updatePeakHold() {
    auto currentTime = juce::Time::getCurrentTime();
    
    for (size_t i = 0; i < peakHoldData.size(); ++i) {
        double timeSincePeak = (currentTime - peakHoldTimes[i]).inSeconds();
        
        if (timeSincePeak > settings.peakHoldTime) {
            // Apply decay
            float decay = settings.peakDecayRate * static_cast<float>(timeSincePeak - settings.peakHoldTime);
            peakHoldData[i] = juce::jmax(peakHoldData[i] - decay, settings.minDecibels);
        }
    }
}

//==============================================================================
// Display Calculations
//==============================================================================

void SpectrumAnalyzer::updateSpectrumPath() {
    spectrumPath.clear();
    
    if (magnitudeData.empty() || getWidth() <= 0 || getHeight() <= 0) {
        return;
    }
    
    float binWidth = static_cast<float>(sampleRate) / static_cast<float>(settings.fftSize);
    
    for (size_t i = 1; i < magnitudeData.size(); ++i) {
        float frequency = static_cast<float>(i) * binWidth;
        
        if (frequency >= settings.minFrequency && frequency <= settings.maxFrequency) {
            float x = frequencyToX(frequency);
            float y = magnitudeToY(magnitudeData[i]);
            
            spectrumPath.emplace_back(x, y);
        }
    }
}

void SpectrumAnalyzer::updatePeakHoldPath() {
    peakHoldPath.clear();
    
    if (peakHoldData.empty() || getWidth() <= 0 || getHeight() <= 0) {
        return;
    }
    
    float binWidth = static_cast<float>(sampleRate) / static_cast<float>(settings.fftSize);
    
    for (size_t i = 1; i < peakHoldData.size(); ++i) {
        float frequency = static_cast<float>(i) * binWidth;
        
        if (frequency >= settings.minFrequency && frequency <= settings.maxFrequency) {
            float x = frequencyToX(frequency);
            float y = magnitudeToY(peakHoldData[i]);
            
            peakHoldPath.emplace_back(x, y);
        }
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
        gridPath.startNewSubPath(x, 0);
        gridPath.lineTo(x, static_cast<float>(getHeight()));
    }
    
    // Amplitude grid lines
    auto ampLines = calculateAmplitudeGridLines();
    for (float amp : ampLines) {
        float y = magnitudeToY(amp);
        gridPath.startNewSubPath(0, y);
        gridPath.lineTo(static_cast<float>(getWidth()), y);
    }
}

//==============================================================================
// Coordinate Transformations
//==============================================================================

float SpectrumAnalyzer::frequencyToX(float frequency) const {
    if (getWidth() <= 0) {
        return 0.0f;
    }
    
    switch (settings.displayMode) {
        case DisplayMode::Linear:
            {
                float ratio = (frequency - settings.minFrequency) / (settings.maxFrequency - settings.minFrequency);
                return ratio * static_cast<float>(getWidth());
            }
            
        case DisplayMode::Logarithmic:
            {
                float logMin = std::log10(settings.minFrequency);
                float logMax = std::log10(settings.maxFrequency);
                float logFreq = std::log10(frequency);
                float ratio = (logFreq - logMin) / (logMax - logMin);
                return ratio * static_cast<float>(getWidth());
            }
            
        case DisplayMode::MelScale:
            {
                // Mel scale conversion (simplified)
                auto freqToMel = [](float f) { return 2595.0f * std::log10(1.0f + f / 700.0f); };
                float melMin = freqToMel(settings.minFrequency);
                float melMax = freqToMel(settings.maxFrequency);
                float melFreq = freqToMel(frequency);
                float ratio = (melFreq - melMin) / (melMax - melMin);
                return ratio * static_cast<float>(getWidth());
            }
            
        default:
            return frequencyToX(frequency); // Default to logarithmic
    }
}

float SpectrumAnalyzer::xToFrequency(float x) const {
    if (getWidth() <= 0) {
        return settings.minFrequency;
    }
    
    float ratio = x / static_cast<float>(getWidth());
    ratio = juce::jlimit(0.0f, 1.0f, ratio);
    
    switch (settings.displayMode) {
        case DisplayMode::Linear:
            return settings.minFrequency + ratio * (settings.maxFrequency - settings.minFrequency);
            
        case DisplayMode::Logarithmic:
            {
                float logMin = std::log10(settings.minFrequency);
                float logMax = std::log10(settings.maxFrequency);
                float logFreq = logMin + ratio * (logMax - logMin);
                return std::pow(10.0f, logFreq);
            }
            
        case DisplayMode::MelScale:
            {
                // Inverse mel scale conversion
                auto melToFreq = [](float mel) { return 700.0f * (std::pow(10.0f, mel / 2595.0f) - 1.0f); };
                auto freqToMel = [](float f) { return 2595.0f * std::log10(1.0f + f / 700.0f); };
                
                float melMin = freqToMel(settings.minFrequency);
                float melMax = freqToMel(settings.maxFrequency);
                float mel = melMin + ratio * (melMax - melMin);
                return melToFreq(mel);
            }
            
        default:
            return xToFrequency(x); // Default to logarithmic
    }
}

float SpectrumAnalyzer::magnitudeToY(float magnitude) const {
    if (getHeight() <= 0) {
        return 0.0f;
    }
    
    float ratio = (magnitude - settings.minDecibels) / (settings.maxDecibels - settings.minDecibels);
    ratio = juce::jlimit(0.0f, 1.0f, ratio);
    
    // Invert Y coordinate (0 at top, height at bottom)
    return static_cast<float>(getHeight()) * (1.0f - ratio);
}

float SpectrumAnalyzer::yToMagnitude(float y) const {
    if (getHeight() <= 0) {
        return settings.minDecibels;
    }
    
    float ratio = 1.0f - (y / static_cast<float>(getHeight()));
    ratio = juce::jlimit(0.0f, 1.0f, ratio);
    
    return settings.minDecibels + ratio * (settings.maxDecibels - settings.minDecibels);
}

//==============================================================================
// Drawing Helpers
//==============================================================================

void SpectrumAnalyzer::drawSpectrum(juce::Graphics& g) {
    if (spectrumPath.empty()) {
        return;
    }
    
    g.setColour(settings.spectrumColor);
    
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
    
    // Fill spectrum
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
    
    // Draw peak hold lines
    for (size_t i = 0; i < peakHoldPath.size(); ++i) {
        const auto& point = peakHoldPath[i];
        g.drawLine(point.x, point.y, point.x, point.y + 2.0f, 1.0f);
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
        juce::String label = formatFrequency(freq);
        
        g.drawText(label, static_cast<int>(x - 20), getHeight() - 15, 40, 12, 
                  juce::Justification::centred);
    }
    
    // Draw amplitude labels
    auto ampLines = calculateAmplitudeGridLines();
    for (float amp : ampLines) {
        float y = magnitudeToY(amp);
        juce::String label = formatAmplitude(amp);
        
        g.drawText(label, 2, static_cast<int>(y - 6), 40, 12, 
                  juce::Justification::left);
    }
}

void SpectrumAnalyzer::drawFrequencyCursor(juce::Graphics& g) {
    if (!showFrequencyCursor) {
        return;
    }
    
    float x = frequencyToX(cursorFrequency);
    
    g.setColour(juce::Colours::yellow.withAlpha(0.8f));
    g.drawLine(x, 0, x, static_cast<float>(getHeight()), 1.0f);
    
    // Draw frequency value
    juce::String freqText = formatFrequency(cursorFrequency);
    float magnitude = getMagnitudeAtFrequency(cursorFrequency);
    juce::String magText = formatAmplitude(magnitude);
    
    juce::String label = freqText + " / " + magText;
    
    g.setColour(juce::Colours::yellow);
    g.setFont(JUCE8_FONT(12.0f));
    g.drawText(label, static_cast<int>(x + 5), 5, 100, 15, juce::Justification::left);
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
                for (int i = 1; i < 10; ++i) {
                    lines.push_back(settings.minFrequency + i * step);
                }
            }
            break;
            
        case DisplayMode::Logarithmic:
            {
                // Octave-based grid lines
                std::vector<float> frequencies = {20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000};
                for (float freq : frequencies) {
                    if (freq >= settings.minFrequency && freq <= settings.maxFrequency) {
                        lines.push_back(freq);
                    }
                }
            }
            break;
            
        default:
            break;
    }
    
    return lines;
}

std::vector<float> SpectrumAnalyzer::calculateAmplitudeGridLines() const {
    std::vector<float> lines;
    
    float step = (settings.maxDecibels - settings.minDecibels) / 8.0f;
    for (int i = 1; i < 8; ++i) {
        lines.push_back(settings.minDecibels + i * step);
    }
    
    return lines;
}

juce::String SpectrumAnalyzer::formatFrequency(float frequency) const {
    if (frequency < 1000.0f) {
        return juce::String(static_cast<int>(frequency)) + " Hz";
    } else {
        return juce::String(frequency / 1000.0f, 1) + " kHz";
    }
}

juce::String SpectrumAnalyzer::formatAmplitude(float amplitude) const {
    return juce::String(amplitude, 1) + " dB";
}

//==============================================================================
// Color Management
//==============================================================================

void SpectrumAnalyzer::updateColors() {
    updateColorsFromTheme();
}

juce::Colour SpectrumAnalyzer::getSpectrumColorAtFrequency(float frequency) const {
    // Create frequency-based color gradient
    float ratio = (frequency - settings.minFrequency) / (settings.maxFrequency - settings.minFrequency);
    ratio = juce::jlimit(0.0f, 1.0f, ratio);
    
    // Interpolate between blue (low) and red (high)
    juce::Colour lowColor = juce::Colours::blue;
    juce::Colour highColor = juce::Colours::red;
    
    return lowColor.interpolatedWith(highColor, ratio);
}

//==============================================================================
// Performance Optimization
//==============================================================================

void SpectrumAnalyzer::optimizeForPerformance() {
    // Reduce FFT size if performance is poor
    if (performanceStats.averageProcessingTime > 10.0) {
        if (settings.fftSize > 512) {
            setFFTSize(settings.fftSize / 2);
        }
    }
    
    // Reduce overlap factor
    if (performanceStats.averageProcessingTime > 5.0) {
        settings.overlapFactor = juce::jmax(1, settings.overlapFactor / 2);
    }
}

bool SpectrumAnalyzer::shouldSkipFrame() const {
    // Skip frame if processing is taking too long
    return performanceStats.averageProcessingTime > 16.0; // 60 FPS target
}

void SpectrumAnalyzer::updatePerformanceStats(double processingTime) {
    processingTimes.push_back(processingTime);
    
    // Keep only recent measurements
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
    if (processingTime > performanceStats.maxProcessingTime) {
        performanceStats.maxProcessingTime = processingTime;
    }
    
    // Estimate CPU usage (simplified)
    performanceStats.cpuUsage = (performanceStats.averageProcessingTime / 16.0) * 100.0; // Assuming 60 FPS target
}

//==============================================================================
// Validation Helpers
//==============================================================================

bool SpectrumAnalyzer::isValidFFTSize(int size) const {
    return size >= 256 && size <= 8192 && (size & (size - 1)) == 0; // Power of 2
}

bool SpectrumAnalyzer::isValidFrequencyRange(float minFreq, float maxFreq) const {
    return minFreq > 0 && maxFreq > minFreq && maxFreq <= static_cast<float>(sampleRate * 0.5);
}

bool SpectrumAnalyzer::isValidAmplitudeRange(float minDb, float maxDb) const {
    return minDb < maxDb && minDb >= -120.0f && maxDb <= 20.0f;
}