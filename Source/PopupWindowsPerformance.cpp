#include "PopupWindowsPerformance.h"
#include "INIConfig.h"
#include "PerformanceOptimizations.h"

PerformanceTab::PerformanceTab(ColorScheme& colorScheme,
                              ResponsiveLayoutManager& layoutManager,
                              FontManager& fontManager)
    : colorScheme(colorScheme), layoutManager(layoutManager),
      fontManager(fontManager),
      cpuMeter(cpuUsage), memoryMeter(memoryUsage) {
    setupComponents();
    startTimerHz(INIConfig::LayoutConstants::perfTabTimerHz);
}

void PerformanceTab::setupComponents() {
    addAndMakeVisible(cpuLabel);
    addAndMakeVisible(memoryLabel);
    addAndMakeVisible(voicesLabel);
    addAndMakeVisible(latencyLabel);

    cpuLabel.setText("CPU Usage:", juce::dontSendNotification);
    memoryLabel.setText("Memory Usage:", juce::dontSendNotification);
    voicesLabel.setText("Active Voices:", juce::dontSendNotification);
    latencyLabel.setText("Latency:", juce::dontSendNotification);

    for (auto* label : {&cpuLabel, &memoryLabel, &voicesLabel, &latencyLabel}) {
        label->setColour(juce::Label::textColourId, colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
        label->setFont(fontManager.getFont(FontManager::FontRole::Body));
    }

    addAndMakeVisible(cpuValueLabel);
    addAndMakeVisible(memoryValueLabel);
    addAndMakeVisible(voicesValueLabel);
    addAndMakeVisible(latencyValueLabel);

    for (auto* label : {&cpuValueLabel, &memoryValueLabel, &voicesValueLabel, &latencyValueLabel}) {
        label->setColour(juce::Label::textColourId, colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
        label->setJustificationType(juce::Justification::right);
    }

    addAndMakeVisible(cpuMeter);
    addAndMakeVisible(memoryMeter);

    cpuMeter.setColour(juce::ProgressBar::backgroundColourId,
                       colorScheme.getColor(ColorScheme::ColorRole::ControlBackground));
    cpuMeter.setColour(juce::ProgressBar::foregroundColourId,
                       colorScheme.getColor(ColorScheme::ColorRole::Accent));

    memoryMeter.setColour(juce::ProgressBar::backgroundColourId,
                          colorScheme.getColor(ColorScheme::ColorRole::ControlBackground));
    memoryMeter.setColour(juce::ProgressBar::foregroundColourId,
                          colorScheme.getColor(ColorScheme::ColorRole::Accent));

    addAndMakeVisible(threadCountSlider);
    addAndMakeVisible(maxVoicesSlider);
    addAndMakeVisible(qualitySlider);

    threadCountSlider.setRange(1, juce::SystemStats::getNumCpus(), 1);
    threadCountSlider.setValue(juce::jmin(4, juce::SystemStats::getNumCpus()));
    threadCountSlider.setTextValueSuffix(" threads");
    threadCountSlider.addListener(this);

    maxVoicesSlider.setRange(8, 128, 1);
    maxVoicesSlider.setValue(32);
    maxVoicesSlider.setTextValueSuffix(" voices");
    maxVoicesSlider.addListener(this);

    qualitySlider.setRange(0, 2, 1);
    qualitySlider.setValue(1);
    qualitySlider.addListener(this);

    for (auto* slider : {&threadCountSlider, &maxVoicesSlider, &qualitySlider}) {
        slider->setColour(juce::Slider::backgroundColourId,
                         colorScheme.getColor(ColorScheme::ColorRole::ControlBackground));
        slider->setColour(juce::Slider::trackColourId,
                         colorScheme.getColor(ColorScheme::ColorRole::Accent));
        slider->setColour(juce::Slider::thumbColourId,
                         colorScheme.getColor(ColorScheme::ColorRole::SliderThumb));
    }

    addAndMakeVisible(threadCountLabel);
    addAndMakeVisible(maxVoicesLabel);
    addAndMakeVisible(qualityLabel);

    threadCountLabel.setText("Thread Count:", juce::dontSendNotification);
    maxVoicesLabel.setText("Max Voices:", juce::dontSendNotification);
    qualityLabel.setText("Quality:", juce::dontSendNotification);

    for (auto* label : {&threadCountLabel, &maxVoicesLabel, &qualityLabel}) {
        label->setColour(juce::Label::textColourId, colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    }

    addAndMakeVisible(multiThreadToggle);
    addAndMakeVisible(gpuAccelToggle);

    multiThreadToggle.setButtonText("Multi-threaded Processing");
    gpuAccelToggle.setButtonText("GPU Acceleration (if available)");

    for (auto* toggle : {&multiThreadToggle, &gpuAccelToggle}) {
        toggle->setColour(juce::ToggleButton::textColourId,
                         colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    }

    addAndMakeVisible(resetMetersButton);
    addAndMakeVisible(optimizeButton);

    resetMetersButton.setButtonText("Reset Meters");
    optimizeButton.setButtonText("Auto-Optimize");

    for (auto* button : {&resetMetersButton, &optimizeButton}) {
        button->addListener(this);
        button->setColour(juce::TextButton::buttonColourId,
                         colorScheme.getColor(ColorScheme::ColorRole::ButtonBackground));
        button->setColour(juce::TextButton::textColourOffId,
                         colorScheme.getColor(ColorScheme::ColorRole::ButtonText));
    }

    updateMeters();
}

void PerformanceTab::paint(juce::Graphics& g) {
    g.fillAll(colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground));

    auto qualityBounds = qualitySlider.getBounds();

    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
    g.setFont(fontManager.getFont(FontManager::FontRole::Body, layoutManager.scaled(INIConfig::LayoutConstants::backupTabFontSizeSmall)));

    g.drawText("Low", qualityBounds.getX(), qualityBounds.getBottom() + INIConfig::LayoutConstants::perfTabQualityLabelY,
              INIConfig::LayoutConstants::perfTabQualityLabelWidth, INIConfig::LayoutConstants::perfTabQualityLabelHeight, juce::Justification::left);
    g.drawText("Med", qualityBounds.getCentreX() - INIConfig::LayoutConstants::perfTabQualityMidOffset, qualityBounds.getBottom() + INIConfig::LayoutConstants::perfTabQualityLabelY,
              INIConfig::LayoutConstants::perfTabQualityLabelWidth - INIConfig::LayoutConstants::defaultSpacing, INIConfig::LayoutConstants::perfTabQualityLabelHeight, juce::Justification::centred);
    g.drawText("High", qualityBounds.getRight() - INIConfig::LayoutConstants::perfTabQualityLabelWidth, qualityBounds.getBottom() + INIConfig::LayoutConstants::perfTabQualityLabelY,
              INIConfig::LayoutConstants::perfTabQualityLabelWidth, INIConfig::LayoutConstants::perfTabQualityLabelHeight, juce::Justification::right);
}

void PerformanceTab::resized() {
    auto bounds = getLocalBounds().reduced(layoutManager.scaled(INIConfig::LayoutConstants::defaultMargin));

    auto rowHeight = layoutManager.scaled(INIConfig::LayoutConstants::dialogRowHeight);
    auto labelWidth = layoutManager.scaled(INIConfig::LayoutConstants::audioTabLabelWidth);
    auto valueWidth = layoutManager.scaled(INIConfig::LayoutConstants::perfTabValueWidth);
    auto spacing = layoutManager.scaled(INIConfig::LayoutConstants::defaultSpacing);

    auto row = bounds.removeFromTop(rowHeight);
    cpuLabel.setBounds(row.removeFromLeft(labelWidth));
    row.removeFromLeft(spacing);
    cpuMeter.setBounds(row.removeFromLeft(layoutManager.scaled(INIConfig::LayoutConstants::perfTabMeterWidth)));
    row.removeFromLeft(spacing);
    cpuValueLabel.setBounds(row.removeFromLeft(valueWidth));

    bounds.removeFromTop(spacing);

    row = bounds.removeFromTop(rowHeight);
    memoryLabel.setBounds(row.removeFromLeft(labelWidth));
    row.removeFromLeft(spacing);
    memoryMeter.setBounds(row.removeFromLeft(layoutManager.scaled(INIConfig::LayoutConstants::perfTabMeterWidth)));
    row.removeFromLeft(spacing);
    memoryValueLabel.setBounds(row.removeFromLeft(valueWidth));

    bounds.removeFromTop(spacing);

    row = bounds.removeFromTop(rowHeight);
    voicesLabel.setBounds(row.removeFromLeft(labelWidth));
    row.removeFromLeft(spacing);
    voicesValueLabel.setBounds(row.removeFromLeft(valueWidth));

    bounds.removeFromTop(spacing);

    row = bounds.removeFromTop(rowHeight);
    latencyLabel.setBounds(row.removeFromLeft(labelWidth));
    row.removeFromLeft(spacing);
    latencyValueLabel.setBounds(row.removeFromLeft(valueWidth));

    bounds.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::dialogRowHeight));

    row = bounds.removeFromTop(rowHeight + layoutManager.scaled(INIConfig::LayoutConstants::perfTabSliderExtraHeight));
    threadCountLabel.setBounds(row.removeFromLeft(labelWidth));
    row.removeFromLeft(spacing);
    threadCountSlider.setBounds(row.removeFromLeft(layoutManager.scaled(INIConfig::LayoutConstants::perfTabSliderWidth)));

    bounds.removeFromTop(spacing);

    row = bounds.removeFromTop(rowHeight + layoutManager.scaled(INIConfig::LayoutConstants::perfTabSliderExtraHeight));
    maxVoicesLabel.setBounds(row.removeFromLeft(labelWidth));
    row.removeFromLeft(spacing);
    maxVoicesSlider.setBounds(row.removeFromLeft(layoutManager.scaled(INIConfig::LayoutConstants::perfTabSliderWidth)));

    bounds.removeFromTop(spacing);

    row = bounds.removeFromTop(rowHeight + layoutManager.scaled(INIConfig::LayoutConstants::perfTabSliderExtraHeight));
    qualityLabel.setBounds(row.removeFromLeft(labelWidth));
    row.removeFromLeft(spacing);
    qualitySlider.setBounds(row.removeFromLeft(layoutManager.scaled(INIConfig::LayoutConstants::perfTabSliderWidth)));

    bounds.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::dialogRowHeight));

    multiThreadToggle.setBounds(bounds.removeFromTop(rowHeight));
    bounds.removeFromTop(spacing);
    gpuAccelToggle.setBounds(bounds.removeFromTop(rowHeight));

    bounds.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::dialogRowHeight));

    row = bounds.removeFromTop(rowHeight);
    auto buttonWidth = layoutManager.scaled(INIConfig::LayoutConstants::perfTabButtonWidth);
    resetMetersButton.setBounds(row.removeFromLeft(buttonWidth));
    row.removeFromLeft(spacing);
    optimizeButton.setBounds(row.removeFromLeft(buttonWidth));
}

void PerformanceTab::timerCallback() {
    updateMeters();
}

void PerformanceTab::updateMeters() {
    static juce::Random random;
    auto& stringCache = StringCache::getInstance();
    
    cpuUsage = juce::jlimit(0.0, 1.0, cpuUsage + (random.nextFloat() - 0.5f) * 0.1);
    int cpuPercent = static_cast<int>(cpuUsage * 100);
    cpuValueLabel.setText(stringCache.getPercentageString(cpuPercent), juce::dontSendNotification);

    #if JUCE_WINDOWS || JUCE_MAC || JUCE_LINUX
        memoryUsage = 0.3 + random.nextFloat() * 0.4;
    #else
        memoryUsage = 0.5;
    #endif

    int memoryPercent = static_cast<int>(memoryUsage * 100);
    memoryValueLabel.setText(stringCache.getPercentageString(memoryPercent), juce::dontSendNotification);

    activeVoices = random.nextInt(33);
    voicesValueLabel.setText(juce::String(activeVoices), juce::dontSendNotification);

    latency = 5.3 + random.nextFloat() * 2.0;
    latencyValueLabel.setText(stringCache.getLatencyString(latency, 1), juce::dontSendNotification);

    cpuMeter.repaint();
    memoryMeter.repaint();
}

void PerformanceTab::sliderValueChanged(juce::Slider* slider) {
    if (slider == &qualitySlider) {
        int quality = static_cast<int>(qualitySlider.getValue());
        switch (quality) {
            case 0:
                maxVoicesSlider.setValue(16);
                multiThreadToggle.setToggleState(false, juce::sendNotification);
                break;
            case 1:
                maxVoicesSlider.setValue(32);
                multiThreadToggle.setToggleState(true, juce::sendNotification);
                break;
            case 2:
                maxVoicesSlider.setValue(64);
                multiThreadToggle.setToggleState(true, juce::sendNotification);
                gpuAccelToggle.setToggleState(true, juce::sendNotification);
                break;
        }
    }
}

void PerformanceTab::buttonClicked(juce::Button* button) {
    if (button == &resetMetersButton) {
        cpuUsage = 0.0;
        memoryUsage = 0.0;
        updateMeters();
    }
    else if (button == &optimizeButton) {
        optimizeSettings();
    }
}

void PerformanceTab::optimizeSettings() {
    auto numCores = juce::SystemStats::getNumCpus();

    if (numCores >= 8) {
        qualitySlider.setValue(2);
        threadCountSlider.setValue(juce::jmin(8, numCores));
        maxVoicesSlider.setValue(64);
    }
    else if (numCores >= 4) {
        qualitySlider.setValue(1);
        threadCountSlider.setValue(juce::jmin(4, numCores));
        maxVoicesSlider.setValue(32);
    }
    else {
        qualitySlider.setValue(0);
        threadCountSlider.setValue(2);
        maxVoicesSlider.setValue(16);
    }

    multiThreadToggle.setToggleState(numCores > 2, juce::sendNotification);
}

void PerformanceTab::savePerformanceSettings(ComponentState& state) {
    state.sliderValues["threadCount"] = static_cast<float>(threadCountSlider.getValue());
    state.sliderValues["maxVoices"] = static_cast<float>(maxVoicesSlider.getValue());
    state.sliderValues["quality"] = static_cast<float>(qualitySlider.getValue());
    state.toggleStates[100] = multiThreadToggle.getToggleState();
    state.toggleStates[101] = gpuAccelToggle.getToggleState();
}

void PerformanceTab::loadPerformanceSettings(const ComponentState& state) {
    auto tcIt = state.sliderValues.find("threadCount");
    if (tcIt != state.sliderValues.end()) {
        threadCountSlider.setValue(tcIt->second);
    }

    auto mvIt = state.sliderValues.find("maxVoices");
    if (mvIt != state.sliderValues.end()) {
        maxVoicesSlider.setValue(mvIt->second);
    }

    auto qIt = state.sliderValues.find("quality");
    if (qIt != state.sliderValues.end()) {
        qualitySlider.setValue(qIt->second);
    }

    auto mtIt = state.toggleStates.find(100);
    if (mtIt != state.toggleStates.end()) {
        multiThreadToggle.setToggleState(mtIt->second, juce::dontSendNotification);
    }

    auto gpuIt = state.toggleStates.find(101);
    if (gpuIt != state.toggleStates.end()) {
        gpuAccelToggle.setToggleState(gpuIt->second, juce::dontSendNotification);
    }
}
