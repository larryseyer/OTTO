#pragma once
#include "RowComponentBase.h"
#include "UtilityComponents.h"
#include "ComponentState.h"
#include "INIConfig.h"
#include "Animation/AnimationManager.h"
#include "DragDrop/DragDropManager.h"

class MidiEngine;
class Mixer;
class MidiFileManager;

class Row5Component : public RowComponentBase {
public:
    Row5Component(MidiEngine& midiEngine,
                 Mixer& mixer,
                 juce::AudioProcessorValueTreeState& valueTreeState,
                 ResponsiveLayoutManager& layoutManager,
                 FontManager& fontManager,
                 ColorScheme& colorScheme);
    
    ~Row5Component() override = default;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    void saveStates(ComponentState& state) override;
    void loadStates(const ComponentState& state) override;
    void updateFromState(const ComponentState& state) override;
    juce::String getRowName() const override { return "InteractiveControls"; }
    juce::Rectangle<int> getRowBounds() const override;
    
    // Interactive control access methods
    float getSwingValue() const { return static_cast<float>(swingSlider.getValue()); }
    float getEnergyValue() const { return static_cast<float>(energySlider.getValue()); }
    float getVolumeValue() const { return static_cast<float>(volumeSlider.getValue()); }
    
    void setSwingValue(float value) { swingSlider.setValue(value); }
    void setEnergyValue(float value) { energySlider.setValue(value); }
    void setVolumeValue(float value) { volumeSlider.setValue(value); }
    
    bool getToggleState(int index) const;
    void setToggleState(int index, bool state);
    bool getFillState(int index) const;
    void setFillState(int index, bool state);
    
    // Drum grid access methods
    bool getEditMode() const { return isEditMode; }
    void setEditModeVisuals(bool isEditing);
    juce::String getMidiFileAssignment(int buttonIndex) const;
    void setMidiFileAssignment(int buttonIndex, const juce::String& midiFile);
    int getSelectedDrumButton() const { return selectedDrumButton; }
    void setSelectedDrumButton(int buttonIndex);
    void triggerDrumPad(int padIndex);
    
    void setAnimationManager(AnimationManager* manager) { animationManager = manager; }
    void setupDragDropTargets();
    void setupHoverEffects();
    void setupRealTimeIndicators();
    void updateBeatVisualization();
    
    // Player state management
    void setCurrentPlayerIndex(int index) { currentPlayerIndex = index; }
    void loadPlayerSpecificState(int playerIndex, const ComponentState& state);
    void saveCurrentPlayerToState(ComponentState& state);
    
    // Callbacks for parent component integration
    std::function<void(const juce::String&, float)> onSliderValueChanged;
    std::function<void(int, const juce::String&, float)> onPlayerSliderValueChanged;
    std::function<void(int, int, bool)> onPlayerToggleChanged;
    std::function<void(int, int, bool)> onPlayerFillChanged;

    class DrumPadDragTarget : public juce::FileDragAndDropTarget, public juce::MouseListener {
    public:
        DrumPadDragTarget(Row5Component& parent, int padIndex) 
            : parentComponent(parent), padIndex(padIndex) {}
        
        bool isInterestedInFileDrag(const juce::StringArray& files) override {
            return files.size() == 1 && 
                   (files[0].endsWith(".mid") || files[0].endsWith(".midi"));
        }
        
        void filesDropped(const juce::StringArray& files, int x, int y) override {
            juce::ignoreUnused(x, y);
            if (files.size() > 0) {
                parentComponent.setMidiFileAssignment(padIndex, files[0]);
                parentComponent.drumButtons[padIndex].setColour(
                    juce::TextButton::buttonColourId,
                    parentComponent.colorScheme.getColor(ColorScheme::ColorRole::Success));
            }
        }
        
        void fileDragEnter(const juce::StringArray& files, int x, int y) override {
            juce::ignoreUnused(files, x, y);
            parentComponent.drumButtons[padIndex].setColour(
                juce::TextButton::buttonColourId,
                parentComponent.colorScheme.getColor(ColorScheme::ColorRole::Accent));
        }
        
        void fileDragExit(const juce::StringArray& files) override {
            juce::ignoreUnused(files);
            parentComponent.drumButtons[padIndex].setColour(
                juce::TextButton::buttonColourId,
                parentComponent.colorScheme.getColor(ColorScheme::ColorRole::ButtonBackground));
        }
        
        // MouseListener methods for hover effects
        void mouseEnter(const juce::MouseEvent& event) override {
            juce::ignoreUnused(event);
            if (parentComponent.animationManager && parentComponent.animationManager->shouldUseAnimations()) {
                parentComponent.drumButtons[padIndex].setColour(juce::TextButton::buttonColourId,
                    parentComponent.colorScheme.getColor(ColorScheme::ColorRole::ButtonBackground).brighter(0.1f));
            }
        }
        
        void mouseExit(const juce::MouseEvent& event) override {
            juce::ignoreUnused(event);
            parentComponent.drumButtons[padIndex].setColour(juce::TextButton::buttonColourId,
                parentComponent.colorScheme.getColor(ColorScheme::ColorRole::ButtonBackground));
        }
        
    private:
        Row5Component& parentComponent;
        int padIndex;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DrumPadDragTarget)
    };
    
    std::function<void(int, const juce::String&)> onMidiFileChanged;
    
private:
    MidiEngine& midiEngine;
    Mixer& mixer;
    AnimationManager* animationManager = nullptr;
    juce::AudioProcessorValueTreeState& valueTreeState;
    
    // LEFT SECTION: 4x4 Drum Pattern Grid (60% width)
    juce::TextButton drumButtons[INIConfig::Audio::NUM_DRUM_PADS];
    SeparatorComponent leftSeparator;
    
    // RIGHT SECTION: Interactive Controls (39.5% width)
    juce::TextButton toggleButtons[INIConfig::UI::MAX_TOGGLE_STATES];
    juce::TextButton fillButtons[INIConfig::UI::MAX_FILL_STATES];
    juce::Slider swingSlider;
    juce::Slider energySlider;
    juce::Slider volumeSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volumeAttachment;
    SeparatorComponent rightSeparator;
    
    // State management
    int currentPlayerIndex = INIConfig::Defaults::DEFAULT_CURRENT_PLAYER;
    bool isEditMode = INIConfig::Defaults::DEFAULT_EDIT_MODE;
    int selectedDrumButton = INIConfig::Defaults::DEFAULT_SELECTED_BUTTON;
    juce::String assignedMidiFiles[INIConfig::Audio::NUM_DRUM_PADS];
    bool toggleStates[INIConfig::UI::MAX_TOGGLE_STATES] = {};
    bool fillStates[INIConfig::UI::MAX_FILL_STATES] = {};
    
    juce::OwnedArray<DrumPadDragTarget> drumPadDragTargets;
    class BeatVisualizationTimer;
    std::unique_ptr<juce::Timer> beatVisualizationTimer;
    
    void setupInteractiveComponents();
    void updateInteractiveLayout();
    void setupDrumGrid();
    void setupControlButtons();
    void setupSliders();
    void updateSliderCallbacks();
    void updateButtonCallbacks();
    void updateDrumButtonCallbacks();
    void validateSliderValues();
    void updateFontsAndColors();
    
    // Event handlers
    void onDrumButtonPressed(int buttonIndex);
    void onDrumButtonRightClicked(int buttonIndex);
    void mouseDown(const juce::MouseEvent& event) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Row5Component)
};
