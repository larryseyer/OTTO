#pragma once

#include <JuceHeader.h>
#include "UtilityComponents.h"
#include "ResponsiveLayoutManager.h"
#include "FontManager.h"
#include "ColorScheme.h"
#include "ComponentState.h"
#include "INIConfig.h"

class MidiEngine;
class INIDataManager;

class TopBarComponent : public juce::Component {
public:
    TopBarComponent(MidiEngine& midiEngine,
                   juce::AudioProcessorValueTreeState& valueTreeState,
                   ResponsiveLayoutManager& layoutManager,
                   FontManager& fontManager,
                   ColorScheme& colorScheme);

    void paint(juce::Graphics&) override;
    void resized() override;

    std::function<void()> onGearButtonClicked;
    std::function<void(bool)> onPlayStateChanged;
    std::function<void(float)> onTempoChanged;
    std::function<void(int)> onPresetChanged;
    std::function<void()> onLinkButtonClicked;
    std::function<void()> onCloudButtonClicked;

    int getPresetSelection() const;
    void setPresetSelection(int index);
    bool getPlayState() const { return isPlaying; }
    void setPlayState(bool playing);
    float getTempo() const;
    void setTempo(float tempo);

    void setMidiClockInEnabled(bool enabled);
    void setMidiClockOutEnabled(bool enabled);
    bool isMidiClockInEnabled() const { return midiClockInEnabled; }
    bool isMidiClockOutEnabled() const { return midiClockOutEnabled; }
    void updateClockSyncStatus(bool isExternalSync, double externalTempo);

    void setINIDataManager(INIDataManager* manager) { iniDataManager = manager; }
    void saveStates(ComponentState& state);
    void loadStates(const ComponentState& state);
    juce::String getCurrentPresetName() const { return currentPresetName; }

    void tapTempo();
    bool isLiveRecording() const { return liveRecording; }
    void setLiveRecording(bool recording);
    bool isOverdubEnabled() const { return overdubEnabled; }
    void setOverdubEnabled(bool enabled);
    bool isLoopRecordingEnabled() const { return loopRecordingEnabled; }
    void setLoopRecordingEnabled(bool enabled);

    void lookAndFeelChanged() override;

private:
    MidiEngine& midiEngine;
    juce::AudioProcessorValueTreeState& valueTreeState;
    ResponsiveLayoutManager& layoutManager;
    FontManager& fontManager;
    ColorScheme& colorScheme;
    INIDataManager* iniDataManager = nullptr;

    PhosphorIconButton gearButton;
    PhosphorIconButton linkButton;
    PhosphorIconButton cloudButton;
    PhosphorIconButton playButton;
    PhosphorIconButton pauseButton;
    PhosphorIconButton leftChevronButton;
    PhosphorIconButton rightChevronButton;

    PhosphorIconButton recordButton;
    PhosphorIconButton tapTempoButton;
    PhosphorIconButton overdubButton;
    PhosphorIconButton loopButton;

    HierarchicalComboBox presetsMenu;
    EditableNumericLabel bpmLabel;
    juce::Label ottoLabel;
    juce::Label versionLabel;
    juce::Label clockSyncLabel;
    juce::Label tapTempoLabel;
    SeparatorComponent bottomSeparator;
    
    // TEMPORARY: Debug label to identify Row 1
    juce::Label row1DebugLabel;

    bool isPlaying = false;
    juce::String currentPresetName = "Default";
    bool midiClockInEnabled = false;
    bool midiClockOutEnabled = false;
    bool isExternalSyncActive = false;

    bool liveRecording = false;
    bool overdubEnabled = false;
    bool loopRecordingEnabled = false;
    double lastTapTime = INIConfig::MIDI::DEFAULT_LAST_TIME;
    int tapCount = INIConfig::Defaults::ZERO_VALUE;

    bool cloudAuthenticated = false;
    bool cloudAutoSync = false;
    bool collaborationActive = false;
    juce::String cloudUsername;
    juce::String collaborationSessionName;
    double backupProgress = INIConfig::MIDI::DEFAULT_POSITION;

    struct PresetMenuMapping {
        juce::String presetName;
        juce::String categoryName;
        int menuId;
    };
    juce::Array<PresetMenuMapping> presetMenuMapping;

    void setupComponents();
    void setupPresets();
    void buildHierarchicalPresetMenu();
    void handlePresetMenuSelection(int result);
    juce::Array<juce::String> getPresetCategoriesFromFilesystem() const;
    juce::StringArray getPresetsInCategory(const juce::String& categoryName) const;
    void ensureDefaultPresetStructure() const;
    void createSamplePresetStructure() const;
    void handlePresetChevrons(bool isRight);
    void updatePlayButtons();
    void updateLinkButtonVisuals();
    void showLinkSyncMenu();
    int getPresetIndexFromName(const juce::String& presetName) const;
    juce::StringArray getAllPresetNames() const;
    void notifyStateChanged();

    void updateRecordButton();
    void updateTapTempoDisplay();

    void showCloudMenu();
    void updateCloudButtonVisuals();
    void showCloudAuthDialog();
    void shareCurrentPattern();
    void shareCurrentDrumKit();
    void startCollaborationSession();
    void joinCollaborationSession();
    void leaveCollaborationSession();
    void backupToCloud();
    void restoreFromCloud();
    void showShareSuccessMessage();
    void showBackupSuccessMessage();
    void showCollaborationCode();
    juce::String generateShareCode();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopBarComponent)
};
