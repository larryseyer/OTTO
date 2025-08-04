#include "RowComponentManager.h"
#include "Row1Component.h"
#include "Row2Component.h"
#include "Row3Component.h"
#include "Row4Component.h"
#include "Row5Component.h"
#include "Row6Component.h"
#include "MidiEngine.h"
#include "Mixer.h"
#include "ResponsiveLayoutManager.h"
#include "FontManager.h"
#include "ColorScheme.h"

bool testRowComponentArchitecture() {
    try {
        // Create layout configuration
        ResponsiveLayoutManager::LayoutConfig layoutConfig;
        layoutConfig.baseWidth = INIConfig::Defaults::DEFAULT_INTERFACE_WIDTH;
        layoutConfig.baseHeight = INIConfig::Defaults::DEFAULT_INTERFACE_HEIGHT;
        layoutConfig.aspectRatio = static_cast<float>(INIConfig::Defaults::DEFAULT_INTERFACE_WIDTH) /
                                   static_cast<float>(INIConfig::Defaults::DEFAULT_INTERFACE_HEIGHT);
        layoutConfig.minScale = 0.5f;
        layoutConfig.maxScale = 2.0f;
        
        ResponsiveLayoutManager layoutManager(layoutConfig);
        FontManager fontManager;
        ColorScheme colorScheme;
        MidiEngine midiEngine;
        Mixer mixer;
        
        // Create a mock AudioProcessor for testing
        class MockAudioProcessor : public juce::AudioProcessor {
        public:
            MockAudioProcessor() : AudioProcessor(BusesProperties()) {}
            const juce::String getName() const override { return "MockProcessor"; }
            void prepareToPlay(double, int) override {}
            void releaseResources() override {}
            void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override {}
            double getTailLengthSeconds() const override { return 0.0; }
            bool acceptsMidi() const override { return true; }
            bool producesMidi() const override { return false; }
            juce::AudioProcessorEditor* createEditor() override { return nullptr; }
            bool hasEditor() const override { return false; }
            int getNumPrograms() override { return 1; }
            int getCurrentProgram() override { return 0; }
            void setCurrentProgram(int) override {}
            const juce::String getProgramName(int) override { return "Default"; }
            void changeProgramName(int, const juce::String&) override {}
            void getStateInformation(juce::MemoryBlock&) override {}
            void setStateInformation(const void*, int) override {}
        };
        
        MockAudioProcessor mockProcessor;
        juce::AudioProcessorValueTreeState valueTreeState(mockProcessor, nullptr, "TestState", {});
        
        RowComponentManager manager;
        
        auto row1 = std::make_unique<Row1Component>(midiEngine, valueTreeState, layoutManager, fontManager, colorScheme);
        auto row2 = std::make_unique<Row2Component>(midiEngine, valueTreeState, layoutManager, fontManager, colorScheme);
        auto row3 = std::make_unique<Row3Component>(midiEngine, mixer, layoutManager, fontManager, colorScheme);
        auto row4 = std::make_unique<Row4Component>(midiEngine, layoutManager, fontManager, colorScheme);
        auto row5 = std::make_unique<Row5Component>(midiEngine, mixer, valueTreeState, layoutManager, fontManager, colorScheme);
        auto row6 = std::make_unique<Row6Component>(layoutManager, fontManager, colorScheme);
        
        manager.registerRowComponent(std::move(row1));
        manager.registerRowComponent(std::move(row2));
        manager.registerRowComponent(std::move(row3));
        manager.registerRowComponent(std::move(row4));
        manager.registerRowComponent(std::move(row5));
        manager.registerRowComponent(std::move(row6));
        
        bool integrityValid = manager.validateRowIntegrity();
        manager.logRowStatus();
        
        return integrityValid;
    } catch (const std::exception& e) {
        DBG("Row component test failed: " << e.what());
        return false;
    }
}
