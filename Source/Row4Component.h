#pragma once
#include "RowComponentBase.h"
#include "UtilityComponents.h"
#include "JUCE8_CODING_STANDARDS.h"
#include "Animation/AnimationManager.h"
#include "DragDrop/DragDropManager.h"

class MidiEngine;
class INIDataManager;

class Row4Component : public RowComponentBase {
public:
    Row4Component(MidiEngine& midiEngine,
                 ResponsiveLayoutManager& layoutManager,
                 FontManager& fontManager,
                 ColorScheme& colorScheme);
    
    ~Row4Component() override = default;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    void saveStates(ComponentState& state) override;
    void loadStates(const ComponentState& state) override;
    void updateFromState(const ComponentState& state) override;
    juce::String getRowName() const override { return "PatternGroupControls"; }
    juce::Rectangle<int> getRowBounds() const override;
    
    void setINIDataManager(INIDataManager* manager) { iniDataManager = manager; }
    
    // Pattern Group Control Interface
    void setPatternGroupEditMode(bool editMode);
    bool isPatternGroupEditMode() const;
    void navigatePatternGroup(bool isNext);
    void togglePatternGroupFavorite();
    int getCurrentPatternGroupIndex() const;
    void setCurrentPatternGroupIndex(int index);
    
    // Label Update Interface - CRITICAL for Phase 5
    void updateLabelsFromControls();
    void setLabelStates(const juce::String& togglesText, const juce::String& fillsText,
                       const juce::String& swingText, const juce::String& energyText,
                       const juce::String& volumeText);
    
    void setupPatternGroupDragDrop();
    void setupPatternGroupAnimations();
    void animatePatternGroupChange(int newIndex);
    void handlePatternGroupAction(const juce::String& action);
    void setAnimationManager(AnimationManager* manager) { animationManager = manager; }
    
private:
    MidiEngine& midiEngine;
    AnimationManager* animationManager = nullptr;
    INIDataManager* iniDataManager = nullptr;
    
    // Row 4: Pattern Group Controls - Complete 10-component layout
    // CRITICAL: These components will display in Row 4, not Row 5
    PhosphorIconButton patternGroupEditButton;        // Edit/Pencil icon
    PhosphorIconButton patternGroupLeftChevron;       // Left chevron
    HierarchicalComboBox patternGroupDropdown;        // Pattern group dropdown
    PhosphorIconButton patternGroupRightChevron;      // Right chevron
    PhosphorIconButton patternGroupFavoriteButton;    // Favorite icon
    juce::Label togglesLabel;                         // Toggles label - MOVED FROM ROW 5
    juce::Label fillsLabel;                           // Fills label - MOVED FROM ROW 5
    juce::Label swingLabel;                           // Swing label - MOVED FROM ROW 5
    juce::Label energyLabel;                          // Energy label - MOVED FROM ROW 5
    juce::Label volumeLabel;                          // Volume label - MOVED FROM ROW 5
    
    // State management
    int currentPatternGroupIndex = 0;
    bool patternGroupEditMode = false;
    
    std::unique_ptr<PatternDragSource> patternDragSource;
    
    void setupPatternGroupComponents();
    void updatePatternGroupLayout();
    void setupPatternGroupCallbacks();
    void setupLabels();
    void updatePatternGroupButtonStates();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Row4Component)
};
