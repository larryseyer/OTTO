#include "AIAssistantPanel.h"
#include "ErrorHandling.h"
#include "INIConfig.h"

AIAssistantPanel::AIAssistantPanel(AutoMixAssistant& mixAssistant,
                                   PatternSuggestionEngine& patternEngine,
                                   Mixer& mixer,
                                   PatternManager& patternManager)
    : autoMixAssistant(mixAssistant)
    , patternSuggestionEngine(patternEngine)
    , mixer(mixer)
    , patternManager(patternManager)
    , patternSuggestionsView("", this) {
    setupUI();
    startTimer(100);
}

AIAssistantPanel::~AIAssistantPanel() {
    stopTimer();
}

void AIAssistantPanel::paint(juce::Graphics& g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    if (currentMode == AIMode::MixAssistant) {
        auto vizArea = getLocalBounds()
            .removeFromBottom(150)
            .reduced(INIConfig::LayoutConstants::defaultMargin);
        drawMixVisualization(g, vizArea);
    }
}

void AIAssistantPanel::resized() {
    auto bounds = getLocalBounds().reduced(INIConfig::LayoutConstants::defaultMargin);

    auto topSection = bounds.removeFromTop(40);
    titleLabel.setBounds(topSection.removeFromLeft(200));
    aiModeSelector.setBounds(topSection.removeFromRight(200));

    bounds.removeFromTop(INIConfig::LayoutConstants::defaultSpacing);

    switch (currentMode) {
        case AIMode::MixAssistant: {
            auto suggestionArea = bounds.removeFromTop(100);
            mixSuggestionView.setBounds(suggestionArea);

            bounds.removeFromTop(INIConfig::LayoutConstants::defaultSpacing);

            auto controlArea = bounds.removeFromTop(60);
            auto sliderArea = controlArea.removeFromLeft(250);
            blendLabel.setBounds(sliderArea.removeFromLeft(80));
            blendSlider.setBounds(sliderArea);

            applyButton.setBounds(controlArea.removeFromRight(100).reduced(5));
            break;
        }

        case AIMode::PatternSuggestions: {
            auto genreArea = bounds.removeFromTop(30);
            genreSelector.setBounds(genreArea.removeFromRight(200));

            bounds.removeFromTop(INIConfig::LayoutConstants::defaultSpacing);

            auto complexityArea = bounds.removeFromTop(40);
            complexityLabel.setBounds(complexityArea.removeFromLeft(100));
            complexitySlider.setBounds(complexityArea.removeFromLeft(200));

            bounds.removeFromTop(INIConfig::LayoutConstants::defaultSpacing);

            patternSuggestionsView.setBounds(bounds.removeFromTop(200));

            bounds.removeFromTop(INIConfig::LayoutConstants::defaultSpacing);
            applyButton.setBounds(bounds.removeFromTop(30).removeFromRight(100));
            break;
        }

        case AIMode::PerformanceMode: {
            performanceDisplay.setBounds(bounds.removeFromTop(150));

            bounds.removeFromTop(INIConfig::LayoutConstants::defaultSpacing);

            auto toggleArea = bounds.removeFromTop(30);
            learningToggle.setBounds(toggleArea.removeFromLeft(200));
            break;
        }
    }
}

void AIAssistantPanel::timerCallback() {
    switch (currentMode) {
        case AIMode::MixAssistant:
            if (isVisible()) {
                analyzeMix();
            }
            break;

        case AIMode::PerformanceMode:
            updatePerformanceStats();
            break;

        default:
            break;
    }
}

void AIAssistantPanel::setupUI() {
    titleLabel.setText("AI Assistant", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(INIConfig::LayoutConstants::fontSizeTitle));
    titleLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(titleLabel);

    aiModeSelector.addItem("Mix Assistant", 1);
    aiModeSelector.addItem("Pattern Suggestions", 2);
    aiModeSelector.addItem("Performance Mode", 3);
    aiModeSelector.setSelectedId(1);
    aiModeSelector.onChange = [this] { modeChanged(); };
    addAndMakeVisible(aiModeSelector);

    mixSuggestionView.setMultiLine(true);
    mixSuggestionView.setReadOnly(true);
    mixSuggestionView.setScrollbarsShown(true);
    addChildComponent(mixSuggestionView);

    blendLabel.setText("Blend Amount:", juce::dontSendNotification);
    addChildComponent(blendLabel);

    blendSlider.setRange(0.0, 1.0, 0.01);
    blendSlider.setValue(0.5);
    blendSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    blendSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 50, 20);
    addChildComponent(blendSlider);

    genreSelector.addItem("Rock", 1);
    genreSelector.addItem("Electronic", 2);
    genreSelector.addItem("Jazz", 3);
    genreSelector.addItem("Hip Hop", 4);
    genreSelector.addItem("Latin", 5);
    genreSelector.setSelectedId(1);
    genreSelector.onChange = [this] { generatePatternSuggestions(); };
    addChildComponent(genreSelector);

    complexityLabel.setText("Complexity:", juce::dontSendNotification);
    addChildComponent(complexityLabel);

    complexitySlider.setRange(1.0, 10.0, 1.0);
    complexitySlider.setValue(5.0);
    complexitySlider.setSliderStyle(juce::Slider::LinearHorizontal);
    complexitySlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 40, 20);
    complexitySlider.onValueChange = [this] { generatePatternSuggestions(); };
    addChildComponent(complexitySlider);

    addChildComponent(patternSuggestionsView);

    performanceDisplay.setMultiLine(true);
    performanceDisplay.setReadOnly(true);
    performanceDisplay.setScrollbarsShown(true);
    addChildComponent(performanceDisplay);

    learningToggle.setButtonText("Enable Learning");
    learningToggle.onClick = [this] { collectPerformanceData(); };
    addChildComponent(learningToggle);

    applyButton.setButtonText("Apply");
    applyButton.onClick = [this] { applySuggestion(); };
    addChildComponent(applyButton);

    modeChanged();
}

void AIAssistantPanel::modeChanged() {
    currentMode = static_cast<AIMode>(aiModeSelector.getSelectedId() - 1);

    mixSuggestionView.setVisible(false);
    blendLabel.setVisible(false);
    blendSlider.setVisible(false);
    genreSelector.setVisible(false);
    complexityLabel.setVisible(false);
    complexitySlider.setVisible(false);
    patternSuggestionsView.setVisible(false);
    performanceDisplay.setVisible(false);
    learningToggle.setVisible(false);
    applyButton.setVisible(false);

    switch (currentMode) {
        case AIMode::MixAssistant:
            mixSuggestionView.setVisible(true);
            blendLabel.setVisible(true);
            blendSlider.setVisible(true);
            applyButton.setVisible(true);
            analyzeMix();
            break;

        case AIMode::PatternSuggestions:
            genreSelector.setVisible(true);
            complexityLabel.setVisible(true);
            complexitySlider.setVisible(true);
            patternSuggestionsView.setVisible(true);
            applyButton.setVisible(true);
            generatePatternSuggestions();
            break;

        case AIMode::PerformanceMode:
            performanceDisplay.setVisible(true);
            learningToggle.setVisible(true);
            updatePerformanceStats();
            break;
    }

    resized();
    repaint();
}

void AIAssistantPanel::analyzeMix() {
    try {
        currentMixSuggestion = autoMixAssistant.analyzeMix();
        updateMixSuggestionView();
    } catch (const std::exception& e) {
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "Mix analysis failed: " + juce::String(e.what()), "AIAssistantPanel");
    }
}

void AIAssistantPanel::generatePatternSuggestions() {
    try {
        juce::String genre = genreSelector.getText();
        int complexity = static_cast<int>(complexitySlider.getValue());

        currentPatternSuggestions.clear();

        for (int i = 0; i < 5; ++i) {
            PatternSuggestionEngine::PatternSuggestion suggestion;
            suggestion.name = genre + " Pattern " + juce::String(i + 1) + " (Complexity: " + juce::String(complexity) + ")";
            currentPatternSuggestions.add(suggestion);
        }

        patternSuggestionsView.updateContent();
    } catch (const std::exception& e) {
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "Pattern suggestion generation failed: " + juce::String(e.what()), "AIAssistantPanel");
    }
}

void AIAssistantPanel::updatePerformanceStats() {
    juce::String stats;
    stats << "Performance Statistics\n";
    stats << "=====================\n\n";

    int activePatterns = 0;
    float cpuUsage = 0.0f;

    stats << "Active Patterns: " << activePatterns << "\n";
    stats << "CPU Usage: " << juce::String(cpuUsage, 1) << "%\n";
    stats << "Learned Patterns: " << learnedPatterns << "\n\n";

    auto mixBalance = autoMixAssistant.analyzeMixBalance();
    stats << "Mix Balance: " << juce::String(mixBalance * 100.0f, 1) << "%\n";

    performanceDisplay.setText(stats);
}

void AIAssistantPanel::applySuggestion() {
    switch (currentMode) {
        case AIMode::MixAssistant: {
            float blendAmount = static_cast<float>(blendSlider.getValue());
            autoMixAssistant.applyMixSuggestion(currentMixSuggestion, blendAmount);
            break;
        }

        case AIMode::PatternSuggestions: {
            int selectedRow = patternSuggestionsView.getSelectedRow();
            if (selectedRow >= 0 && selectedRow < currentPatternSuggestions.size()) {

            }
            break;
        }

        default:
            break;
    }
}

void AIAssistantPanel::updateMixSuggestionView() {
    juce::String text;
    text << "Mix Analysis Results\n";
    text << "===================\n\n";
    text << "Confidence: " << juce::String(currentMixSuggestion.confidence * 100.0f, 1) << "%\n\n";

    text << "Channel Adjustments:\n";
    for (int i = 0; i < 8; ++i) {
        auto currentVol = mixer.getChannelVolume(i);
        auto suggestedVol = currentMixSuggestion.channelVolumes[i];

        if (std::abs(currentVol - suggestedVol) > 0.01f) {
            text << "  Channel " << (i + 1) << ": ";
            text << "Volume " << juce::String(currentVol, 2) << " → " << juce::String(suggestedVol, 2);
            text << "\n";
        }
    }

    if (!currentMixSuggestion.description.isEmpty()) {
        text << "\n" << currentMixSuggestion.description;
    }

    mixSuggestionView.setText(text);
}

void AIAssistantPanel::drawMixVisualization(juce::Graphics& g, juce::Rectangle<int> area) {
    g.setColour(juce::Colours::darkgrey);
    g.fillRoundedRectangle(area.toFloat(), INIConfig::LayoutConstants::cornerRadiusMedium);

    g.setColour(juce::Colours::white);
    g.drawText("Mix Visualization", area.removeFromTop(20), juce::Justification::centred);

    auto meterArea = area.reduced(10);
    int channelWidth = meterArea.getWidth() / 8;

    for (int i = 0; i < 8; ++i) {
        auto channelArea = meterArea.removeFromLeft(channelWidth).reduced(2);

        auto levels = mixer.getChannelLevels(i);
        float level = (levels.left + levels.right) * 0.5f;

        g.setColour(juce::Colours::darkgreen);
        int meterHeight = static_cast<int>(channelArea.getHeight() * level);
        g.fillRect(channelArea.removeFromBottom(meterHeight));

        float suggestedLevel = currentMixSuggestion.channelVolumes[i];
        int suggestedY = channelArea.getBottom() - static_cast<int>(channelArea.getHeight() * suggestedLevel);

        g.setColour(juce::Colours::yellow);
        g.drawLine(static_cast<float>(channelArea.getX()), static_cast<float>(suggestedY),
                   static_cast<float>(channelArea.getRight()), static_cast<float>(suggestedY), 2.0f);
    }
}

void AIAssistantPanel::collectPerformanceData() {
    if (learningToggle.getToggleState()) {
        autoMixAssistant.learnFromUserAdjustments();
        learnedPatterns++;
    }
}

int AIAssistantPanel::getNumRows() {
    return currentPatternSuggestions.size();
}

void AIAssistantPanel::paintListBoxItem(int rowNumber, juce::Graphics& g,
                                       int width, int height,
                                       bool rowIsSelected) {
    if (rowNumber < currentPatternSuggestions.size()) {
        if (rowIsSelected) {
            g.fillAll(juce::Colours::lightblue);
        }

        g.setColour(juce::Colours::black);

        const auto& suggestion = currentPatternSuggestions[rowNumber];
        g.drawText(suggestion.name, 10, 0, width - 20, height, juce::Justification::centredLeft);
    }
}
