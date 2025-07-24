#include "PopupWindowsMidi.h"
#include "INIConfig.h"

MidiMappingTab::MidiMappingTab(ColorScheme& colorScheme,
                              ResponsiveLayoutManager& layoutManager,
                              FontManager& fontManager,
                              MidiEngine* midiEngine)
    : colorScheme(colorScheme), layoutManager(layoutManager),
      fontManager(fontManager), midiEngine(midiEngine) {
    setupComponents();
}

void MidiMappingTab::setupComponents() {
    addAndMakeVisible(mappingTable);
    mappingTable.setModel(this);
    mappingTable.setColour(juce::ListBox::backgroundColourId,
                          colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground));
    mappingTable.setColour(juce::ListBox::outlineColourId,
                          colorScheme.getColor(ColorScheme::ColorRole::Separator));

    mappingTable.getHeader().addColumn("Parameter", 1, layoutManager.scaled(INIConfig::LayoutConstants::midiTabColumnWidths[0]));
    mappingTable.getHeader().addColumn("CC", 2, layoutManager.scaled(INIConfig::LayoutConstants::midiTabColumnWidths[1]));
    mappingTable.getHeader().addColumn("Channel", 3, layoutManager.scaled(INIConfig::LayoutConstants::midiTabColumnWidths[2]));
    mappingTable.getHeader().addColumn("Range", 4, layoutManager.scaled(INIConfig::LayoutConstants::midiTabColumnWidths[3]));
    mappingTable.getHeader().addColumn("Enabled", 5, layoutManager.scaled(INIConfig::LayoutConstants::midiTabColumnWidths[4]));

    mappingTable.getHeader().setColour(juce::TableHeaderComponent::textColourId,
                                      colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    mappingTable.getHeader().setColour(juce::TableHeaderComponent::backgroundColourId,
                                      colorScheme.getColor(ColorScheme::ColorRole::ControlBackground));

    addAndMakeVisible(learnButton);
    addAndMakeVisible(clearButton);
    addAndMakeVisible(clearAllButton);
    addAndMakeVisible(loadPresetButton);
    addAndMakeVisible(savePresetButton);

    learnButton.setButtonText("Learn");
    clearButton.setButtonText("Clear");
    clearAllButton.setButtonText("Clear All");
    loadPresetButton.setButtonText("Load Preset");
    savePresetButton.setButtonText("Save Preset");

    for (auto* button : {&learnButton, &clearButton, &clearAllButton,
                        &loadPresetButton, &savePresetButton}) {
        button->addListener(this);
        button->setColour(juce::TextButton::buttonColourId,
                         colorScheme.getColor(ColorScheme::ColorRole::ButtonBackground));
        button->setColour(juce::TextButton::textColourOffId,
                         colorScheme.getColor(ColorScheme::ColorRole::ButtonText));
    }

    addAndMakeVisible(statusLabel);
    statusLabel.setColour(juce::Label::textColourId,
                         colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
    statusLabel.setJustificationType(juce::Justification::centred);

    updateMappings();
}

void MidiMappingTab::updateMappings() {
    if (midiEngine) {
        currentMappings = midiEngine->getAllMidiMappings();
    } else {
        currentMappings.clear();

        MidiEngine::MidiMapping defaultMapping;
        defaultMapping.parameterID = "masterVolume";
        defaultMapping.ccNumber = 7;
        defaultMapping.channel = 0;
        defaultMapping.minValue = 0.0f;
        defaultMapping.maxValue = 1.0f;
        defaultMapping.enabled = true;
        currentMappings.add(defaultMapping);

        defaultMapping.parameterID = "tempo";
        defaultMapping.ccNumber = 14;
        defaultMapping.minValue = INIConfig::Validation::MIN_TEMPO;
        defaultMapping.maxValue = INIConfig::Validation::MAX_TEMPO;
        currentMappings.add(defaultMapping);
    }

    mappingTable.updateContent();
}

void MidiMappingTab::resized() {
    auto bounds = getLocalBounds().reduced(layoutManager.scaled(INIConfig::LayoutConstants::defaultMargin));

    auto tableHeight = bounds.getHeight() - layoutManager.scaled(INIConfig::LayoutConstants::midiTabListHeight);
    mappingTable.setBounds(bounds.removeFromTop(tableHeight));

    bounds.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::defaultSpacing));

    auto buttonRow = bounds.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::audioTabRowHeight));
    auto buttonWidth = layoutManager.scaled(INIConfig::LayoutConstants::midiTabButtonWidth);
    auto spacing = layoutManager.scaled(INIConfig::LayoutConstants::defaultSpacing);

    learnButton.setBounds(buttonRow.removeFromLeft(buttonWidth));
    buttonRow.removeFromLeft(spacing);
    clearButton.setBounds(buttonRow.removeFromLeft(buttonWidth));
    buttonRow.removeFromLeft(spacing);
    clearAllButton.setBounds(buttonRow.removeFromLeft(buttonWidth));
    buttonRow.removeFromLeft(spacing * 2);
    loadPresetButton.setBounds(buttonRow.removeFromLeft(buttonWidth));
    buttonRow.removeFromLeft(spacing);
    savePresetButton.setBounds(buttonRow.removeFromLeft(buttonWidth));

    bounds.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::defaultSpacing));

    statusLabel.setBounds(bounds);
}

int MidiMappingTab::getNumRows() {
    return currentMappings.size();
}

void MidiMappingTab::paintRowBackground(juce::Graphics& g, int rowNumber,
                                       int width, int height, bool rowIsSelected) {
    if (rowIsSelected) {
        g.fillAll(colorScheme.getColor(ColorScheme::ColorRole::Accent).withAlpha(0.3f));
    } else if (rowNumber % 2) {
        g.fillAll(colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground).darker(0.05f));
    }
}

void MidiMappingTab::paintCell(juce::Graphics& g, int rowNumber, int columnId,
                              int width, int height, bool rowIsSelected) {
    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    g.setFont(fontManager.getFont(FontManager::FontRole::Body));

    if (rowNumber < currentMappings.size()) {
        const auto& mapping = currentMappings[rowNumber];
        juce::String text;

        switch (columnId) {
            case 1:
                text = mapping.parameterID;
                break;
            case 2:
                text = mapping.ccNumber >= 0 ? juce::String(mapping.ccNumber) : "--";
                break;
            case 3:
                text = mapping.channel == 0 ? "All" : juce::String(mapping.channel);
                break;
            case 4:
                text = juce::String(mapping.minValue, 1) + " - " + juce::String(mapping.maxValue, 1);
                break;
            case 5:
                {
                    auto checkArea = juce::Rectangle<float>(width/2 - INIConfig::LayoutConstants::midiTabCheckboxSize/2,
                                                           height/2 - INIConfig::LayoutConstants::midiTabCheckboxSize/2,
                                                           INIConfig::LayoutConstants::midiTabCheckboxSize,
                                                           INIConfig::LayoutConstants::midiTabCheckboxSize);
                    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::Separator));
                    g.drawRect(checkArea, 1.0f);

                    if (mapping.enabled) {
                        g.setColour(colorScheme.getColor(ColorScheme::ColorRole::Accent));
                        g.fillRect(checkArea.reduced(INIConfig::LayoutConstants::midiTabCheckboxReduction));
                    }
                    return;
                }
        }

        g.drawText(text, 2, 0, width - 4, height, juce::Justification::centredLeft, true);
    }
}

void MidiMappingTab::cellClicked(int rowNumber, int columnId, const juce::MouseEvent& e) {
    if (columnId == 5 && rowNumber < currentMappings.size()) {
        currentMappings.getReference(rowNumber).enabled = !currentMappings[rowNumber].enabled;

        if (midiEngine) {
            midiEngine->setMidiMappingEnabled(currentMappings[rowNumber].parameterID,
                                            currentMappings[rowNumber].enabled);
        }

        mappingTable.repaintRow(rowNumber);
    }

    selectedRow = rowNumber;
}

void MidiMappingTab::buttonClicked(juce::Button* button) {
    if (button == &learnButton) {
        if (selectedRow >= 0 && selectedRow < currentMappings.size()) {
            startMidiLearn();
        } else {
            statusLabel.setText("Select a parameter to learn", juce::dontSendNotification);
        }
    }
    else if (button == &clearButton) {
        if (selectedRow >= 0 && selectedRow < currentMappings.size()) {
            auto& mapping = currentMappings.getReference(selectedRow);
            mapping.ccNumber = -1;

            if (midiEngine) {
                midiEngine->removeMidiMapping(mapping.parameterID);
            }

            mappingTable.repaintRow(selectedRow);
            statusLabel.setText("Cleared mapping for " + mapping.parameterID, juce::dontSendNotification);
        }
    }
    else if (button == &clearAllButton) {
        if (midiEngine) {
            midiEngine->clearAllMidiMappings();
        }

        for (auto& mapping : currentMappings) {
            mapping.ccNumber = -1;
            mapping.enabled = false;
        }

        mappingTable.updateContent();
        statusLabel.setText("All mappings cleared", juce::dontSendNotification);
    }
    else if (button == &loadPresetButton) {
        juce::PopupMenu menu;

        if (midiEngine) {
            auto presets = midiEngine->getAvailableControllerPresets();
            for (int i = 0; i < presets.size(); ++i) {
                menu.addItem(i + 1, presets[i]);
            }
        } else {
            menu.addItem(1, "Default Mapping");
            menu.addItem(2, "Performance Mapping");
        }

        menu.showMenuAsync(juce::PopupMenu::Options(),
                          [this](int result) {
                              if (result > 0) {
                                  statusLabel.setText("Loaded preset", juce::dontSendNotification);
                                  updateMappings();
                              }
                          });
    }
    else if (button == &savePresetButton) {
        auto alertWindow = std::make_unique<juce::AlertWindow>("Save MIDI Mapping Preset",
                                                               "Enter preset name:",
                                                               juce::AlertWindow::NoIcon);

        alertWindow->addTextEditor("presetName", "My Preset");
        alertWindow->addButton("Save", 1, juce::KeyPress(juce::KeyPress::returnKey));
        alertWindow->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));

        alertWindow->enterModalState(true, juce::ModalCallbackFunction::create(
            [this, alertWindowPtr = alertWindow.release()](int result) {
                std::unique_ptr<juce::AlertWindow> aw(alertWindowPtr);

                if (result == 1 && midiEngine) {
                    auto name = aw->getTextEditorContents("presetName");
                    if (name.isNotEmpty()) {
                        midiEngine->saveControllerPreset(name);
                        statusLabel.setText("Saved preset: " + name, juce::dontSendNotification);
                    }
                }
            }
        ));
    }
}

void MidiMappingTab::startMidiLearn() {
    if (!midiEngine || selectedRow < 0 || selectedRow >= currentMappings.size()) {
        return;
    }

    isLearning = true;
    const auto& mapping = currentMappings[selectedRow];

    midiEngine->startMidiLearn(mapping.parameterID);
    statusLabel.setText("Learning " + mapping.parameterID + " - Move a MIDI control...",
                       juce::dontSendNotification);

    midiEngine->onMidiLearnComplete = [this](const MidiEngine::MidiMapping& learned) {
        isLearning = false;
        updateMappings();
        statusLabel.setText("Learned: CC" + juce::String(learned.ccNumber) +
                          " -> " + learned.parameterID, juce::dontSendNotification);
    };
}

void MidiMappingTab::cancelMidiLearn() {
    if (isLearning && midiEngine) {
        midiEngine->cancelMidiLearn();
        isLearning = false;
        statusLabel.setText("MIDI learn cancelled", juce::dontSendNotification);
    }
}

void MidiMappingTab::saveMidiMappings(ComponentState& state) {
    state.midiMappings.clear();

    for (const auto& mapping : currentMappings) {
        MidiMappingState mappingState;
        mappingState.ccNumber = mapping.ccNumber;
        mappingState.channel = mapping.channel;
        mappingState.parameterID = mapping.parameterID;
        mappingState.minValue = mapping.minValue;
        mappingState.maxValue = mapping.maxValue;
        mappingState.enabled = mapping.enabled;

        state.midiMappings.add(mappingState);
    }
}

void MidiMappingTab::loadMidiMappings(const ComponentState& state) {
    currentMappings.clear();

    for (const auto& mappingState : state.midiMappings) {
        MidiEngine::MidiMapping mapping;
        mapping.ccNumber = mappingState.ccNumber;
        mapping.channel = mappingState.channel;
        mapping.parameterID = mappingState.parameterID;
        mapping.minValue = mappingState.minValue;
        mapping.maxValue = mappingState.maxValue;
        mapping.enabled = mappingState.enabled;
        mapping.isLearning = false;
        mapping.sendFeedback = false;
        mapping.feedbackType = 0;

        currentMappings.add(mapping);
    }

    if (currentMappings.isEmpty()) {
        updateMappings();
    }

    mappingTable.updateContent();
}
