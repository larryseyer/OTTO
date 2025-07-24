#pragma once
#include <JuceHeader.h>
#include "ColorScheme.h"
#include "ResponsiveLayoutManager.h"
#include "FontManager.h"
#include "ComponentState.h"
#include "MidiEngine.h"
#include "INIConfig.h"

class MidiMappingTab : public juce::Component,
                      public juce::TableListBoxModel,
                      public juce::Button::Listener {
public:
    MidiMappingTab(ColorScheme& colorScheme,
                  ResponsiveLayoutManager& layoutManager,
                  FontManager& fontManager,
                  MidiEngine* midiEngine);

    void resized() override;
    void buttonClicked(juce::Button* button) override;

    int getNumRows() override;
    void paintRowBackground(juce::Graphics& g, int rowNumber,
                          int width, int height, bool rowIsSelected) override;
    void paintCell(juce::Graphics& g, int rowNumber, int columnId,
                  int width, int height, bool rowIsSelected) override;
    void cellClicked(int rowNumber, int columnId, const juce::MouseEvent& e) override;

    void saveMidiMappings(ComponentState& state);
    void loadMidiMappings(const ComponentState& state);

private:
    ColorScheme& colorScheme;
    ResponsiveLayoutManager& layoutManager;
    FontManager& fontManager;
    MidiEngine* midiEngine;

    juce::TableListBox mappingTable;
    juce::TextButton learnButton, clearButton, clearAllButton;
    juce::TextButton loadPresetButton, savePresetButton;
    juce::Label statusLabel;

    juce::Array<MidiEngine::MidiMapping> currentMappings;
    int selectedRow = INIConfig::MIDI::INACTIVE_PATTERN;
    bool isLearning = false;

    void setupComponents();
    void updateMappings();
    void startMidiLearn();
    void cancelMidiLearn();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiMappingTab)
};
