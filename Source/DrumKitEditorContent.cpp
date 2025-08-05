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
    g.fillAll(juce::Colour(0xFF2A2A2A));

    if (thumbnail->getTotalLength() > 0.0)
    {
        g.setColour(juce::Colour(0xFF7A7A7A));
        thumbnail->drawChannels(g, getLocalBounds(), 0.0, thumbnail->getTotalLength(), INIConfig::LayoutConstants::waveformDrawAlpha);

        g.setColour(juce::Colours::white.withAlpha(INIConfig::LayoutConstants::waveformLineAlpha));
        int startX = getXFromPosition(startPosition);
        int endX = getXFromPosition(endPosition);
        g.fillRect(startX, 0, endX - startX, getHeight());

        g.setColour(juce::Colours::green);
        g.drawVerticalLine(startX, 0, getHeight());
        g.setColour(juce::Colours::red);
        g.drawVerticalLine(endX, 0, getHeight());
    }
    else
    {
        g.setColour(juce::Colours::grey);
        g.drawText("Drop audio file here", getLocalBounds(), juce::Justification::centred);
    }

    g.setColour(juce::Colour(0xFF3A3A3A));
    g.drawRect(getLocalBounds());
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
    g.fillAll(juce::Colour(0xFF2A2A2A));

    g.setColour(juce::Colour(0xFF3A3A3A));
    for (int i = 1; i < INIConfig::LayoutConstants::drumKitButtonColumns; ++i)
    {
        float x = getWidth() * i / static_cast<float>(INIConfig::LayoutConstants::drumKitButtonColumns);
        float y = getHeight() * i / static_cast<float>(INIConfig::LayoutConstants::drumKitButtonColumns);
        g.drawVerticalLine(juce::roundToInt(x), 0, getHeight());
        g.drawHorizontalLine(juce::roundToInt(y), 0, getWidth());
    }

    g.setColour(juce::Colours::cyan);
    g.strokePath(curvePath, juce::PathStrokeType(INIConfig::LayoutConstants::velocityEditorLineWidth));

    g.setColour(juce::Colour(0xFF3A3A3A));
    g.drawRect(getLocalBounds());
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
    juce::Colour bgColor = isSelectedPad ? juce::Colour(0xFF5A5A5A) :
                          isHovering ? juce::Colour(0xFF4A4A4A) :
                          juce::Colour(0xFF3A3A3A);

    g.fillAll(bgColor);

    g.setColour(juce::Colours::white);
    g.setFont(static_cast<float>(INIConfig::LayoutConstants::padEditorTextSize));
    g.drawText(juce::String(padNumber + 1), getLocalBounds().reduced(INIConfig::LayoutConstants::drumKitSpacing, INIConfig::LayoutConstants::drumKitSpacing),
               juce::Justification::topLeft);

    if (hasSample())
    {
        g.setFont(INIConfig::LayoutConstants::padEditorSmallTextSize);
        g.drawText(currentSampleFile.getFileNameWithoutExtension(),
                  getLocalBounds().reduced(INIConfig::LayoutConstants::drumKitSpacing, INIConfig::LayoutConstants::drumKitSpacing),
                  juce::Justification::bottomLeft);
    }
    else
    {
        g.setColour(juce::Colours::grey);
        g.setFont(INIConfig::LayoutConstants::padEditorSmallTextSize);
        g.drawText("Drop sample", getLocalBounds(), juce::Justification::centred);
    }

    g.setColour(juce::Colour(0xFF7A7A7A));
    g.drawRect(getLocalBounds(), isSelectedPad ? INIConfig::LayoutConstants::playerTabHighlightHeight : INIConfig::LayoutConstants::separatorThickness);
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
    startLabel.setText("Start:", juce::dontSendNotification);
    endLabel.setText("End:", juce::dontSendNotification);
    pitchLabel.setText("Pitch:", juce::dontSendNotification);
    filterLabel.setText("Filter:", juce::dontSendNotification);
    chokeLabel.setText("Choke:", juce::dontSendNotification);

    addAndMakeVisible(startLabel);
    addAndMakeVisible(endLabel);
    addAndMakeVisible(pitchLabel);
    addAndMakeVisible(filterLabel);
    addAndMakeVisible(chokeLabel);

    startSlider.setRange(0.0, 1.0, 0.001);
    startSlider.setValue(0.0);
    startSlider.addListener(this);
    addAndMakeVisible(startSlider);

    endSlider.setRange(0.0, 1.0, 0.001);
    endSlider.setValue(1.0);
    endSlider.addListener(this);
    addAndMakeVisible(endSlider);

    pitchSlider.setRange(-24.0, 24.0, 0.1);
    pitchSlider.setValue(0.0);
    pitchSlider.addListener(this);
    addAndMakeVisible(pitchSlider);

    filterFreqSlider.setRange(20.0, 20000.0);
    filterFreqSlider.setSkewFactorFromMidPoint(2000.0);
    filterFreqSlider.setValue(20000.0);
    filterFreqSlider.addListener(this);
    addAndMakeVisible(filterFreqSlider);

    filterResSlider.setRange(0.1, 10.0);
    filterResSlider.setValue(1.0);
    filterResSlider.addListener(this);
    addAndMakeVisible(filterResSlider);

    filterTypeCombo.addItem("Off", 1);
    filterTypeCombo.addItem("Low Pass", 2);
    filterTypeCombo.addItem("High Pass", 3);
    filterTypeCombo.addItem("Band Pass", 4);
    filterTypeCombo.setSelectedId(1);
    filterTypeCombo.addListener(this);
    addAndMakeVisible(filterTypeCombo);

    for (int i = 0; i <= INIConfig::LayoutConstants::drumKitEditorChokeGroupCount; ++i)
    {
        chokeGroupCombo.addItem(i == 0 ? "None" : juce::String(i), i + 1);
    }
    chokeGroupCombo.setSelectedId(1);
    chokeGroupCombo.addListener(this);
    addAndMakeVisible(chokeGroupCombo);

    reverseButton.setButtonText("Reverse");
    reverseButton.onClick = [this]() { updateParametersAndNotify(); };
    addAndMakeVisible(reverseButton);

    normalizeButton.setButtonText("Normalize");
    normalizeButton.onClick = [this]() { updateParametersAndNotify(); };
    addAndMakeVisible(normalizeButton);
}

void SampleEditControls::resized()
{
    auto bounds = getLocalBounds();
    const int labelWidth = INIConfig::LayoutConstants::audioTabLabelWidth / INIConfig::LayoutConstants::sampleEditControlsLabelWidthDivisor;
    const int rowHeight = INIConfig::LayoutConstants::dialogRowHeight;
    const int spacing = INIConfig::LayoutConstants::drumKitSpacing;

    auto row = bounds.removeFromTop(rowHeight);
    startLabel.setBounds(row.removeFromLeft(labelWidth));
    startSlider.setBounds(row.reduced(spacing, 0));

    bounds.removeFromTop(spacing);
    row = bounds.removeFromTop(rowHeight);
    endLabel.setBounds(row.removeFromLeft(labelWidth));
    endSlider.setBounds(row.reduced(spacing, 0));

    bounds.removeFromTop(spacing);
    row = bounds.removeFromTop(rowHeight);
    pitchLabel.setBounds(row.removeFromLeft(labelWidth));
    pitchSlider.setBounds(row.reduced(spacing, 0));

    bounds.removeFromTop(spacing);
    row = bounds.removeFromTop(rowHeight);
    filterLabel.setBounds(row.removeFromLeft(labelWidth));
    filterTypeCombo.setBounds(row.removeFromLeft(INIConfig::LayoutConstants::dialogButtonWidth));
    row.removeFromLeft(spacing);
    filterFreqSlider.setBounds(row.removeFromLeft(row.getWidth() / INIConfig::LayoutConstants::sampleEditControlsSliderWidthDivisor));
    row.removeFromLeft(spacing);
    filterResSlider.setBounds(row);

    bounds.removeFromTop(spacing);
    row = bounds.removeFromTop(rowHeight);
    chokeLabel.setBounds(row.removeFromLeft(labelWidth));
    chokeGroupCombo.setBounds(row.removeFromLeft(INIConfig::LayoutConstants::dialogButtonWidth));

    bounds.removeFromTop(spacing);
    row = bounds.removeFromTop(rowHeight);
    reverseButton.setBounds(row.removeFromLeft(row.getWidth() / INIConfig::LayoutConstants::sampleEditControlsButtonWidthDivisor - spacing/INIConfig::LayoutConstants::sampleEditControlsButtonSpacingDivisor));
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

    addAndMakeVisible(newKitButton);
    addAndMakeVisible(saveKitButton);
    addAndMakeVisible(loadKitButton);

    newKitButton.addListener(this);
    saveKitButton.addListener(this);
    loadKitButton.addListener(this);

    kitNameLabel.setText("Kit Name:", juce::dontSendNotification);
    addAndMakeVisible(kitNameLabel);

    kitNameEditor.setText("New Kit");
    addAndMakeVisible(kitNameEditor);
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
    const int margin = layoutManager.scaled(INIConfig::LayoutConstants::drumKitSectionMargin);
    bounds = bounds.reduced(margin);

    auto topBar = bounds.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::rightSectionButtonHeight + INIConfig::LayoutConstants::drumKitSpacing));
    kitNameLabel.setBounds(topBar.removeFromLeft(layoutManager.scaled(INIConfig::LayoutConstants::rightSectionLabelWidth)));
    topBar.removeFromLeft(margin);
    kitNameEditor.setBounds(topBar.removeFromLeft(layoutManager.scaled(INIConfig::LayoutConstants::themeTabComboWidth)));
    topBar.removeFromLeft(margin * INIConfig::LayoutConstants::drumKitEditorTopBarSpacingMultiplier);

    const int buttonWidth = layoutManager.scaled(INIConfig::LayoutConstants::rightSectionButtonWidth);
    newKitButton.setBounds(topBar.removeFromLeft(buttonWidth));
    topBar.removeFromLeft(margin);
    saveKitButton.setBounds(topBar.removeFromLeft(buttonWidth));
    topBar.removeFromLeft(margin);
    loadKitButton.setBounds(topBar.removeFromLeft(buttonWidth));

    bounds.removeFromTop(margin);

    auto leftPanel = bounds.removeFromLeft(layoutManager.scaled(INIConfig::LayoutConstants::drumKitSectionMargin * INIConfig::LayoutConstants::dialogButtonWidth / INIConfig::LayoutConstants::rightSectionButtonHeight));
    bounds.removeFromLeft(margin);

    sampleBrowser->setBounds(leftPanel);

    auto rightArea = bounds;

    const int padSize = layoutManager.scaled(INIConfig::LayoutConstants::drumButtonWidth - INIConfig::LayoutConstants::drumKitSectionMargin);
    const int padSpacing = layoutManager.scaled(INIConfig::LayoutConstants::drumKitSpacing);

    for (int row = 0; row < INIConfig::LayoutConstants::drumKitEditorPadRows; ++row)
    {
        for (int col = 0; col < INIConfig::LayoutConstants::drumKitEditorPadColumns; ++col)
        {
            int index = row * INIConfig::LayoutConstants::drumKitEditorPadColumns + col;
            if (index < padEditors.size())
            {
                int x = col * (padSize + padSpacing);
                int y = row * (padSize + padSpacing);
                padEditors[index]->setBounds(x, y, padSize, padSize);
            }
        }
    }

    rightArea.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::mixerStripFaderTopMargin * INIConfig::LayoutConstants::drumKitSectionMargin));
    rightArea.removeFromTop(margin);

    auto bottomArea = rightArea;
    auto waveformArea = bottomArea.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::mixerStripTitleHeight * INIConfig::LayoutConstants::playerTabHeight / INIConfig::LayoutConstants::drumKitButtonHeight));
    waveformDisplay->setBounds(waveformArea);

    bottomArea.removeFromTop(margin);

    auto controlsArea = bottomArea.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::themeTabComboWidth));
    auto editControlsArea = controlsArea.removeFromLeft(controlsArea.getWidth() * INIConfig::LayoutConstants::drumKitEditorEditControlsWidthRatio / INIConfig::LayoutConstants::drumKitEditorControlsWidthRatio);
    controlsArea.removeFromLeft(margin);

    sampleEditControls->setBounds(editControlsArea);
    velocityCurveEditor->setBounds(controlsArea);
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
