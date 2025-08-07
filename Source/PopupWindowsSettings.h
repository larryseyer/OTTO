#pragma once
#include <JuceHeader.h>
#include "PopupWindows.h"
#include "FontManager.h"
#include "ColorScheme.h"
#include "ResponsiveLayoutManager.h"
#include "INIDataManager.h"
#include "ComponentState.h"
#include "UtilityComponents.h"
#include "MidiEngine.h"
#include "INIConfig.h"

class AudioSettingsTab;
class MidiMappingTab;
class ThemeEditorTab;
class PerformanceTab;
class BackupRestoreTab;

class SettingsPanelWindowImpl {
public:
    SettingsPanelWindowImpl(SettingsPanelWindow& owner,
                           FontManager& fontManager,
                           ColorScheme& colorScheme,
                           ResponsiveLayoutManager& layoutManager,
                           INIDataManager& INIManager,
                           MidiEngine* midiEngine,
                           juce::AudioDeviceManager* deviceManager);

    ~SettingsPanelWindowImpl();

    void paint(juce::Graphics&);
    void resized();
    void mouseDown(const juce::MouseEvent& event);

    void saveStates(ComponentState& state);
    void loadStates(const ComponentState& state);

private:
    SettingsPanelWindow& owner;
    FontManager& fontManager;
    ColorScheme& colorScheme;
    ResponsiveLayoutManager& layoutManager;
    INIDataManager& INIManager;
    MidiEngine* midiEngine;
    juce::AudioDeviceManager* deviceManager;

    juce::Label titleLabel;
    PhosphorIconButton closeButton;
    juce::TabbedComponent tabbedComponent;

    std::unique_ptr<AudioSettingsTab> audioSettingsTab;
    std::unique_ptr<MidiMappingTab> midiMappingTab;
    std::unique_ptr<ThemeEditorTab> themeEditorTab;
    std::unique_ptr<PerformanceTab> performanceTab;
    std::unique_ptr<BackupRestoreTab> backupRestoreTab;

    void setupComponents();
    void saveAllTabStates(ComponentState& state);
    void loadAllTabStates(const ComponentState& state);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsPanelWindowImpl)
};
