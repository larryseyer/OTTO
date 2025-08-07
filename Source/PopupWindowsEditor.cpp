#include "PopupWindows.h"
#include "DrumKitEditorContent.h"
#include "INIConfig.h"

DrumKitEditorWindow::DrumKitEditorWindow(SFZEngine& sfzEngine,
                                        ColorScheme& colorScheme,
                                        FontManager& fontManager,
                                        ResponsiveLayoutManager& layoutManager,
                                        INIDataManager& iniManager)
    : juce::DocumentWindow("DrumKit Editor", colorScheme.getColor(ColorScheme::ColorRole::WindowBackground), DocumentWindow::closeButton),
      sfzEngine(sfzEngine),
      colorScheme(colorScheme),
      fontManager(fontManager),
      layoutManager(layoutManager),
      iniManager(iniManager)
{
    content = std::make_unique<DrumKitEditorContent>(sfzEngine, colorScheme, fontManager,
                                                    layoutManager, iniManager);
    setContentOwned(content.release(), false);
    setResizable(true, true);
    setUsingNativeTitleBar(true);

    ComponentState state;
    if (iniManager.loadAllSettings(state)) {
        loadStates(state);
    } else {
        setBounds(INIConfig::LayoutConstants::editorWindowDefaultX, INIConfig::LayoutConstants::editorWindowDefaultY,
                  INIConfig::LayoutConstants::editorWindowDefaultWidth, INIConfig::LayoutConstants::editorWindowDefaultHeight);
    }
}

DrumKitEditorWindow::~DrumKitEditorWindow() = default;

void DrumKitEditorWindow::closeButtonPressed() {
    ComponentState state;
    if (iniManager.loadAllSettings(state)) {
        saveStates(state);
        if (!iniManager.saveAllSettings(state)) {
        }
    }
    setVisible(false);
}

void DrumKitEditorWindow::saveStates(ComponentState& state) {
    auto bounds = getBounds();
    state.sliderValues["editorWindowX"] = static_cast<float>(INIConfig::clampWindowSize(bounds.getX()));
    state.sliderValues["editorWindowY"] = static_cast<float>(INIConfig::clampWindowSize(bounds.getY()));
    state.sliderValues["editorWindowWidth"] = static_cast<float>(INIConfig::clampWindowSize(bounds.getWidth()));
    state.sliderValues["editorWindowHeight"] = static_cast<float>(INIConfig::clampWindowSize(bounds.getHeight()));
}

void DrumKitEditorWindow::loadStates(const ComponentState& state) {
    int windowX = INIConfig::LayoutConstants::editorWindowDefaultX;
    int windowY = INIConfig::LayoutConstants::editorWindowDefaultY;
    int windowWidth = INIConfig::LayoutConstants::editorWindowDefaultWidth;
    int windowHeight = INIConfig::LayoutConstants::editorWindowDefaultHeight;

    auto xIt = state.sliderValues.find("editorWindowX");
    if (xIt != state.sliderValues.end()) {
        windowX = INIConfig::clampWindowSize(static_cast<int>(xIt->second));
    }

    auto yIt = state.sliderValues.find("editorWindowY");
    if (yIt != state.sliderValues.end()) {
        windowY = INIConfig::clampWindowSize(static_cast<int>(yIt->second));
    }

    auto widthIt = state.sliderValues.find("editorWindowWidth");
    if (widthIt != state.sliderValues.end()) {
        windowWidth = INIConfig::clampWindowSize(static_cast<int>(widthIt->second));
    }

    auto heightIt = state.sliderValues.find("editorWindowHeight");
    if (heightIt != state.sliderValues.end()) {
        windowHeight = INIConfig::clampWindowSize(static_cast<int>(heightIt->second));
    }

    if (windowWidth < INIConfig::LayoutConstants::editorWindowMinWidth) windowWidth = INIConfig::LayoutConstants::editorWindowDefaultWidth;
    if (windowHeight < INIConfig::LayoutConstants::editorWindowMinHeight) windowHeight = INIConfig::LayoutConstants::editorWindowDefaultHeight;

    setBounds(windowX, windowY, windowWidth, windowHeight);
}
