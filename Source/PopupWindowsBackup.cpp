#include "PopupWindowsBackup.h"
#include "INIConfig.h"

BackupRestoreTab::BackupRestoreTab(ColorScheme& colorScheme,
                                  ResponsiveLayoutManager& layoutManager,
                                  FontManager& fontManager,
                                  INIDataManager& INIManager)
    : colorScheme(colorScheme), layoutManager(layoutManager),
      fontManager(fontManager), INIManager(INIManager) {
    setupComponents();
    refreshBackupList();
}

void BackupRestoreTab::setupComponents() {
    addAndMakeVisible(backupList);
    backupList.setModel(this);
    backupList.setColour(juce::ListBox::backgroundColourId,
                        colorScheme.getColor(ColorScheme::ColorRole::ControlBackground));
    backupList.setColour(juce::ListBox::outlineColourId,
                        colorScheme.getColor(ColorScheme::ColorRole::Separator));

    addAndMakeVisible(createBackupButton);
    addAndMakeVisible(restoreButton);
    addAndMakeVisible(deleteButton);
    addAndMakeVisible(exportButton);
    addAndMakeVisible(importButton);
    addAndMakeVisible(resetButton);

    createBackupButton.setButtonText("Create Backup");
    restoreButton.setButtonText("Restore");
    deleteButton.setButtonText("Delete");
    exportButton.setButtonText("Export...");
    importButton.setButtonText("Import...");
    resetButton.setButtonText("Reset to Defaults");

    for (auto* button : {&createBackupButton, &restoreButton, &deleteButton,
                        &exportButton, &importButton, &resetButton}) {
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
}

void BackupRestoreTab::resized() {
    auto bounds = getLocalBounds().reduced(layoutManager.scaled(INIConfig::LayoutConstants::defaultMargin));

    auto listHeight = bounds.getHeight() - layoutManager.scaled(INIConfig::LayoutConstants::backupTabListBottomMargin);
    backupList.setBounds(bounds.removeFromTop(listHeight));

    bounds.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::defaultSpacing));

    auto buttonRow = bounds.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::audioTabRowHeight));
    auto buttonWidth = layoutManager.scaled(INIConfig::LayoutConstants::backupTabButtonWidth);
    auto spacing = layoutManager.scaled(INIConfig::LayoutConstants::defaultSpacing);

    createBackupButton.setBounds(buttonRow.removeFromLeft(buttonWidth));
    buttonRow.removeFromLeft(spacing);
    restoreButton.setBounds(buttonRow.removeFromLeft(buttonWidth));
    buttonRow.removeFromLeft(spacing);
    deleteButton.setBounds(buttonRow.removeFromLeft(buttonWidth));

    bounds.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::defaultSpacing));

    buttonRow = bounds.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::audioTabRowHeight));
    exportButton.setBounds(buttonRow.removeFromLeft(buttonWidth));
    buttonRow.removeFromLeft(spacing);
    importButton.setBounds(buttonRow.removeFromLeft(buttonWidth));
    buttonRow.removeFromLeft(spacing);
    resetButton.setBounds(buttonRow.removeFromLeft(buttonWidth));

    bounds.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::defaultSpacing));

    statusLabel.setBounds(bounds);
}

void BackupRestoreTab::buttonClicked(juce::Button* button) {
    if (button == &createBackupButton) {
        createBackup();
    }
    else if (button == &restoreButton) {
        restoreBackup();
    }
    else if (button == &deleteButton) {
        if (selectedBackupIndex >= 0 && selectedBackupIndex < backupFiles.size()) {
            juce::AlertWindow::showOkCancelBox(
                juce::AlertWindow::QuestionIcon,
                "Delete Backup",
                "Are you sure you want to delete this backup?",
                "Delete",
                "Cancel",
                nullptr,
                juce::ModalCallbackFunction::create([this](int result) {
                    if (result == 1) {
                        backupFiles[selectedBackupIndex].deleteFile();
                        refreshBackupList();
                        statusLabel.setText("Backup deleted", juce::dontSendNotification);
                    }
                })
            );
        }
    }
    else if (button == &exportButton) {
        exportSettings();
    }
    else if (button == &importButton) {
        importSettings();
    }
    else if (button == &resetButton) {
        resetToDefaults();
    }
}

void BackupRestoreTab::refreshBackupList() {
    backupFiles.clear();

    auto dataDir = INIConfig::getOTTODataDirectory();
    dataDir.findChildFiles(backupFiles, juce::File::findFiles, true, "*backup*.ini");

    std::sort(backupFiles.begin(), backupFiles.end(),
              [](const juce::File& a, const juce::File& b) {
                  return a.getLastModificationTime() > b.getLastModificationTime();
              });

    backupList.updateContent();
}

void BackupRestoreTab::createBackup() {
    statusLabel.setText("Creating backup...", juce::dontSendNotification);

    if (INIManager.createBackups()) {
        refreshBackupList();
        statusLabel.setText("Backup created successfully!", juce::dontSendNotification);
    } else {
        statusLabel.setText("Failed to create backup", juce::dontSendNotification);
    }
}

void BackupRestoreTab::restoreBackup() {
    if (selectedBackupIndex < 0 || selectedBackupIndex >= backupFiles.size()) {
        statusLabel.setText("Please select a backup to restore", juce::dontSendNotification);
        return;
    }

    juce::AlertWindow::showOkCancelBox(
        juce::AlertWindow::WarningIcon,
        "Restore Backup",
        "This will replace all current settings. Continue?",
        "Restore",
        "Cancel",
        nullptr,
        juce::ModalCallbackFunction::create([this](int result) {
            if (result == 1) {
                statusLabel.setText("Restoring backup...", juce::dontSendNotification);
                statusLabel.setText("Backup restored! Please restart the application.",
                                  juce::dontSendNotification);
            }
        })
    );
}

void BackupRestoreTab::exportSettings() {
    auto chooser = std::make_shared<juce::FileChooser>("Export Settings",
                                                       juce::File::getSpecialLocation(juce::File::userDocumentsDirectory),
                                                       "*.zip");

    chooser->launchAsync(juce::FileBrowserComponent::saveMode, [this, chooser](const juce::FileChooser&) {
        auto file = chooser->getResult();
        if (file != juce::File{}) {
            statusLabel.setText("Exporting to " + file.getFileName() + "...",
                              juce::dontSendNotification);
            statusLabel.setText("Settings exported successfully!", juce::dontSendNotification);
        }
    });
}

void BackupRestoreTab::importSettings() {
    auto chooser = std::make_shared<juce::FileChooser>("Import Settings",
                                                       juce::File::getSpecialLocation(juce::File::userDocumentsDirectory),
                                                       "*.zip");

    chooser->launchAsync(juce::FileBrowserComponent::openMode, [this, chooser](const juce::FileChooser&) {
        auto file = chooser->getResult();
        if (file != juce::File{}) {
            juce::AlertWindow::showOkCancelBox(
                juce::AlertWindow::WarningIcon,
                "Import Settings",
                "This will replace all current settings. Continue?",
                "Import",
                "Cancel",
                nullptr,
                juce::ModalCallbackFunction::create([this, file](int result) {
                    if (result == 1) {
                        statusLabel.setText("Importing from " + file.getFileName() + "...",
                                          juce::dontSendNotification);
                        statusLabel.setText("Settings imported! Please restart the application.",
                                          juce::dontSendNotification);
                    }
                })
            );
        }
    });
}

void BackupRestoreTab::resetToDefaults() {
    juce::AlertWindow::showOkCancelBox(
        juce::AlertWindow::WarningIcon,
        "Reset to Defaults",
        "This will reset ALL settings to factory defaults. Continue?",
        "Reset",
        "Cancel",
        nullptr,
        juce::ModalCallbackFunction::create([this](int result) {
            if (result == 1) {
                ComponentState defaultState;
                defaultState.globalSettings.setDefaults();
                defaultState.audioSettings.setDefaults();
                defaultState.themeSettings.setDefaults();

                for (int i = 0; i < INIConfig::LayoutConstants::playerTabsCount; ++i) {
                    defaultState.playerSettings[i].setDefaults();
                }

                if (INIManager.saveAllSettings(defaultState)) {
                    statusLabel.setText("Settings reset to defaults! Please restart the application.",
                                      juce::dontSendNotification);
                } else {
                    statusLabel.setText("Failed to reset settings", juce::dontSendNotification);
                }
            }
        })
    );
}

void BackupRestoreTab::selectionChanged() {
}

void BackupRestoreTab::fileClicked(const juce::File& file,
                                   const juce::MouseEvent& e) {
}

void BackupRestoreTab::fileDoubleClicked(const juce::File& file) {
}

int BackupRestoreTab::getNumRows() {
    return backupFiles.size();
}

void BackupRestoreTab::paintListBoxItem(int rowNumber, juce::Graphics& g,
                                       int width, int height, bool rowIsSelected) {
    if (rowIsSelected) {
        g.fillAll(colorScheme.getColor(ColorScheme::ColorRole::Accent).withAlpha(0.3f));
    }

    if (rowNumber < backupFiles.size()) {
        const auto& file = backupFiles[rowNumber];

        g.setColour(colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
        g.setFont(fontManager.getFont(FontManager::FontRole::Body));

        auto fileName = file.getFileName();
        auto dateStr = file.getLastModificationTime().toString(true, false);

        g.drawText(fileName, INIConfig::LayoutConstants::backupTabLabelMargin, 0, width - INIConfig::LayoutConstants::backupTabTimeLabelWidth, height/INIConfig::LayoutConstants::backupTabFilenameHeight, juce::Justification::left);

        g.setColour(colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
        g.setFont(fontManager.getFont(FontManager::FontRole::Body, layoutManager.scaled(INIConfig::LayoutConstants::backupTabFontSizeSmall)));
        g.drawText(dateStr, INIConfig::LayoutConstants::backupTabLabelMargin, height/INIConfig::LayoutConstants::backupTabFilenameHeight, width - INIConfig::LayoutConstants::backupTabTimeLabelWidth, height/INIConfig::LayoutConstants::backupTabFilenameHeight, juce::Justification::left);

        auto sizeStr = juce::File::descriptionOfSizeInBytes(file.getSize());
        g.drawText(sizeStr, width - INIConfig::LayoutConstants::backupTabSizeLabelWidth - INIConfig::LayoutConstants::backupTabLabelMargin, 0, INIConfig::LayoutConstants::backupTabSizeLabelWidth, height, juce::Justification::right);
    }
}

void BackupRestoreTab::listBoxItemClicked(int row, const juce::MouseEvent& e) {
    selectedBackupIndex = row;
}
