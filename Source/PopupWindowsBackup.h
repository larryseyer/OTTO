#pragma once
#include <JuceHeader.h>
#include "ColorScheme.h"
#include "ResponsiveLayoutManager.h"
#include "FontManager.h"
#include "INIDataManager.h"
#include "ComponentState.h"
#include "INIConfig.h"

class BackupRestoreTab : public juce::Component,
                        public juce::ListBoxModel,
                        public juce::FileBrowserListener,
                        public juce::Button::Listener {
public:
    BackupRestoreTab(ColorScheme& colorScheme,
                    ResponsiveLayoutManager& layoutManager,
                    FontManager& fontManager,
                    INIDataManager& INIManager);

    void resized() override;
    void buttonClicked(juce::Button* button) override;

    int getNumRows() override;
    void paintListBoxItem(int rowNumber, juce::Graphics& g,
                        int width, int height, bool rowIsSelected) override;
    void listBoxItemClicked(int row, const juce::MouseEvent& e) override;

    void selectionChanged() override;
    void fileClicked(const juce::File& file, const juce::MouseEvent& e) override;
    void fileDoubleClicked(const juce::File& file) override;
    void browserRootChanged(const juce::File&) override {}

private:
    ColorScheme& colorScheme;
    ResponsiveLayoutManager& layoutManager;
    FontManager& fontManager;
    INIDataManager& INIManager;

    juce::ListBox backupList;
    juce::TextButton createBackupButton, restoreButton, deleteButton;
    juce::TextButton exportButton, importButton, resetButton;
    juce::Label statusLabel;

    juce::Array<juce::File> backupFiles;
    int selectedBackupIndex = -1;

    void setupComponents();
    void refreshBackupList();
    void createBackup();
    void restoreBackup();
    void exportSettings();
    void importSettings();
    void resetToDefaults();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BackupRestoreTab)
};
