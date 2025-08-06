#include "PopupWindows.h"
#include "PopupWindowsAudio.h"
#include "PopupWindowsMidi.h"
#include "PopupWindowsTheme.h"
#include "PopupWindowsPerformance.h"
#include "PopupWindowsBackup.h"
#include "INIConfig.h"

SettingsPanelWindow::SettingsPanelWindow(FontManager& fontManager,
                                        ColorScheme& colorScheme,
                                        ResponsiveLayoutManager& layoutManager,
                                        INIDataManager& INIManager,
                                        MidiEngine* midiEngine,
                                        juce::AudioDeviceManager* deviceManager)
    : fontManager(fontManager), colorScheme(colorScheme), layoutManager(layoutManager),
      INIManager(INIManager), midiEngine(midiEngine), deviceManager(deviceManager),
      closeButton("close"),
      tabbedComponent(juce::TabbedButtonBar::TabsAtTop)
{
    setupComponents();

    ComponentState state;
    if (INIManager.loadAllSettings(state)) {
        loadStates(state);
    }
}

SettingsPanelWindow::~SettingsPanelWindow() {
    try {
        ComponentState state;
        if (INIManager.loadAllSettings(state)) {
            saveStates(state);
            INIManager.saveAllSettings(state);
        }
    } catch (const std::exception& e) {
        // Log error but don't crash during destruction
        DBG("Error saving settings in destructor: " << e.what());
    } catch (...) {
        // Catch any other exceptions during destruction
        DBG("Unknown error saving settings in destructor");
    }
}

void SettingsPanelWindow::setupComponents() {
    addAndMakeVisible(titleLabel);
    addAndMakeVisible(closeButton);
    addAndMakeVisible(tabbedComponent);

    titleLabel.setText("Settings", juce::dontSendNotification);
    titleLabel.setColour(juce::Label::textColourId, colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setFont(fontManager.getFont(FontManager::FontRole::Header, layoutManager.scaled(INIConfig::LayoutConstants::defaultMargin)));

    closeButton.setColour(juce::TextButton::buttonColourId, colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground));
    closeButton.setColour(juce::TextButton::textColourOnId, colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
    closeButton.setColour(juce::TextButton::textColourOffId, colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));

    closeButton.onClick = [this] {
        ComponentState state;
        if (INIManager.loadAllSettings(state)) {
            saveStates(state);
            INIManager.saveAllSettings(state);
        }

        setVisible(false);
        if (onClose) {
            onClose();
        }
    };

    audioSettingsTab = std::make_unique<AudioSettingsTab>(colorScheme, layoutManager, fontManager, deviceManager);
    midiMappingTab = std::make_unique<MidiMappingTab>(colorScheme, layoutManager, fontManager, midiEngine);
    themeEditorTab = std::make_unique<ThemeEditorTab>(colorScheme, layoutManager, fontManager, INIManager);
    performanceTab = std::make_unique<PerformanceTab>(colorScheme, layoutManager, fontManager);
    backupRestoreTab = std::make_unique<BackupRestoreTab>(colorScheme, layoutManager, fontManager, INIManager);

    tabbedComponent.addTab("Audio", colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground),
                          audioSettingsTab.get(), false);
    tabbedComponent.addTab("MIDI Mapping", colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground),
                          midiMappingTab.get(), false);
    tabbedComponent.addTab("Theme", colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground),
                          themeEditorTab.get(), false);
    tabbedComponent.addTab("Performance", colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground),
                          performanceTab.get(), false);
    tabbedComponent.addTab("Backup/Restore", colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground),
                          backupRestoreTab.get(), false);

    themeEditorTab->onThemeChanged = [this] {
        if (onColorSchemeChanged) {
            onColorSchemeChanged();
        }
    };
}

void SettingsPanelWindow::saveStates(ComponentState& state) {
    state.dropdownSelections["settingsTab"] = tabbedComponent.getCurrentTabIndex();
    saveAllTabStates(state);
}

void SettingsPanelWindow::loadStates(const ComponentState& state) {
    auto tabIt = state.dropdownSelections.find("settingsTab");
    if (tabIt != state.dropdownSelections.end()) {
        tabbedComponent.setCurrentTabIndex(tabIt->second);
    }
    loadAllTabStates(state);
}

void SettingsPanelWindow::saveAllTabStates(ComponentState& state) {
    if (audioSettingsTab) {
        audioSettingsTab->saveToAudioSettings(state.audioSettings);
    }

    if (midiMappingTab) {
        midiMappingTab->saveMidiMappings(state);
    }

    if (themeEditorTab) {
        themeEditorTab->saveThemeSettings(state.themeSettings);
    }

    if (performanceTab) {
        performanceTab->savePerformanceSettings(state);
    }
}

void SettingsPanelWindow::loadAllTabStates(const ComponentState& state) {
    if (audioSettingsTab) {
        audioSettingsTab->loadFromAudioSettings(state.audioSettings);
    }

    if (midiMappingTab) {
        midiMappingTab->loadMidiMappings(state);
    }

    if (themeEditorTab) {
        themeEditorTab->loadThemeSettings(state.themeSettings);
    }

    if (performanceTab) {
        performanceTab->loadPerformanceSettings(state);
    }
}

void SettingsPanelWindow::paint(juce::Graphics& g) {
    g.fillAll(colorScheme.getColor(ColorScheme::ColorRole::WindowBackground).withAlpha(0.8f));

    auto bounds = getLocalBounds();
    auto panelBounds = bounds.withSizeKeepingCentre(layoutManager.scaled(INIConfig::LayoutConstants::settingsPanelWidth), layoutManager.scaled(INIConfig::LayoutConstants::settingsPanelHeight));

    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::WindowBackground));
    g.fillRoundedRectangle(panelBounds.toFloat(), layoutManager.scaled(INIConfig::LayoutConstants::settingsPanelCornerRadius));

    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::Separator));
    g.drawRoundedRectangle(panelBounds.toFloat(), layoutManager.scaled(INIConfig::LayoutConstants::settingsPanelCornerRadius), layoutManager.scaled(INIConfig::LayoutConstants::separatorThickness));
}

void SettingsPanelWindow::resized() {
    auto bounds = getLocalBounds();
    auto panelBounds = bounds.withSizeKeepingCentre(layoutManager.scaled(INIConfig::LayoutConstants::settingsPanelWidth), layoutManager.scaled(INIConfig::LayoutConstants::settingsPanelHeight));

    int margin = layoutManager.scaled(INIConfig::LayoutConstants::settingsPanelMargin);
    auto contentBounds = panelBounds.reduced(margin);

    titleLabel.setBounds(contentBounds.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::settingsPanelTitleHeight)));
    contentBounds.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::defaultSpacing));

    int closeButtonSize = layoutManager.scaled(INIConfig::LayoutConstants::settingsPanelCloseButtonSize);
    closeButton.setBounds(panelBounds.getRight() - layoutManager.scaled(INIConfig::LayoutConstants::settingsPanelCloseButtonX),
                         panelBounds.getY() + layoutManager.scaled(INIConfig::LayoutConstants::settingsPanelCloseButtonY),
                         closeButtonSize, closeButtonSize);

    tabbedComponent.setBounds(contentBounds);
}

void SettingsPanelWindow::mouseDown(const juce::MouseEvent& event) {
    auto bounds = getLocalBounds();
    auto panelBounds = bounds.withSizeKeepingCentre(layoutManager.scaled(INIConfig::LayoutConstants::settingsPanelWidth), layoutManager.scaled(INIConfig::LayoutConstants::settingsPanelHeight));

    if (!panelBounds.contains(event.getPosition())) {
        // Save settings safely before closing
        try {
            ComponentState currentState;
            if (INIManager.loadAllSettings(currentState)) {
                saveStates(currentState);
                INIManager.saveAllSettings(currentState);
            }
        } catch (const std::exception& e) {
            // Log error but don't crash
            DBG("Error saving settings: " << e.what());
        } catch (...) {
            // Catch any other exceptions
            DBG("Unknown error saving settings");
        }

        // Close the window
        if (auto* parentWindow = findParentComponentOfClass<juce::DialogWindow>()) {
            parentWindow->exitModalState(0);
        } else {
            setVisible(false);
            if (onClose) {
                onClose();
            }
        }
    }
}
