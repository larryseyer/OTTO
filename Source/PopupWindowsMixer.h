#pragma once
#include <JuceHeader.h>
#include "PopupWindows.h"
#include "Mixer.h"
#include "SFZEngine.h"
#include "ColorScheme.h"
#include "FontManager.h"
#include "ResponsiveLayoutManager.h"
#include "INIDataManager.h"
#include "INIConfig.h"

class DrumKitMixerWindowImpl {
public:
    DrumKitMixerWindowImpl(DrumKitMixerWindow& owner,
                          Mixer& mixer,
                          SFZEngine& sfzEngine,
                          ColorScheme& colorScheme,
                          FontManager& fontManager,
                          ResponsiveLayoutManager& layoutManager,
                          INIDataManager& iniManager);

    ~DrumKitMixerWindowImpl();

    void closeButtonPressed();
    void saveStates(ComponentState& state);
    void loadStates(const ComponentState& state);
    void setCurrentPlayerIndex(int playerIndex);

private:
    class MixerContent;

    DrumKitMixerWindow& owner;
    Mixer& mixer;
    SFZEngine& sfzEngine;
    ColorScheme& colorScheme;
    FontManager& fontManager;
    ResponsiveLayoutManager& layoutManager;
    INIDataManager& iniManager;

    std::unique_ptr<MixerContent> content;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DrumKitMixerWindowImpl)
};
