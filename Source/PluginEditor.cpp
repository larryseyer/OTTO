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
            // PHASE 8: Pattern initialization moved to Row4Component
            if (mainContent) {
                // PHASE 8: Pattern initialization moved to Row4Component - no action needed
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
        // PHASE 8: Pattern initialization moved to Row4Component
        if (mainContent) {
            // PHASE 8: Pattern initialization moved to Row4Component - no action needed
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

    if (isInitialized && drumKitPopup && dataManager)
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
            // TopBar and PlayerTabs now handled by Row1Component and Row2Component in MainContentComponent
            // DrumKit popup is now handled separately - no main GUI updates needed
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
        // TopBar functionality now handled by Row1Component in MainContentComponent
        // Removed standalone TopBarComponent to eliminate duplicate UI elements


        // PlayerTabs functionality now handled by Row2Component in MainContentComponent
        // Removed standalone PlayerTabsComponent to eliminate duplicate UI elements

        // Create DrumKit popup window (not added to main GUI)
        if (dataManager) {
            drumKitPopup = ErrorHandler::safeCreate<DrumKitSelectionWindow>(
                [&]() {
                    return std::make_unique<DrumKitSelectionWindow>(
                        audioProcessor.getPresetManager(),
                        audioProcessor.getSFZEngine(),
                        *colorScheme,
                        *fontManager,
                        *layoutManager,
                        *dataManager,
                        &audioProcessor.getMixer()
                    );
                },
                "DrumKitSelectionWindow"
            );
            
            if (drumKitPopup) {
                // Don't add DocumentWindow as child - it manages itself
                drumKitPopup->setVisible(false); // Initially hidden
            } else {
                DBG("PluginEditor: Failed to create DrumKitSelectionWindow");
            }
        } else {
            DBG("PluginEditor: Cannot create DrumKitPopup - dataManager is null");        }

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
    // TopBar and PlayerTabs callbacks now handled by Row1Component and Row2Component in MainContentComponent
    // Removed standalone component callback setup to eliminate duplicate UI elements

    // Setup drumkit popup callbacks
    if (drumKitPopup) {
        drumKitPopup->onKitChanged = [this](int kitIndex) {
            if (INIConfig::isValidPlayerIndex(currentPlayerIndex))
            {
                componentState.playerSettings[currentPlayerIndex].selectedButton =
                    INIConfig::clampButtonIndex(kitIndex);
                saveEditorState();
            }
        };

        drumKitPopup->onPowerStateChanged = [this](bool enabled) {
            if (INIConfig::isValidPlayerIndex(currentPlayerIndex))
            {
                componentState.playerSettings[currentPlayerIndex].enabled = enabled;
                saveEditorState();
            }
        };
        
        drumKitPopup->onEditRequested = [this]() {
            // Handle drumkit editing if needed
        };
        
        drumKitPopup->onMixerRequested = [this]() {
            // Handle mixer request if needed
        };
    }

    if (mainContent) {
        mainContent->onEditModeChanged = [this](bool editMode) {
            showDrumKitEditorWindow();
            saveEditorState();
        };

        mainContent->onSliderValueChanged = [this](const juce::String& sliderId, float value) {
            saveEditorState();
        };
        
        // Connect DrumKit popup trigger - Phase 2 of DrumKit Popup Refactor
        mainContent->onDrumKitPopupRequested = [this]() {
            showDrumKitPopup();
        };
        
        // Connect Mixer popup trigger
        mainContent->onMixerPopupRequested = [this]() {
            showMixerPopup();
        };    }
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

    // ========================================================================
    // UPDATED: Row-based layout system - all 6 rows handled by MainContentComponent
    // TopBar = Row 1, PlayerTabs = Row 2, MainContent handles all Rows 1-6
    // Removed standalone TopBar and PlayerTabs components to eliminate duplicates
    // ========================================================================

    // Set MainContent bounds (remaining area after top bars - full width now)
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
    // TopBar and PlayerTabs state saving now handled by Row1Component and Row2Component in MainContentComponent
    
    if (drumKitPopup)
        drumKitPopup->saveStates(componentState);

    if (mainContent)
        mainContent->saveStates(componentState);

    if (colorScheme)
        colorScheme->saveStates(componentState);

    if (fontManager)
        fontManager->saveStates(componentState);
}

void OTTOAudioProcessorEditor::loadAllComponentStates()
{
    // TopBar and PlayerTabs state loading now handled by Row1Component and Row2Component in MainContentComponent
    
    if (drumKitPopup)
        drumKitPopup->loadStates(componentState);

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
        // TopBar play state updates now handled by Row1Component in MainContentComponent
    }

    float currentTempo = audioProcessor.getMidiEngine().getTempo();
    if (std::abs(currentTempo - static_cast<float>(componentState.globalSettings.tempo)) > 0.01f)
    {
        componentState.globalSettings.tempo = INIConfig::clampTempo(static_cast<int>(currentTempo));
        // TopBar tempo updates now handled by Row1Component in MainContentComponent
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

        // TopBar and PlayerTabs repainting now handled by Row1Component and Row2Component in MainContentComponent
        // DrumKit popup handles its own repainting
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
        if (drumKitPopup)
            drumKitPopup->saveStates(componentState);
    }

    currentPlayerIndex = newPlayerIndex;
    componentState.currentPlayer = currentPlayerIndex;

    if (drumKitPopup) {
        drumKitPopup->setCurrentPlayerIndex(currentPlayerIndex);
        drumKitPopup->loadStates(componentState);
    }

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

    // PlayerTabs and TopBar UI updates now handled by Row2Component and Row1Component in MainContentComponent
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
            // TopBar and PlayerTabs repainting now handled by Row1Component and Row2Component in MainContentComponent
            if (mainContent) mainContent->repaint();
            // DrumKit is now in popup - no main GUI repaint needed
            saveEditorState();
        };

        addAndMakeVisible(settingsPanel.get());
    }

    settingsPanel->setBounds(getLocalBounds());
    settingsPanel->setVisible(true);
    settingsPanel->toFront(true);
}

void OTTOAudioProcessorEditor::showDrumKitPopup()
{
    if (drumKitPopup) {
        // Update the popup with current player index
        drumKitPopup->setCurrentPlayerIndex(currentPlayerIndex);
        
        // Load current state
        drumKitPopup->loadStates(componentState);
        
        
        // Center the popup relative to the main window
        auto mainBounds = getBounds();
        drumKitPopup->centreAroundComponent(this, drumKitPopup->getWidth(), drumKitPopup->getHeight());
        
        // Show and bring to front
        drumKitPopup->setVisible(true);
        drumKitPopup->toFront(true);
    }
}

void OTTOAudioProcessorEditor::showMixerPopup()
{
    // TODO: Implement mixer popup window
    // For now, show a debug message
    if (!drumKitMixerWindow) {
        drumKitMixerWindow = std::make_unique<DrumKitMixerWindow>(
            audioProcessor.getMixer(),
            audioProcessor.getSFZEngine(),
            *colorScheme,
            *fontManager,
            *layoutManager,
            *dataManager);
        
        drumKitMixerWindow->setSize(layoutManager->scaled(900), layoutManager->scaled(700));
        drumKitMixerWindow->centreAroundComponent(this, drumKitMixerWindow->getWidth(), drumKitMixerWindow->getHeight());
    }
    
    drumKitMixerWindow->setCurrentPlayerIndex(currentPlayerIndex);
    
    drumKitMixerWindow->setVisible(!drumKitMixerWindow->isVisible());
    if (drumKitMixerWindow->isVisible()) {
        drumKitMixerWindow->toFront(true);
    }
}

void OTTOAudioProcessorEditor::showDrumKitEditorWindow()
{
    if (!drumKitEditorWindow) {
        drumKitEditorWindow = std::make_unique<DrumKitEditorWindow>(
            audioProcessor.getSFZEngine(),
            *colorScheme,
            *fontManager,
            *layoutManager,
            *dataManager);
        
        drumKitEditorWindow->setSize(layoutManager->scaled(800), layoutManager->scaled(600));
        drumKitEditorWindow->centreAroundComponent(this, drumKitEditorWindow->getWidth(), drumKitEditorWindow->getHeight());
    }
    
    drumKitEditorWindow->setVisible(!drumKitEditorWindow->isVisible());
    if (drumKitEditorWindow->isVisible()) {
        drumKitEditorWindow->toFront(true);
    }
}
