#pragma once

#include <JuceHeader.h>
#include "UtilityComponents.h"
#include "ResponsiveLayoutManager.h"
#include "FontManager.h"
#include "ColorScheme.h"
#include "ComponentState.h"
#include "INIConfig.h"
#include "PlatformResponsiveManager.h"

// Phase 9D Component Includes
#include "UI/Themes/ThemeManager.h"
#include "UI/Responsive/AdaptiveLayoutManager.h"
#include "UI/Touch/GestureRecognizer.h"
#include "UI/Visualizations/SpectrumAnalyzer.h"
#include "UI/Visualizations/WaveformDisplay.h"


class MidiEngine;
class Mixer;
class INIDataManager;


class SceneLauncherComponent;
class LoopSectionComponent;
class Row1Component;
class Row2Component;
class Row3Component;
class Row4Component;
class Row5Component;
class Row6Component;
class AnimationManager;
class KeyboardHandler;
class ContextMenuManager;
class RenderOptimizer;
class PlatformResponsiveManager;

// PHASE 6: Performance Optimization Cache
struct PatternGroupCache {
    juce::HashMap<int, juce::String> groupNameCache;
    juce::HashMap<int, float> swingCache;
    juce::HashMap<int, float> energyCache;
    double lastCacheUpdate = 0.0;
    bool cacheValid = false;
};

class MainContentComponent : public juce::Component,
                              public juce::Button::Listener,
                              public juce::ComboBox::Listener,
                              public ThemeManager::Listener,
                              public GestureRecognizer::Listener {
public:
    MainContentComponent(MidiEngine& midiEngine,
                        Mixer& mixer,
                        juce::AudioProcessorValueTreeState& valueTreeState,
                        ResponsiveLayoutManager& layoutManager,
                        FontManager& fontManager,
                        ColorScheme& colorScheme);

    ~MainContentComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void lookAndFeelChanged() override;
    
    // juce::Button::Listener
    void buttonClicked(juce::Button* button) override;
    
    // juce::ComboBox::Listener
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
    
    // ThemeManager::Listener
    void themeChanged(const juce::String& newThemeName) override;
    void themePreviewStarted(const juce::String& previewThemeName) override;
    void themePreviewStopped() override;
    
    // GestureRecognizer::Listener
    void gestureDetected(GestureRecognizer::GestureType type, const GestureRecognizer::GestureData& data) override;

    void updatePlayerDisplay(int playerIndex);

    float getSwingValue() const;
    void setSwingValue(float value);
    float getEnergyValue() const;
    void setEnergyValue(float value);
    float getVolumeValue() const;
    void setVolumeValue(float value);

    bool getEditMode() const;
    void setEditModeVisuals(bool isEditing);
    juce::String getMidiFileAssignment(int buttonIndex) const;
    void setMidiFileAssignment(int buttonIndex, const juce::String& midiFile);

    void saveStates(ComponentState& state);
    void loadStates(const ComponentState& state);

    int getFavoritesSelection() const;
    void setFavoritesSelection(int selection);

    void setINIDataManager(INIDataManager* manager) { 
        iniDataManager = manager; 
        // Initialize pattern groups when INI manager is available
        initializePatternGroups();
    }
    
    void setupRow4LabelIntegration();

    void updateFromState(const ComponentState& state);
    void switchToPlayer(int playerIndex, const ComponentState& state);




    void setLivePerformanceMode(bool enabled);
    bool isLivePerformanceMode() const { return livePerformanceMode; }
    void showSceneLauncher(bool show);
    
    void updateDrumKitList(const juce::StringArray& drumkitNames);
    void setSelectedDrumKit(const juce::String& drumkitName);
    void updateDrumKitDisplayToggle();
    void showDrumKitLabel();
    void showDrumKitMenu();
    
    // Row3Component interface methods
    void setDrumKitEditMode(bool editMode);
    bool isDrumKitEditMode() const;
    void setDrumKitMuteState(bool muted);
    bool isDrumKitMuted() const;
    
    // Row6Component interface methods
    float getLoopPosition() const;
    void setLoopPosition(float position);
    
    void mouseDown(const juce::MouseEvent& event) override;

    std::function<void(bool)> onEditModeChanged;
    std::function<void(int, const juce::String&)> onMidiFileChanged;
    std::function<void(const juce::String&, float)> onSliderValueChanged;
    std::function<void()> onGrooveAndFillsRequested;
    std::function<void(ComponentState&)> onStateChanged;
    std::function<void()> onDrumKitPopupRequested;
    std::function<void()> onMixerPopupRequested;
    std::function<void(const juce::String&)> onDrumKitChanged;

private:
    MidiEngine& midiEngine;
    Mixer& mixer;
    juce::AudioProcessorValueTreeState& valueTreeState;
    ResponsiveLayoutManager& layoutManager;
    FontManager& fontManager;
    ColorScheme& colorScheme;

    juce::Label rhythmLabel;

    
    // TEMPORARY: Row identification labels for debugging layout
    juce::Label rowLabel1, rowLabel2, rowLabel3, rowLabel4, rowLabel5, rowLabel6;
    
    // Row 3: Player & DrumKit Controls
    PhosphorIconButton drumKitEditButton;
    PhosphorIconButton drumKitLeftChevron;
    HierarchicalComboBox drumKitDropdown;
    PhosphorIconButton drumKitRightChevron;
    PhosphorIconButton drumKitMuteButton;
    PhosphorIconButton drumKitMixerButton;
    juce::Label drumKitSelectedLabel;  // Shows currently selected drumkit name
    
    bool showingDrumKitLabel = true;  // Toggle state: true = show label, false = show menu
    
    // Row 4: Pattern Group Controls - Complete 10-component layout
    // Components moved from LeftSection and RightSection to create unified Row 4
    PhosphorIconButton patternGroupEditButton;        // Edit/Pencil icon (from LeftSection editButton)
    PhosphorIconButton patternGroupLeftChevron;       // Left chevron (from LeftSection leftChevronButton)
    HierarchicalComboBox patternGroupDropdown;        // Pattern group dropdown (existing)
    PhosphorIconButton patternGroupRightChevron;      // Right chevron (from LeftSection rightChevronButton)
    PhosphorIconButton patternGroupFavoriteButton;    // Favorite icon (from LeftSection favoriteButton)
    juce::Label togglesLabel;                         // Toggles label (from RightSection)
    juce::Label fillsLabel;                           // Fills label (from RightSection)
    juce::Label swingLabel;                           // Swing label (from RightSection)
    juce::Label energyLabel;                          // Energy label (from RightSection)
    juce::Label volumeLabel;                          // Volume label (from RightSection)
    


    std::unique_ptr<SceneLauncherComponent> sceneLauncher;
    std::unique_ptr<LoopSectionComponent> loopSection;
    std::unique_ptr<Row1Component> row1Component;
    std::unique_ptr<Row2Component> row2Component;
    std::unique_ptr<Row3Component> row3Component;
    std::unique_ptr<Row4Component> row4Component;
    std::unique_ptr<Row5Component> row5Component;
    std::unique_ptr<Row6Component> row6Component;
    
    std::unique_ptr<PlatformResponsiveManager> platformManager;
    std::unique_ptr<AnimationManager> animationManager;
    std::unique_ptr<KeyboardHandler> keyboardHandler;
    std::unique_ptr<ContextMenuManager> contextMenuManager;
    std::unique_ptr<RenderOptimizer> renderOptimizer;
    
    // Phase 9D Components
    std::unique_ptr<ThemeManager> themeManager;
    std::unique_ptr<AdaptiveLayoutManager> adaptiveLayoutManager;
    std::unique_ptr<GestureRecognizer> gestureRecognizer;
    std::unique_ptr<SpectrumAnalyzer> spectrumAnalyzer;
    std::unique_ptr<WaveformDisplay> waveformDisplay;
    // Row separators - one between each row for visual debugging
    SeparatorComponent row1Separator;  // Between Row 1 (TopBar) and Row 2 (Player Tabs)
    SeparatorComponent row2Separator;  // Between Row 2 (Player Tabs) and Row 3 (DrumKit Menu)
    SeparatorComponent row3Separator;  // Between Row 3 (DrumKit Menu) and Row 4 (Pattern Menu)
    SeparatorComponent row4Separator;  // Between Row 4 (Pattern Menu) and Row 5 (4x4 Matrix)
    SeparatorComponent row5Separator;  // Between Row 5 (4x4 Matrix) and Row 6 (Loop Controls)

    int currentPlayerIndex = INIConfig::Defaults::DEFAULT_CURRENT_PLAYER;
    INIDataManager* iniDataManager = nullptr;
    bool livePerformanceMode = false;
    
    // Pattern Group Management State
    int currentPatternGroupIndex = 0;
    juce::Array<BeatsButtonGroup> availablePatternGroups;
    bool patternGroupEditMode = false;
    
    // PHASE 6: Performance Optimization Cache
    PatternGroupCache patternGroupCache;

    void validatePlayerIndex(int playerIndex) const;

    void notifyStateChanged(ComponentState& state);
    void updateLayoutForPerformanceMode();
    void setupRow3Components();
    void setupRow3Callbacks();
    void setupRow4Components();
    void setupRow4Callbacks();
    void setupRow4Labels();
    void setupRow5Components();
    void setupRow5Callbacks();
    void setupRow6Components();
    void setupRow6Callbacks();
    void updateRow6ComponentState(const ComponentState& state);
    void updateRow2Layout();
    void updateRow3Layout();
    void updateRow4Layout();
    void updateRow5Layout();
    void updateRow6Layout();
    void handleDrumKitChevrons(bool isRight);
    
    // Pattern Group Management Methods
    void initializePatternGroups();
    void updatePatternGroupDropdown();
    void handlePatternGroupNavigation(bool isNext);
    void togglePatternGroupEditMode();
    void togglePatternGroupFavorite();
    void switchToPatternGroup(int groupIndex);
    BeatsButtonGroup* getCurrentPatternGroup();
    void updateRow4LabelsFromControls();
    void updateRow4LabelStates();
    void refreshPatternGroupUI();
    
    // PHASE 5: Enhanced Row 4 Visual Polish Methods
    void setupRow4Label(juce::Label& label, const juce::String& componentId, 
                       const juce::String& text, const juce::String& tooltip);
    void updatePatternGroupButtonStates();
    void validatePatternGroupIntegrity();
    void handlePatternGroupLoadingState(bool isLoading);
    void createDefaultPatternGroups();
    void updateRow4LabelsForEditMode(bool editModeActive);
    void setupRow4KeyboardNavigation();
    void setupRow4AccessibilityFeatures();
    
    // PHASE 6: Row 5 Beat Grid Integration Methods
    void synchronizePatternGroupWithBeatGrid(int groupIndex);
    void updateBeatGridFromPatternGroup(const BeatsButtonGroup& patternGroup);
    void updatePatternGroupFromBeatGrid();
    void handlePatternGroupTransition(int fromIndex, int toIndex);
    void optimizePatternGroupSwitching();
    void validateBeatGridIntegrity();
    void setupBidirectionalCommunication();
    void handleRealTimePatternUpdates();
    
    // PHASE 6: Performance Optimization Methods
    void updatePatternGroupCache();
    void optimizeDropdownForLargeDatasets();
    void cleanupUnusedPatternData();
    void precacheFrequentPatternGroups();
    
    // Override for keyboard shortcuts
    bool keyPressed(const juce::KeyPress& key) override;
    
    // PHASE 8: Layout helper methods
    void updateRowSeparators();
    void updateDebugLabels();
    
    void setupKeyboardHandlerCallbacks();
    void setupContextMenuCallbacks();
    void setupAnimationManagers();
    void setCurrentPlayerIndex(int index) { currentPlayerIndex = index; }
    
    // PHASE 9D: Integration helper methods
    void initializePhase9DComponents();
    void setupThemeManagerIntegration();
    void setupAdaptiveLayoutIntegration();
    void setupGestureRecognizerIntegration();
    void setupVisualizationIntegration();
    void updateVisualizationBounds();
    void handleGestureInput(GestureRecognizer::GestureType type, const GestureRecognizer::GestureData& data);
    void broadcastThemeChangeToComponents();
    void optimizeLayoutForDevice();
    void updateTouchTargetsForPlatform();
    
#ifdef JUCE_DEBUG
    void performIntegrationValidation(const juce::Rectangle<int>& bounds);
    
    // PHASE 5: Row 4 Pattern Group Testing Methods
    void performRow4IntegrationTesting();
    void testPatternGroupButtonStates();
    void testPatternGroupDropdownBehavior();
    void testPatternGroupNavigationEdgeCases();
    void testPatternGroupEditModeIntegration();
    void testPatternGroupFavoriteSystem();
    void testRow4LabelVisualFeedback();
    void testPatternGroupPerformance();
    void testPatternGroupMemoryManagement();
#endif

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};
