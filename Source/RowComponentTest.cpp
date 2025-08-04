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
        ResponsiveLayoutManager layoutManager;
        FontManager fontManager;
        ColorScheme colorScheme;
        MidiEngine midiEngine;
        Mixer mixer;
        juce::AudioProcessorValueTreeState valueTreeState(nullptr, nullptr, "TestState", {});
        
        RowComponentManager manager;
        
        auto row1 = std::make_unique<Row1Component>(midiEngine, mixer, layoutManager, fontManager, colorScheme);
        auto row2 = std::make_unique<Row2Component>(midiEngine, layoutManager, fontManager, colorScheme);
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
