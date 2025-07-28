#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "INIConfig.h"
#include "ErrorHandling.h"

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

        splashOverlay = std::make_unique<OTTOSplashOverlay>(
            customLookAndFeel->getSplashImage(),
            splashTime,
            [this]() { 
                if (splashOverlay) {
                    removeChildComponent(splashOverlay.get());
                    splashOverlay.reset();
                }
                repaint(); 
            },
            appBgColor
        );

        splashOverlay->setBounds(getLocalBounds());
        addAndMakeVisible(splashOverlay.get());
        splashOverlay->toFront(false);


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
    try {
        // Initialize layout configuration with safe defaults
        ResponsiveLayoutManager::LayoutConfig layoutConfig;
        layoutConfig.baseWidth = INIConfig::Defaults::DEFAULT_INTERFACE_WIDTH;
        layoutConfig.baseHeight = INIConfig::Defaults::DEFAULT_INTERFACE_HEIGHT;
        layoutConfig.aspectRatio = static_cast<float>(INIConfig::Defaults::DEFAULT_INTERFACE_WIDTH) /
                                   static_cast<float>(INIConfig::Defaults::DEFAULT_INTERFACE_HEIGHT);
        layoutConfig.minScale = INIConfig::Validation::MIN_INTERFACE_SCALE;
        layoutConfig.maxScale = INIConfig::Validation::MAX_INTERFACE_SCALE;

        // Null-pointer safety: Create ColorScheme with exception handling
        colorScheme = ErrorHandler::safeCreate<ColorScheme>(
            []() { return std::make_unique<ColorScheme>(); },
            "ColorScheme"
        );
        if (!colorScheme) {
            DBG("PluginEditor: Failed to create ColorScheme, using fallback");
            return;
        }

        // Null-pointer safety: Create ResponsiveLayoutManager with exception handling
        layoutManager = ErrorHandler::safeCreate<ResponsiveLayoutManager>(
            [&layoutConfig]() { return std::make_unique<ResponsiveLayoutManager>(layoutConfig); },
            "ResponsiveLayoutManager"
        );
        if (!layoutManager) {
            DBG("PluginEditor: Failed to create ResponsiveLayoutManager");
            return;
        }

        // Set up responsive layout callback to handle scale changes
        layoutManager->onScaleChanged = [this](float newScale) {
            if (fontManager) {
                fontManager->setScaleFactor(newScale);
            }
            
            // Notify all components that look and feel has changed to update fonts
            // This is crucial for JUCE 8 - components need to know when fonts change
            if (topBar) {
                topBar->lookAndFeelChanged();
                topBar->resized();
            }
            if (playerTabs) {
                playerTabs->lookAndFeelChanged();
                playerTabs->resized();
            }
            if (drumKitSection) {
                drumKitSection->lookAndFeelChanged();
                drumKitSection->resized();
            }
            if (mainContent) {
                mainContent->lookAndFeelChanged();
                mainContent->resized();
            }
            
            // Also call lookAndFeelChanged on the main editor
            lookAndFeelChanged();
            
            // Force a repaint to update visuals
            repaint();
        };

        // Null-pointer safety: Create FontManager with exception handling
        fontManager = ErrorHandler::safeCreate<FontManager>(
            []() { return std::make_unique<FontManager>(); },
            "FontManager"
        );
        if (!fontManager) {
            DBG("PluginEditor: Failed to create FontManager");
            return;
        }

        // Null-pointer safety: Load fonts with error handling
        auto assetsPath = FontManager::getAssetsPath();
        
        ErrorHandler::safeExecute([&]() {
            fontManager->loadCustomFonts(assetsPath);
        }, "FontManager font loading");
        
        // Validate font loading results
        if (fontManager->hasError()) {
            DBG("FontManager Error: " + fontManager->getLastError());
        }
        
        if (!fontManager->arePhosphorFontsLoaded()) {
            DBG("Warning: No Phosphor fonts loaded - using system defaults");
        } else {
            DBG("Phosphor fonts loaded successfully");
        }
        
        auto loadedFonts = fontManager->getLoadedFontNames();
        DBG("Loaded fonts: " + loadedFonts.joinIntoString(", "));

        // Null-pointer safety: Create INIDataManager with exception handling
        dataManager = ErrorHandler::safeCreate<INIDataManager>(
            []() { return std::make_unique<INIDataManager>(); },
            "INIDataManager"
        );
        if (!dataManager) {
            DBG("PluginEditor: Failed to create INIDataManager");
            return;
        }

        // Null-pointer safety: Create CustomLookAndFeel with exception handling
        if (fontManager && colorScheme) {
            customLookAndFeel = ErrorHandler::safeCreate<CustomLookAndFeel>(
                [&]() { return std::make_unique<CustomLookAndFeel>(*fontManager, *colorScheme); },
                "CustomLookAndFeel"
            );
            
            if (customLookAndFeel) {
                ErrorHandler::safeExecute([&]() {
                    setLookAndFeel(customLookAndFeel.get());
                    juce::LookAndFeel::setDefaultLookAndFeel(customLookAndFeel.get());
                }, "LookAndFeel setup");
            } else {
                DBG("PluginEditor: Failed to create CustomLookAndFeel, using default");
            }
        }

        // Null-pointer safety: Add listener only if colorScheme is valid
        if (colorScheme) {
            colorScheme->addListener(this);
        }

    } catch (const std::exception& e) {
        DBG("PluginEditor: Exception in initializeManagers - " + juce::String(e.what()));
    }
}

void OTTOAudioProcessorEditor::createComponents()
{
    // Null-pointer safety: Validate all required managers
    if (!layoutManager || !fontManager || !colorScheme) {
        DBG("PluginEditor: Cannot create components - missing required managers");
        DBG("  layoutManager: " + juce::String(layoutManager ? "OK" : "NULL"));
        DBG("  fontManager: " + juce::String(fontManager ? "OK" : "NULL"));
        DBG("  colorScheme: " + juce::String(colorScheme ? "OK" : "NULL"));
        return;
    }

    try {
        // Null-pointer safety: Create TopBarComponent with error handling
        topBar = ErrorHandler::safeCreate<TopBarComponent>(
            [&]() {
                return std::make_unique<TopBarComponent>(
                    audioProcessor.getMidiEngine(),
                    audioProcessor.getValueTreeState(),
                    *layoutManager,
                    *fontManager,
                    *colorScheme
                );
            },
            "TopBarComponent"
        );
        
        if (topBar) {
            if (dataManager) {
                topBar->setINIDataManager(dataManager.get());
            }
            ErrorHandler::safeExecute([&]() {
                addAndMakeVisible(topBar.get());
            }, "TopBar visibility");
        } else {
            DBG("PluginEditor: Failed to create TopBarComponent");
        }

        // Null-pointer safety: Create PlayerTabsComponent with error handling
        playerTabs = ErrorHandler::safeCreate<PlayerTabsComponent>(
            [&]() {
                return std::make_unique<PlayerTabsComponent>(
                    audioProcessor.getMidiEngine(),
                    *layoutManager,
                    *fontManager,
                    *colorScheme
                );
            },
            "PlayerTabsComponent"
        );
        
        if (playerTabs) {
            ErrorHandler::safeExecute([&]() {
                addAndMakeVisible(playerTabs.get());
            }, "PlayerTabs visibility");
        } else {
            DBG("PluginEditor: Failed to create PlayerTabsComponent");
        }

        // Null-pointer safety: Create DrumKitSectionComponent with error handling
        if (dataManager) {
            drumKitSection = ErrorHandler::safeCreate<DrumKitSectionComponent>(
                [&]() {
                    return std::make_unique<DrumKitSectionComponent>(
                        audioProcessor.getPresetManager(),
                        audioProcessor.getSFZEngine(),
                        *layoutManager,
                        *fontManager,
                        *colorScheme,
                        *dataManager,
                        &audioProcessor.getMixer()
                    );
                },
                "DrumKitSectionComponent"
            );
            
            if (drumKitSection) {
                ErrorHandler::safeExecute([&]() {
                    addAndMakeVisible(drumKitSection.get());
                }, "DrumKitSection visibility");
            } else {
                DBG("PluginEditor: Failed to create DrumKitSectionComponent");
            }
        } else {
            DBG("PluginEditor: Cannot create DrumKitSection - dataManager is null");
        }

        // Null-pointer safety: Create MainContentComponent with error handling
        mainContent = ErrorHandler::safeCreate<MainContentComponent>(
            [&]() {
                return std::make_unique<MainContentComponent>(
                    audioProcessor.getMidiEngine(),
                    audioProcessor.getMixer(),
                    audioProcessor.getValueTreeState(),
                    *layoutManager,
                    *fontManager,
                    *colorScheme
                );
            },
            "MainContentComponent"
        );
        
        if (mainContent) {
            if (dataManager) {
                mainContent->setINIDataManager(dataManager.get());
            }
            ErrorHandler::safeExecute([&]() {
                addAndMakeVisible(mainContent.get());
            }, "MainContent visibility");
        } else {
            DBG("PluginEditor: Failed to create MainContentComponent");
        }

    } catch (const std::exception& e) {
        DBG("PluginEditor: Exception in createComponents - " + juce::String(e.what()));
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

    // CRITICAL FIX: Update the layout manager's scale based on new window dimensions
    // This is what was missing - the scale needs to be recalculated when the window resizes
    if (layoutManager) {
        layoutManager->updateScale(getWidth(), getHeight());
        
        // Update font manager scale factor to match layout scale for responsive fonts
        if (fontManager) {
            fontManager->setScaleFactor(layoutManager->getCurrentScale());
        }
    }

    // Handle splash overlay resizing
    if (splashOverlay) {
        splashOverlay->setBounds(getLocalBounds());
    }

    // Calculate responsive window areas as proportions of current window size
    // These proportions are based on the original design at 1200x800
    const float topBarHeightRatio = 0.075f;        // 7.5% of height (60/800)
    const float playerTabsHeightRatio = 0.0625f;   // 6.25% of height (50/800) 
    const float drumKitWidthRatio = 0.25f;         // 25% of width (300/1200)
    
    // Calculate actual dimensions based on current window size
    const int topBarHeight = static_cast<int>(getHeight() * topBarHeightRatio);
    const int playerTabsHeight = static_cast<int>(getHeight() * playerTabsHeightRatio);
    const int drumKitWidth = static_cast<int>(getWidth() * drumKitWidthRatio);
    
    // Ensure minimum sizes for usability - these scale with the interface
    const int minTopBarHeight = layoutManager ? layoutManager->scaled(50) : 50;
    const int minPlayerTabsHeight = layoutManager ? layoutManager->scaled(40) : 40;
    const int minDrumKitWidth = layoutManager ? layoutManager->scaled(200) : 200;
    
    // Maximum sizes to prevent areas from becoming too large on very big screens
    const int maxTopBarHeight = layoutManager ? layoutManager->scaled(120) : 120;
    const int maxPlayerTabsHeight = layoutManager ? layoutManager->scaled(80) : 80;
    const int maxDrumKitWidth = static_cast<int>(getWidth() * 0.4f); // Maximum 40% of width
    
    // Apply constraints
    const int finalTopBarHeight = juce::jlimit(minTopBarHeight, maxTopBarHeight, topBarHeight);
    const int finalPlayerTabsHeight = juce::jlimit(minPlayerTabsHeight, maxPlayerTabsHeight, playerTabsHeight);
    const int finalDrumKitWidth = juce::jlimit(minDrumKitWidth, maxDrumKitWidth, drumKitWidth);
    
    // Debug output for responsive layout (uncomment for debugging)
    // DBG("Window: " << getWidth() << "x" << getHeight() << " | TopBar: " << finalTopBarHeight << " | PlayerTabs: " << finalPlayerTabsHeight << " | DrumKit: " << finalDrumKitWidth);

    // Set TopBar bounds (top area, full width)
    if (topBar && finalTopBarHeight > 0) {
        auto topBarBounds = bounds.removeFromTop(finalTopBarHeight);
        if (!topBarBounds.isEmpty()) {
            topBar->setBounds(topBarBounds);
            // Force the component to recalculate its internal layout
            topBar->resized();
        }
    }

    // Set PlayerTabs bounds (below top bar, full width)
    if (playerTabs && finalPlayerTabsHeight > 0) {
        auto playerTabsBounds = bounds.removeFromTop(finalPlayerTabsHeight);
        if (!playerTabsBounds.isEmpty()) {
            playerTabs->setBounds(playerTabsBounds);
            // Force the component to recalculate its internal layout
            playerTabs->resized();
        }
    }

    // Set DrumKitSection bounds (right side of remaining area)
    // Only show drum kit section if there's enough remaining width for main content
    const int minMainContentWidth = layoutManager ? layoutManager->scaled(300) : 300;
    const bool shouldShowDrumKit = (bounds.getWidth() - finalDrumKitWidth) >= minMainContentWidth;
    
    if (drumKitSection && finalDrumKitWidth > 0 && shouldShowDrumKit) {
        auto drumKitBounds = bounds.removeFromRight(finalDrumKitWidth);
        if (!drumKitBounds.isEmpty()) {
            drumKitSection->setBounds(drumKitBounds);
            drumKitSection->setVisible(true);
            // Force the component to recalculate its internal layout
            drumKitSection->resized();
        }
    } else if (drumKitSection) {
        // Hide drum kit section if window is too small
        drumKitSection->setVisible(false);
    }

    // Set MainContent bounds (remaining area after top bars and drum kit section)
    if (mainContent && !bounds.isEmpty()) {
        mainContent->setBounds(bounds);
        mainContent->setVisible(true);
        // Force the component to recalculate its internal layout
        mainContent->resized();
    } else if (mainContent && bounds.isEmpty()) {
        // Handle case where no space is left for main content
        mainContent->setVisible(false);
        DBG("Warning: No space remaining for main content area");
    }

    // Update component state with new dimensions
    componentState.interfaceWidth = getWidth();
    componentState.interfaceHeight = getHeight();

    // Save the new state if initialized
    if (isInitialized && componentState.interfaceWidth > 0 && componentState.interfaceHeight > 0) {
        saveEditorState();
    }

    // Force a repaint to ensure all visual elements update with new scaling
    repaint();
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
