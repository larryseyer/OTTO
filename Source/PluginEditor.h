#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
// TopBarComponent and PlayerTabsComponent includes removed - functionality now in MainContentComponent
#include "PopupWindowsDrumKit.h"
#include "ColorScheme.h"
#include "ResponsiveLayoutManager.h"
#include "FontManager.h"
#include "ComponentState.h"
#include "INIDataManager.h"
#include "CustomLookAndFeel.h"
#include "MainContentComponent.h"


class OTTOAudioProcessorEditor : public juce::AudioProcessorEditor,
                                 public juce::Button::Listener,
                                 public juce::Slider::Listener,
                                 public juce::ComboBox::Listener,
                                 public ColorScheme::Listener,
                                 private juce::Timer
{
public:
    OTTOAudioProcessorEditor(OTTOAudioProcessor&);
    ~OTTOAudioProcessorEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;
    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;
    void comboBoxChanged(juce::ComboBox* comboBox) override;
    void themeChanged(const juce::String& newThemeName) override;
    void colorChanged(const juce::String& component, const juce::String& property, const juce::Colour& newColor) override;
    void timerCallback() override;
    void saveEditorState();
private:
    OTTOAudioProcessor& audioProcessor;
    std::unique_ptr<ColorScheme> colorScheme;
    std::unique_ptr<ResponsiveLayoutManager> layoutManager;
    std::unique_ptr<FontManager> fontManager;
    std::unique_ptr<INIDataManager> dataManager;
    std::unique_ptr<CustomLookAndFeel> customLookAndFeel;
    // TopBar and PlayerTabs functionality now handled by Row1Component and Row2Component in MainContentComponent
    // Removed standalone component declarations to eliminate duplicate UI elements
    std::unique_ptr<DrumKitSelectionWindow> drumKitPopup;
    std::unique_ptr<DrumKitEditorWindow> drumKitEditorWindow;
    std::unique_ptr<DrumKitMixerWindow> drumKitMixerWindow;
    std::unique_ptr<MainContentComponent> mainContent;
    std::unique_ptr<juce::Component> splashOverlay;
    ComponentState componentState;
    std::unique_ptr<SettingsPanelWindow> settingsPanel;

    void showSettingsPanel();
    void showDrumKitPopup();
    void showDrumKitEditorWindow();
    void showMixerPopup();
    int currentPlayerIndex = INIConfig::Defaults::DEFAULT_CURRENT_PLAYER;
    bool isInitialized = false;
    void initializeManagers();
    void createComponents();
    void setupCallbacks();
    void updateFromProcessor();
    void updateProcessorFromState();
    void applyColorScheme();
    void handlePlayerChange(int newPlayerIndex);
    void handleTransportStateChange();
    void handlePatternSelection(int patternIndex);
    void saveAllComponentStates();
    void loadAllComponentStates();
    void updateUIFromState();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OTTOAudioProcessorEditor)
};
