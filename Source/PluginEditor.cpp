#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "INIConfig.h"

class OTTOSplashOverlay : public juce::Component, private juce::Timer
{
public:
    OTTOSplashOverlay(const juce::Image& image, float displayTime,
                      std::function<void()> onComplete,
                      const juce::Colour& backgroundColor)
        : splashImage(image), totalTime(displayTime),
          completionCallback(onComplete), bgColor(backgroundColor)
    {
        setOpaque(true);
        startTimer(50);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(bgColor);

        g.setOpacity(opacity);

        auto bounds = getLocalBounds();
        int imgX = (bounds.getWidth() - splashImage.getWidth()) / 2;
        int imgY = (bounds.getHeight() - splashImage.getHeight()) / 2;

        g.drawImageAt(splashImage, imgX, imgY);
    }

    void timerCallback() override
    {
        elapsed += 0.05f;

        if (elapsed >= totalTime)
        {
            overallOpacity -= 0.1f;
            if (overallOpacity <= 0.0f)
            {
                stopTimer();
                setVisible(false);
                if (completionCallback)
                    completionCallback();
                if (auto* parent = getParentComponent())
                {
                    parent->removeChildComponent(this);
                }
                delete this;
            }
            else
            {
                setAlpha(overallOpacity);
            }
            repaint();
        }
    }

private:
    juce::Image splashImage;
    float totalTime;
    float elapsed = 0.0f;
    float opacity = 1.0f;
    float overallOpacity = 1.0f;
    std::function<void()> completionCallback;
    juce::Colour bgColor;
};

OTTOAudioProcessorEditor::OTTOAudioProcessorEditor(OTTOAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setOpaque(true);

    initializeManagers();

    dataManager->initializeDefaults(componentState);

    if (componentState.interfaceWidth <= 0) {
        componentState.interfaceWidth = INIConfig::Defaults::DEFAULT_INTERFACE_WIDTH;
    }
    if (componentState.interfaceHeight <= 0) {
        componentState.interfaceHeight = INIConfig::Defaults::DEFAULT_INTERFACE_HEIGHT;
    }

    if (dataManager) {
        if (!dataManager->allFilesExist()) {
            if (!dataManager->createAllRequiredFiles()) {

            } else {

            }
        }

        if (dataManager->allFilesExist()) {
            if (!dataManager->loadAllData(componentState)) {

            } else {


                if (componentState.interfaceWidth <= 0) {
                    componentState.interfaceWidth = INIConfig::Defaults::DEFAULT_INTERFACE_WIDTH;
                }
                if (componentState.interfaceHeight <= 0) {
                    componentState.interfaceHeight = INIConfig::Defaults::DEFAULT_INTERFACE_HEIGHT;
                }
            }
        } else {
            if (mainContent && mainContent->getLeftSection()) {
                mainContent->getLeftSection()->checkAndLoadDefaultPatterns(componentState);
            }
        }
    }

    int width = juce::jmax(100, componentState.interfaceWidth);
    int height = juce::jmax(100, componentState.interfaceHeight);
    setSize(width, height);

    if (layoutManager) {
        layoutManager->updateScale(width, height);
    }

    createComponents();
    setupCallbacks();

    if (dataManager && dataManager->allFilesExist()) {
        loadAllComponentStates();
        updateUIFromState();
    } else {
        if (mainContent && mainContent->getLeftSection()) {
            mainContent->getLeftSection()->checkAndLoadDefaultPatterns(componentState);
        }
    }

    resized();
    repaint();

    setResizable(true, true);

    const int minWidth = static_cast<int>(INIConfig::Defaults::DEFAULT_INTERFACE_WIDTH * INIConfig::Validation::MIN_INTERFACE_SCALE);
    const int minHeight = static_cast<int>(INIConfig::Defaults::DEFAULT_INTERFACE_HEIGHT * INIConfig::Validation::MIN_INTERFACE_SCALE);
    const int maxWidth = static_cast<int>(INIConfig::Defaults::DEFAULT_INTERFACE_WIDTH * INIConfig::Validation::MAX_INTERFACE_SCALE);
    const int maxHeight = static_cast<int>(INIConfig::Defaults::DEFAULT_INTERFACE_HEIGHT * INIConfig::Validation::MAX_INTERFACE_SCALE);

    setResizeLimits(minWidth, minHeight, maxWidth, maxHeight);

    setVisible(true);

    if (customLookAndFeel && customLookAndFeel->getSplashImage().isValid()) {
        float splashTime = componentState.globalSettings.splashScreenOnTime;
        if (splashTime <= 0.0f) {
            splashTime = INIConfig::UI::SPLASH_SCREEN_ON_TIME;
        }

        juce::Colour appBgColor = colorScheme->getBackgroundColor();

        auto* splash = new OTTOSplashOverlay(
            customLookAndFeel->getSplashImage(),
            splashTime,
            [this]() { repaint(); },
            appBgColor
        );

        splash->setBounds(getLocalBounds());
        addAndMakeVisible(splash);
        splash->toFront(false);


    }

    resized();
    repaint();

    startTimer(INIConfig::LayoutConstants::tapTempoDisplayMs / INIConfig::LayoutConstants::defaultMargin);
    isInitialized = true;
}

OTTOAudioProcessorEditor::~OTTOAudioProcessorEditor()
{
    stopTimer();

    if (colorScheme)
        colorScheme->removeListener(this);

    if (isInitialized && topBar && playerTabs && drumKitSection && dataManager)
    {
        try {
            saveEditorState();
        }
        catch (...) {

        }
    }

    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
    setLookAndFeel(nullptr);
}

void OTTOAudioProcessorEditor::initializeManagers()
{
    ResponsiveLayoutManager::LayoutConfig layoutConfig;
    layoutConfig.baseWidth = INIConfig::Defaults::DEFAULT_INTERFACE_WIDTH;
    layoutConfig.baseHeight = INIConfig::Defaults::DEFAULT_INTERFACE_HEIGHT;
    layoutConfig.aspectRatio = static_cast<float>(INIConfig::Defaults::DEFAULT_INTERFACE_WIDTH) /
                               static_cast<float>(INIConfig::Defaults::DEFAULT_INTERFACE_HEIGHT);
    layoutConfig.minScale = INIConfig::Validation::MIN_INTERFACE_SCALE;
    layoutConfig.maxScale = INIConfig::Validation::MAX_INTERFACE_SCALE;

    colorScheme = std::make_unique<ColorScheme>();

    layoutManager = std::make_unique<ResponsiveLayoutManager>(layoutConfig);

    fontManager = std::make_unique<FontManager>();

    auto assetsPath = FontManager::getAssetsPath();

    if (assetsPath.exists()) {
        fontManager->loadCustomFonts(assetsPath);

    } else {

    }

    dataManager = std::make_unique<INIDataManager>();

    customLookAndFeel = std::make_unique<CustomLookAndFeel>(*fontManager, *colorScheme);

    setLookAndFeel(customLookAndFeel.get());

    juce::LookAndFeel::setDefaultLookAndFeel(customLookAndFeel.get());

    colorScheme->addListener(this);
}

void OTTOAudioProcessorEditor::createComponents()
{
    if (!layoutManager || !fontManager || !colorScheme) {

        return;
    }

    topBar = std::make_unique<TopBarComponent>(
        audioProcessor.getMidiEngine(),
        audioProcessor.getValueTreeState(),
        *layoutManager,
        *fontManager,
        *colorScheme
    );
    if (topBar) {
        topBar->setINIDataManager(dataManager.get());
        addAndMakeVisible(topBar.get());

    } else {

    }

    playerTabs = std::make_unique<PlayerTabsComponent>(
        audioProcessor.getMidiEngine(),
        *layoutManager,
        *fontManager,
        *colorScheme
    );
    if (playerTabs) {
        addAndMakeVisible(playerTabs.get());

    } else {

    }

    drumKitSection = std::make_unique<DrumKitSectionComponent>(
        audioProcessor.getPresetManager(),
        audioProcessor.getSFZEngine(),
        *layoutManager,
        *fontManager,
        *colorScheme,
        *dataManager,
        &audioProcessor.getMixer()
    );
    if (drumKitSection) {
        addAndMakeVisible(drumKitSection.get());

    } else {

    }

    mainContent = std::make_unique<MainContentComponent>(
        audioProcessor.getMidiEngine(),
        audioProcessor.getMixer(),
        audioProcessor.getValueTreeState(),
        *layoutManager,
        *fontManager,
        *colorScheme
    );
    if (mainContent) {
        mainContent->setINIDataManager(dataManager.get());
        addAndMakeVisible(mainContent.get());
    } else {

    }
}

void OTTOAudioProcessorEditor::setupCallbacks()
{
    if (!topBar || !playerTabs || !drumKitSection) {

        return;
    }

    topBar->onGearButtonClicked = [this]() {
        showSettingsPanel();
    };

    topBar->onPlayStateChanged = [this](bool isPlaying) {
        componentState.playState = isPlaying;
        handleTransportStateChange();
        saveEditorState();
    };

    topBar->onTempoChanged = [this](float newTempo) {
        componentState.globalSettings.tempo = INIConfig::clampTempo(static_cast<int>(newTempo));
        updateProcessorFromState();
        saveEditorState();
    };

    topBar->onPresetChanged = [this](int presetIndex) {
        componentState.globalSettings.presetID = INIConfig::clampPresetIndex(presetIndex);
        saveEditorState();
    };

    playerTabs->onTabChanged = [this](int playerIndex) {
        handlePlayerChange(playerIndex);
        saveEditorState();
    };

    drumKitSection->onKitChanged = [this](int kitIndex) {
        if (INIConfig::isValidPlayerIndex(currentPlayerIndex))
        {
            componentState.playerSettings[currentPlayerIndex].selectedButton =
                INIConfig::clampButtonIndex(kitIndex);
            saveEditorState();
        }
    };

    drumKitSection->onPowerStateChanged = [this](bool enabled) {
        if (INIConfig::isValidPlayerIndex(currentPlayerIndex))
        {
            componentState.playerSettings[currentPlayerIndex].enabled = enabled;
            saveEditorState();
        }
    };

    if (mainContent) {
        mainContent->onEditModeChanged = [this](bool editMode) {
            saveEditorState();
        };

        mainContent->onSliderValueChanged = [this](const juce::String& sliderId, float value) {
            saveEditorState();
        };
    }
}

void OTTOAudioProcessorEditor::paint(juce::Graphics& g)
{
    juce::Colour bgColor = juce::Colours::darkgrey;

    if (colorScheme) {
        bgColor = colorScheme->getBackgroundColor();
    }

    g.fillAll(bgColor);
}

void OTTOAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    if (bounds.isEmpty()) {

        return;
    }



    const int topBarHeight = layoutManager ? layoutManager->scaled(INIConfig::LayoutConstants::mainHeaderHeight) : 60;
    const int playerTabsHeight = layoutManager ? layoutManager->scaled(INIConfig::LayoutConstants::playerTabHeight * INIConfig::LayoutConstants::defaultMargin) : 100;
    const int drumKitWidth = layoutManager ? layoutManager->scaled(INIConfig::LayoutConstants::drumKitSectionMargin * INIConfig::LayoutConstants::dialogButtonWidth / 2) : 500;

    if (topBar && topBarHeight > 0) {
        auto topBarBounds = bounds.removeFromTop(topBarHeight);
        if (!topBarBounds.isEmpty()) {
            topBar->setBounds(topBarBounds);

        }
    }

    if (playerTabs && playerTabsHeight > 0) {
        auto playerTabsBounds = bounds.removeFromTop(playerTabsHeight);
        if (!playerTabsBounds.isEmpty()) {
            playerTabs->setBounds(playerTabsBounds);

        }
    }

    if (drumKitSection && drumKitWidth > 0 && bounds.getWidth() > 0) {
        auto drumKitBounds = bounds.removeFromRight(drumKitWidth);
        if (!drumKitBounds.isEmpty()) {
            drumKitSection->setBounds(drumKitBounds);

        }
    }

    if (mainContent && !bounds.isEmpty()) {
        mainContent->setBounds(bounds);
    }

    componentState.interfaceWidth = getWidth();
    componentState.interfaceHeight = getHeight();

    if (isInitialized && componentState.interfaceWidth > 0 && componentState.interfaceHeight > 0) {
        saveEditorState();
    }
}

void OTTOAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    juce::ignoreUnused(button);
}

void OTTOAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    juce::ignoreUnused(slider);
}

void OTTOAudioProcessorEditor::comboBoxChanged(juce::ComboBox* comboBox)
{
    juce::ignoreUnused(comboBox);
}

void OTTOAudioProcessorEditor::themeChanged(const juce::String& newThemeName)
{
    juce::ignoreUnused(newThemeName);
    applyColorScheme();
    repaint();
    saveEditorState();
}

void OTTOAudioProcessorEditor::colorChanged(const juce::String& component,
                                           const juce::String& property,
                                           const juce::Colour& newColor)
{
    juce::ignoreUnused(component, property, newColor);
    applyColorScheme();
    repaint();
    saveEditorState();
}

void OTTOAudioProcessorEditor::timerCallback()
{
    updateFromProcessor();
}

void OTTOAudioProcessorEditor::saveEditorState()
{
    saveAllComponentStates();

    if (dataManager)
    {
        if (!dataManager->saveAllData(componentState)) {

        }
    }
}

void OTTOAudioProcessorEditor::saveAllComponentStates()
{
    if (topBar)
        topBar->saveStates(componentState);

    if (playerTabs)
        playerTabs->saveStates(componentState);

    if (drumKitSection)
        drumKitSection->saveStates(componentState);

    if (mainContent)
        mainContent->saveStates(componentState);

    if (colorScheme)
        colorScheme->saveStates(componentState);

    if (fontManager)
        fontManager->saveStates(componentState);
}

void OTTOAudioProcessorEditor::loadAllComponentStates()
{
    if (topBar)
        topBar->loadStates(componentState);

    if (playerTabs)
        playerTabs->loadStates(componentState);

    if (drumKitSection)
        drumKitSection->loadStates(componentState);

    if (mainContent)
        mainContent->loadStates(componentState);

    if (colorScheme) {
        colorScheme->loadStates(componentState);
        applyColorScheme();
    }

    if (fontManager)
        fontManager->loadStates(componentState);
}

void OTTOAudioProcessorEditor::updateFromProcessor()
{
    bool isPlaying = audioProcessor.getMidiEngine().isPlaybackActive();
    if (isPlaying != componentState.playState)
    {
        componentState.playState = isPlaying;
        if (topBar)
        {
            topBar->setPlayState(isPlaying);
        }
    }

    float currentTempo = audioProcessor.getMidiEngine().getTempo();
    if (std::abs(currentTempo - static_cast<float>(componentState.globalSettings.tempo)) > 0.01f)
    {
        componentState.globalSettings.tempo = INIConfig::clampTempo(static_cast<int>(currentTempo));
        if (topBar)
        {
            topBar->setTempo(currentTempo);
        }
    }
}

void OTTOAudioProcessorEditor::updateProcessorFromState()
{
    audioProcessor.getMidiEngine().setTempo(static_cast<float>(componentState.globalSettings.tempo));

    auto& parameters = audioProcessor.getValueTreeState();
    if (auto* param = parameters.getRawParameterValue("tempo"))
    {
        *param = static_cast<float>(componentState.globalSettings.tempo);
    }
}

void OTTOAudioProcessorEditor::applyColorScheme()
{
    if (customLookAndFeel)
    {
        customLookAndFeel->forceRefreshFromColorScheme();

        if (topBar) topBar->repaint();
        if (playerTabs) playerTabs->repaint();
        if (drumKitSection) drumKitSection->repaint();
        if (mainContent) mainContent->repaint();

        repaint();
    }
}

void OTTOAudioProcessorEditor::handlePlayerChange(int newPlayerIndex)
{
    if (!INIConfig::isValidPlayerIndex(newPlayerIndex))
        return;

    if (INIConfig::isValidPlayerIndex(currentPlayerIndex))
    {
        drumKitSection->saveStates(componentState);
    }

    currentPlayerIndex = newPlayerIndex;
    componentState.currentPlayer = currentPlayerIndex;

    drumKitSection->setCurrentPlayerIndex(currentPlayerIndex);
    drumKitSection->loadStates(componentState);

    audioProcessor.getMidiEngine().setCurrentPlayer(currentPlayerIndex);
}

void OTTOAudioProcessorEditor::handleTransportStateChange()
{
    if (componentState.playState)
    {
        audioProcessor.getMidiEngine().startPlayback();
    }
    else
    {
        audioProcessor.getMidiEngine().stopPlayback();
    }
}

void OTTOAudioProcessorEditor::handlePatternSelection(int patternIndex)
{
    if (INIConfig::isValidPlayerIndex(currentPlayerIndex) &&
        INIConfig::isValidButtonIndex(patternIndex))
    {
        componentState.playerSettings[currentPlayerIndex].selectedButton = patternIndex;

        audioProcessor.getMidiEngine().selectPattern(currentPlayerIndex, patternIndex);
    }
}

void OTTOAudioProcessorEditor::updateUIFromState()
{
    if (layoutManager)
    {
        float scale = componentState.globalSettings.interfaceScale;
        layoutManager->setScale(scale);
    }

    setSize(componentState.interfaceWidth, componentState.interfaceHeight);

    if (playerTabs)
    {
        playerTabs->setSelectedTab(componentState.currentPlayer);
    }

    if (topBar)
    {
        topBar->setPlayState(componentState.playState);
        topBar->setTempo(static_cast<float>(componentState.globalSettings.tempo));
    }
}

void OTTOAudioProcessorEditor::showSettingsPanel()
{
    if (!settingsPanel) {
        settingsPanel = std::make_unique<SettingsPanelWindow>(
            *fontManager,
            *colorScheme,
            *layoutManager,
            *dataManager
        );

        settingsPanel->onClose = [this]() {
            settingsPanel.reset();
            saveEditorState();
        };

        settingsPanel->onColorSchemeChanged = [this]() {
            applyColorScheme();
            repaint();
            saveEditorState();
        };

        settingsPanel->onPhosphorWeightChanged = [this](FontManager::PhosphorWeight weight) {
            juce::ignoreUnused(weight);
            if (topBar) topBar->repaint();
            if (playerTabs) playerTabs->repaint();
            if (drumKitSection) drumKitSection->repaint();
            saveEditorState();
        };

        addAndMakeVisible(settingsPanel.get());
    }

    settingsPanel->setBounds(getLocalBounds());
    settingsPanel->setVisible(true);
    settingsPanel->toFront(true);
}
