#include "SettingsPanel.h"
#include "INIConfig.h"

SettingsPanelWindow::SettingsPanelWindow(FontManager& fontManager,
                                        ColorScheme& colorScheme,
                                        ResponsiveLayoutManager& layoutManager,
                                        INIDataManager& INIManager)
    : fontManager(fontManager), colorScheme(colorScheme), layoutManager(layoutManager),
      INIManager(INIManager), closeButton("close") {
    setupComponents();

    ComponentState state;
    if (INIManager.loadAllSettings(state)) {
        loadStates(state);
    }
}

void SettingsPanelWindow::setupComponents() {
    addAndMakeVisible(titleLabel);
    addAndMakeVisible(phosphorLabel);
    addAndMakeVisible(phosphorWeightCombo);
    addAndMakeVisible(darkThemeButton);
    addAndMakeVisible(lightThemeButton);
    addAndMakeVisible(closeButton);

    titleLabel.setText("Settings", juce::dontSendNotification);
    titleLabel.setColour(juce::Label::textColourId, colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
    titleLabel.setJustificationType(juce::Justification::centred);

    phosphorLabel.setText("Icon Style:", juce::dontSendNotification);
    phosphorLabel.setColour(juce::Label::textColourId, colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));

    phosphorWeightCombo.addItem("Thin", 1);
    phosphorWeightCombo.addItem("Light", 2);
    phosphorWeightCombo.addItem("Regular", 3);
    phosphorWeightCombo.addItem("Bold", 4);
    phosphorWeightCombo.addItem("Fill", 5);
    phosphorWeightCombo.addItem("Duotone", 6);

    phosphorWeightCombo.setColour(juce::ComboBox::backgroundColourId, juce::Colours::transparentBlack);
    phosphorWeightCombo.setColour(juce::ComboBox::textColourId, colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
    phosphorWeightCombo.setColour(juce::ComboBox::outlineColourId, juce::Colours::transparentBlack);
    phosphorWeightCombo.setColour(juce::ComboBox::arrowColourId, colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
    phosphorWeightCombo.setColour(juce::ComboBox::buttonColourId, juce::Colours::transparentBlack);

    phosphorWeightCombo.setSelectedId(static_cast<int>(fontManager.getPhosphorWeight()) + 1);

    phosphorWeightCombo.onChange = [this] {
        int selectedId = phosphorWeightCombo.getSelectedId();
        if (selectedId > 0) {
            FontManager::PhosphorWeight weight = static_cast<FontManager::PhosphorWeight>(selectedId - 1);
            fontManager.setPhosphorWeight(weight);

            if (onPhosphorWeightChanged) {
                onPhosphorWeightChanged(weight);
            }

            ComponentState currentState;
            if (INIManager.loadAllSettings(currentState)) {
                saveStates(currentState);
                if (!INIManager.saveAllSettings(currentState)) {

                }
            }
        }
    };

    darkThemeButton.setButtonText("Dark Theme");
    darkThemeButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xFF4A4A4A));
    darkThemeButton.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
    darkThemeButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);

    darkThemeButton.onClick = [this] {
        colorScheme.setDarkTheme();
        if (onColorSchemeChanged) {
            onColorSchemeChanged();
        }

        ComponentState currentState;
        if (INIManager.loadAllSettings(currentState)) {
            saveStates(currentState);
            currentState.globalSettings.currentThemeName = "Dark";
            if (!INIManager.saveAllSettings(currentState)) {

            }
        }
    };

    lightThemeButton.setButtonText("Light Theme");
    lightThemeButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xFF4A4A4A));
    lightThemeButton.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
    lightThemeButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);

    lightThemeButton.onClick = [this] {
        colorScheme.setLightTheme();
        if (onColorSchemeChanged) {
            onColorSchemeChanged();
        }

        ComponentState currentState;
        if (INIManager.loadAllSettings(currentState)) {
            saveStates(currentState);
            currentState.globalSettings.currentThemeName = "Light";
            if (!INIManager.saveAllSettings(currentState)) {

            }
        }
    };

    closeButton.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
    closeButton.setColour(juce::TextButton::textColourOnId, colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
    closeButton.setColour(juce::TextButton::textColourOffId, colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));

    closeButton.onClick = [this] {
        ComponentState currentState;
        if (INIManager.loadAllSettings(currentState)) {
            saveStates(currentState);
            if (!INIManager.saveAllSettings(currentState)) {

            }
        }

        setVisible(false);
        if (onClose) {
            onClose();
        }
    };
}

void SettingsPanelWindow::saveStates(ComponentState& state) {
    state.phosphorIconWeight = INIConfig::clampPhosphorWeight(static_cast<int>(fontManager.getPhosphorWeight()));
    state.globalSettings.interfaceScale = INIConfig::clampScale(layoutManager.getCurrentScale());

    if (colorScheme.getCurrentThemeName() == "Dark") {
        state.globalSettings.currentThemeName = "Dark";
    } else if (colorScheme.getCurrentThemeName() == "Light") {
        state.globalSettings.currentThemeName = "Light";
    }
}

void SettingsPanelWindow::loadStates(const ComponentState& state) {
    FontManager::PhosphorWeight weight = static_cast<FontManager::PhosphorWeight>(
        INIConfig::clampPhosphorWeight(state.phosphorIconWeight));
    fontManager.setPhosphorWeight(weight);
    phosphorWeightCombo.setSelectedId(static_cast<int>(weight) + 1);

    layoutManager.setScale(INIConfig::clampScale(state.globalSettings.interfaceScale));

    if (state.globalSettings.currentThemeName == "Dark") {
        colorScheme.setDarkTheme();
    } else if (state.globalSettings.currentThemeName == "Light") {
        colorScheme.setLightTheme();
    }
}

void SettingsPanelWindow::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0x60000000));

    auto bounds = getLocalBounds();
    auto panelBounds = bounds.withSizeKeepingCentre(layoutManager.scaled(INIConfig::LayoutConstants::settingsPanelWidth), layoutManager.scaled(INIConfig::LayoutConstants::settingsPanelHeight));

    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::WindowBackground));
    g.fillRoundedRectangle(panelBounds.toFloat(), layoutManager.scaled(INIConfig::LayoutConstants::settingsPanelCornerRadius));

    g.setColour(juce::Colour(0xFF3A3A3A));
    g.drawRoundedRectangle(panelBounds.toFloat(), layoutManager.scaled(INIConfig::LayoutConstants::settingsPanelCornerRadius), layoutManager.scaled(1));
}

void SettingsPanelWindow::resized() {
    auto bounds = getLocalBounds();
    auto panelBounds = bounds.withSizeKeepingCentre(layoutManager.scaled(INIConfig::LayoutConstants::settingsPanelWidth), layoutManager.scaled(INIConfig::LayoutConstants::settingsPanelHeight));

    int margin = layoutManager.scaled(INIConfig::LayoutConstants::settingsPanelMargin);
    auto contentBounds = panelBounds.reduced(margin);

    titleLabel.setBounds(contentBounds.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::settingsPanelTitleHeight)));
    contentBounds.removeFromTop(layoutManager.scaled(8));

    phosphorLabel.setBounds(contentBounds.removeFromTop(layoutManager.scaled(20)));
    phosphorWeightCombo.setBounds(contentBounds.removeFromTop(layoutManager.scaled(30)));

    contentBounds.removeFromTop(layoutManager.scaled(15));

    auto themeArea = contentBounds.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::themeTabRowHeight));
    int buttonWidth = layoutManager.scaled(INIConfig::LayoutConstants::themeTabButtonWidth);
    darkThemeButton.setBounds(themeArea.removeFromLeft(buttonWidth));
    themeArea.removeFromLeft(layoutManager.scaled(15));
    lightThemeButton.setBounds(themeArea.removeFromLeft(buttonWidth));

    int closeButtonSize = layoutManager.scaled(INIConfig::LayoutConstants::settingsPanelCloseButtonSize);
    closeButton.setBounds(panelBounds.getRight() - layoutManager.scaled(INIConfig::LayoutConstants::settingsPanelCloseButtonX),
                         panelBounds.getY() + layoutManager.scaled(INIConfig::LayoutConstants::settingsPanelCloseButtonY),
                         closeButtonSize, closeButtonSize);
}

void SettingsPanelWindow::mouseDown(const juce::MouseEvent& event) {
    auto bounds = getLocalBounds();
    auto panelBounds = bounds.withSizeKeepingCentre(layoutManager.scaled(INIConfig::LayoutConstants::settingsPanelWidth), layoutManager.scaled(INIConfig::LayoutConstants::settingsPanelHeight));

    if (!panelBounds.contains(event.getPosition())) {
        ComponentState currentState;
        if (INIManager.loadAllSettings(currentState)) {
            saveStates(currentState);
            if (!INIManager.saveAllSettings(currentState)) {

            }
        }

        setVisible(false);
        if (onClose) {
            onClose();
        }
    }
}
