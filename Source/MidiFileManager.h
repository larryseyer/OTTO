#pragma once
#include <JuceHeader.h>
#include "ComponentState.h"
#include "MidiAnalysisTypes.h"
#include "INIConfig.h"

struct MidiFileGroup {
    juce::String groupName;
    juce::String folderPath;
    juce::Array<juce::String> midiFiles;
    juce::Array<juce::String> displayNames;
    bool isCustomGroup = false;
    bool isFavorite = false;
    int selectedButton = INIConfig::Defaults::DEFAULT_SELECTED_BUTTON;

    MidiFileGroup() = default;
    MidiFileGroup(const juce::String& name, const juce::String& path, bool custom = false)
        : groupName(name), folderPath(path), isCustomGroup(custom), isFavorite(false), selectedButton(INIConfig::Defaults::DEFAULT_SELECTED_BUTTON) {}
};

class MidiFileManager {
public:
    MidiFileManager();
    ~MidiFileManager() = default;

    void setMidiFilesFolder(const juce::File& folder);
    void scanMidiFiles();
    void initializeBeatsButtonGroups();

    juce::Array<MidiFileGroup> getAvailableGroups() const { return availableGroups; }
    juce::StringArray getGroupNames() const;
    juce::String getCurrentGroupName() const { return currentGroupName; }
    juce::Array<juce::String> getCurrentGroupFiles() const;
    juce::Array<juce::String> getCurrentGroupDisplayNames() const;
    bool selectGroup(const juce::String& groupName);
    juce::File getMidiFile(const juce::String& fileName) const;

    void addBeatsButtonGroup(const juce::String& groupName, const juce::Array<juce::String>& fileNames, bool isFavorite = false, int selectedButton = INIConfig::Defaults::DEFAULT_SELECTED_BUTTON);
    void removeBeatsButtonGroup(const juce::String& groupName);
    bool isBeatsButtonGroup(const juce::String& groupName) const;
    juce::Array<juce::String> getBeatsButtonGroupFiles(const juce::String& groupName) const;
    void setBeatsButtonGroupFiles(const juce::String& groupName, const juce::Array<juce::String>& fileNames);
    void setBeatsButtonGroupFavorite(const juce::String& groupName, bool isFavorite);
    bool getBeatsButtonGroupFavorite(const juce::String& groupName) const;
    void setBeatsButtonGroupSelectedButton(const juce::String& groupName, int buttonIndex);
    int getBeatsButtonGroupSelectedButton(const juce::String& groupName) const;

    void addCustomGroup(const juce::String& groupName, const juce::Array<juce::String>& fileNames);
    void removeCustomGroup(const juce::String& groupName);

    static juce::String truncateTextForButton(const juce::String& text, int maxLength = INIConfig::UI::BUTTON_TEXT_MAX_LENGTH / INIConfig::UI::BEATS_BUTTON_TEXT_LENGTH_DIVISOR);
    void autoPopulateGroupsFromMidiFiles();
    void assignMidiFilesToButtons(const juce::Array<juce::String>& midiFiles);

    void createUnlimitedGroupsWithSixteenMidiFiles();
    juce::Array<juce::String> getAllMidiFilesAlphabetically() const;
    int getNumberOfRequiredGroups() const;
    void createGroupsFromSortedMidiFiles(const juce::Array<juce::String>& sortedFiles);

    MidiGrooveAnalysis analyzeMidiFile(const juce::String& fileName);
    void autoMapMidiFileToKit(const juce::String& fileName, int playerIndex);
    juce::StringArray suggestSimilarGrooves(const juce::String& fileName, int maxSuggestions = INIConfig::UI::MAX_TOGGLE_STATES);

    juce::MidiMessageSequence extractGrooveFromMidiFile(const juce::String& fileName);
    bool saveGrooveAsTemplate(const juce::String& sourceMidiFile, const juce::String& templateName);
    juce::StringArray getGrooveTemplates() const;

    bool exportMidiWithMixerAutomation(const juce::String& outputFileName,
                                      const juce::Array<juce::MidiMessageSequence>& tracks,
                                      const juce::Array<float>& volumeAutomation,
                                      const juce::Array<float>& panAutomation);

    bool isREXFile(const juce::File& file) const;
    juce::Array<juce::MidiMessageSequence> loadREXFile(const juce::File& rexFile);
    bool convertREXToMidi(const juce::File& rexFile, const juce::File& outputMidiFile);

    juce::MidiMessageSequence mergePatterns(const juce::Array<juce::MidiMessageSequence>& patterns);
    juce::Array<juce::MidiMessageSequence> splitPattern(const juce::MidiMessageSequence& pattern,
                                                       int numberOfParts);
    juce::MidiMessageSequence createVariation(const juce::MidiMessageSequence& pattern,
                                             float variationAmount);

    void organizeFilesByTempo();
    void organizeFilesByGroove();
    void createSmartGroups();

    void saveStates(ComponentState& state);
    void loadStates(const ComponentState& state);

private:
    juce::File midiFilesFolder;
    juce::Array<MidiFileGroup> availableGroups;
    juce::String currentGroupName;
    juce::StringArray rootMidiFileNames;

    std::map<juce::String, MidiGrooveAnalysis> analysisCache;

    juce::File grooveTemplatesFolder;
    juce::Array<MidiGrooveAnalysis> grooveTemplates;

    juce::File getAssetsPath();
    void scanFolderRecursively(const juce::File& folder, const juce::String& relativePath = "");
    void createInitialBeatsButtonGroups();

    float calculateSwing(const juce::MidiMessageSequence& sequence);
    float calculateGrooveTightness(const juce::MidiMessageSequence& sequence);
    void detectTimeSignature(const juce::MidiMessageSequence& sequence, int& numerator, int& denominator);
    float estimateTempo(const juce::MidiMessageSequence& sequence);

    void quantizeToGrid(juce::MidiMessageSequence& sequence, int gridSubdivision);
    void humanizePattern(juce::MidiMessageSequence& sequence, float amount);
    void applySwing(juce::MidiMessageSequence& sequence, float swingAmount);

    struct REXSlice {
        double startTime;
        double length;
        int pitchOffset;
        juce::MemoryBlock audioData;
    };

    juce::Array<REXSlice> parseREXFile(const juce::File& rexFile);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiFileManager)
};
