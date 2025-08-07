#pragma once
#include <JuceHeader.h>
#include "PatternSuggestionEngine.h"
#include "AutoMixAssistant.h"
#include "Mixer.h"
#include "PatternManager.h"
#include "INIConfig.h"

class AIAssistantPanel : public juce::Component,
                       public juce::Timer,
                       private juce::ListBoxModel {
public:
   AIAssistantPanel(AutoMixAssistant& mixAssistant,
                   PatternSuggestionEngine& patternEngine,
                   Mixer& mixer,
                   PatternManager& patternManager);

   ~AIAssistantPanel() override;

   void paint(juce::Graphics& g) override;
   void resized() override;

   void timerCallback() override;

private:
   AutoMixAssistant& autoMixAssistant;
   PatternSuggestionEngine& patternSuggestionEngine;
   Mixer& mixer;
   PatternManager& patternManager;

   enum class AIMode {
       MixAssistant,
       PatternSuggestions,
       PerformanceMode
   };

   AIMode currentMode = AIMode::MixAssistant;

   juce::Label titleLabel;
   juce::ComboBox aiModeSelector;

   juce::TextEditor mixSuggestionView;
   juce::Slider blendSlider;
   juce::Label blendLabel;

   juce::ComboBox genreSelector;
   juce::Slider complexitySlider;
   juce::Label complexityLabel;
   juce::ListBox patternSuggestionsView;

   juce::TextEditor performanceDisplay;
   juce::ToggleButton learningToggle;

   juce::TextButton applyButton;

   AutoMixAssistant::MixSuggestion currentMixSuggestion;
   juce::Array<PatternSuggestionEngine::PatternSuggestion> currentPatternSuggestions;
   int learnedPatterns = INIConfig::Defaults::ZERO_VALUE;

   void setupUI();
   void modeChanged();
   void analyzeMix();
   void generatePatternSuggestions();
   void updatePerformanceStats();
   void applySuggestion();
   void updateMixSuggestionView();
   void drawMixVisualization(juce::Graphics& g, juce::Rectangle<int> area);
   void collectPerformanceData();

   int getNumRows() override;
   void paintListBoxItem(int rowNumber, juce::Graphics& g,
                        int width, int height,
                        bool rowIsSelected) override;

   JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AIAssistantPanel)
};
