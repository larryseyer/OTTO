#include "DrumKitSectionComponent.h"
#include "PresetManager.h"
#include "INIConfig.h"
#include "Mixer.h"
#include "PopupWindowsMixer.h"

DrumKitSectionComponent::DrumKitSectionComponent(PresetManager& pm,
                                               SFZEngine& sfz,
                                               ResponsiveLayoutManager& lm,
                                               FontManager& fm,
                                               ColorScheme& cs,
                                               INIDataManager& dm,
                                               Mixer* mixerPtr)
    : presetManager(pm), sfzEngine(sfz), layoutManager(lm), fontManager(fm), colorScheme(cs), dataManager(dm), mixer(mixerPtr)
{
    drumkitComboBox = std::make_unique<juce::ComboBox>();
    drumkitComboBox->addListener(this);
    addAndMakeVisible(*drumkitComboBox);

    sfzFileComboBox = std::make_unique<juce::ComboBox>();
    sfzFileComboBox->addListener(this);
    addAndMakeVisible(*sfzFileComboBox);

    drumkitLabel = std::make_unique<juce::Label>("", "Drumkit:");
    drumkitLabel->setJustificationType(juce::Justification::centredLeft);
    // Hide drumkit label - now handled in MainContentComponent Row 3
    // addAndMakeVisible(*drumkitLabel);

    sfzFileLabel = std::make_unique<juce::Label>("", "SFZ File:");
    sfzFileLabel->setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(*sfzFileLabel);

    refreshButton = std::make_unique<juce::TextButton>("Refresh");
    refreshButton->onClick = [this]() { refreshDrumkits(); };
    addAndMakeVisible(*refreshButton);

    loadButton = std::make_unique<juce::TextButton>("Load");
    loadButton->onClick = [this]() { loadSelectedDrumkit(); };
    addAndMakeVisible(*loadButton);

    editButton = std::make_unique<juce::TextButton>("Edit");
    editButton->onClick = [this]() {
        if (!drumKitEditorWindow) {
            drumKitEditorWindow = std::make_unique<DrumKitEditorWindow>(
                sfzEngine,
                colorScheme,
                fontManager,
                layoutManager,
                dataManager
            );
        }

        drumKitEditorWindow->setVisible(true);
        drumKitEditorWindow->toFront(true);

        if (onEditRequested)
            onEditRequested();
    };

    addAndMakeVisible(*editButton);

    mixerButton = std::make_unique<juce::TextButton>("Mixer");
    mixerButton->onClick = [this]() {
        if (!drumKitMixerWindow && mixer) {
            drumKitMixerWindow = std::make_unique<DrumKitMixerWindow>(
                *mixer,
                sfzEngine,
                colorScheme,
                fontManager,
                layoutManager,
                dataManager
            );
        }
        
        if (drumKitMixerWindow) {
            drumKitMixerWindow->setVisible(true);
            drumKitMixerWindow->toFront(true);
        }
        
        if (onMixerRequested)
            onMixerRequested();
    };
    addAndMakeVisible(*mixerButton);

    powerButton = std::make_unique<juce::ToggleButton>("Power");
    powerButton->onClick = [this]() {
        if (onPowerStateChanged)
            onPowerStateChanged(powerButton->getToggleState());
    };
    addAndMakeVisible(*powerButton);

    colorScheme.addListener(this);

    updateDrumkitList();
}

DrumKitSectionComponent::~DrumKitSectionComponent()
{
    colorScheme.removeListener(this);
}

void DrumKitSectionComponent::lookAndFeelChanged() {
    if (drumkitComboBox) drumkitComboBox->repaint();
    if (sfzFileComboBox) sfzFileComboBox->repaint();
    if (drumkitLabel) drumkitLabel->repaint();
    if (sfzFileLabel) sfzFileLabel->repaint();
    if (refreshButton) refreshButton->repaint();
   if (loadButton) loadButton->repaint();
   if (editButton) editButton->repaint();
   if (mixerButton) mixerButton->repaint();
   if (powerButton) powerButton->repaint();

   repaint();
}

void DrumKitSectionComponent::paint(juce::Graphics& g)
{
   g.fillAll(colorScheme.getBackgroundColor());

   g.setColour(colorScheme.getColor(ColorScheme::ColorRole::Separator));
   g.drawRect(getLocalBounds(), INIConfig::LayoutConstants::drumKitSectionBorderThickness);
}

void DrumKitSectionComponent::resized()
{
   auto bounds = getLocalBounds().reduced(layoutManager.scaled(INIConfig::LayoutConstants::drumKitSectionMargin));
   const int labelHeight = layoutManager.scaled(INIConfig::LayoutConstants::drumKitLabelHeight);
   const int comboHeight = layoutManager.scaled(INIConfig::LayoutConstants::drumKitComboHeight);
   const int buttonHeight = layoutManager.scaled(INIConfig::LayoutConstants::drumKitButtonHeight);
   const int spacing = layoutManager.scaled(INIConfig::LayoutConstants::drumKitSpacing);

   auto powerBounds = bounds.removeFromTop(buttonHeight);
   powerButton->setBounds(powerBounds.removeFromRight(layoutManager.scaled(INIConfig::LayoutConstants::drumKitPowerButtonWidth)));
   bounds.removeFromTop(spacing);

   // Hide drumkit label - now handled in MainContentComponent Row 3
   // drumkitLabel->setBounds(bounds.removeFromTop(labelHeight));
   bounds.removeFromTop(spacing);
   drumkitComboBox->setBounds(bounds.removeFromTop(comboHeight));
   bounds.removeFromTop(spacing * 2);

   sfzFileLabel->setBounds(bounds.removeFromTop(labelHeight));
   bounds.removeFromTop(spacing);
   sfzFileComboBox->setBounds(bounds.removeFromTop(comboHeight));
   bounds.removeFromTop(spacing * 2);

   auto buttonBounds = bounds.removeFromTop(buttonHeight);
   const int buttonWidth = buttonBounds.getWidth() / INIConfig::LayoutConstants::drumKitButtonColumns - spacing;
   refreshButton->setBounds(buttonBounds.removeFromLeft(buttonWidth));
   buttonBounds.removeFromLeft(spacing);
   loadButton->setBounds(buttonBounds.removeFromLeft(buttonWidth));
   buttonBounds.removeFromLeft(spacing);
   editButton->setBounds(buttonBounds.removeFromLeft(buttonWidth));
   buttonBounds.removeFromLeft(spacing);
   mixerButton->setBounds(buttonBounds);
}

void DrumKitSectionComponent::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
   if (comboBoxThatHasChanged == drumkitComboBox.get())
   {
       updateSFZFileList();

       if (onKitChanged)
       {
           int selectedIndex = drumkitComboBox->getSelectedItemIndex();
           onKitChanged(selectedIndex);
       }
   }
   else if (comboBoxThatHasChanged == sfzFileComboBox.get())
   {
       loadSelectedDrumkit();
   }
}

void DrumKitSectionComponent::updateDrumkitList()
{
   drumkitComboBox->clear();

   auto availableDrumkits = sfzEngine.getAvailableDrumkits();

   int itemId = 1;
   for (const auto& drumkit : availableDrumkits)
   {
       drumkitComboBox->addItem(drumkit.name, itemId++);
   }

   juce::String currentDrumkit = sfzEngine.getCurrentDrumkitName();
   if (currentDrumkit.isNotEmpty())
   {
       for (int i = 0; i < drumkitComboBox->getNumItems(); ++i)
       {
           if (drumkitComboBox->getItemText(i) == currentDrumkit)
           {
               drumkitComboBox->setSelectedItemIndex(i, juce::dontSendNotification);
               break;
           }
       }
   }
}

void DrumKitSectionComponent::updateSFZFileList()
{
   sfzFileComboBox->clear();

   juce::String selectedDrumkit = drumkitComboBox->getText();
   if (selectedDrumkit.isEmpty()) return;

   auto availableDrumkits = sfzEngine.getAvailableDrumkits();

   for (const auto& drumkit : availableDrumkits)
   {
       if (drumkit.name == selectedDrumkit)
       {
           int itemId = 1;
           for (const auto& sfzFile : drumkit.sfzFiles)
           {
               sfzFileComboBox->addItem(sfzFile, itemId++);
           }

           juce::String currentSFZ = sfzEngine.getCurrentSFZFile();
           if (currentSFZ.isNotEmpty())
           {
               for (int i = 0; i < sfzFileComboBox->getNumItems(); ++i)
               {
                   if (sfzFileComboBox->getItemText(i) == currentSFZ)
                   {
                       sfzFileComboBox->setSelectedItemIndex(i, juce::dontSendNotification);
                       break;
                   }
               }
           }
           else if (sfzFileComboBox->getNumItems() > 0)
           {
               sfzFileComboBox->setSelectedItemIndex(0, juce::dontSendNotification);
           }

           break;
       }
   }
}

void DrumKitSectionComponent::loadSelectedDrumkit()
{
   juce::String drumkitName = drumkitComboBox->getText();
   juce::String sfzFileName = sfzFileComboBox->getText();

   if (drumkitName.isNotEmpty() && sfzFileName.isNotEmpty())
   {
       sfzEngine.loadDrumkit(drumkitName, sfzFileName);
   }
}

void DrumKitSectionComponent::refreshDrumkits()
{
   sfzEngine.scanDrumkitsFolder();
   updateDrumkitList();
   updateSFZFileList();
}

void DrumKitSectionComponent::saveStates(ComponentState& state)
{
   if (!INIConfig::isValidPlayerIndex(currentPlayerIndex))
       return;

   auto& playerSettings = state.playerSettings[currentPlayerIndex];

   juce::String selectedDrumkit = drumkitComboBox->getText();
   if (selectedDrumkit.isNotEmpty())
   {
       playerSettings.selectedDrumkit = selectedDrumkit;
   }

   juce::String selectedSFZ = sfzFileComboBox->getText();
   if (selectedSFZ.isNotEmpty())
   {
       playerSettings.assignedMidiFiles[15] = selectedSFZ;
   }

   playerSettings.enabled = powerButton->getToggleState();

   playerSettings.selectedButton = INIConfig::clampButtonIndex(drumkitComboBox->getSelectedItemIndex());
}

void DrumKitSectionComponent::loadStates(const ComponentState& state)
{
   if (!INIConfig::isValidPlayerIndex(currentPlayerIndex))
       return;

   const auto& playerSettings = state.playerSettings[currentPlayerIndex];

   if (playerSettings.selectedDrumkit.isNotEmpty())
   {
       for (int i = 0; i < drumkitComboBox->getNumItems(); ++i)
       {
           if (drumkitComboBox->getItemText(i) == playerSettings.selectedDrumkit)
           {
               drumkitComboBox->setSelectedItemIndex(i, juce::dontSendNotification);
               break;
           }
       }
   }

   updateSFZFileList();

   juce::String storedSFZ = playerSettings.assignedMidiFiles[15];
   if (storedSFZ.isNotEmpty())
   {
       for (int i = 0; i < sfzFileComboBox->getNumItems(); ++i)
       {
           if (sfzFileComboBox->getItemText(i) == storedSFZ)
           {
               sfzFileComboBox->setSelectedItemIndex(i, juce::dontSendNotification);
               break;
           }
       }
   }

   powerButton->setToggleState(playerSettings.enabled, juce::dontSendNotification);

   if (playerSettings.selectedDrumkit.isNotEmpty())
   {
       sfzEngine.setPlayerDrumkit(currentPlayerIndex, playerSettings.selectedDrumkit);
   }
}

void DrumKitSectionComponent::setCurrentPlayerIndex(int index)
{
    currentPlayerIndex = INIConfig::clampPlayerIndex(index);
    
    if (drumKitMixerWindow && drumKitMixerWindow->isVisible()) {
        ComponentState state;
        if (dataManager.loadAllSettings(state)) {
            drumKitMixerWindow->setCurrentPlayerIndex(currentPlayerIndex);
        }
    }
}

void DrumKitSectionComponent::themeChanged(const juce::String& newThemeName)
{
   juce::ignoreUnused(newThemeName);
   repaint();
}

void DrumKitSectionComponent::colorChanged(const juce::String& component,
                                         const juce::String& property,
                                         const juce::Colour& newColor)
{
   juce::ignoreUnused(component, property, newColor);
   repaint();
}
