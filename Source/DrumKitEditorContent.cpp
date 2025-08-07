#include "DrumKitEditorContent.h"
#include "INIConfig.h"

DrumKitWaveformDisplay::DrumKitWaveformDisplay()
    : thumbnailCache(INIConfig::LayoutConstants::waveformThumbnailCache),
      thumbnail(std::make_unique<juce::AudioThumbnail>(INIConfig::LayoutConstants::waveformThumbnailCache, formatManager, thumbnailCache))
{
    formatManager.registerBasicFormats();
    thumbnail->addChangeListener(this);
}

DrumKitWaveformDisplay::~DrumKitWaveformDisplay()
{
    thumbnail->removeChangeListener(this);
}

void DrumKitWaveformDisplay::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == thumbnail.get())
        repaint();
}

void DrumKitWaveformDisplay::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    // Draw background with rounded corners
    g.setColour(juce::Colour(0xFF1A1A1A));
    g.fillRoundedRectangle(bounds, INIConfig::LayoutConstants::cornerRadiusSmall);

    // Draw border
    g.setColour(juce::Colour(0xFF4A4A4A));
    g.drawRoundedRectangle(bounds.reduced(0.5f), INIConfig::LayoutConstants::cornerRadiusSmall, 1.0f);

    auto contentBounds = getLocalBounds().reduced(2);

    if (thumbnail->getTotalLength() > 0.0)
    {
        // Draw waveform
        g.setColour(juce::Colour(0xFF6AB7FF).withAlpha(0.8f));
        thumbnail->drawChannels(g, contentBounds, 0.0, thumbnail->getTotalLength(), 1.0f);

        // Draw selection area
        int startX = getXFromPosition(startPosition);
        int endX = getXFromPosition(endPosition);
        
        g.setColour(juce::Colour(0xFF4A90E2).withAlpha(0.3f));
        g.fillRect(startX, contentBounds.getY(), endX - startX, contentBounds.getHeight());

        // Draw start/end markers
        g.setColour(juce::Colour(0xFF4CAF50)); // Green for start
        g.drawVerticalLine(startX, contentBounds.getY(), contentBounds.getBottom());
        g.fillRect(startX - 2, contentBounds.getY(), 4, 8); // Start handle

        g.setColour(juce::Colour(0xFFFF5722)); // Red for end
        g.drawVerticalLine(endX, contentBounds.getY(), contentBounds.getBottom());
        g.fillRect(endX - 2, contentBounds.getBottom() - 8, 4, 8); // End handle
    }
    else
    {
        // Draw drop hint with better styling
        g.setColour(juce::Colours::grey.withAlpha(0.7f));
        g.setFont(juce::FontOptions(INIConfig::LayoutConstants::fontSizeBody));
        g.drawText("Drop audio file here", contentBounds, juce::Justification::centred);
        
        // Draw dashed border for drop zone
        g.setColour(juce::Colours::grey.withAlpha(0.5f));
        juce::Path dashedPath;
        dashedPath.addRoundedRectangle(contentBounds.reduced(10).toFloat(), 
                                      INIConfig::LayoutConstants::cornerRadiusSmall);
        float dashLengths[] = { 5.0f, 5.0f };
        juce::PathStrokeType strokeType(1.0f);
        juce::Path strokedPath;
        strokeType.createDashedStroke(strokedPath, dashedPath, dashLengths, 2, juce::AffineTransform());
        g.fillPath(strokedPath);
    }
}

void DrumKitWaveformDisplay::resized()
{
}

void VelocityCurveEditor::resized()
{
    updateCurvePath();
}

void PadEditor::resized()
{
}

void DrumKitWaveformDisplay::loadAudioFile(const juce::File& file)
{
    if (file.existsAsFile())
    {
        thumbnail->setSource(new juce::FileInputSource(file));
        startPosition = 0.0f;
        endPosition = 1.0f;
        repaint();
    }
}

void DrumKitWaveformDisplay::clearWaveform()
{
    thumbnail->setSource(nullptr);
    repaint();
}

bool DrumKitWaveformDisplay::isInterestedInFileDrag(const juce::StringArray& files)
{
    for (const auto& file : files)
    {
        if (file.endsWith(".wav") || file.endsWith(".aif") || file.endsWith(".aiff") ||
            file.endsWith(".flac") || file.endsWith(".ogg") || file.endsWith(".mp3"))
        {
            return true;
        }
    }
    return false;
}

void DrumKitWaveformDisplay::filesDropped(const juce::StringArray& files, int x, int y)
{
    juce::ignoreUnused(x, y);

    for (const auto& filename : files)
    {
        juce::File file(filename);
        if (file.existsAsFile())
        {
            loadAudioFile(file);
            if (onFileDropped)
                onFileDropped(file);
            break;
        }
    }
}

void DrumKitWaveformDisplay::setStartPosition(float position)
{
    startPosition = juce::jlimit(0.0f, endPosition - INIConfig::LayoutConstants::waveformMinSelectionSize, position);
    repaint();
    if (onSelectionChanged)
        onSelectionChanged(startPosition, endPosition);
}

void DrumKitWaveformDisplay::setEndPosition(float position)
{
    endPosition = juce::jlimit(startPosition + INIConfig::LayoutConstants::waveformMinSelectionSize, 1.0f, position);
    repaint();
    if (onSelectionChanged)
        onSelectionChanged(startPosition, endPosition);
}

void DrumKitWaveformDisplay::mouseDown(const juce::MouseEvent& e)
{
    if (thumbnail->getTotalLength() > 0.0)
    {
        float clickPos = getPositionFromX(e.x);
        float startDist = std::abs(clickPos - startPosition);
        float endDist = std::abs(clickPos - endPosition);

        if (startDist < endDist && startDist < INIConfig::LayoutConstants::waveformPositionTolerance)
        {
            isDraggingStart = true;
        }
        else if (endDist < INIConfig::LayoutConstants::waveformPositionTolerance)
        {
            isDraggingEnd = true;
        }
    }
}

void DrumKitWaveformDisplay::mouseDrag(const juce::MouseEvent& e)
{
    if (isDraggingStart)
    {
        setStartPosition(getPositionFromX(e.x));
    }
    else if (isDraggingEnd)
    {
        setEndPosition(getPositionFromX(e.x));
    }
}

void DrumKitWaveformDisplay::mouseUp(const juce::MouseEvent& e)
{
    juce::ignoreUnused(e);
    isDraggingStart = false;
    isDraggingEnd = false;
}

float DrumKitWaveformDisplay::getPositionFromX(int x) const
{
    return juce::jlimit(0.0f, 1.0f, static_cast<float>(x) / static_cast<float>(getWidth()));
}

int DrumKitWaveformDisplay::getXFromPosition(float position) const
{
    return juce::roundToInt(position * getWidth());
}

VelocityCurveEditor::VelocityCurveEditor()
{
    updateCurvePath();
}

void VelocityCurveEditor::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    // Draw background with rounded corners
    g.setColour(juce::Colour(0xFF1A1A1A));
    g.fillRoundedRectangle(bounds, INIConfig::LayoutConstants::cornerRadiusSmall);

    // Draw border
    g.setColour(juce::Colour(0xFF4A4A4A));
    g.drawRoundedRectangle(bounds.reduced(0.5f), INIConfig::LayoutConstants::cornerRadiusSmall, 1.0f);

    auto contentBounds = getLocalBounds().reduced(4);

    // Draw grid lines
    g.setColour(juce::Colour(0xFF3A3A3A).withAlpha(0.5f));
    for (int i = 1; i < 4; ++i)
    {
        float x = contentBounds.getX() + contentBounds.getWidth() * i / 4.0f;
        float y = contentBounds.getY() + contentBounds.getHeight() * i / 4.0f;
        g.drawVerticalLine(juce::roundToInt(x), contentBounds.getY(), contentBounds.getBottom());
        g.drawHorizontalLine(juce::roundToInt(y), contentBounds.getX(), contentBounds.getRight());
    }

    // Draw curve with better styling
    g.setColour(juce::Colour(0xFF4A90E2));
    g.strokePath(curvePath, juce::PathStrokeType(2.0f));

    // Draw curve type label
    g.setColour(juce::Colours::white.withAlpha(0.7f));
    g.setFont(juce::FontOptions(INIConfig::LayoutConstants::fontSizeSmall));
    juce::String curveTypeText;
    switch (curveType) {
        case Linear: curveTypeText = "Linear"; break;
        case Exponential: curveTypeText = "Exponential"; break;
        case Logarithmic: curveTypeText = "Logarithmic"; break;
        case SCurve: curveTypeText = "S-Curve"; break;
        default: curveTypeText = "Linear"; break;
    }
    g.drawText("Velocity: " + curveTypeText, 
              contentBounds.removeFromTop(16).toFloat(), 
              juce::Justification::centredLeft);
}

void VelocityCurveEditor::setCurveType(int type)
{
    curveType = type;
    updateCurvePath();
    repaint();
    if (onCurveChanged)
        onCurveChanged(curveType, curveAmount);
}

void VelocityCurveEditor::setCurveAmount(float amount)
{
    curveAmount = juce::jlimit(0.0f, 2.0f, amount);
    updateCurvePath();
    repaint();
    if (onCurveChanged)
        onCurveChanged(curveType, curveAmount);
}

void VelocityCurveEditor::updateCurvePath()
{
    curvePath.clear();

    const int numPoints = INIConfig::LayoutConstants::velocityEditorNumPoints;
    for (int i = 0; i < numPoints; ++i)
    {
        float x = static_cast<float>(i) / (numPoints - 1);
        float y = 1.0f - applyCurve(x);

        float px = x * getWidth();
        float py = y * getHeight();

        if (i == 0)
            curvePath.startNewSubPath(px, py);
        else
            curvePath.lineTo(px, py);
    }
}

float VelocityCurveEditor::applyCurve(float input) const
{
    switch (curveType)
    {
        case Linear:
            return input;

        case Exponential:
            return std::pow(input, curveAmount);

        case Logarithmic:
            return std::log(1.0f + input * curveAmount) / std::log(1.0f + curveAmount);

        case SCurve:
        {
            float t = input;
            float a = curveAmount;
            return t * t * (INIConfig::LayoutConstants::velocityEditorSCurveFactor - 2.0f * t) * (1.0f - a) + t * a;
        }

        default:
            return input;
    }
}

void VelocityCurveEditor::mouseDown(const juce::MouseEvent& e)
{
    float x = static_cast<float>(e.x) / getWidth();
    float y = 1.0f - static_cast<float>(e.y) / getHeight();

    setCurveAmount(y / x);
}

void VelocityCurveEditor::mouseDrag(const juce::MouseEvent& e)
{
    mouseDown(e);
}

PadEditor::PadEditor(int padNum) : padNumber(padNum)
{
}

void PadEditor::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    // Use ColorScheme for consistent styling like Row5 drum buttons
    juce::Colour bgColor;
    if (isSelectedPad) {
        bgColor = juce::Colour(0xFF4A90E2); // Accent color for selected
    } else if (isHovering) {
        bgColor = juce::Colour(0xFF3A3A3A).brighter(0.2f);
    } else {
        bgColor = juce::Colour(0xFF2A2A2A); // Dark background like Row5 buttons
    }

    // Draw rounded rectangle background like Row5 buttons
    g.setColour(bgColor);
    g.fillRoundedRectangle(bounds, INIConfig::LayoutConstants::cornerRadiusSmall);

    // Draw border
    g.setColour(isSelectedPad ? juce::Colour(0xFF4A90E2) : juce::Colour(0xFF4A4A4A));
    g.drawRoundedRectangle(bounds.reduced(0.5f), INIConfig::LayoutConstants::cornerRadiusSmall, 
                          isSelectedPad ? 2.0f : 1.0f);

    // Draw pad number (top-left)
    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(INIConfig::LayoutConstants::fontSizeButton));
    auto numberBounds = getLocalBounds().reduced(INIConfig::LayoutConstants::defaultPadding);
    g.drawText(juce::String(padNumber + 1), numberBounds, juce::Justification::topLeft);

    if (hasSample())
    {
        // Draw sample name (bottom area)
        g.setFont(juce::FontOptions(INIConfig::LayoutConstants::fontSizeSmall));
        auto sampleNameBounds = getLocalBounds().reduced(INIConfig::LayoutConstants::defaultPadding);
        g.drawText(currentSampleFile.getFileNameWithoutExtension(),
                  sampleNameBounds, juce::Justification::bottomLeft);
        
        // Draw sample indicator (center)
        g.setColour(juce::Colour(0xFF4A90E2));
        auto centerBounds = getLocalBounds().reduced(getWidth() / 3);
        g.fillEllipse(centerBounds.toFloat());
    }
    else
    {
        // Draw drop hint
        g.setColour(juce::Colours::grey);
        g.setFont(juce::FontOptions(INIConfig::LayoutConstants::fontSizeSmall));
        g.drawText("Drop\nSample", getLocalBounds(), juce::Justification::centred);
    }
}

bool PadEditor::isInterestedInFileDrag(const juce::StringArray& files)
{
    for (const auto& file : files)
    {
        if (file.endsWith(".wav") || file.endsWith(".aif") || file.endsWith(".aiff") ||
            file.endsWith(".flac") || file.endsWith(".ogg") || file.endsWith(".mp3"))
        {
            return true;
        }
    }
    return false;
}

void PadEditor::filesDropped(const juce::StringArray& files, int x, int y)
{
    juce::ignoreUnused(x, y);

    for (const auto& filename : files)
    {
        juce::File file(filename);
        if (file.existsAsFile())
        {
            setSample(file);
            if (onSampleAssigned)
                onSampleAssigned(padNumber, file);
            break;
        }
    }
}

void PadEditor::setSample(const juce::File& file)
{
    currentSampleFile = file;
    repaint();
}

void PadEditor::clearSample()
{
    currentSampleFile = juce::File();
    repaint();
}

void PadEditor::setSelected(bool selected)
{
    isSelectedPad = selected;
    repaint();
}

void PadEditor::mouseDown(const juce::MouseEvent& e)
{
    juce::ignoreUnused(e);
    if (onPadSelected)
        onPadSelected(padNumber);
}

void PadEditor::mouseEnter(const juce::MouseEvent& e)
{
    juce::ignoreUnused(e);
    isHovering = true;
    repaint();
}

void PadEditor::mouseExit(const juce::MouseEvent& e)
{
    juce::ignoreUnused(e);
    isHovering = false;
    repaint();
}

SampleEditControls::SampleEditControls()
{
    setupControls();
}

void SampleEditControls::setupControls()
{
    // Setup labels with proper fonts
    startLabel.setText("Start:", juce::dontSendNotification);
    endLabel.setText("End:", juce::dontSendNotification);
    pitchLabel.setText("Pitch:", juce::dontSendNotification);
    filterLabel.setText("Filter:", juce::dontSendNotification);
    chokeLabel.setText("Choke:", juce::dontSendNotification);

    // Apply consistent label styling
    for (auto* label : { &startLabel, &endLabel, &pitchLabel, &filterLabel, &chokeLabel }) {
        label->setFont(juce::FontOptions(INIConfig::LayoutConstants::fontSizeLabel));
        label->setColour(juce::Label::textColourId, juce::Colours::white);
        addAndMakeVisible(label);
    }

    // Setup sliders with mixer-style appearance
    startSlider.setRange(0.0, 1.0, 0.001);
    startSlider.setValue(0.0);
    startSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    startSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    startSlider.addListener(this);
    addAndMakeVisible(startSlider);

    endSlider.setRange(0.0, 1.0, 0.001);
    endSlider.setValue(1.0);
    endSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    endSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    endSlider.addListener(this);
    addAndMakeVisible(endSlider);

    pitchSlider.setRange(-24.0, 24.0, 0.1);
    pitchSlider.setValue(0.0);
    pitchSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    pitchSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    pitchSlider.setTextValueSuffix(" st");
    pitchSlider.addListener(this);
    addAndMakeVisible(pitchSlider);

    filterFreqSlider.setRange(20.0, 20000.0);
    filterFreqSlider.setSkewFactorFromMidPoint(2000.0);
    filterFreqSlider.setValue(20000.0);
    filterFreqSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    filterFreqSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    filterFreqSlider.setTextValueSuffix(" Hz");
    filterFreqSlider.addListener(this);
    addAndMakeVisible(filterFreqSlider);

    filterResSlider.setRange(0.1, 10.0);
    filterResSlider.setValue(1.0);
    filterResSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    filterResSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    filterResSlider.addListener(this);
    addAndMakeVisible(filterResSlider);

    // Apply mixer-style slider colors
    auto sliderBgColor = juce::Colour(0xFF2A2A2A);
    auto sliderTrackColor = juce::Colour(0xFF4A90E2);
    auto sliderThumbColor = juce::Colour(0xFF6AB7FF);

    for (auto* slider : { &startSlider, &endSlider, &pitchSlider, &filterFreqSlider, &filterResSlider }) {
        slider->setColour(juce::Slider::backgroundColourId, sliderBgColor);
        slider->setColour(juce::Slider::trackColourId, sliderTrackColor);
        slider->setColour(juce::Slider::thumbColourId, sliderThumbColor);
        slider->setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
        slider->setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xFF1A1A1A));
        slider->setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xFF4A4A4A));
    }

    // Setup combo boxes
    filterTypeCombo.addItem("Off", 1);
    filterTypeCombo.addItem("Low Pass", 2);
    filterTypeCombo.addItem("High Pass", 3);
    filterTypeCombo.addItem("Band Pass", 4);
    filterTypeCombo.setSelectedId(1);
    filterTypeCombo.addListener(this);
    filterTypeCombo.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xFF2A2A2A));
    filterTypeCombo.setColour(juce::ComboBox::textColourId, juce::Colours::white);
    filterTypeCombo.setColour(juce::ComboBox::outlineColourId, juce::Colour(0xFF4A4A4A));
    addAndMakeVisible(filterTypeCombo);

    for (int i = 0; i <= INIConfig::LayoutConstants::drumKitEditorChokeGroupCount; ++i)
    {
        chokeGroupCombo.addItem(i == 0 ? "None" : juce::String(i), i + 1);
    }
    chokeGroupCombo.setSelectedId(1);
    chokeGroupCombo.addListener(this);
    chokeGroupCombo.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xFF2A2A2A));
    chokeGroupCombo.setColour(juce::ComboBox::textColourId, juce::Colours::white);
    chokeGroupCombo.setColour(juce::ComboBox::outlineColourId, juce::Colour(0xFF4A4A4A));
    addAndMakeVisible(chokeGroupCombo);

    // Setup toggle buttons with consistent styling
    reverseButton.setButtonText("Reverse");
    reverseButton.setClickingTogglesState(true);
    reverseButton.onClick = [this]() { updateParametersAndNotify(); };
    reverseButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xFF2A2A2A));
    reverseButton.setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xFF4A90E2));
    reverseButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    reverseButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    addAndMakeVisible(reverseButton);

    normalizeButton.setButtonText("Normalize");
    normalizeButton.setClickingTogglesState(true);
    normalizeButton.onClick = [this]() { updateParametersAndNotify(); };
    normalizeButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xFF2A2A2A));
    normalizeButton.setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xFF4A90E2));
    normalizeButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    normalizeButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    addAndMakeVisible(normalizeButton);
}

void SampleEditControls::resized()
{
    auto bounds = getLocalBounds();
    const int labelWidth = 60;
    const int rowHeight = 30;
    const int spacing = 8;
    const int comboWidth = 80;

    // Start position
    auto row = bounds.removeFromTop(rowHeight);
    startLabel.setBounds(row.removeFromLeft(labelWidth));
    row.removeFromLeft(spacing);
    startSlider.setBounds(row);

    bounds.removeFromTop(spacing);

    // End position
    row = bounds.removeFromTop(rowHeight);
    endLabel.setBounds(row.removeFromLeft(labelWidth));
    row.removeFromLeft(spacing);
    endSlider.setBounds(row);

    bounds.removeFromTop(spacing);

    // Pitch
    row = bounds.removeFromTop(rowHeight);
    pitchLabel.setBounds(row.removeFromLeft(labelWidth));
    row.removeFromLeft(spacing);
    pitchSlider.setBounds(row);

    bounds.removeFromTop(spacing);

    // Filter section
    row = bounds.removeFromTop(rowHeight);
    filterLabel.setBounds(row.removeFromLeft(labelWidth));
    row.removeFromLeft(spacing);
    filterTypeCombo.setBounds(row.removeFromLeft(comboWidth));
    row.removeFromLeft(spacing);
    
    // Split remaining space for frequency and resonance
    auto freqWidth = row.getWidth() / 2 - spacing / 2;
    filterFreqSlider.setBounds(row.removeFromLeft(freqWidth));
    row.removeFromLeft(spacing);
    filterResSlider.setBounds(row);

    bounds.removeFromTop(spacing);

    // Choke group
    row = bounds.removeFromTop(rowHeight);
    chokeLabel.setBounds(row.removeFromLeft(labelWidth));
    row.removeFromLeft(spacing);
    chokeGroupCombo.setBounds(row.removeFromLeft(comboWidth));

    bounds.removeFromTop(spacing);

    // Toggle buttons
    row = bounds.removeFromTop(rowHeight);
    auto buttonWidth = (row.getWidth() - spacing) / 2;
    reverseButton.setBounds(row.removeFromLeft(buttonWidth));
    row.removeFromLeft(spacing);
    normalizeButton.setBounds(row);
}

void SampleEditControls::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &startSlider)
        currentParameters.startPosition = static_cast<float>(slider->getValue());
    else if (slider == &endSlider)
        currentParameters.endPosition = static_cast<float>(slider->getValue());
    else if (slider == &pitchSlider)
        currentParameters.pitchShift = static_cast<float>(slider->getValue());
    else if (slider == &filterFreqSlider)
        currentParameters.filterFrequency = static_cast<float>(slider->getValue());
    else if (slider == &filterResSlider)
        currentParameters.filterResonance = static_cast<float>(slider->getValue());

    updateParametersAndNotify();
}

void SampleEditControls::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == &filterTypeCombo)
        currentParameters.filterType = comboBoxThatHasChanged->getSelectedId() - 1;
    else if (comboBoxThatHasChanged == &chokeGroupCombo)
        currentParameters.chokeGroup = comboBoxThatHasChanged->getSelectedId() - 1;

    updateParametersAndNotify();
}

void SampleEditControls::updateParametersAndNotify()
{
    currentParameters.reverse = reverseButton.getToggleState();
    currentParameters.normalize = normalizeButton.getToggleState();

    if (onParametersChanged)
        onParametersChanged(currentParameters);
}

void SampleEditControls::setSampleParameters(float start, float end, float pitch,
                                            int filterType, float filterFreq, float filterRes)
{
    startSlider.setValue(start, juce::dontSendNotification);
    endSlider.setValue(end, juce::dontSendNotification);
    pitchSlider.setValue(pitch, juce::dontSendNotification);
    filterTypeCombo.setSelectedId(filterType + 1, juce::dontSendNotification);
    filterFreqSlider.setValue(filterFreq, juce::dontSendNotification);
    filterResSlider.setValue(filterRes, juce::dontSendNotification);

    currentParameters.startPosition = start;
    currentParameters.endPosition = end;
    currentParameters.pitchShift = pitch;
    currentParameters.filterType = filterType;
    currentParameters.filterFrequency = filterFreq;
    currentParameters.filterResonance = filterRes;
}

SampleBrowser::SampleBrowser()
    : fileFilter("*.wav;*.aif;*.aiff;*.flac;*.ogg;*.mp3", "*", "Audio Files")
{
    juce::File defaultLocation = juce::File::getSpecialLocation(juce::File::userMusicDirectory);

    fileBrowser = std::make_unique<juce::FileBrowserComponent>(
        juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        defaultLocation,
        &fileFilter,
        nullptr
    );

    fileBrowser->addListener(this);
    addAndMakeVisible(fileBrowser.get());
}

SampleBrowser::~SampleBrowser()
{
    fileBrowser->removeListener(this);
}

void SampleBrowser::resized()
{
    fileBrowser->setBounds(getLocalBounds());
}

void SampleBrowser::selectionChanged()
{
    auto selectedFile = fileBrowser->getSelectedFile(0);
    if (selectedFile.existsAsFile() && onFileSelected)
    {
        onFileSelected(selectedFile);
    }
}

void SampleBrowser::fileClicked(const juce::File& file, const juce::MouseEvent& e)
{
    juce::ignoreUnused(e);
    if (file.existsAsFile() && onFileSelected)
    {
        onFileSelected(file);
    }
}

void SampleBrowser::fileDoubleClicked(const juce::File& file)
{
    if (file.existsAsFile() && onFileDoubleClicked)
    {
        onFileDoubleClicked(file);
    }
}

DrumKitEditorContent::DrumKitEditorContent(SFZEngine& sfz,
                                         ColorScheme& cs,
                                         FontManager& fm,
                                         ResponsiveLayoutManager& lm,
                                         INIDataManager& ini)
    : sfzEngine(sfz),
      colorScheme(cs),
      fontManager(fm),
      layoutManager(lm),
      iniManager(ini),
      newKitButton("New"),
      saveKitButton("Save"),
      loadKitButton("Load")
{
    setupComponents();
    connectCallbacks();
}

DrumKitEditorContent::~DrumKitEditorContent()
{
}

void DrumKitEditorContent::setupComponents()
{
    sampleBrowser = std::make_unique<SampleBrowser>();
    addAndMakeVisible(sampleBrowser.get());

    waveformDisplay = std::make_unique<DrumKitWaveformDisplay>();
    addAndMakeVisible(waveformDisplay.get());

    velocityCurveEditor = std::make_unique<VelocityCurveEditor>();
    addAndMakeVisible(velocityCurveEditor.get());

    sampleEditControls = std::make_unique<SampleEditControls>();
    addAndMakeVisible(sampleEditControls.get());

    for (int i = 0; i < INIConfig::LayoutConstants::drumKitEditorPadCount; ++i)
    {
        auto* pad = new PadEditor(i);
        padEditors.add(pad);
        addAndMakeVisible(pad);
    }

    // Setup main action buttons with consistent styling
    setupMainButtons();

    // Setup kit name controls
    kitNameLabel.setText("Kit Name:", juce::dontSendNotification);
    kitNameLabel.setFont(juce::FontOptions(INIConfig::LayoutConstants::fontSizeLabel));
    kitNameLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(kitNameLabel);

    kitNameEditor.setText("New Kit");
    kitNameEditor.setFont(juce::FontOptions(INIConfig::LayoutConstants::fontSizeBody));
    kitNameEditor.setColour(juce::TextEditor::textColourId, juce::Colours::white);
    kitNameEditor.setColour(juce::TextEditor::backgroundColourId, juce::Colour(0xFF2A2A2A));
    kitNameEditor.setColour(juce::TextEditor::outlineColourId, juce::Colour(0xFF4A4A4A));
    kitNameEditor.setColour(juce::TextEditor::focusedOutlineColourId, juce::Colour(0xFF4A90E2));
    addAndMakeVisible(kitNameEditor);
}

void DrumKitEditorContent::setupMainButtons()
{
    // Apply consistent button styling like Row5 buttons
    auto buttonBgColor = juce::Colour(0xFF2A2A2A);
    auto buttonHoverColor = juce::Colour(0xFF3A3A3A);
    auto buttonTextColor = juce::Colours::white;

    newKitButton.setColour(juce::TextButton::buttonColourId, buttonBgColor);
    newKitButton.setColour(juce::TextButton::buttonOnColourId, buttonHoverColor);
    newKitButton.setColour(juce::TextButton::textColourOffId, buttonTextColor);
    newKitButton.setColour(juce::TextButton::textColourOnId, buttonTextColor);
    newKitButton.addListener(this);
    addAndMakeVisible(newKitButton);

    saveKitButton.setColour(juce::TextButton::buttonColourId, buttonBgColor);
    saveKitButton.setColour(juce::TextButton::buttonOnColourId, buttonHoverColor);
    saveKitButton.setColour(juce::TextButton::textColourOffId, buttonTextColor);
    saveKitButton.setColour(juce::TextButton::textColourOnId, buttonTextColor);
    saveKitButton.addListener(this);
    addAndMakeVisible(saveKitButton);

    loadKitButton.setColour(juce::TextButton::buttonColourId, buttonBgColor);
    loadKitButton.setColour(juce::TextButton::buttonOnColourId, buttonHoverColor);
    loadKitButton.setColour(juce::TextButton::textColourOffId, buttonTextColor);
    loadKitButton.setColour(juce::TextButton::textColourOnId, buttonTextColor);
    loadKitButton.addListener(this);
    addAndMakeVisible(loadKitButton);
}

void DrumKitEditorContent::connectCallbacks()
{
    sampleBrowser->onFileSelected = [this](const juce::File& file)
    {
        waveformDisplay->loadAudioFile(file);
    };

    sampleBrowser->onFileDoubleClicked = [this](const juce::File& file)
    {
        if (selectedPadIndex >= 0 && selectedPadIndex < INIConfig::LayoutConstants::drumKitEditorPadCount)
        {
            handleSampleAssignment(selectedPadIndex, file);
        }
    };

    waveformDisplay->onFileDropped = [this](const juce::File& file)
    {
        if (selectedPadIndex >= 0 && selectedPadIndex < INIConfig::LayoutConstants::drumKitEditorPadCount)
        {
            handleSampleAssignment(selectedPadIndex, file);
        }
    };

    waveformDisplay->onSelectionChanged = [this](float start, float end)
    {
        if (selectedPadIndex >= 0 && selectedPadIndex < INIConfig::LayoutConstants::drumKitEditorPadCount)
        {
            padData[selectedPadIndex].parameters.startPosition = start;
            padData[selectedPadIndex].parameters.endPosition = end;
            sampleEditControls->setSampleParameters(
                start, end,
                padData[selectedPadIndex].parameters.pitchShift,
                padData[selectedPadIndex].parameters.filterType,
                padData[selectedPadIndex].parameters.filterFrequency,
                padData[selectedPadIndex].parameters.filterResonance
            );
        }
    };

    for (auto* pad : padEditors)
    {
        pad->onPadSelected = [this](int padNumber)
        {
            handlePadSelection(padNumber);
        };

        pad->onSampleAssigned = [this](int padNumber, const juce::File& file)
        {
            handleSampleAssignment(padNumber, file);
        };
    }

    sampleEditControls->onParametersChanged = [this](const SampleEditControls::SampleParameters& params)
    {
        handleSampleParameterChange(params);
    };

    velocityCurveEditor->onCurveChanged = [this](int type, float amount)
    {
        if (selectedPadIndex >= 0 && selectedPadIndex < INIConfig::LayoutConstants::drumKitEditorPadCount)
        {
            padData[selectedPadIndex].velocityCurveType = type;
            padData[selectedPadIndex].velocityCurve = amount;
        }
    };
}

void DrumKitEditorContent::paint(juce::Graphics& g)
{
    g.fillAll(colorScheme.getBackgroundColor());
}

void DrumKitEditorContent::resized()
{
    auto bounds = getLocalBounds();
    const int margin = layoutManager.scaled(INIConfig::LayoutConstants::defaultMargin);
    const int spacing = layoutManager.scaled(INIConfig::LayoutConstants::defaultSpacing);
    
    bounds = bounds.reduced(margin);

    // TOP BAR: Kit name and action buttons
    auto topBar = bounds.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::defaultButtonHeight + spacing));
    
    // Kit name section
    kitNameLabel.setBounds(topBar.removeFromLeft(layoutManager.scaled(INIConfig::LayoutConstants::drumKitEditorLabelWidth)));
    topBar.removeFromLeft(spacing);
    kitNameEditor.setBounds(topBar.removeFromLeft(layoutManager.scaled(INIConfig::LayoutConstants::themeTabComboWidth)));
    topBar.removeFromLeft(margin);

    // Action buttons - right aligned
    const int buttonWidth = layoutManager.scaled(INIConfig::LayoutConstants::defaultButtonWidth);
    loadKitButton.setBounds(topBar.removeFromRight(buttonWidth));
    topBar.removeFromRight(spacing);
    saveKitButton.setBounds(topBar.removeFromRight(buttonWidth));
    topBar.removeFromRight(spacing);
    newKitButton.setBounds(topBar.removeFromRight(buttonWidth));

    bounds.removeFromTop(spacing);

    // MAIN CONTENT AREA: Three-column layout
    // Left: Sample Browser (25%)
    // Center: Pad Grid (40%) 
    // Right: Controls (35%)
    
    auto leftPanel = bounds.removeFromLeft(static_cast<int>(bounds.getWidth() * 0.25f));
    bounds.removeFromLeft(spacing);
    
    auto centerPanel = bounds.removeFromLeft(static_cast<int>(bounds.getWidth() * 0.53f)); // 40% of original width
    bounds.removeFromLeft(spacing);
    
    auto rightPanel = bounds; // Remaining 35%

    // LEFT PANEL: Sample Browser
    sampleBrowser->setBounds(leftPanel);

    // CENTER PANEL: 4x4 Pad Grid (styled like Row5 drum buttons)
    const int padGridSize = juce::jmin(centerPanel.getWidth(), centerPanel.getHeight());
    const int padSize = (padGridSize - (spacing * 3)) / 4; // 4x4 grid with spacing
    const int gridStartX = centerPanel.getX() + (centerPanel.getWidth() - padGridSize) / 2;
    const int gridStartY = centerPanel.getY() + (centerPanel.getHeight() - padGridSize) / 2;

    for (int row = 0; row < INIConfig::LayoutConstants::drumKitEditorPadRows; ++row)
    {
        for (int col = 0; col < INIConfig::LayoutConstants::drumKitEditorPadColumns; ++col)
        {
            int index = row * INIConfig::LayoutConstants::drumKitEditorPadColumns + col;
            if (index < padEditors.size())
            {
                int x = gridStartX + col * (padSize + spacing);
                int y = gridStartY + row * (padSize + spacing);
                padEditors[index]->setBounds(x, y, padSize, padSize);
            }
        }
    }

    // RIGHT PANEL: Controls in vertical sections
    auto waveformArea = rightPanel.removeFromTop(static_cast<int>(rightPanel.getHeight() * 0.3f));
    rightPanel.removeFromTop(spacing);
    
    auto editControlsArea = rightPanel.removeFromTop(static_cast<int>(rightPanel.getHeight() * 0.6f));
    rightPanel.removeFromTop(spacing);
    
    auto velocityCurveArea = rightPanel; // Remaining space

    // Set bounds for right panel components
    waveformDisplay->setBounds(waveformArea);
    sampleEditControls->setBounds(editControlsArea);
    velocityCurveEditor->setBounds(velocityCurveArea);
}

void DrumKitEditorContent::buttonClicked(juce::Button* button)
{
    if (button == &newKitButton)
        newKit();
    else if (button == &saveKitButton)
        saveKit();
    else if (button == &loadKitButton)
        loadKit();
}

void DrumKitEditorContent::handlePadSelection(int padNumber)
{
    if (padNumber >= 0 && padNumber < INIConfig::LayoutConstants::drumKitEditorPadCount)
    {
        for (int i = 0; i < padEditors.size(); ++i)
        {
            padEditors[i]->setSelected(i == padNumber);
        }

        selectedPadIndex = padNumber;

        updateWaveformForPad(padNumber);

        const auto& params = padData[padNumber].parameters;
        sampleEditControls->setSampleParameters(
            params.startPosition,
            params.endPosition,
            params.pitchShift,
            params.filterType,
            params.filterFrequency,
            params.filterResonance
        );

        velocityCurveEditor->setCurveType(padData[padNumber].velocityCurveType);
        velocityCurveEditor->setCurveAmount(padData[padNumber].velocityCurve);
    }
}

void DrumKitEditorContent::handleSampleAssignment(int padNumber, const juce::File& file)
{
    if (padNumber >= 0 && padNumber < INIConfig::LayoutConstants::drumKitEditorPadCount && file.existsAsFile())
    {
        padData[padNumber].sampleFile = file;
        padEditors[padNumber]->setSample(file);

        if (padNumber == selectedPadIndex)
        {
            waveformDisplay->loadAudioFile(file);
        }
    }
}

void DrumKitEditorContent::handleSampleParameterChange(const SampleEditControls::SampleParameters& params)
{
    if (selectedPadIndex >= 0 && selectedPadIndex < INIConfig::LayoutConstants::drumKitEditorPadCount)
    {
        padData[selectedPadIndex].parameters = params;

        waveformDisplay->setStartPosition(params.startPosition);
        waveformDisplay->setEndPosition(params.endPosition);
    }
}

void DrumKitEditorContent::updateWaveformForPad(int padNumber)
{
    if (padNumber >= 0 && padNumber < INIConfig::LayoutConstants::drumKitEditorPadCount)
    {
        const auto& file = padData[padNumber].sampleFile;
        if (file.existsAsFile())
        {
            waveformDisplay->loadAudioFile(file);
            waveformDisplay->setStartPosition(padData[padNumber].parameters.startPosition);
            waveformDisplay->setEndPosition(padData[padNumber].parameters.endPosition);
        }
        else
        {
            waveformDisplay->clearWaveform();
        }
    }
}

void DrumKitEditorContent::newKit()
{
    for (int i = 0; i < INIConfig::LayoutConstants::drumKitEditorPadCount; ++i)
    {
        padData[i] = PadSampleData();
        padEditors[i]->clearSample();
    }

    kitNameEditor.setText("New Kit");
    waveformDisplay->clearWaveform();
    selectedPadIndex = -1;

    for (auto* pad : padEditors)
    {
        pad->setSelected(false);
    }
}

void DrumKitEditorContent::saveKit()
{
    auto chooser = std::make_unique<juce::FileChooser>(
        "Save Drum Kit",
        juce::File::getSpecialLocation(juce::File::userDocumentsDirectory),
        "*.sfz"
    );

    chooser->launchAsync(juce::FileBrowserComponent::saveMode, [this](const juce::FileChooser& fc)
    {
        auto result = fc.getResult();
        if (result.existsAsFile() || result.hasWriteAccess())
        {
            exportKitAsSFZ(result);
        }
    });
}

void DrumKitEditorContent::loadKit()
{
    auto chooser = std::make_unique<juce::FileChooser>(
        "Load Drum Kit",
        juce::File::getSpecialLocation(juce::File::userDocumentsDirectory),
        "*.sfz"
    );

    chooser->launchAsync(juce::FileBrowserComponent::openMode, [this](const juce::FileChooser& fc)
    {
        auto result = fc.getResult();
        if (result.existsAsFile())
        {
            importKitFromSFZ(result);
        }
    });
}

void DrumKitEditorContent::exportKitAsSFZ(const juce::File& destination)
{
    juce::String sfzContent;

    sfzContent += "// Drum Kit: " + kitNameEditor.getText() + "\n";
    sfzContent += "// Created with OTTO DrumKit Editor\n\n";

    for (int i = 0; i < INIConfig::LayoutConstants::drumKitEditorPadCount; ++i)
    {
        if (padData[i].sampleFile.existsAsFile())
        {
            const auto& params = padData[i].parameters;

            sfzContent += "<region>\n";
            sfzContent += "sample=" + padData[i].sampleFile.getFullPathName() + "\n";
            sfzContent += "key=" + juce::String(INIConfig::LayoutConstants::sfzBaseMidiNote + i) + "\n";

            if (params.startPosition > 0.0f)
                sfzContent += "offset=" + juce::String(juce::roundToInt(params.startPosition * INIConfig::LayoutConstants::sfzOffsetMultiplier)) + "\n";

            if (params.pitchShift != 0.0f)
                sfzContent += "pitch_keycenter=" + juce::String(INIConfig::LayoutConstants::sfzPitchCenterBase + juce::roundToInt(params.pitchShift)) + "\n";

            if (params.filterType > 0)
            {
                juce::String filterTypeStr = params.filterType == 1 ? "lpf_2p" :
                                           params.filterType == 2 ? "hpf_2p" : "bpf_2p";
                sfzContent += "fil_type=" + filterTypeStr + "\n";
                sfzContent += "cutoff=" + juce::String(params.filterFrequency) + "\n";
                sfzContent += "resonance=" + juce::String(params.filterResonance) + "\n";
            }

            if (padData[i].velocityCurveType != 0 || padData[i].velocityCurve != 1.0f)
            {
                sfzContent += "amp_velcurve_" + juce::String(padData[i].velocityCurveType) + "=" +
                             juce::String(padData[i].velocityCurve) + "\n";
            }

            if (params.chokeGroup > 0)
            {
                sfzContent += "group=" + juce::String(params.chokeGroup) + "\n";
                sfzContent += "off_by=" + juce::String(params.chokeGroup) + "\n";
            }

            sfzContent += "\n";
        }
    }

    destination.replaceWithText(sfzContent);
}

void DrumKitEditorContent::importKitFromSFZ(const juce::File& source)
{
    newKit();

    auto content = source.loadFileAsString();
    auto lines = juce::StringArray::fromLines(content);
    int currentPad = 0;
    PadSampleData currentData;
    bool inRegion = false;

    for (const auto& line : lines)
    {
        auto trimmed = line.trim();

        if (trimmed.startsWith("<region>"))
        {
            inRegion = true;
            currentData = PadSampleData();
        }
        else if (inRegion && trimmed.contains("="))
        {
            auto key = trimmed.upToFirstOccurrenceOf("=", false, false).trim();
            auto value = trimmed.fromFirstOccurrenceOf("=", false, false).trim();

            if (key == "sample")
            {
                juce::File sampleFile(value);
                if (sampleFile.existsAsFile())
                {
                    currentData.sampleFile = sampleFile;
                }
            }
            else if (key == "key")
            {
                int midiNote = value.getIntValue();
                currentPad = juce::jlimit(0, INIConfig::LayoutConstants::drumKitEditorPadCount - 1, midiNote - INIConfig::LayoutConstants::sfzBaseMidiNote);
            }
            else if (key == "pitch_keycenter")
            {
                currentData.parameters.pitchShift = value.getFloatValue() - INIConfig::LayoutConstants::sfzPitchCenterBase;
            }
            else if (key == "cutoff")
            {
                currentData.parameters.filterFrequency = value.getFloatValue();
            }
            else if (key == "resonance")
            {
                currentData.parameters.filterResonance = value.getFloatValue();
            }
        }
        else if (inRegion && trimmed.isEmpty() && currentData.sampleFile.existsAsFile())
        {
            padData[currentPad] = currentData;
            padEditors[currentPad]->setSample(currentData.sampleFile);
            inRegion = false;
        }
    }

    kitNameEditor.setText(source.getFileNameWithoutExtension());
}
