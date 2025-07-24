#pragma once
#include <JuceHeader.h>
#include "ComponentState.h"
#include "SFZVoiceAllocator.h"
#include <unordered_map>

struct DrumkitInfo {
    juce::String name;
    juce::String folderPath;
    juce::Array<juce::String> sfzFiles;

    DrumkitInfo() = default;
    DrumkitInfo(const juce::String& kitName, const juce::String& path)
        : name(kitName), folderPath(path) {}
};

class SFZEngine {
public:
    SFZEngine();
    ~SFZEngine() {
        release();
    }

    void prepare(double sampleRate, int samplesPerBlock);
    void process(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);
    void release();

    void setSFZFolder(const juce::File& folder);
    juce::Array<juce::File> getSFZFiles() const;
    void loadSFZFile(const juce::String& fileName);
    juce::File getSFZFolder() const { return sfzFolder; }

    void scanDrumkitsFolder();
    juce::Array<DrumkitInfo> getAvailableDrumkits() const { return availableDrumkits; }
    void loadDrumkit(const juce::String& drumkitName, const juce::String& sfzFileName);

    juce::String getCurrentDrumkitName() const { return currentDrumkitName; }
    juce::String getCurrentSFZFile() const { return currentSFZFile; }

    void setCurrentPlayer(int playerIndex);
    int getCurrentPlayer() const { return currentPlayerIndex; }

    juce::String getPlayerDrumkit(int playerIndex) const;
    void setPlayerDrumkit(int playerIndex, const juce::String& drumkitName);

    void saveStates(ComponentState& state);
    void loadStates(const ComponentState& state);

    struct PlayerDrumkitSelection {
        juce::String drumkitName;
        juce::String sfzFileName;
        bool enabled = true;
    };

    PlayerDrumkitSelection getPlayerSelection(int playerIndex) const {
       if (playerIndex >= INIConfig::Validation::MIN_PLAYER_INDEX && playerIndex <= INIConfig::Validation::MAX_PLAYER_INDEX) {
           return playerSelections[playerIndex];
       }
       return PlayerDrumkitSelection();
   }

   void setPlayerSelection(int playerIndex, const PlayerDrumkitSelection& selection) {
       if (playerIndex >= INIConfig::Validation::MIN_PLAYER_INDEX && playerIndex <= INIConfig::Validation::MAX_PLAYER_INDEX) {
           playerSelections[playerIndex] = selection;
           if (playerIndex == currentPlayerIndex && !selection.drumkitName.isEmpty()) {
               loadDrumkit(selection.drumkitName, selection.sfzFileName);
           }
       }
   }

   void setMaxVoices(int maxVoices) { voiceAllocator.setMaxVoices(maxVoices); }
   int getMaxVoices() const { return voiceAllocator.getMaxVoices(); }
   int getActiveVoiceCount() const { return voiceAllocator.getActiveVoiceCount(); }

private:
   struct VelocityLayer {
       int loVel = INIConfig::Validation::MIN_MIDI_VELOCITY;
       int hiVel = INIConfig::Validation::MAX_MIDI_VELOCITY;
       juce::String samplePath;
       std::unique_ptr<juce::AudioFormatReaderSource> source;
       float volume = INIConfig::Defaults::VOLUME;
   };

   struct Region {
       int key = INIConfig::Validation::MIN_MIDI_NOTE;
       int loKey = INIConfig::Validation::MIN_MIDI_NOTE;
       int hiKey = INIConfig::Validation::MAX_MIDI_NOTE;
       std::vector<VelocityLayer> velocityLayers;
       SFZVoice::ADSRParameters adsr;
       int roundRobinCount = INIConfig::Defaults::ONE_VALUE;
       int currentRoundRobin = INIConfig::Defaults::ZERO_VALUE;

       VelocityLayer* getLayerForVelocity(int velocity);
       VelocityLayer* getNextRoundRobinLayer(int velocity);
   };

   juce::AudioFormatManager formatManager;
   std::unordered_map<int, std::unique_ptr<Region>> regions;
   SFZVoiceAllocator voiceAllocator;
   double sampleRate = static_cast<double>(INIConfig::Defaults::DEFAULT_SAMPLE_RATE);
   juce::File sfzFolder;
   juce::Array<DrumkitInfo> availableDrumkits;
   juce::String currentDrumkitName;
   juce::String currentSFZFile;
   PlayerDrumkitSelection playerSelections[INIConfig::Defaults::MAX_PLAYERS];
   int currentPlayerIndex = INIConfig::Defaults::DEFAULT_CURRENT_PLAYER;

   juce::File getAssetsPath();
   void loadSFZFile();
   void createDefaultSFZMapping();
   void loadSFZFileFromPath(const juce::File& sfzFile);
   void initializeDefaultPlayerDrumkits();
   void loadPlayerDrumkitFromState(int playerIndex);
   void parseSFZOpcode(Region& region, const juce::String& opcode, const juce::String& value);

   JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SFZEngine)
};
