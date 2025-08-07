#include "PopupWindowsDrumKit.h"
#include "PresetManager.h"
#include "INIConfig.h"
#include "Mixer.h"
#include "PopupWindowsMixer.h"
#include "DrumKitEditorContent.h"

//==============================================================================
// DrumKitSelectionWindow Implementation
//==============================================================================

DrumKitSelectionWindow::DrumKitSelectionWindow(PresetManager& pm,
                                             SFZEngine& sfz,
                                             ColorScheme& cs,
                                             FontManager& fm,
                                             ResponsiveLayoutManager& lm,
                                             INIDataManager& dm,
                                             Mixer* mixerPtr)
    : juce::DocumentWindow("DrumKit Selection", 
                          cs.getBackgroundColor(), 
                          juce::DocumentWindow::allButtons),
      presetManager(pm), 
      sfzEngine(sfz), 
      colorScheme(cs), 
      fontManager(fm), 
      layoutManager(lm), 
      iniManager(dm), 
      mixer(mixerPtr)
{
    // Create the content component
    content = std::make_unique<DrumKitContent>(presetManager, sfzEngine, colorScheme, 
                                              fontManager, layoutManager, iniManager, mixer);
    
    // Forward callbacks from content to this window's callbacks
    content->onEditRequested = [this]() {
        if (onEditRequested) onEditRequested();
    };
    
    content->onMixerRequested = [this]() {
        if (onMixerRequested) onMixerRequested();
    };
    
    content->onPowerStateChanged = [this](bool state) {
        if (onPowerStateChanged) onPowerStateChanged(state);
    };
    
    content->onKitChanged = [this](int kitIndex) {
        if (onKitChanged) onKitChanged(kitIndex);
    };

    // Set up the window
    setContentNonOwned(content.get(), true);
    setResizable(true, true);
    
    // Set minimum and preferred sizes (responsive to current scale)
    const int minWidth = layoutManager.scaled(400);
    const int minHeight = layoutManager.scaled(300);
    const int prefWidth = layoutManager.scaled(500);
    const int prefHeight = layoutManager.scaled(400);
    
    setResizeLimits(minWidth, minHeight, prefWidth * 2, prefHeight * 2);
    centreWithSize(prefWidth, prefHeight);
    
    // Apply JUCE 8 window styling
    setUsingNativeTitleBar(true);
    
    // Ensure window is always on top when shown and has proper decorations
    setAlwaysOnTop(true);
    
    // Start invisible - will be shown when requested
    setVisible(false);
}

DrumKitSelectionWindow::~DrumKitSelectionWindow()
{
    content = nullptr;
}

void DrumKitSelectionWindow::closeButtonPressed()
{
    setVisible(false);
}

void DrumKitSelectionWindow::saveStates(ComponentState& state)
{
    if (content) {
        content->saveStates(state);
    }
    
    // Save window position and size
    auto bounds = getBounds();
    state.drumKitWindowX = bounds.getX();
    state.drumKitWindowY = bounds.getY();
    state.drumKitWindowWidth = bounds.getWidth();
    state.drumKitWindowHeight = bounds.getHeight();
}

void DrumKitSelectionWindow::loadStates(const ComponentState& state)
{
    if (content) {
        content->loadStates(state);
    }
    
    // Restore window position and size if valid
    if (state.drumKitWindowWidth > 0 && state.drumKitWindowHeight > 0) {
        setBounds(state.drumKitWindowX, state.drumKitWindowY, 
                 state.drumKitWindowWidth, state.drumKitWindowHeight);
    }
}

void DrumKitSelectionWindow::setCurrentPlayerIndex(int playerIndex)
{
    currentPlayerIndex = INIConfig::clampPlayerIndex(playerIndex);
    if (content) {
        content->setCurrentPlayerIndex(currentPlayerIndex);
    }
}

//==============================================================================
// DrumKitContent Implementation
//==============================================================================

DrumKitSelectionWindow::DrumKitContent::DrumKitContent(PresetManager& pm,
                                                       SFZEngine& sfz,
                                                       ColorScheme& cs,
                                                       FontManager& fm,
                                                       ResponsiveLayoutManager& lm,
                                                       INIDataManager& dm,
                                                       Mixer* mixerPtr)
    : presetManager(pm), 
      sfzEngine(sfz), 
      colorScheme(cs), 
      fontManager(fm), 
      layoutManager(lm), 
      iniManager(dm), 
      mixer(mixerPtr)
{
    setupComponents();
    applyColorScheme();
    colorScheme.addListener(this);
}

DrumKitSelectionWindow::DrumKitContent::~DrumKitContent()
{
    colorScheme.removeListener(this);
}

void DrumKitSelectionWindow::DrumKitContent::setupComponents()
{
    // DrumKit ComboBox
    drumkitComboBox = std::make_unique<juce::ComboBox>();
    drumkitComboBox->addListener(this);
    addAndMakeVisible(*drumkitComboBox);

    // SFZ File ComboBox
    sfzFileComboBox = std::make_unique<juce::ComboBox>();
    sfzFileComboBox->addListener(this);
    addAndMakeVisible(*sfzFileComboBox);

    // Labels
    drumkitLabel = std::make_unique<juce::Label>("", "Drumkit:");
    drumkitLabel->setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(*drumkitLabel);

    sfzFileLabel = std::make_unique<juce::Label>("", "SFZ File:");
    sfzFileLabel->setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(*sfzFileLabel);

    // Buttons
    refreshButton = std::make_unique<juce::TextButton>("Refresh");
    refreshButton->addListener(this);
    addAndMakeVisible(*refreshButton);

    loadButton = std::make_unique<juce::TextButton>("Load");
    loadButton->addListener(this);
    addAndMakeVisible(*loadButton);

    editButton = std::make_unique<juce::TextButton>("Edit");
    editButton->addListener(this);
    addAndMakeVisible(*editButton);

    mixerButton = std::make_unique<juce::TextButton>("Mixer");
    mixerButton->addListener(this);
    addAndMakeVisible(*mixerButton);

    powerButton = std::make_unique<juce::ToggleButton>("Power");
    powerButton->addListener(this);
    addAndMakeVisible(*powerButton);

    updateDrumkitList();
}

void DrumKitSelectionWindow::DrumKitContent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    // Background
    g.fillAll(colorScheme.getBackgroundColor());
    
    // Title area
    auto titleArea = bounds.removeFromTop(layoutManager.scaled(40));
    g.setColour(colorScheme.getLabelTextColor());
    g.setFont(fontManager.getFont(FontManager::FontRole::Header, layoutManager.scaled(16)));
    g.drawText("DrumKit Selection", titleArea, juce::Justification::centred);
}

void DrumKitSelectionWindow::DrumKitContent::resized()
{
    auto bounds = getLocalBounds();
    const int margin = layoutManager.scaled(10);
    const int componentHeight = layoutManager.scaled(25);
    const int spacing = layoutManager.scaled(5);
    
    bounds.reduce(margin, margin);
    
    // Title space
    bounds.removeFromTop(layoutManager.scaled(40));
    
    // DrumKit section
    drumkitLabel->setBounds(bounds.removeFromTop(componentHeight));
    bounds.removeFromTop(spacing);
    drumkitComboBox->setBounds(bounds.removeFromTop(componentHeight));
    bounds.removeFromTop(spacing * 2);
    
    // SFZ File section  
    sfzFileLabel->setBounds(bounds.removeFromTop(componentHeight));
    bounds.removeFromTop(spacing);
    sfzFileComboBox->setBounds(bounds.removeFromTop(componentHeight));
    bounds.removeFromTop(spacing * 2);
    
    // Buttons section
    auto buttonArea = bounds.removeFromTop(componentHeight);
    const int buttonWidth = (buttonArea.getWidth() - spacing * 4) / 5;
    
    refreshButton->setBounds(buttonArea.removeFromLeft(buttonWidth));
    buttonArea.removeFromLeft(spacing);
    loadButton->setBounds(buttonArea.removeFromLeft(buttonWidth));
    buttonArea.removeFromLeft(spacing);
    editButton->setBounds(buttonArea.removeFromLeft(buttonWidth));
    buttonArea.removeFromLeft(spacing);
    mixerButton->setBounds(buttonArea.removeFromLeft(buttonWidth));
    buttonArea.removeFromLeft(spacing);
    powerButton->setBounds(buttonArea);
}

void DrumKitSelectionWindow::DrumKitContent::lookAndFeelChanged()
{
    updateFontsAndColors();
}

void DrumKitSelectionWindow::DrumKitContent::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == drumkitComboBox.get()) {
        updateSFZFileList();
        if (onKitChanged) {
            onKitChanged(drumkitComboBox->getSelectedItemIndex());
        }
    } else if (comboBoxThatHasChanged == sfzFileComboBox.get()) {
        loadSelectedDrumkit();
    }
}

void DrumKitSelectionWindow::DrumKitContent::buttonClicked(juce::Button* button)
{
    if (button == refreshButton.get()) {
        refreshDrumkits();
    } else if (button == loadButton.get()) {
        loadSelectedDrumkit();
    } else if (button == editButton.get()) {
        // Create and show drumkit editor
        if (!drumKitEditorWindow) {
            drumKitEditorWindow = std::make_unique<DrumKitEditorWindow>(
                sfzEngine, colorScheme, fontManager, layoutManager, iniManager
            );
        }
        drumKitEditorWindow->setVisible(true);
        drumKitEditorWindow->toFront(true);
        
        if (onEditRequested) onEditRequested();
    } else if (button == mixerButton.get()) {
        // Create and show mixer window
        if (!drumKitMixerWindow && mixer) {
            drumKitMixerWindow = std::make_unique<DrumKitMixerWindow>(
                *mixer, sfzEngine, colorScheme, fontManager, layoutManager, iniManager
            );
        }
        if (drumKitMixerWindow) {
            drumKitMixerWindow->setVisible(true);
            drumKitMixerWindow->toFront(true);
        }
        
        if (onMixerRequested) onMixerRequested();
    } else if (button == powerButton.get()) {
        if (onPowerStateChanged) {
            onPowerStateChanged(powerButton->getToggleState());
        }
    }
}

void DrumKitSelectionWindow::DrumKitContent::themeChanged(const juce::String& newThemeName)
{
    juce::ignoreUnused(newThemeName);
    applyColorScheme();
    repaint();
}

void DrumKitSelectionWindow::DrumKitContent::colorChanged(const juce::String& component,
                                                         const juce::String& property,
                                                         const juce::Colour& newColor)
{
    juce::ignoreUnused(component, property, newColor);
    applyColorScheme();
    repaint();
}

void DrumKitSelectionWindow::DrumKitContent::saveStates(ComponentState& state)
{
    if (INIConfig::isValidPlayerIndex(currentPlayerIndex)) {
        auto& playerState = state.playerSettings[currentPlayerIndex];
        playerState.selectedButton = drumkitComboBox->getSelectedItemIndex();
        playerState.enabled = powerButton->getToggleState();
    }
}

void DrumKitSelectionWindow::DrumKitContent::loadStates(const ComponentState& state)
{
    if (INIConfig::isValidPlayerIndex(currentPlayerIndex)) {
        const auto& playerState = state.playerSettings[currentPlayerIndex];
        
        if (playerState.selectedButton >= 0 && playerState.selectedButton < drumkitComboBox->getNumItems()) {
            drumkitComboBox->setSelectedItemIndex(playerState.selectedButton, juce::dontSendNotification);
        }
        
        powerButton->setToggleState(playerState.enabled, juce::dontSendNotification);
        updateSFZFileList();
    }
}

void DrumKitSelectionWindow::DrumKitContent::setCurrentPlayerIndex(int playerIndex)
{
    currentPlayerIndex = INIConfig::clampPlayerIndex(playerIndex);
    
    // Update mixer window if it exists
    if (drumKitMixerWindow) {
        drumKitMixerWindow->setCurrentPlayerIndex(currentPlayerIndex);
    }
}

void DrumKitSelectionWindow::DrumKitContent::updateDrumkitList()
{
    drumkitComboBox->clear();
    
    auto availableDrumkits = sfzEngine.getAvailableDrumkits();
    
    int itemId = 1;
    for (const auto& drumkit : availableDrumkits) {
        drumkitComboBox->addItem(drumkit.name, itemId++);
    }
    
    juce::String currentDrumkit = sfzEngine.getCurrentDrumkitName();
    if (currentDrumkit.isNotEmpty()) {
        for (int i = 0; i < drumkitComboBox->getNumItems(); ++i) {
            if (drumkitComboBox->getItemText(i) == currentDrumkit) {
                drumkitComboBox->setSelectedItemIndex(i, juce::dontSendNotification);
                break;
            }
        }
    }
    
    updateSFZFileList();
}

void DrumKitSelectionWindow::DrumKitContent::updateSFZFileList()
{
    sfzFileComboBox->clear();
    
    juce::String selectedDrumkit = drumkitComboBox->getText();
    if (selectedDrumkit.isEmpty()) return;
    
    auto availableDrumkits = sfzEngine.getAvailableDrumkits();
    
    for (const auto& drumkit : availableDrumkits) {
        if (drumkit.name == selectedDrumkit) {
            int itemId = 1;
            for (const auto& sfzFile : drumkit.sfzFiles) {
                sfzFileComboBox->addItem(sfzFile, itemId++);
            }
            
            juce::String currentSFZ = sfzEngine.getCurrentSFZFile();
            if (currentSFZ.isNotEmpty()) {
                for (int i = 0; i < sfzFileComboBox->getNumItems(); ++i) {
                    if (sfzFileComboBox->getItemText(i) == currentSFZ) {
                        sfzFileComboBox->setSelectedItemIndex(i, juce::dontSendNotification);
                        break;
                    }
                }
            } else if (sfzFileComboBox->getNumItems() > 0) {
                sfzFileComboBox->setSelectedItemIndex(0, juce::dontSendNotification);
            }
            
            break;
        }
    }
}

void DrumKitSelectionWindow::DrumKitContent::loadSelectedDrumkit()
{
    juce::String drumkitName = drumkitComboBox->getText();
    juce::String sfzFileName = sfzFileComboBox->getText();
    
    if (drumkitName.isNotEmpty() && sfzFileName.isNotEmpty()) {
        sfzEngine.loadDrumkit(drumkitName, sfzFileName);
    }
}

void DrumKitSelectionWindow::DrumKitContent::refreshDrumkits()
{
    sfzEngine.scanDrumkitsFolder();
    updateDrumkitList();
}

void DrumKitSelectionWindow::DrumKitContent::applyColorScheme()
{
    // Apply colors to all components
    if (drumkitLabel) {
        drumkitLabel->setColour(juce::Label::textColourId, colorScheme.getLabelTextColor());
    }
    
    if (sfzFileLabel) {
        sfzFileLabel->setColour(juce::Label::textColourId, colorScheme.getLabelTextColor());
    }
    
    // ComboBoxes
    if (drumkitComboBox) {
        drumkitComboBox->setColour(juce::ComboBox::backgroundColourId, colorScheme.getComboBoxBackgroundColor());
        drumkitComboBox->setColour(juce::ComboBox::textColourId, colorScheme.getComboBoxTextColor());
    }
    
    if (sfzFileComboBox) {
        sfzFileComboBox->setColour(juce::ComboBox::backgroundColourId, colorScheme.getComboBoxBackgroundColor());
        sfzFileComboBox->setColour(juce::ComboBox::textColourId, colorScheme.getComboBoxTextColor());
    }
    
    // Buttons
    auto buttonBg = colorScheme.getButtonBackgroundColor();
    auto buttonText = colorScheme.getButtonTextColor();
    
    for (auto* button : {refreshButton.get(), loadButton.get(), editButton.get(), mixerButton.get()}) {
        if (button) {
            button->setColour(juce::TextButton::buttonColourId, buttonBg);
            button->setColour(juce::TextButton::textColourOffId, buttonText);
        }
    }
    
    if (powerButton) {
        powerButton->setColour(juce::ToggleButton::textColourId, buttonText);
    }
}

void DrumKitSelectionWindow::DrumKitContent::updateFontsAndColors()
{
    applyColorScheme();
    
    // Update fonts - JUCE 8 compatible
    auto labelFont = fontManager.getFont(FontManager::FontRole::Body, layoutManager.scaled(12));
    
    if (drumkitLabel) drumkitLabel->setFont(labelFont);
    if (sfzFileLabel) sfzFileLabel->setFont(labelFont);
    
    // For buttons in JUCE 8, fonts are handled through LookAndFeel
    // The buttons will inherit appropriate fonts from the global LookAndFeel
    repaint();
}