#include "PopupWindowsTheme.h"
#include "INIConfig.h"

ThemeEditorTab::ThemeEditorTab(ColorScheme& colorScheme,
                              ResponsiveLayoutManager& layoutManager,
                              FontManager& fontManager,
                              INIDataManager& INIManager)
    : colorScheme(colorScheme), layoutManager(layoutManager),
      fontManager(fontManager), INIManager(INIManager) {
    setupComponents();
}

void ThemeEditorTab::setupComponents() {
    addAndMakeVisible(themePresetCombo);
    themePresetCombo.addListener(this);

    auto themes = INIManager.getAvailableThemeNames();
    for (int i = 0; i < themes.size(); ++i) {
        themePresetCombo.addItem(themes[i], i + 1);
    }

    if (themes.contains(colorScheme.getCurrentThemeName())) {
        themePresetCombo.setText(colorScheme.getCurrentThemeName());
    }

    addAndMakeVisible(saveThemeButton);
    addAndMakeVisible(newThemeButton);
    addAndMakeVisible(deleteThemeButton);

    saveThemeButton.setButtonText("Save Theme");
    newThemeButton.setButtonText("New Theme");
    deleteThemeButton.setButtonText("Delete Theme");

    for (auto* button : {&saveThemeButton, &newThemeButton, &deleteThemeButton}) {
        button->addListener(this);
    }

    addAndMakeVisible(scrollViewport);
    scrollViewport.setViewedComponent(&colorEditorsContainer, false);
    scrollViewport.setScrollBarsShown(true, false);

    createColorEditors();
}

void ThemeEditorTab::createColorEditors() {
    struct ColorProperty {
        juce::String label;
        juce::String propertyName;
        ColorScheme::ColorRole role;
    };

    ColorProperty properties[] = {
        {"Background", "backgroundColor", ColorScheme::ColorRole::WindowBackground},
        {"Component Background", "componentBackground", ColorScheme::ColorRole::ComponentBackground},
        {"Control Background", "controlBackground", ColorScheme::ColorRole::ControlBackground},
        {"Primary Text", "primaryText", ColorScheme::ColorRole::PrimaryText},
        {"Secondary Text", "secondaryText", ColorScheme::ColorRole::SecondaryText},
        {"Button Background", "buttonBackground", ColorScheme::ColorRole::ButtonBackground},
        {"Button Hover", "buttonHover", ColorScheme::ColorRole::ButtonBackgroundHover},
        {"Button Pressed", "buttonPressed", ColorScheme::ColorRole::ButtonBackgroundPressed},
        {"Accent", "accent", ColorScheme::ColorRole::Accent},
        {"Accent Hover", "accentHover", ColorScheme::ColorRole::AccentHover},
        {"Separator", "separator", ColorScheme::ColorRole::Separator},
        {"Grid Line", "gridLine", ColorScheme::ColorRole::GridLine},
        {"Pattern Active", "patternActive", ColorScheme::ColorRole::PatternActive},
        {"Pattern Inactive", "patternInactive", ColorScheme::ColorRole::PatternInactive}
    };

    for (const auto& prop : properties) {
        auto* newEditor = colorEditors.add(new ColorEditor());
        colorEditorsContainer.addAndMakeVisible(newEditor->label);
        newEditor->label.setText(prop.label + ":", juce::dontSendNotification);
        newEditor->label.setColour(juce::Label::textColourId,
                               colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));

        colorEditorsContainer.addAndMakeVisible(newEditor->colorButton);
        newEditor->propertyName = prop.propertyName;
        newEditor->currentColor = colorScheme.getColor(prop.role);

        newEditor->colorButton.onClick = [this, editorPtr = newEditor]() {
            showColorPicker(editorPtr);
        };
    }
}

void ThemeEditorTab::showColorPicker(ColorEditor* editor) {
    currentColorEditor = editor;

    if (!colorSelector) {
        colorSelector = std::make_unique<juce::ColourSelector>(
            juce::ColourSelector::showColourAtTop |
            juce::ColourSelector::showSliders |
            juce::ColourSelector::showColourspace);

        colorSelector->addChangeListener(this);
        addChildComponent(colorSelector.get());
    }

    colorSelector->setCurrentColour(editor->currentColor);
    colorSelector->setVisible(true);
    colorSelector->toFront(true);
    resized();
}

void ThemeEditorTab::paint(juce::Graphics& g) {
    g.fillAll(colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground));

    auto bounds = getLocalBounds();
    auto previewArea = bounds.removeFromBottom(layoutManager.scaled(INIConfig::LayoutConstants::themeTabPreviewHeight));

    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::Separator));
    g.drawLine(0, previewArea.getY(), getWidth(), previewArea.getY(), layoutManager.scaled(INIConfig::LayoutConstants::separatorThickness));

    previewArea = previewArea.reduced(layoutManager.scaled(INIConfig::LayoutConstants::defaultMargin));

    auto buttonArea = previewArea.removeFromLeft(layoutManager.scaled(INIConfig::LayoutConstants::themeTabPreviewButtonWidth));
    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::ButtonBackground));
    g.fillRoundedRectangle(buttonArea.toFloat(), layoutManager.scaled(INIConfig::LayoutConstants::cornerRadiusSmall));

    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::ButtonText));
    g.setFont(fontManager.getFont(FontManager::FontRole::Button));
    g.drawText("Button", buttonArea, juce::Justification::centred);

    previewArea.removeFromLeft(layoutManager.scaled(INIConfig::LayoutConstants::defaultMargin));

    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    g.drawText("Primary Text", previewArea.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::defaultLabelHeight)),
              juce::Justification::left);

    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
    g.drawText("Secondary Text", previewArea.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::defaultLabelHeight)),
              juce::Justification::left);
}

void ThemeEditorTab::resized() {
    auto bounds = getLocalBounds().reduced(layoutManager.scaled(INIConfig::LayoutConstants::defaultMargin));

    auto topRow = bounds.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::themeTabRowHeight));
    themePresetCombo.setBounds(topRow.removeFromLeft(layoutManager.scaled(INIConfig::LayoutConstants::themeTabComboWidth)));

    topRow.removeFromLeft(layoutManager.scaled(INIConfig::LayoutConstants::defaultSpacing));
    saveThemeButton.setBounds(topRow.removeFromLeft(layoutManager.scaled(INIConfig::LayoutConstants::themeTabButtonWidth)));
    topRow.removeFromLeft(layoutManager.scaled(INIConfig::LayoutConstants::themeTabButtonSpacing));
    newThemeButton.setBounds(topRow.removeFromLeft(layoutManager.scaled(INIConfig::LayoutConstants::themeTabButtonWidth)));
    topRow.removeFromLeft(layoutManager.scaled(INIConfig::LayoutConstants::themeTabButtonSpacing));
    deleteThemeButton.setBounds(topRow.removeFromLeft(layoutManager.scaled(INIConfig::LayoutConstants::themeTabButtonWidth)));

    bounds.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::defaultMargin));

    auto editorArea = bounds.removeFromTop(bounds.getHeight() - layoutManager.scaled(INIConfig::LayoutConstants::themeTabPreviewHeight + INIConfig::LayoutConstants::defaultMargin));
    scrollViewport.setBounds(editorArea);

    auto containerBounds = juce::Rectangle<int>(0, 0, editorArea.getWidth() - layoutManager.scaled(INIConfig::LayoutConstants::defaultMargin),
                                               colorEditors.size() * layoutManager.scaled(INIConfig::LayoutConstants::themeTabRowHeight));
    colorEditorsContainer.setBounds(containerBounds);

    int y = 0;
    for (auto* editor : colorEditors) {
        auto row = containerBounds.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::themeTabRowHeight));
        editor->label.setBounds(row.removeFromLeft(layoutManager.scaled(INIConfig::LayoutConstants::themeTabLabelWidth)));
        row.removeFromLeft(layoutManager.scaled(INIConfig::LayoutConstants::defaultSpacing));

        auto buttonBounds = row.removeFromLeft(layoutManager.scaled(INIConfig::LayoutConstants::themeTabColorButtonWidth)).reduced(layoutManager.scaled(INIConfig::LayoutConstants::themeTabColorButtonReduction));
        editor->colorButton.setBounds(buttonBounds);

        editor->colorButton.setColour(juce::TextButton::buttonColourId, editor->currentColor);

        y += layoutManager.scaled(INIConfig::LayoutConstants::themeTabRowHeight);
    }

    if (colorSelector && colorSelector->isVisible()) {
        auto selectorBounds = getBounds().withSizeKeepingCentre(layoutManager.scaled(INIConfig::LayoutConstants::themeTabColorSelectorWidth),
                                                               layoutManager.scaled(INIConfig::LayoutConstants::themeTabColorSelectorHeight));
        colorSelector->setBounds(selectorBounds);
    }
}

void ThemeEditorTab::sliderValueChanged(juce::Slider* slider) {
}

void ThemeEditorTab::buttonClicked(juce::Button* button) {
    if (button == &saveThemeButton) {
        ThemeSettings settings;
        saveThemeSettings(settings);

        if (INIManager.saveThemeSettings(settings)) {
            juce::AlertWindow::showMessageBoxAsync(
                juce::AlertWindow::InfoIcon,
                "Theme Saved",
                "Theme '" + settings.themeName + "' saved successfully!"
            );
        }
    }
    else if (button == &newThemeButton) {
        auto alertWindow = std::make_unique<juce::AlertWindow>("New Theme",
                                                               "Enter theme name:",
                                                               juce::AlertWindow::NoIcon);

        alertWindow->addTextEditor("themeName", "My Theme");
        alertWindow->addButton("Create", 1, juce::KeyPress(juce::KeyPress::returnKey));
        alertWindow->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));

        alertWindow->enterModalState(true, juce::ModalCallbackFunction::create(
            [this, alertWindowPtr = alertWindow.release()](int result) {
                std::unique_ptr<juce::AlertWindow> aw(alertWindowPtr);

                if (result == 1) {
                    auto name = aw->getTextEditorContents("themeName");
                    if (name.isNotEmpty()) {
                        colorScheme.createNewTheme(name, colorScheme.getCurrentThemeName());

                        themePresetCombo.clear();
                        auto themes = INIManager.getAvailableThemeNames();
                        for (int i = 0; i < themes.size(); ++i) {
                            themePresetCombo.addItem(themes[i], i + 1);
                        }
                        themePresetCombo.setText(name);
                    }
                }
            }
        ));
    }
    else if (button == &deleteThemeButton) {
        auto currentTheme = themePresetCombo.getText();

        if (currentTheme == "Dark" || currentTheme == "Light" || currentTheme == "Classic") {
            juce::AlertWindow::showMessageBoxAsync(
                juce::AlertWindow::WarningIcon,
                "Cannot Delete",
                "Cannot delete built-in themes!"
            );
            return;
        }

        juce::AlertWindow::showOkCancelBox(
            juce::AlertWindow::QuestionIcon,
            "Delete Theme",
            "Are you sure you want to delete '" + currentTheme + "'?",
            "Delete",
            "Cancel",
            nullptr,
            juce::ModalCallbackFunction::create([this, currentTheme](int result) {
                if (result == 1) {
                    colorScheme.deleteTheme(currentTheme);

                    themePresetCombo.clear();
                    auto themes = INIManager.getAvailableThemeNames();
                    for (int i = 0; i < themes.size(); ++i) {
                        themePresetCombo.addItem(themes[i], i + 1);
                    }
                    themePresetCombo.setText("Dark");
                    colorScheme.setTheme("Dark");
                }
            })
        );
    }
}

void ThemeEditorTab::changeListenerCallback(juce::ChangeBroadcaster* source) {
    if (source == colorSelector.get() && currentColorEditor) {
        currentColorEditor->currentColor = colorSelector->getCurrentColour();

        colorScheme.setColor("theme", currentColorEditor->propertyName,
                           currentColorEditor->currentColor);

        repaint();

        if (onThemeChanged) {
            onThemeChanged();
        }
    }
}

void ThemeEditorTab::comboBoxChanged(juce::ComboBox* comboBox) {
    if (comboBox == &themePresetCombo) {
        auto selectedTheme = themePresetCombo.getText();
        colorScheme.setTheme(selectedTheme);

        for (int i = 0; i < colorEditors.size(); ++i) {
        }

        repaint();

        if (onThemeChanged) {
            onThemeChanged();
        }
    }
}

void ThemeEditorTab::saveThemeSettings(ThemeSettings& settings) {
    settings.setDefaults();

    settings.themeName = themePresetCombo.getText();
    settings.modifiedDate = juce::Time::getCurrentTime().toString(true, true);

    juce::Array<ThemeSettings> existingThemes;
    if (INIManager.loadAllThemes(existingThemes)) {
        for (const auto& theme : existingThemes) {
            if (theme.themeName == settings.themeName) {
                settings.themeID = theme.themeID;
                settings.createdDate = theme.createdDate;

                settings.fontMain = theme.fontMain;
                settings.fontMainSize = theme.fontMainSize;
                settings.fontMainWeight = theme.fontMainWeight;
                settings.fontSecondary = theme.fontSecondary;
                settings.fontSecondarySize = theme.fontSecondarySize;
                settings.fontSecondaryWeight = theme.fontSecondaryWeight;
                break;
            }
        }
    }

    if (settings.themeID == 1 && settings.themeName != "Dark") {
        int maxID = 0;
        for (const auto& theme : existingThemes) {
            maxID = juce::jmax(maxID, theme.themeID);
        }
        settings.themeID = maxID + 1;
    }

    for (auto* editor : colorEditors) {
        auto colorString = editor->currentColor.toDisplayString(true);

        if (editor->propertyName == "backgroundColor")
            settings.backgroundColor = colorString;
        else if (editor->propertyName == "componentBackground")
            settings.foregroundColor = colorString;
        else if (editor->propertyName == "controlBackground")
            settings.borderColor = colorString;
        else if (editor->propertyName == "primaryText")
            settings.textColor = colorString;
        else if (editor->propertyName == "secondaryText")
            continue;
        else if (editor->propertyName == "buttonBackground")
            settings.buttonColor = colorString;
        else if (editor->propertyName == "buttonHover")
            settings.buttonHoverColor = colorString;
        else if (editor->propertyName == "buttonPressed")
            settings.buttonActiveColor = colorString;
        else if (editor->propertyName == "accent")
            settings.accentColor = colorString;
        else if (editor->propertyName == "accentHover")
            continue;
        else if (editor->propertyName == "separator")
            continue;
        else if (editor->propertyName == "gridLine")
            settings.gridLineColor = colorString;
        else if (editor->propertyName == "patternActive")
            settings.patternActiveColor = colorString;
        else if (editor->propertyName == "patternInactive")
            settings.patternInactiveColor = colorString;
    }
}

void ThemeEditorTab::loadThemeSettings(const ThemeSettings& settings) {
    colorScheme.setTheme(settings.themeName);
    themePresetCombo.setText(settings.themeName);

    repaint();
}
