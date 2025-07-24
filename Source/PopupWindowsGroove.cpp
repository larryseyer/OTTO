#include "PopupWindows.h"
#include "INIConfig.h"

class GrooveAndFillsWindow::GrooveContent : public juce::Component {
public:
    GrooveContent(ColorScheme& colorScheme) : colorScheme(colorScheme) {
        addAndMakeVisible(placeholderLabel);
        placeholderLabel.setText("Grooves and Fills\n(Implementation Coming Soon)", juce::dontSendNotification);
        placeholderLabel.setJustificationType(juce::Justification::centred);
        placeholderLabel.setColour(juce::Label::textColourId, colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    }

    void resized() override {
        placeholderLabel.setBounds(getLocalBounds());
    }

private:
    ColorScheme& colorScheme;
    juce::Label placeholderLabel;
};

GrooveAndFillsWindow::GrooveAndFillsWindow(INIDataManager& INIManager,
                                           ColorScheme& colorScheme)
    : juce::DocumentWindow("Grooves and Fills",
                          colorScheme.getColor(ColorScheme::ColorRole::WindowBackground),
                          DocumentWindow::closeButton),
      INIManager(INIManager),
      colorScheme(colorScheme) {

    content = std::make_unique<GrooveContent>(colorScheme);
    setContentOwned(content.release(), false);
    setResizable(true, true);

    ComponentState state;
    if (INIManager.loadAllSettings(state)) {
        loadStates(state);
    } else {
        setBounds(INIConfig::LayoutConstants::grooveWindowDefaultX, INIConfig::LayoutConstants::grooveWindowDefaultY,
                  INIConfig::LayoutConstants::grooveWindowDefaultWidth, INIConfig::LayoutConstants::grooveWindowDefaultHeight);
    }
}

GrooveAndFillsWindow::~GrooveAndFillsWindow() = default;

void GrooveAndFillsWindow::closeButtonPressed() {
    ComponentState state;
    if (INIManager.loadAllSettings(state)) {
        saveStates(state);
        if (!INIManager.saveAllSettings(state)) {
        }
    }
    setVisible(false);
}

void GrooveAndFillsWindow::saveStates(ComponentState& state) {
    auto bounds = getBounds();
    state.sliderValues["grooveWindowX"] = static_cast<float>(INIConfig::clampWindowSize(bounds.getX()));
    state.sliderValues["grooveWindowY"] = static_cast<float>(INIConfig::clampWindowSize(bounds.getY()));
    state.sliderValues["grooveWindowWidth"] = static_cast<float>(INIConfig::clampWindowSize(bounds.getWidth()));
    state.sliderValues["grooveWindowHeight"] = static_cast<float>(INIConfig::clampWindowSize(bounds.getHeight()));
}

void GrooveAndFillsWindow::loadStates(const ComponentState& state) {
    int windowX = INIConfig::LayoutConstants::grooveWindowDefaultX;
    int windowY = INIConfig::LayoutConstants::grooveWindowDefaultY;
    int windowWidth = INIConfig::LayoutConstants::grooveWindowDefaultWidth;
    int windowHeight = INIConfig::LayoutConstants::grooveWindowDefaultHeight;

    auto xIt = state.sliderValues.find("grooveWindowX");
    if (xIt != state.sliderValues.end()) {
        windowX = INIConfig::clampWindowSize(static_cast<int>(xIt->second));
    }

    auto yIt = state.sliderValues.find("grooveWindowY");
    if (yIt != state.sliderValues.end()) {
        windowY = INIConfig::clampWindowSize(static_cast<int>(yIt->second));
    }

    auto widthIt = state.sliderValues.find("grooveWindowWidth");
    if (widthIt != state.sliderValues.end()) {
        windowWidth = INIConfig::clampWindowSize(static_cast<int>(widthIt->second));
    }

    auto heightIt = state.sliderValues.find("grooveWindowHeight");
    if (heightIt != state.sliderValues.end()) {
        windowHeight = INIConfig::clampWindowSize(static_cast<int>(heightIt->second));
    }

    if (windowWidth < INIConfig::LayoutConstants::grooveWindowMinWidth) windowWidth = INIConfig::LayoutConstants::grooveWindowDefaultWidth;
    if (windowHeight < INIConfig::LayoutConstants::grooveWindowMinHeight) windowHeight = INIConfig::LayoutConstants::grooveWindowDefaultHeight;

    setBounds(windowX, windowY, windowWidth, windowHeight);
}
