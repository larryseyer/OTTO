#pragma once
#include "RowComponentBase.h"
#include "UtilityComponents.h"

class MidiEngine;
class INIDataManager;

struct PresetMenuMapping {
    juce::String presetName;
    juce::String categoryName;
    int menuId;
};

class Row1Component : public RowComponentBase {
public:
    Row1Component(MidiEngine& midiEngine,
                 juce::AudioProcessorValueTreeState& valueTreeState,
                 ResponsiveLayoutManager& layoutManager,
                 FontManager& fontManager,
                 ColorScheme& colorScheme);
    
    ~Row1Component() override = default;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    void saveStates(ComponentState& state) override;
    void loadStates(const ComponentState& state) override;
    void updateFromState(const ComponentState& state) override;
    juce::String getRowName() const override { return "TopBar"; }
    juce::Rectangle<int> getRowBounds() const override;
    void lookAndFeelChanged() override;
    void mouseDown(const juce::MouseEvent& event) override;

    std::function<void()> onGearButtonClicked;
    std::function<void(bool)> onPlayStateChanged;
    std::function<void(float)> onTempoChanged;
    std::function<void(int)> onPresetChanged;
    std::function<void()> onLinkButtonClicked;
    std::function<void()> onCloudButtonClicked;

    int getPresetSelection() const;
    void setPresetSelection(int index);
    bool getPlayState() const;
    void setPlayState(bool playing);
    float getTempo() const;
    void setTempo(float tempo);

    void setMidiClockInEnabled(bool enabled);
    void setMidiClockOutEnabled(bool enabled);
    bool isMidiClockInEnabled() const;
    bool isMidiClockOutEnabled() const;
    void updateClockSyncStatus(bool isExternalSync, double externalTempo);

    void setINIDataManager(INIDataManager* manager);
    juce::String getCurrentPresetName() const;

    void tapTempo();
    bool isLiveRecording() const;
    void setLiveRecording(bool recording);
    bool isOverdubEnabled() const;
    void setOverdubEnabled(bool enabled);
    bool isLoopRecordingEnabled() const;
    void setLoopRecordingEnabled(bool enabled);

    void refreshPresetLabelFont();
    
    // PHASE 9D: Theme management integration
    void setThemeManager(class ThemeManager* themeManager);
    void showThemeSelector(bool show);
    bool isThemeSelectorVisible() const;

private:
    MidiEngine& midiEngine;
    juce::AudioProcessorValueTreeState& valueTreeState;
    
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
    juce::Label presetDisplayLabel;
    EditableNumericLabel bpmLabel;
    juce::Label ottoLabel;
    juce::Label versionLabel;
    juce::Label clockSyncLabel;
    juce::Label tapTempoLabel;
    SeparatorComponent bottomSeparator;
    
    // PHASE 9D: Theme selector components
    PhosphorIconButton themeButton;
    HierarchicalComboBox themeSelector;
    class ThemeManager* themeManager = nullptr;  // Non-owning pointer
    bool themeSelectorVisible = false;

    bool isPlaying = false;
    bool liveRecording = false;
    bool overdubEnabled = false;
    bool loopRecordingEnabled = false;
    bool midiClockInEnabled = false;
    bool midiClockOutEnabled = false;
    bool isExternalSyncActive = false;
    bool cloudAuthenticated = false;
    bool cloudAutoSync = false;
    bool collaborationActive = false;
    bool showingPresetLabel = true;

    juce::String currentPresetName = "Default";
    juce::String cloudUsername;
    juce::String collaborationSessionName;

    int tapCount = 0;
    double lastTapTime = 0.0;

    INIDataManager* iniDataManager = nullptr;
    juce::Array<PresetMenuMapping> presetMenuMapping;

    void setupTopBarComponents();
    void setupPresets();
    void buildHierarchicalPresetMenu();
    void handlePresetMenuSelection(int result);
    int getPresetIndexFromName(const juce::String& presetName) const;
    juce::StringArray getAllPresetNames() const;
    juce::Array<juce::String> getPresetCategoriesFromFilesystem() const;
    juce::StringArray getPresetsInCategory(const juce::String& categoryName) const;
    void ensureDefaultPresetStructure() const;
    void createSamplePresetStructure() const;
    void handlePresetChevrons(bool isRight);
    void updatePlayButtons();
    void updateTapTempoDisplay();
    void togglePresetDisplay();
    void showPresetLabel();
    void showPresetMenu();
    void updatePresetDisplayToggle();
    void updateRecordButton();
    void updateLinkButtonVisuals();
    void showLinkSyncMenu();
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
    void notifyStateChanged();
    
    // PHASE 9D: Theme management helper methods
    void setupThemeSelector();
    void updateThemeSelector();
    void handleThemeSelection(int themeId);
    void toggleThemeSelector();
    void populateThemeSelector();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Row1Component)
};
