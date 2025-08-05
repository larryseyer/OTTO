#include "WaveformDisplay.h"
#include "JUCE8_CODING_STANDARDS.h"
#include <algorithm>
#include <cmath>

//==============================================================================
// WaveformDisplay Implementation
//==============================================================================

WaveformDisplay::WaveformDisplay(ColorScheme& colorScheme)
    : colorScheme(colorScheme)
{
    // Initialize waveform caches
    initializeWaveformCaches();
    
    // Load settings from INI
    ComponentState state;
    if (INIDataManager::loadComponentState("WaveformDisplay", state)) {
        loadState(state);
    }
    
    // Update colors from theme
    updateColorsFromTheme();
    
    // Enable mouse tracking
    setMouseClickGrabsKeyboardFocus(false);
    
    // Start timer for animations (60 FPS)
    startTimer(16);
}

WaveformDisplay::~WaveformDisplay() {
    // Save current state
    ComponentState state;
    saveState(state);
    INIDataManager::saveComponentState("WaveformDisplay", state);
    
    stopTimer();
}

//==============================================================================
// Component Overrides
//==============================================================================

void WaveformDisplay::paint(juce::Graphics& g) {
    // Clear background
    g.fillAll(settings.backgroundColor);
    
    // Early return if no audio data
    if (!hasAudioData()) {
        g.setColour(colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
        g.setFont(JUCE8_FONT(14.0f));
        g.drawText("No audio loaded", getLocalBounds(), juce::Justification::centred);
        return;
    }
    
    // Enable antialiasing for smooth rendering
    g.setImageResamplingQuality(juce::Graphics::highResamplingQuality);
    
    // Update cache if needed
    updateCacheIfNeeded();
    
    // Draw components in order
    if (settings.showGrid) {
        drawGrid(g);
    }
    
    drawWaveform(g);
    
    if (settings.showSelection && selection.active) {
        drawSelection(g);
    }
    
    if (settings.showLoopRegion && loopRegion.enabled) {
        drawLoopRegion(g);
    }
    
    if (settings.showPlayhead && playheadVisible) {
        drawPlayhead(g);
    }
    
    if (settings.showZeroCrossings) {
        drawZeroCrossings(g);
    }
    
    if (settings.showPeakMarkers) {
        drawPeakMarkers(g);
    }
    
    if (settings.showRuler) {
        drawRuler(g);
    }
}

void WaveformDisplay::resized() {
    // Invalidate cache when size changes
    invalidateCache();
    
    // Update pixels per second calculation
    updatePixelsPerSecond();
    
    // Constrain view range to new bounds
    constrainViewRange();
}

void WaveformDisplay::mouseDown(const juce::MouseEvent& e) {
    mouseDownPosition = e.getPosition();
    mouseDownTime = pixelToTime(e.x);
    
    // Determine mouse mode based on position
    currentMouseMode = getMouseModeForPosition(e.getPosition());
    
    // Store original states for dragging
    originalSelection = selection;
    originalLoop = loopRegion;
    originalViewStart = settings.viewStart;
    originalViewEnd = settings.viewEnd;
    
    switch (currentMouseMode) {
        case MouseMode::Selecting:
            // Start new selection
            selection.startTime = mouseDownTime;
            selection.endTime = mouseDownTime;
            selection.active = true;
            break;
            
        case MouseMode::DraggingPlayhead:
            // Move playhead to click position
            setPlayheadPosition(mouseDownTime);
            break;
            
        case MouseMode::Panning:
            // Prepare for panning
            break;
            
        default:
            break;
    }
    
    repaint();
}

void WaveformDisplay::mouseDrag(const juce::MouseEvent& e) {
    switch (currentMouseMode) {
        case MouseMode::Selecting:
            handleSelectionDrag(e);
            break;
            
        case MouseMode::DraggingSelection:
            handleSelectionDrag(e);
            break;
            
        case MouseMode::DraggingPlayhead:
            handlePlayheadDrag(e);
            break;
            
        case MouseMode::DraggingLoopStart:
        case MouseMode::DraggingLoopEnd:
            handleLoopDrag(e);
            break;
            
        case MouseMode::Panning:
            handlePanning(e);
            break;
            
        default:
            break;
    }
    
    repaint();
}

void WaveformDisplay::mouseUp(const juce::MouseEvent& e) {
    // Finalize operations based on mouse mode
    switch (currentMouseMode) {
        case MouseMode::Selecting:
            // Finalize selection
            if (std::abs(selection.endTime - selection.startTime) < 0.001) {
                // Very small selection, clear it
                clearSelection();
            } else {
                notifySelectionChanged();
            }
            break;
            
        case MouseMode::DraggingSelection:
            notifySelectionChanged();
            break;
            
        case MouseMode::DraggingPlayhead:
            notifyPlayheadMoved();
            break;
            
        case MouseMode::DraggingLoopStart:
        case MouseMode::DraggingLoopEnd:
            notifyLoopRegionChanged();
            break;
            
        case MouseMode::Panning:
            notifyViewRangeChanged();
            break;
            
        default:
            break;
    }
    
    currentMouseMode = MouseMode::None;
    repaint();
}

void WaveformDisplay::mouseMove(const juce::MouseEvent& e) {
    // Update cursor based on position
    auto mode = getMouseModeForPosition(e.getPosition());
    
    switch (mode) {
        case MouseMode::DraggingPlayhead:
            setMouseCursor(juce::MouseCursor::LeftRightResizeCursor);
            break;
            
        case MouseMode::DraggingSelection:
            setMouseCursor(juce::MouseCursor::DraggingHandCursor);
            break;
            
        case MouseMode::DraggingLoopStart:
        case MouseMode::DraggingLoopEnd:
            setMouseCursor(juce::MouseCursor::LeftRightResizeCursor);
            break;
            
        case MouseMode::Panning:
            setMouseCursor(juce::MouseCursor::DraggingHandCursor);
            break;
            
        default:
            setMouseCursor(juce::MouseCursor::NormalCursor);
            break;
    }
}

void WaveformDisplay::mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel) {
    handleZooming(e, wheel);
}

void WaveformDisplay::mouseDoubleClick(const juce::MouseEvent& e) {
    if (e.mods.isShiftDown()) {
        // Shift+double-click: zoom to fit
        zoomToFit();
    } else if (hasSelection()) {
        // Double-click with selection: zoom to selection
        zoomToSelection();
    } else {
        // Double-click: select all
        if (hasAudioData()) {
            setSelection(0.0, getAudioLength());
        }
    }
}

//==============================================================================
// Audio Data Management
//==============================================================================

void WaveformDisplay::setAudioBuffer(const juce::AudioBuffer<float>& buffer, double newSampleRate) {
    juce::ScopedLock lock(audioDataLock);
    
    audioBuffer = buffer;
    sampleRate = newSampleRate;
    
    // Reset view to show entire waveform
    settings.viewStart = 0.0;
    settings.viewEnd = getAudioLength();
    
    // Clear existing state
    clearSelection();
    clearLoopRegion();
    setPlayheadPosition(0.0);
    
    // Invalidate cache
    invalidateCache();
    
    repaint();
}

void WaveformDisplay::setAudioFile(const juce::File& file) {
    if (!file.exists()) {
        clearAudioData();
        return;
    }
    
    // Load audio file
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();
    
    auto reader = std::unique_ptr<juce::AudioFormatReader>(formatManager.createReaderFor(file));
    if (!reader) {
        clearAudioData();
        return;
    }
    
    // Create buffer and read audio data
    juce::AudioBuffer<float> buffer(static_cast<int>(reader->numChannels), 
                                   static_cast<int>(reader->lengthInSamples));
    reader->read(&buffer, 0, static_cast<int>(reader->lengthInSamples), 0, true, true);
    
    setAudioBuffer(buffer, reader->sampleRate);
}

void WaveformDisplay::clearAudioData() {
    juce::ScopedLock lock(audioDataLock);
    
    audioBuffer.clear();
    sampleRate = 44100.0;
    
    clearSelection();
    clearLoopRegion();
    setPlayheadPosition(0.0);
    
    invalidateCache();
    repaint();
}

double WaveformDisplay::getAudioLength() const {
    if (!hasAudioData()) {
        return 0.0;
    }
    
    return audioBuffer.getNumSamples() / sampleRate;
}

//==============================================================================
// Playback Position
//==============================================================================

void WaveformDisplay::setPlayheadPosition(double timeInSeconds) {
    double newPosition = juce::jlimit(0.0, getAudioLength(), timeInSeconds);
    
    if (std::abs(newPosition - playheadPosition) > 0.001) {
        playheadPosition = newPosition;
        repaint();
    }
}

void WaveformDisplay::setPlayheadVisible(bool visible) {
    if (visible != playheadVisible) {
        playheadVisible = visible;
        repaint();
    }
}

//==============================================================================
// View Control
//==============================================================================

void WaveformDisplay::setViewRange(double startTime, double endTime) {
    if (!isValidTimeRange(startTime, endTime)) {
        return;
    }
    
    settings.viewStart = startTime;
    settings.viewEnd = endTime;
    
    constrainViewRange();
    updatePixelsPerSecond();
    invalidateCache();
    
    notifyViewRangeChanged();
    repaint();
}

void WaveformDisplay::getViewRange(double& startTime, double& endTime) const {
    startTime = settings.viewStart;
    endTime = settings.viewEnd;
}

void WaveformDisplay::zoomToFit() {
    if (!hasAudioData()) {
        return;
    }
    
    double audioLength = getAudioLength();
    startViewAnimation(0.0, audioLength);
}

void WaveformDisplay::zoomToSelection() {
    if (!hasSelection()) {
        return;
    }
    
    // Add small padding around selection
    double padding = selection.getLength() * 0.1;
    double start = juce::jmax(0.0, selection.startTime - padding);
    double end = juce::jmin(getAudioLength(), selection.endTime + padding);
    
    startViewAnimation(start, end);
}

void WaveformDisplay::zoomIn(double centerTime) {
    if (centerTime < 0) {
        centerTime = (settings.viewStart + settings.viewEnd) * 0.5;
    }
    
    double currentRange = settings.viewEnd - settings.viewStart;
    double newRange = currentRange * 0.5; // Zoom in by 2x
    
    double newStart = centerTime - newRange * 0.5;
    double newEnd = centerTime + newRange * 0.5;
    
    startViewAnimation(newStart, newEnd);
}

void WaveformDisplay::zoomOut(double centerTime) {
    if (centerTime < 0) {
        centerTime = (settings.viewStart + settings.viewEnd) * 0.5;
    }
    
    double currentRange = settings.viewEnd - settings.viewStart;
    double newRange = currentRange * 2.0; // Zoom out by 2x
    
    double newStart = centerTime - newRange * 0.5;
    double newEnd = centerTime + newRange * 0.5;
    
    startViewAnimation(newStart, newEnd);
}

void WaveformDisplay::setZoomLevel(double zoomLevel) {
    if (!isValidZoomLevel(zoomLevel)) {
        return;
    }
    
    settings.horizontalZoom = zoomLevel;
    updatePixelsPerSecond();
    invalidateCache();
    
    notifyZoomChanged();
    repaint();
}

void WaveformDisplay::setVerticalZoom(float zoom) {
    zoom = juce::jlimit(0.1f, 10.0f, zoom);
    
    if (std::abs(zoom - settings.verticalZoom) > 0.01f) {
        settings.verticalZoom = zoom;
        repaint();
    }
}

//==============================================================================
// Selection Management
//==============================================================================

void WaveformDisplay::setSelection(double startTime, double endTime) {
    if (!isValidTimeRange(startTime, endTime)) {
        return;
    }
    
    selection.startTime = startTime;
    selection.endTime = endTime;
    selection.active = true;
    
    notifySelectionChanged();
    repaint();
}

void WaveformDisplay::setSelection(const SelectionRegion& newSelection) {
    selection = newSelection;
    
    if (selection.active) {
        notifySelectionChanged();
    }
    
    repaint();
}

void WaveformDisplay::clearSelection() {
    if (selection.active) {
        selection.clear();
        notifySelectionChanged();
        repaint();
    }
}

//==============================================================================
// Loop Region Management
//==============================================================================

void WaveformDisplay::setLoopRegion(double startTime, double endTime) {
    if (!isValidTimeRange(startTime, endTime)) {
        return;
    }
    
    loopRegion.startTime = startTime;
    loopRegion.endTime = endTime;
    loopRegion.enabled = true;
    
    notifyLoopRegionChanged();
    repaint();
}

void WaveformDisplay::setLoopRegion(const LoopRegion& newLoop) {
    loopRegion = newLoop;
    
    if (loopRegion.enabled) {
        notifyLoopRegionChanged();
    }
    
    repaint();
}

void WaveformDisplay::clearLoopRegion() {
    if (loopRegion.enabled) {
        loopRegion.clear();
        notifyLoopRegionChanged();
        repaint();
    }
}

//==============================================================================
// Display Settings
//==============================================================================

void WaveformDisplay::setWaveformSettings(const WaveformSettings& newSettings) {
    settings = newSettings;
    
    constrainViewRange();
    updatePixelsPerSecond();
    invalidateCache();
    
    repaint();
}

void WaveformDisplay::setDisplayMode(DisplayMode mode) {
    if (mode != settings.displayMode) {
        settings.displayMode = mode;
        invalidateCache();
        repaint();
    }
}

void WaveformDisplay::setShowRuler(bool show) {
    if (show != settings.showRuler) {
        settings.showRuler = show;
        repaint();
    }
}

void WaveformDisplay::setShowGrid(bool show) {
    if (show != settings.showGrid) {
        settings.showGrid = show;
        repaint();
    }
}

void WaveformDisplay::setShowZeroCrossings(bool show) {
    if (show != settings.showZeroCrossings) {
        settings.showZeroCrossings = show;
        repaint();
    }
}

//==============================================================================
// Color Customization
//==============================================================================

void WaveformDisplay::setWaveformColor(const juce::Colour& color) {
    settings.waveformColor = color;
    repaint();
}

void WaveformDisplay::setSelectionColor(const juce::Colour& color) {
    settings.selectionColor = color;
    repaint();
}

void WaveformDisplay::setPlayheadColor(const juce::Colour& color) {
    settings.playheadColor = color;
    repaint();
}

void WaveformDisplay::updateColorsFromTheme() {
    settings.backgroundColor = colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground);
    settings.waveformColor = colorScheme.getColor(ColorScheme::ColorRole::Accent);
    settings.gridColor = colorScheme.getColor(ColorScheme::ColorRole::GridLine);
    settings.selectionColor = colorScheme.getColor(ColorScheme::ColorRole::PatternActive).withAlpha(0.3f);
    settings.playheadColor = colorScheme.getColor(ColorScheme::ColorRole::Error);
    settings.loopColor = colorScheme.getColor(ColorScheme::ColorRole::Success).withAlpha(0.3f);
    
    repaint();
}

//==============================================================================
// Coordinate Transformations
//==============================================================================

double WaveformDisplay::pixelToTime(int pixel) const {
    if (getWidth() <= 0) {
        return settings.viewStart;
    }
    
    double ratio = static_cast<double>(pixel) / getWidth();
    return settings.viewStart + ratio * (settings.viewEnd - settings.viewStart);
}

int WaveformDisplay::timeToPixel(double time) const {
    if (settings.viewEnd <= settings.viewStart) {
        return 0;
    }
    
    double ratio = (time - settings.viewStart) / (settings.viewEnd - settings.viewStart);
    return static_cast<int>(ratio * getWidth());
}

float WaveformDisplay::sampleToY(float sample, int channel) const {
    // Apply vertical zoom
    sample *= settings.verticalZoom;
    
    // Clamp to [-1, 1] range
    sample = juce::jlimit(-1.0f, 1.0f, sample);
    
    int channelHeight = getHeight();
    if (settings.displayMode == DisplayMode::Stereo && audioBuffer.getNumChannels() > 1) {
        channelHeight = getHeight() / audioBuffer.getNumChannels();
    }
    
    float centerY = channelHeight * 0.5f;
    if (settings.displayMode == DisplayMode::Stereo) {
        centerY += channel * channelHeight;
    }
    
    return centerY - sample * channelHeight * 0.4f; // 0.4f leaves some margin
}

float WaveformDisplay::yToSample(float y, int channel) const {
    int channelHeight = getHeight();
    if (settings.displayMode == DisplayMode::Stereo && audioBuffer.getNumChannels() > 1) {
        channelHeight = getHeight() / audioBuffer.getNumChannels();
    }
    
    float centerY = channelHeight * 0.5f;
    if (settings.displayMode == DisplayMode::Stereo) {
        centerY += channel * channelHeight;
    }
    
    float sample = (centerY - y) / (channelHeight * 0.4f);
    return sample / settings.verticalZoom;
}

//==============================================================================
// Audio Analysis
//==============================================================================

WaveformDisplay::WaveformStats WaveformDisplay::analyzeWaveform() const {
    WaveformStats stats;
    
    if (!hasAudioData()) {
        return stats;
    }
    
    juce::ScopedLock lock(audioDataLock);
    
    stats.duration = getAudioLength();
    
    float sumSquares = 0.0f;
    int totalSamples = 0;
    
    for (int channel = 0; channel < audioBuffer.getNumChannels(); ++channel) {
        const float* channelData = audioBuffer.getReadPointer(channel);
        int numSamples = audioBuffer.getNumSamples();
        
        for (int sample = 0; sample < numSamples; ++sample) {
            float value = std::abs(channelData[sample]);
            
            // Update peak
            if (value > stats.peakLevel) {
                stats.peakLevel = value;
            }
            
            // Accumulate for RMS
            sumSquares += value * value;
            totalSamples++;
            
            // Count zero crossings
            if (sample > 0) {
                if ((channelData[sample - 1] >= 0) != (channelData[sample] >= 0)) {
                    stats.zeroCrossings++;
                }
            }
        }
    }
    
    // Calculate RMS
    if (totalSamples > 0) {
        stats.rmsLevel = std::sqrt(sumSquares / totalSamples);
    }
    
    // Calculate dynamic range (simplified)
    stats.dynamicRange = 20.0f * std::log10(stats.peakLevel / (stats.rmsLevel + 1e-10f));
    
    return stats;
}

WaveformDisplay::WaveformStats WaveformDisplay::analyzeSelection() const {
    WaveformStats stats;
    
    if (!hasAudioData() || !hasSelection()) {
        return stats;
    }
    
    juce::ScopedLock lock(audioDataLock);
    
    int startSample = static_cast<int>(selection.startTime * sampleRate);
    int endSample = static_cast<int>(selection.endTime * sampleRate);
    
    startSample = juce::jlimit(0, audioBuffer.getNumSamples(), startSample);
    endSample = juce::jlimit(startSample, audioBuffer.getNumSamples(), endSample);
    
    stats.duration = (endSample - startSample) / sampleRate;
    
    float sumSquares = 0.0f;
    int totalSamples = 0;
    
    for (int channel = 0; channel < audioBuffer.getNumChannels(); ++channel) {
        const float* channelData = audioBuffer.getReadPointer(channel);
        
        for (int sample = startSample; sample < endSample; ++sample) {
            float value = std::abs(channelData[sample]);
            
            // Update peak
            if (value > stats.peakLevel) {
                stats.peakLevel = value;
            }
            
            // Accumulate for RMS
            sumSquares += value * value;
            totalSamples++;
            
            // Count zero crossings
            if (sample > startSample) {
                if ((channelData[sample - 1] >= 0) != (channelData[sample] >= 0)) {
                    stats.zeroCrossings++;
                }
            }
        }
    }
    
    // Calculate RMS
    if (totalSamples > 0) {
        stats.rmsLevel = std::sqrt(sumSquares / totalSamples);
    }
    
    // Calculate dynamic range
    stats.dynamicRange = 20.0f * std::log10(stats.peakLevel / (stats.rmsLevel + 1e-10f));
    
    return stats;
}

juce::Array<double> WaveformDisplay::findZeroCrossings() const {
    juce::Array<double> crossings;
    
    if (!hasAudioData()) {
        return crossings;
    }
    
    juce::ScopedLock lock(audioDataLock);
    
    // Analyze first channel only for simplicity
    const float* channelData = audioBuffer.getReadPointer(0);
    int numSamples = audioBuffer.getNumSamples();
    
    for (int sample = 1; sample < numSamples; ++sample) {
        if ((channelData[sample - 1] >= 0) != (channelData[sample] >= 0)) {
            double time = sample / sampleRate;
            crossings.add(time);
        }
    }
    
    return crossings;
}

juce::Array<double> WaveformDisplay::findPeaks(float threshold) const {
    juce::Array<double> peaks;
    
    if (!hasAudioData()) {
        return peaks;
    }
    
    juce::ScopedLock lock(audioDataLock);
    
    // Analyze first channel only for simplicity
    const float* channelData = audioBuffer.getReadPointer(0);
    int numSamples = audioBuffer.getNumSamples();
    
    for (int sample = 1; sample < numSamples - 1; ++sample) {
        float current = std::abs(channelData[sample]);
        float prev = std::abs(channelData[sample - 1]);
        float next = std::abs(channelData[sample + 1]);
        
        // Check if this is a local maximum above threshold
        if (current > threshold && current > prev && current > next) {
            double time = sample / sampleRate;
            peaks.add(time);
        }
    }
    
    return peaks;
}

//==============================================================================
// State Management
//==============================================================================

void WaveformDisplay::saveState(ComponentState& state) const {
    state.setValue("displayMode", static_cast<int>(settings.displayMode));
    state.setValue("showRuler", settings.showRuler);
    state.setValue("showGrid", settings.showGrid);
    state.setValue("showZeroCrossings", settings.showZeroCrossings);
    state.setValue("showPeakMarkers", settings.showPeakMarkers);
    state.setValue("verticalZoom", settings.verticalZoom);
    state.setValue("horizontalZoom", settings.horizontalZoom);
    state.setValue("viewStart", settings.viewStart);
    state.setValue("viewEnd", settings.viewEnd);
}

void WaveformDisplay::loadState(const ComponentState& state) {
    settings.displayMode = static_cast<DisplayMode>(state.getValue("displayMode", static_cast<int>(DisplayMode::Peak)));
    settings.showRuler = state.getValue("showRuler", true);
    settings.showGrid = state.getValue("showGrid", true);
    settings.showZeroCrossings = state.getValue("showZeroCrossings", false);
    settings.showPeakMarkers = state.getValue("showPeakMarkers", false);
    settings.verticalZoom = state.getValue("verticalZoom", 1.0f);
    settings.horizontalZoom = state.getValue("horizontalZoom", 1.0);
    settings.viewStart = state.getValue("viewStart", 0.0);
    settings.viewEnd = state.getValue("viewEnd", 10.0);
    
    // Apply loaded settings
    constrainViewRange();
    updatePixelsPerSecond();
    invalidateCache();
}

//==============================================================================
// Timer Callback
//==============================================================================

void WaveformDisplay::timerCallback() {
    if (animationState.animating) {
        updateAnimation();
    }
}

//==============================================================================
// ChangeListener
//==============================================================================

void WaveformDisplay::changeListenerCallback(juce::ChangeBroadcaster* source) {
    // Handle changes from external sources (e.g., theme changes)
    updateColorsFromTheme();
}

//==============================================================================
// Listener Management
//==============================================================================

void WaveformDisplay::addListener(Listener* listener) {
    listeners.add(listener);
}

void WaveformDisplay::removeListener(Listener* listener) {
    listeners.remove(listener);
}

//==============================================================================
// Private Implementation
//==============================================================================

void WaveformDisplay::initializeWaveformCaches() {
    // Initialize multiple cache levels for different zoom levels
    waveformCaches.resize(8); // 8 levels should be sufficient
    
    for (auto& cache : waveformCaches) {
        cache.clear();
    }
}

void WaveformDisplay::updateWaveformCache(int cacheLevel) {
    if (!hasAudioData() || cacheLevel >= static_cast<int>(waveformCaches.size())) {
        return;
    }
    
    juce::ScopedLock lock(audioDataLock);
    
    auto& cache = waveformCaches[cacheLevel];
    
    // Calculate samples per pixel for this cache level
    int samplesPerPixel = 1 << cacheLevel; // 1, 2, 4, 8, 16, 32, 64, 128
    cache.samplesPerPixel = samplesPerPixel;
    
    int numPixels = (audioBuffer.getNumSamples() + samplesPerPixel - 1) / samplesPerPixel;
    
    cache.minValues.resize(numPixels);
    cache.maxValues.resize(numPixels);
    cache.rmsValues.resize(numPixels);
    
    // Process audio data
    for (int pixel = 0; pixel < numPixels; ++pixel) {
        int startSample = pixel * samplesPerPixel;
        int endSample = juce::jmin(startSample + samplesPerPixel, audioBuffer.getNumSamples());
        
        float minVal = 1.0f;
        float maxVal = -1.0f;
        float sumSquares = 0.0f;
        int sampleCount = 0;
        
        // Process all channels (mix to mono for cache)
        for (int channel = 0; channel < audioBuffer.getNumChannels(); ++channel) {
            const float* channelData = audioBuffer.getReadPointer(channel);
            
            for (int sample = startSample; sample < endSample; ++sample) {
                float value = channelData[sample];
                
                minVal = juce::jmin(minVal, value);
                maxVal = juce::jmax(maxVal, value);
                sumSquares += value * value;
                sampleCount++;
            }
        }
        
        // Average across channels
        if (audioBuffer.getNumChannels() > 1) {
            minVal /= audioBuffer.getNumChannels();
            maxVal /= audioBuffer.getNumChannels();
            sumSquares /= audioBuffer.getNumChannels();
        }
        
        cache.minValues[pixel] = minVal;
        cache.maxValues[pixel] = maxVal;
        cache.rmsValues[pixel] = sampleCount > 0 ? std::sqrt(sumSquares / sampleCount) : 0.0f;
    }
    
    cache.valid = true;
}

void WaveformDisplay::drawWaveform(juce::Graphics& g) {
    if (!hasAudioData()) {
        return;
    }
    
    switch (settings.displayMode) {
        case DisplayMode::Peak:
            drawWaveformPeak(g, 0, getLocalBounds());
            break;
            
        case DisplayMode::RMS:
            drawWaveformRMS(g, 0, getLocalBounds());
            break;
            
        case DisplayMode::Filled:
            drawWaveformFilled(g, 0, getLocalBounds());
            break;
            
        case DisplayMode::Line:
            drawWaveformLine(g, 0, getLocalBounds());
            break;
            
        case DisplayMode::Stereo:
            drawWaveformStereo(g);
            break;
            
        case DisplayMode::MidSide:
            drawWaveformMidSide(g);
            break;
    }
}

void WaveformDisplay::drawWaveformPeak(juce::Graphics& g, int channel, const juce::Rectangle<int>& area) {
    g.setColour(getWaveformColorForChannel(channel));
    
    int cacheLevel = getBestCacheLevel();
    if (cacheLevel >= 0 && cacheLevel < static_cast<int>(waveformCaches.size()) && 
        waveformCaches[cacheLevel].valid) {
        
        // Use cached data
        const auto& cache = waveformCaches[cacheLevel];
        
        juce::Path waveformPath;
        bool pathStarted = false;
        
        for (int x = area.getX(); x < area.getRight(); ++x) {
            double time = pixelToTime(x);
            int sampleIndex = static_cast<int>(time * sampleRate);
            int cacheIndex = sampleIndex / cache.samplesPerPixel;
            
            if (cacheIndex >= 0 && cacheIndex < static_cast<int>(cache.minValues.size())) {
                float minY = sampleToY(cache.minValues[cacheIndex], channel);
                float maxY = sampleToY(cache.maxValues[cacheIndex], channel);
                
                if (!pathStarted) {
                    waveformPath.startNewSubPath(static_cast<float>(x), maxY);
                    pathStarted = true;
                } else {
                    waveformPath.lineTo(static_cast<float>(x), maxY);
                }
            }
        }
        
        // Draw the path
        g.strokePath(waveformPath, juce::PathStrokeType(1.0f));
        
        // Draw negative part
        waveformPath.clear();
        pathStarted = false;
        
        for (int x = area.getX(); x < area.getRight(); ++x) {
            double time = pixelToTime(x);
            int sampleIndex = static_cast<int>(time * sampleRate);
            int cacheIndex = sampleIndex / cache.samplesPerPixel;
            
            if (cacheIndex >= 0 && cacheIndex < static_cast<int>(cache.minValues.size())) {
                float minY = sampleToY(cache.minValues[cacheIndex], channel);
                
                if (!pathStarted) {
                    waveformPath.startNewSubPath(static_cast<float>(x), minY);
                    pathStarted = true;
                } else {
                    waveformPath.lineTo(static_cast<float>(x), minY);
                }
            }
        }
        
        g.strokePath(waveformPath, juce::PathStrokeType(1.0f));
    }
}

void WaveformDisplay::drawWaveformRMS(juce::Graphics& g, int channel, const juce::Rectangle<int>& area) {
    g.setColour(getWaveformColorForChannel(channel));
    
    int cacheLevel = getBestCacheLevel();
    if (cacheLevel >= 0 && cacheLevel < static_cast<int>(waveformCaches.size()) && 
        waveformCaches[cacheLevel].valid) {
        
        const auto& cache = waveformCaches[cacheLevel];
        
        juce::Path rmsPath;
        bool pathStarted = false;
        
        for (int x = area.getX(); x < area.getRight(); ++x) {
            double time = pixelToTime(x);
            int sampleIndex = static_cast<int>(time * sampleRate);
            int cacheIndex = sampleIndex / cache.samplesPerPixel;
            
            if (cacheIndex >= 0 && cacheIndex < static_cast<int>(cache.rmsValues.size())) {
                float rmsY = sampleToY(cache.rmsValues[cacheIndex], channel);
                
                if (!pathStarted) {
                    rmsPath.startNewSubPath(static_cast<float>(x), rmsY);
                    pathStarted = true;
                } else {
                    rmsPath.lineTo(static_cast<float>(x), rmsY);
                }
            }
        }
        
        g.strokePath(rmsPath, juce::PathStrokeType(2.0f));
    }
}

void WaveformDisplay::drawWaveformFilled(juce::Graphics& g, int channel, const juce::Rectangle<int>& area) {
    g.setColour(getWaveformColorForChannel(channel).withAlpha(0.6f));
    
    int cacheLevel = getBestCacheLevel();
    if (cacheLevel >= 0 && cacheLevel < static_cast<int>(waveformCaches.size()) && 
        waveformCaches[cacheLevel].valid) {
        
        const auto& cache = waveformCaches[cacheLevel];
        
        juce::Path filledPath;
        float centerY = sampleToY(0.0f, channel);
        
        bool pathStarted = false;
        
        for (int x = area.getX(); x < area.getRight(); ++x) {
            double time = pixelToTime(x);
            int sampleIndex = static_cast<int>(time * sampleRate);
            int cacheIndex = sampleIndex / cache.samplesPerPixel;
            
            if (cacheIndex >= 0 && cacheIndex < static_cast<int>(cache.maxValues.size())) {
                float maxY = sampleToY(cache.maxValues[cacheIndex], channel);
                float minY = sampleToY(cache.minValues[cacheIndex], channel);
                
                if (!pathStarted) {
                    filledPath.startNewSubPath(static_cast<float>(x), centerY);
                    filledPath.lineTo(static_cast<float>(x), maxY);
                    pathStarted = true;
                } else {
                    filledPath.lineTo(static_cast<float>(x), maxY);
                }
            }
        }
        
        // Close the path back to center
        for (int x = area.getRight() - 1; x >= area.getX(); --x) {
            double time = pixelToTime(x);
            int sampleIndex = static_cast<int>(time * sampleRate);
            int cacheIndex = sampleIndex / cache.samplesPerPixel;
            
            if (cacheIndex >= 0 && cacheIndex < static_cast<int>(cache.minValues.size())) {
                float minY = sampleToY(cache.minValues[cacheIndex], channel);
                filledPath.lineTo(static_cast<float>(x), minY);
            }
        }
        
        filledPath.closeSubPath();
        g.fillPath(filledPath);
    }
}

void WaveformDisplay::drawWaveformLine(juce::Graphics& g, int channel, const juce::Rectangle<int>& area) {
    g.setColour(getWaveformColorForChannel(channel));
    
    // Draw center line
    float centerY = sampleToY(0.0f, channel);
    g.drawLine(static_cast<float>(area.getX()), centerY, 
               static_cast<float>(area.getRight()), centerY, 1.0f);
}

void WaveformDisplay::drawWaveformStereo(juce::Graphics& g) {
    if (audioBuffer.getNumChannels() < 2) {
        drawWaveformPeak(g, 0, getLocalBounds());
        return;
    }
    
    int channelHeight = getHeight() / 2;
    
    // Draw left channel (top half)
    auto leftArea = juce::Rectangle<int>(0, 0, getWidth(), channelHeight);
    drawWaveformPeak(g, 0, leftArea);
    
    // Draw right channel (bottom half)
    auto rightArea = juce::Rectangle<int>(0, channelHeight, getWidth(), channelHeight);
    drawWaveformPeak(g, 1, rightArea);
    
    // Draw separator line
    g.setColour(settings.gridColor);
    g.drawLine(0.0f, static_cast<float>(channelHeight), 
               static_cast<float>(getWidth()), static_cast<float>(channelHeight), 1.0f);
}

void WaveformDisplay::drawWaveformMidSide(juce::Graphics& g) {
    // Mid/Side processing would require more complex implementation
    // For now, fall back to stereo display
    drawWaveformStereo(g);
}

void WaveformDisplay::drawRuler(juce::Graphics& g) {
    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
    g.setFont(JUCE8_FONT(10.0f));
    
    // Calculate time intervals
    double viewDuration = settings.viewEnd - settings.viewStart;
    double interval = 1.0; // Start with 1 second
    
    // Adjust interval based on zoom level
    if (viewDuration < 10.0) {
        interval = 0.1;
    } else if (viewDuration < 60.0) {
        interval = 1.0;
    } else if (viewDuration < 600.0) {
        interval = 10.0;
    } else {
        interval = 60.0;
    }
    
    // Draw time markers
    double startTime = std::ceil(settings.viewStart / interval) * interval;
    
    for (double time = startTime; time <= settings.viewEnd; time += interval) {
        int x = timeToPixel(time);
        
        if (x >= 0 && x <= getWidth()) {
            // Draw tick mark
            g.drawLine(static_cast<float>(x), static_cast<float>(getHeight() - 15), 
                      static_cast<float>(x), static_cast<float>(getHeight()), 1.0f);
            
            // Draw time label
            auto timeText = formatTime(time);
            g.drawText(timeText, x - 30, getHeight() - 15, 60, 12, juce::Justification::centred);
        }
    }
}

void WaveformDisplay::drawGrid(juce::Graphics& g) {
    g.setColour(settings.gridColor);
    
    // Vertical grid lines (time)
    double viewDuration = settings.viewEnd - settings.viewStart;
    double interval = viewDuration / 10.0; // 10 grid lines
    
    for (int i = 1; i < 10; ++i) {
        double time = settings.viewStart + i * interval;
        int x = timeToPixel(time);
        
        if (x >= 0 && x <= getWidth()) {
            g.drawLine(static_cast<float>(x), 0.0f, 
                      static_cast<float>(x), static_cast<float>(getHeight()), 0.5f);
        }
    }
    
    // Horizontal grid lines (amplitude)
    for (int i = 1; i < 4; ++i) {
        float amplitude = i * 0.25f; // 25%, 50%, 75%
        float y = sampleToY(amplitude, 0);
        
        g.drawLine(0.0f, y, static_cast<float>(getWidth()), y, 0.5f);
        
        // Negative amplitude
        y = sampleToY(-amplitude, 0);
        g.drawLine(0.0f, y, static_cast<float>(getWidth()), y, 0.5f);
    }
}

void WaveformDisplay::drawSelection(juce::Graphics& g) {
    if (!selection.active) {
        return;
    }
    
    int startX = timeToPixel(selection.startTime);
    int endX = timeToPixel(selection.endTime);
    
    if (endX < 0 || startX > getWidth()) {
        return;
    }
    
    startX = juce::jmax(0, startX);
    endX = juce::jmin(getWidth(), endX);
    
    // Draw selection background
    g.setColour(settings.selectionColor);
    g.fillRect(startX, 0, endX - startX, getHeight());
    
    // Draw selection borders
    g.setColour(settings.selectionColor.brighter(0.5f));
    g.drawLine(static_cast<float>(startX), 0.0f, 
               static_cast<float>(startX), static_cast<float>(getHeight()), 2.0f);
    g.drawLine(static_cast<float>(endX), 0.0f, 
               static_cast<float>(endX), static_cast<float>(getHeight()), 2.0f);
}

void WaveformDisplay::drawPlayhead(juce::Graphics& g) {
    if (!playheadVisible) {
        return;
    }
    
    int x = timeToPixel(playheadPosition);
    
    if (x >= 0 && x <= getWidth()) {
        g.setColour(settings.playheadColor);
        g.drawLine(static_cast<float>(x), 0.0f, 
                   static_cast<float>(x), static_cast<float>(getHeight()), 2.0f);
        
        // Draw playhead triangle at top
        juce::Path triangle;
        triangle.addTriangle(static_cast<float>(x - 5), 0.0f,
                           static_cast<float>(x + 5), 0.0f,
                           static_cast<float>(x), 10.0f);
        g.fillPath(triangle);
    }
}

void WaveformDisplay::drawLoopRegion(juce::Graphics& g) {
    if (!loopRegion.enabled) {
        return;
    }
    
    int startX = timeToPixel(loopRegion.startTime);
    int endX = timeToPixel(loopRegion.endTime);
    
    if (endX < 0 || startX > getWidth()) {
        return;
    }
    
    startX = juce::jmax(0, startX);
    endX = juce::jmin(getWidth(), endX);
    
    // Draw loop background
    g.setColour(settings.loopColor);
    g.fillRect(startX, 0, endX - startX, getHeight());
    
    // Draw loop borders
    g.setColour(settings.loopColor.brighter(0.5f));
    g.drawLine(static_cast<float>(startX), 0.0f, 
               static_cast<float>(startX), static_cast<float>(getHeight()), 2.0f);
    g.drawLine(static_cast<float>(endX), 0.0f, 
               static_cast<float>(endX), static_cast<float>(getHeight()), 2.0f);
}

void WaveformDisplay::drawZeroCrossings(juce::Graphics& g) {
    auto crossings = findZeroCrossings();
    
    g.setColour(juce::Colours::yellow.withAlpha(0.7f));
    
    for (double crossing : crossings) {
        if (crossing >= settings.viewStart && crossing <= settings.viewEnd) {
            int x = timeToPixel(crossing);
            g.drawLine(static_cast<float>(x), 0.0f, 
                      static_cast<float>(x), static_cast<float>(getHeight()), 1.0f);
        }
    }
}

void WaveformDisplay::drawPeakMarkers(juce::Graphics& g) {
    auto peaks = findPeaks(0.5f);
    
    g.setColour(juce::Colours::red.withAlpha(0.7f));
    
    for (double peak : peaks) {
        if (peak >= settings.viewStart && peak <= settings.viewEnd) {
            int x = timeToPixel(peak);
            g.fillEllipse(static_cast<float>(x - 2), static_cast<float>(getHeight() / 2 - 2), 4.0f, 4.0f);
        }
    }
}

//==============================================================================
// Cache Management
//==============================================================================

int WaveformDisplay::getBestCacheLevel() const {
    if (!hasAudioData() || getWidth() <= 0) {
        return -1;
    }
    
    double viewDuration = settings.viewEnd - settings.viewStart;
    double samplesPerPixel = (viewDuration * sampleRate) / getWidth();
    
    // Find the best cache level
    for (int level = 0; level < static_cast<int>(waveformCaches.size()); ++level) {
        int levelSamplesPerPixel = 1 << level;
        if (levelSamplesPerPixel >= samplesPerPixel) {
            return level;
        }
    }
    
    return static_cast<int>(waveformCaches.size()) - 1;
}

void WaveformDisplay::invalidateCache() {
    for (auto& cache : waveformCaches) {
        cache.valid = false;
    }
}

void WaveformDisplay::updateCacheIfNeeded() {
    int bestLevel = getBestCacheLevel();
    if (bestLevel >= 0 && bestLevel < static_cast<int>(waveformCaches.size()) && 
        !waveformCaches[bestLevel].valid) {
        updateWaveformCache(bestLevel);
    }
}

//==============================================================================
// Mouse Handling Helpers
//==============================================================================

WaveformDisplay::MouseMode WaveformDisplay::getMouseModeForPosition(const juce::Point<int>& position) {
    double time = pixelToTime(position.x);
    
    // Check if near playhead
    if (std::abs(timeToPixel(playheadPosition) - position.x) < 5) {
        return MouseMode::DraggingPlayhead;
    }
    
    // Check if near selection edges
    if (selection.active) {
        if (std::abs(timeToPixel(selection.startTime) - position.x) < 5) {
            return MouseMode::DraggingSelection;
        }
        if (std::abs(timeToPixel(selection.endTime) - position.x) < 5) {
            return MouseMode::DraggingSelection;
        }
        
        // Check if inside selection
        if (time >= selection.startTime && time <= selection.endTime) {
            return MouseMode::DraggingSelection;
        }
    }
    
    // Check if near loop region edges
    if (loopRegion.enabled) {
        if (std::abs(timeToPixel(loopRegion.startTime) - position.x) < 5) {
            return MouseMode::DraggingLoopStart;
        }
        if (std::abs(timeToPixel(loopRegion.endTime) - position.x) < 5) {
            return MouseMode::DraggingLoopEnd;
        }
    }
    
    // Default to selection mode
    return MouseMode::Selecting;
}

void WaveformDisplay::handleSelectionDrag(const juce::MouseEvent& e) {
    double currentTime = pixelToTime(e.x);
    
    if (currentMouseMode == MouseMode::Selecting) {
        // Update selection end
        selection.endTime = currentTime;
        
        // Ensure start is before end
        if (selection.endTime < selection.startTime) {
            std::swap(selection.startTime, selection.endTime);
        }
    } else if (currentMouseMode == MouseMode::DraggingSelection) {
        // Move entire selection
        double deltaTime = currentTime - mouseDownTime;
        double selectionLength = originalSelection.getLength();
        
        selection.startTime = originalSelection.startTime + deltaTime;
        selection.endTime = selection.startTime + selectionLength;
        
        // Constrain to audio bounds
        if (selection.startTime < 0) {
            selection.startTime = 0;
            selection.endTime = selectionLength;
        }
        
        double audioLength = getAudioLength();
        if (selection.endTime > audioLength) {
            selection.endTime = audioLength;
            selection.startTime = audioLength - selectionLength;
        }
    }
}

void WaveformDisplay::handlePlayheadDrag(const juce::MouseEvent& e) {
    double time = pixelToTime(e.x);
    setPlayheadPosition(time);
}

void WaveformDisplay::handleLoopDrag(const juce::MouseEvent& e) {
    double currentTime = pixelToTime(e.x);
    
    if (currentMouseMode == MouseMode::DraggingLoopStart) {
        loopRegion.startTime = juce::jlimit(0.0, loopRegion.endTime, currentTime);
    } else if (currentMouseMode == MouseMode::DraggingLoopEnd) {
        loopRegion.endTime = juce::jlimit(loopRegion.startTime, getAudioLength(), currentTime);
    }
}

void WaveformDisplay::handlePanning(const juce::MouseEvent& e) {
    double deltaTime = pixelToTime(mouseDownPosition.x) - pixelToTime(e.x);
    double viewDuration = originalViewEnd - originalViewStart;
    
    settings.viewStart = originalViewStart + deltaTime;
    settings.viewEnd = settings.viewStart + viewDuration;
    
    constrainViewRange();
}

void WaveformDisplay::handleZooming(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel) {
    double centerTime = pixelToTime(e.x);
    
    if (wheel.deltaY > 0) {
        zoomIn(centerTime);
    } else {
        zoomOut(centerTime);
    }
}

//==============================================================================
// Animation Helpers
//==============================================================================

void WaveformDisplay::startViewAnimation(double targetStart, double targetEnd) {
    if (!isValidTimeRange(targetStart, targetEnd)) {
        return;
    }
    
    animationState.animating = true;
    animationState.startViewStart = settings.viewStart;
    animationState.startViewEnd = settings.viewEnd;
    animationState.targetViewStart = targetStart;
    animationState.targetViewEnd = targetEnd;
    animationState.animationStartTime = juce::Time::getCurrentTime();
}

void WaveformDisplay::updateAnimation() {
    if (!animationState.animating) {
        return;
    }
    
    auto currentTime = juce::Time::getCurrentTime();
    double elapsed = (currentTime - animationState.animationStartTime).inSeconds();
    double progress = elapsed / animationState.animationDuration;
    
    if (progress >= 1.0) {
        // Animation complete
        settings.viewStart = animationState.targetViewStart;
        settings.viewEnd = animationState.targetViewEnd;
        animationState.reset();
        
        constrainViewRange();
        updatePixelsPerSecond();
        invalidateCache();
        notifyViewRangeChanged();
    } else {
        // Interpolate
        double easedProgress = easeInOutCubic(progress);
        
        settings.viewStart = animationState.startViewStart + 
            easedProgress * (animationState.targetViewStart - animationState.startViewStart);
        settings.viewEnd = animationState.startViewEnd + 
            easedProgress * (animationState.targetViewEnd - animationState.startViewEnd);
        
        updatePixelsPerSecond();
        invalidateCache();
    }
    
    repaint();
}

double WaveformDisplay::easeInOutCubic(double t) const {
    return t < 0.5 ? 4 * t * t * t : 1 - std::pow(-2 * t + 2, 3) / 2;
}

//==============================================================================
// Utility Methods
//==============================================================================

void WaveformDisplay::constrainViewRange() {
    double audioLength = getAudioLength();
    
    if (audioLength <= 0) {
        settings.viewStart = 0.0;
        settings.viewEnd = 10.0;
        return;
    }
    
    // Ensure view range is within audio bounds
    settings.viewStart = juce::jlimit(0.0, audioLength, settings.viewStart);
    settings.viewEnd = juce::jlimit(settings.viewStart, audioLength, settings.viewEnd);
    
    // Ensure minimum view duration
    double minDuration = 0.001; // 1ms minimum
    if (settings.viewEnd - settings.viewStart < minDuration) {
        settings.viewEnd = settings.viewStart + minDuration;
        if (settings.viewEnd > audioLength) {
            settings.viewEnd = audioLength;
            settings.viewStart = audioLength - minDuration;
        }
    }
}

void WaveformDisplay::updatePixelsPerSecond() {
    if (getWidth() <= 0 || settings.viewEnd <= settings.viewStart) {
        return;
    }
    
    double viewDuration = settings.viewEnd - settings.viewStart;
    settings.pixelsPerSecond = static_cast<int>(getWidth() / viewDuration);
}

juce::String WaveformDisplay::formatTime(double timeInSeconds) const {
    int minutes = static_cast<int>(timeInSeconds / 60.0);
    double seconds = timeInSeconds - minutes * 60.0;
    
    return juce::String::formatted("%d:%05.2f", minutes, seconds);
}

juce::Colour WaveformDisplay::getWaveformColorForChannel(int channel) const {
    if (channel == 0) {
        return settings.waveformColor;
    } else {
        return settings.waveformColor.withHue(settings.waveformColor.getHue() + 0.1f);
    }
}

//==============================================================================
// Validation Helpers
//==============================================================================

bool WaveformDisplay::isValidTimeRange(double startTime, double endTime) const {
    return startTime >= 0 && endTime > startTime && endTime <= getAudioLength();
}

bool WaveformDisplay::isValidZoomLevel(double zoom) const {
    return zoom > 0.001 && zoom < 1000.0;
}

//==============================================================================
// Notification Helpers
//==============================================================================

void WaveformDisplay::notifySelectionChanged() {
    listeners.call([this](Listener& l) { l.waveformSelectionChanged(selection); });
}

void WaveformDisplay::notifyPlayheadMoved() {
    listeners.call([this](Listener& l) { l.waveformPlayheadMoved(playheadPosition); });
}

void WaveformDisplay::notifyZoomChanged() {
    listeners.call([this](Listener& l) { l.waveformZoomChanged(settings.horizontalZoom); });
}

void WaveformDisplay::notifyViewRangeChanged() {
    listeners.call([this](Listener& l) { l.waveformViewRangeChanged(settings.viewStart, settings.viewEnd); });
}

void WaveformDisplay::notifyLoopRegionChanged() {
    listeners.call([this](Listener& l) { l.waveformLoopRegionChanged(loopRegion); });
}