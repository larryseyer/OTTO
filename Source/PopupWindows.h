#pragma once
#include <JuceHeader.h>
#include "FontManager.h"
#include "ColorScheme.h"
#include "ResponsiveLayoutManager.h"
#include "INIDataManager.h"
#include "ComponentState.h"
#include "UtilityComponents.h"
#include "Mixer.h"
#include "MidiEngine.h"

class SFZEngine;

class AudioSettingsTab;
class MidiMappingTab;
class ThemeEditorTab;
class PerformanceTab;
class BackupRestoreTab;

class SettingsPanelWindow : public juce::Component {
public:
    SettingsPanelWindow(FontManager& fontManager,
                       ColorScheme& colorScheme,
                       ResponsiveLayoutManager& layoutManager,
                       INIDataManager& INIManager,
                       MidiEngine* midiEngine = nullptr,
                       juce::AudioDeviceManager* deviceManager = nullptr);

    ~SettingsPanelWindow() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& event) override;

    void saveStates(ComponentState& state);
    void loadStates(const ComponentState& state);

    std::function<void(FontManager::PhosphorWeight)> onPhosphorWeightChanged;
    std::function<void()> onColorSchemeChanged;
    std::function<void()> onClose;

private:
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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsPanelWindow)
};

class DrumKitEditorWindow : public juce::DocumentWindow {
public:
    DrumKitEditorWindow(SFZEngine& sfzEngine,
                       ColorScheme& colorScheme,
                       FontManager& fontManager,
                       ResponsiveLayoutManager& layoutManager,
                       INIDataManager& iniManager);

    ~DrumKitEditorWindow() override;

    void closeButtonPressed() override;

    void saveStates(ComponentState& state);
    void loadStates(const ComponentState& state);

private:
    SFZEngine& sfzEngine;
    ColorScheme& colorScheme;
    FontManager& fontManager;
    ResponsiveLayoutManager& layoutManager;
    INIDataManager& iniManager;

    std::unique_ptr<juce::Component> content;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DrumKitEditorWindow)
};

class DrumKitMixerWindow : public juce::DocumentWindow {
public:
    DrumKitMixerWindow(Mixer& mixer,
                      SFZEngine& sfzEngine,
                      ColorScheme& colorScheme,
                      FontManager& fontManager,
                      ResponsiveLayoutManager& layoutManager,
                      INIDataManager& iniManager);

    ~DrumKitMixerWindow() override;

    void closeButtonPressed() override;
    void saveStates(ComponentState& state);
    void loadStates(const ComponentState& state);
    void setCurrentPlayerIndex(int playerIndex);

private:
    class MixerContent;
    std::unique_ptr<MixerContent> content;

    Mixer& mixer;
    SFZEngine& sfzEngine;
    ColorScheme& colorScheme;
    FontManager& fontManager;
    ResponsiveLayoutManager& layoutManager;
    INIDataManager& iniManager;
    
    int currentPlayerIndex = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DrumKitMixerWindow)
};

class GrooveAndFillsWindow : public juce::DocumentWindow {
public:
    GrooveAndFillsWindow(INIDataManager& INIManager,
                        ColorScheme& colorScheme);

    ~GrooveAndFillsWindow() override;

    void closeButtonPressed() override;

    void saveStates(ComponentState& state);
    void loadStates(const ComponentState& state);

private:
    class GrooveContent;
    std::unique_ptr<GrooveContent> content;
    INIDataManager& INIManager;
    ColorScheme& colorScheme;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GrooveAndFillsWindow)
};

class PatternGroupEditorWindow : public juce::DocumentWindow {
public:
    PatternGroupEditorWindow(int patternGroupIndex,
                           INIDataManager* iniDataManager,
                           ColorScheme& colorScheme,
                           FontManager& fontManager);

    ~PatternGroupEditorWindow() override;

    void closeButtonPressed() override;

    void saveStates(ComponentState& state);
    void loadStates(const ComponentState& state);

    std::function<void(int)> onPatternGroupChanged;

private:
    class PatternGroupEditorContent;
    std::unique_ptr<PatternGroupEditorContent> content;
    
    int patternGroupIndex;
    INIDataManager* iniDataManager;
    ColorScheme& colorScheme;
    FontManager& fontManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatternGroupEditorWindow)
};
