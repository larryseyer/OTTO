#include "MainContentComponentLeftSection.h"
#include "MidiEngine.h"
#include "CustomLookAndFeel.h"
#include "INIConfig.h"

void MainContentComponentLeftSection::setEditModeVisuals(bool isEditing) {
    editButton.setToggleState(isEditing, juce::dontSendNotification);

    for (int i = 0; i < 16; ++i) {
        if (isEditing) {
            drumButtons[i].setColour(juce::TextButton::buttonColourId,
                colorScheme.getColor(ColorScheme::ColorRole::ButtonBackgroundHover));
        } else {
            drumButtons[i].setColour(juce::TextButton::buttonColourId,
                colorScheme.getColor(ColorScheme::ColorRole::ButtonBackground));
        }
    }

    repaint();
}

void MainContentComponentLeftSection::showMidiFileSelectionForButton(int buttonIndex, ComponentState& state) {
    closeCurrentDropdown();
    currentDropdownMenu = std::make_unique<juce::PopupMenu>();
    currentDropdownMenu->setLookAndFeel(&getLookAndFeel());
    currentlyOpenDropdownButton = buttonIndex;

    currentDropdownMenu->addItem(INIConfig::LayoutConstants::menuItemClearAssignment, "Clear Assignment");
    currentDropdownMenu->addSeparator();

    auto currentGroupFiles = midiFileManager->getCurrentGroupFiles();
    if (currentGroupFiles.isEmpty()) {
        forceRefreshMidiFiles(state);
        currentGroupFiles = midiFileManager->getCurrentGroupFiles();
    }

    for (int i = 0; i < currentGroupFiles.size(); ++i) {
        if (currentGroupFiles[i].isNotEmpty()) {
            currentDropdownMenu->addItem(i + INIConfig::LayoutConstants::menuItemStartOffset, currentGroupFiles[i]);
        }
    }

    if (currentGroupFiles.isEmpty()) {
        currentDropdownMenu->addItem(INIConfig::LayoutConstants::menuItemNoFilesFound, "No MIDI files found - check Assets/MidiFiles folder");
    }

    currentDropdownMenu->showMenuAsync(juce::PopupMenu::Options().withTargetComponent(&drumButtons[buttonIndex]),
                                      [this, buttonIndex, currentGroupFiles, &state](int result) {
                                          currentDropdownMenu.reset();
                                          currentlyOpenDropdownButton = -1;

                                          if (result == INIConfig::LayoutConstants::menuItemClearAssignment) {
                                              assignedMidiFiles[buttonIndex] = "";
                                              drumButtons[buttonIndex].setButtonText("Empty");
                                          } else if (result > INIConfig::LayoutConstants::menuItemClearAssignment && result < INIConfig::LayoutConstants::menuItemNoFilesFound && (result - INIConfig::LayoutConstants::menuItemStartOffset) < currentGroupFiles.size()) {
                                              juce::String fileName = currentGroupFiles[result - INIConfig::LayoutConstants::menuItemStartOffset];
                                              if (fileName.isNotEmpty()) {
                                                  assignedMidiFiles[buttonIndex] = fileName;
                                                  drumButtons[buttonIndex].setButtonText(MidiFileManager::truncateTextForButton(fileName));
                                                  if (onMidiFileChanged) {
                                                      onMidiFileChanged(buttonIndex, fileName);
                                                  }
                                              }
                                          }

                                          savePlayerBeatsButtonState(currentPlayerIndex, state);
                                      });
}

void MainContentComponentLeftSection::showAdvancedMidiMenu(int buttonIndex, ComponentState& state) {
    closeCurrentDropdown();

    if (assignedMidiFiles[buttonIndex].isEmpty()) {
        return;
    }

    currentDropdownMenu = std::make_unique<juce::PopupMenu>();
    currentDropdownMenu->setLookAndFeel(&getLookAndFeel());

    currentDropdownMenu->addItem(INIConfig::LayoutConstants::menuItemAnalyzeGroove, "Analyze Groove", true, false, nullptr);
    currentDropdownMenu->addItem(INIConfig::LayoutConstants::menuItemFindSimilar, "Find Similar Grooves", true, false, nullptr);
    currentDropdownMenu->addItem(INIConfig::LayoutConstants::menuItemCreateVariation, "Create Variation", true, false, nullptr);
    currentDropdownMenu->addSeparator();
    currentDropdownMenu->addItem(INIConfig::LayoutConstants::menuItemSaveTemplate, "Save as Template", true, false, nullptr);
    currentDropdownMenu->addItem(INIConfig::LayoutConstants::menuItemExportAutomation, "Export with Automation", true, false, nullptr);
    currentDropdownMenu->addSeparator();
    currentDropdownMenu->addItem(INIConfig::LayoutConstants::menuItemReplace, "Replace with...", true, false, nullptr);
    currentDropdownMenu->addItem(INIConfig::LayoutConstants::menuItemClear, "Clear", true, false, nullptr);

    currentDropdownMenu->showMenuAsync(juce::PopupMenu::Options().withTargetComponent(&drumButtons[buttonIndex]),
                                      [this, buttonIndex, &state](int result) {
                                          currentDropdownMenu.reset();

                                          switch (result) {
                                              case INIConfig::LayoutConstants::menuItemAnalyzeGroove:
                                                  showGrooveAnalysis(buttonIndex);
                                                  break;
                                              case INIConfig::LayoutConstants::menuItemFindSimilar:
                                                  suggestSimilarGrooves(buttonIndex);
                                                  break;
                                              case INIConfig::LayoutConstants::menuItemCreateVariation:
                                                  createGrooveVariation(buttonIndex);
                                                  break;
                                              case INIConfig::LayoutConstants::menuItemSaveTemplate:
                                                  if (midiFileManager) {
                                                      juce::String templateName = "Template_" +
                                                          juce::Time::getCurrentTime().formatted("%Y%m%d_%H%M%S");
                                                      midiFileManager->saveGrooveAsTemplate(assignedMidiFiles[buttonIndex], templateName);
                                                  }
                                                  break;
                                              case INIConfig::LayoutConstants::menuItemExportAutomation:
                                                  exportCurrentPattern();
                                                  break;
                                              case INIConfig::LayoutConstants::menuItemReplace:
                                                  showMidiFileSelectionForButton(buttonIndex, state);
                                                  break;
                                              case INIConfig::LayoutConstants::menuItemClear:
                                                  assignedMidiFiles[buttonIndex] = "";
                                                  drumButtons[buttonIndex].setButtonText("Empty");
                                                  savePlayerBeatsButtonState(currentPlayerIndex, state);
                                                  break;
                                          }
                                      });
}

void MainContentComponentLeftSection::showGrooveAnalysis(int buttonIndex) {
    if (!midiFileManager || assignedMidiFiles[buttonIndex].isEmpty()) {
        return;
    }

    MidiGrooveAnalysis analysis = midiFileManager->analyzeMidiFile(assignedMidiFiles[buttonIndex]);

    juce::String info = formatGrooveAnalysis(analysis);

    juce::AlertWindow::showMessageBoxAsync(
        juce::AlertWindow::InfoIcon,
        "Groove Analysis: " + assignedMidiFiles[buttonIndex],
        info,
        "OK"
    );
}

juce::String MainContentComponentLeftSection::formatGrooveAnalysis(const MidiGrooveAnalysis& analysis) {
    juce::String info;

    info << "Tempo: " << juce::String(analysis.tempo, 1) << " BPM\n";
    info << "Time Signature: " << analysis.timeSignatureNumerator << "/" << analysis.timeSignatureDenominator << "\n";
    info << "Bars: " << analysis.numberOfBars << "\n\n";

    info << "Groove Characteristics:\n";
    info << "- Swing: " << juce::String(analysis.averageSwing, 1) << "%\n";
    info << "- Tightness: " << juce::String(analysis.grooveTightness * 100.0f, 1) << "%\n";
    info << "- Note Density: " << juce::String(analysis.noteDensity, 2) << " notes/beat\n\n";

    info << "Velocity Dynamics:\n";
    info << "- Average: " << juce::String(analysis.averageVelocity, 1) << "\n";
    info << "- Range: " << juce::String(analysis.velocityRange, 1) << "\n";
    info << "- Variation: " << juce::String(analysis.velocityVariation, 1) << "\n";

    return info;
}

void MainContentComponentLeftSection::suggestSimilarGrooves(int buttonIndex) {
    if (!midiFileManager || assignedMidiFiles[buttonIndex].isEmpty()) {
        return;
    }

    juce::StringArray suggestions = midiFileManager->suggestSimilarGrooves(assignedMidiFiles[buttonIndex], 10);

    if (suggestions.isEmpty()) {
        juce::AlertWindow::showMessageBoxAsync(
            juce::AlertWindow::InfoIcon,
            "Similar Grooves",
            "No similar grooves found.",
            "OK"
        );
        return;
    }

    currentDropdownMenu = std::make_unique<juce::PopupMenu>();
    currentDropdownMenu->setLookAndFeel(&getLookAndFeel());

    currentDropdownMenu->addSectionHeader("Similar Grooves:");

    for (int i = 0; i < suggestions.size(); ++i) {
        currentDropdownMenu->addItem(i + 1, suggestions[i]);
    }

    currentDropdownMenu->showMenuAsync(juce::PopupMenu::Options().withTargetComponent(&drumButtons[buttonIndex]),
                                      [this, buttonIndex, suggestions](int result) {
                                          currentDropdownMenu.reset();

                                          if (result > 0 && result <= suggestions.size()) {
                                              assignedMidiFiles[buttonIndex] = suggestions[result - 1];
                                              drumButtons[buttonIndex].setButtonText(
                                                  MidiFileManager::truncateTextForButton(suggestions[result - 1]));

                                              if (onMidiFileChanged) {
                                                  onMidiFileChanged(buttonIndex, suggestions[result - 1]);
                                              }

                                              if (componentState) {
                                                  savePlayerBeatsButtonState(currentPlayerIndex, *componentState);
                                              }
                                          }
                                      });
}

void MainContentComponentLeftSection::createGrooveVariation(int buttonIndex) {
    if (!midiFileManager || assignedMidiFiles[buttonIndex].isEmpty()) {
        return;
    }

    auto variationDialog = std::make_unique<juce::AlertWindow>("Create Groove Variation",
                                                               "Adjust variation amount:",
                                                               juce::AlertWindow::NoIcon);

    auto* slider = new juce::Slider();
    slider->setRange(INIConfig::LayoutConstants::variationSliderMin, INIConfig::LayoutConstants::variationSliderMax, INIConfig::LayoutConstants::variationSliderStep);
    slider->setValue(INIConfig::LayoutConstants::variationSliderDefault);
    slider->setSliderStyle(juce::Slider::LinearHorizontal);
    slider->setTextBoxStyle(juce::Slider::TextBoxRight, false, INIConfig::LayoutConstants::sliderTextBoxWidth, INIConfig::LayoutConstants::sliderTextBoxHeight);
    slider->setSize(INIConfig::LayoutConstants::variationDialogSliderWidth, INIConfig::LayoutConstants::variationDialogSliderHeight);

    variationDialog->addCustomComponent(slider);
    variationDialog->addButton("Create", 1);
    variationDialog->addButton("Cancel", 0);

    variationDialog->enterModalState(true, juce::ModalCallbackFunction::create(
        [this, buttonIndex, slider](int result) {
            if (result == 1) {
                float variationAmount = static_cast<float>(slider->getValue() / 100.0);

                juce::MidiMessageSequence pattern = midiFileManager->extractGrooveFromMidiFile(assignedMidiFiles[buttonIndex]);
                juce::MidiMessageSequence variation = midiFileManager->createVariation(pattern, variationAmount);

                juce::String newFileName = assignedMidiFiles[buttonIndex] + "_var_" +
                                         juce::String(static_cast<int>(variationAmount * 100));

                juce::Array<juce::MidiMessageSequence> tracks;
                tracks.add(variation);
                juce::Array<float> volumes, pans;
                volumes.add(1.0f);
                pans.add(0.0f);

                if (midiFileManager->exportMidiWithMixerAutomation(newFileName + ".mid", tracks, volumes, pans)) {
                    assignedMidiFiles[buttonIndex] = newFileName;
                    drumButtons[buttonIndex].setButtonText(MidiFileManager::truncateTextForButton(newFileName));

                    if (onMidiFileChanged) {
                        onMidiFileChanged(buttonIndex, newFileName);
                    }

                    if (componentState) {
                        savePlayerBeatsButtonState(currentPlayerIndex, *componentState);
                    }
                }
            }
            delete slider;
        }), true);
}

void MainContentComponentLeftSection::exportCurrentPattern() {
    if (!midiFileManager || !componentState) {
        return;
    }

    juce::Array<juce::MidiMessageSequence> tracks;
    juce::Array<float> volumes, pans;

    for (int i = 0; i < 16; ++i) {
        if (!assignedMidiFiles[i].isEmpty()) {
            juce::MidiMessageSequence pattern = midiFileManager->extractGrooveFromMidiFile(assignedMidiFiles[i]);
            if (pattern.getNumEvents() > 0) {
                tracks.add(pattern);
                volumes.add(componentState->playerSettings[currentPlayerIndex].volume);
                pans.add(componentState->playerSettings[currentPlayerIndex].pan);
            }
        }
    }

    if (tracks.isEmpty()) {
        juce::AlertWindow::showMessageBoxAsync(
            juce::AlertWindow::WarningIcon,
            "Export Pattern",
            "No patterns to export.",
            "OK"
        );
        return;
    }

    auto chooser = std::make_shared<juce::FileChooser>("Export MIDI Pattern",
                                                       juce::File::getSpecialLocation(juce::File::userDocumentsDirectory),
                                                       "*.mid");

    chooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles,
                        [this, chooser, tracks, volumes, pans](const juce::FileChooser& fc) {
        auto results = fc.getResults();
        if (results.size() > 0) {
            juce::File outputFile = results[0];
            juce::String fileName = outputFile.getFullPathName();

            if (midiFileManager->exportMidiWithMixerAutomation(fileName, tracks, volumes, pans)) {
                juce::AlertWindow::showMessageBoxAsync(
                    juce::AlertWindow::InfoIcon,
                    "Export Complete",
                    "Pattern exported successfully to:\n" + fileName,
                    "OK"
                );
            } else {
                juce::AlertWindow::showMessageBoxAsync(
                    juce::AlertWindow::WarningIcon,
                    "Export Failed",
                    "Failed to export pattern.",
                    "OK"
                );
            }
        }
    });
}
