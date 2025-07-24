#include "SFZEngine.h"
#include "INIConfig.h"

SFZEngine::VelocityLayer* SFZEngine::Region::getLayerForVelocity(int velocity) {
    for (auto& layer : velocityLayers) {
        if (velocity >= layer.loVel && velocity <= layer.hiVel) {
            return &layer;
        }
    }
    return velocityLayers.empty() ? nullptr : &velocityLayers[0];
}

SFZEngine::VelocityLayer* SFZEngine::Region::getNextRoundRobinLayer(int velocity) {
    if (roundRobinCount <= 1 || velocityLayers.empty()) {
        return getLayerForVelocity(velocity);
    }

    std::vector<VelocityLayer*> validLayers;
    for (auto& layer : velocityLayers) {
        if (velocity >= layer.loVel && velocity <= layer.hiVel) {
            validLayers.push_back(&layer);
        }
    }

    if (validLayers.empty()) return nullptr;

    currentRoundRobin = (currentRoundRobin + 1) % validLayers.size();
    return validLayers[currentRoundRobin];
}

SFZEngine::SFZEngine() {
    formatManager.registerBasicFormats();

    sfzFolder = getAssetsPath().getChildFile("Drumkits");

    if (sfzFolder.exists()) {
        scanDrumkitsFolder();
        initializeDefaultPlayerDrumkits();

        if (!availableDrumkits.isEmpty()) {
            for (const auto& kit : availableDrumkits) {
                if (kit.name == "Acoustic") {
                    if (!kit.sfzFiles.isEmpty()) {
                        loadDrumkit(kit.name, kit.sfzFiles[0]);
                    }
                    return;
                }
            }

            const auto& firstKit = availableDrumkits.getFirst();
            if (!firstKit.sfzFiles.isEmpty()) {
                loadDrumkit(firstKit.name, firstKit.sfzFiles[0]);
            }
        }
    }
}

juce::File SFZEngine::getAssetsPath() {
    juce::File exePath = juce::File::getSpecialLocation(juce::File::currentExecutableFile);

    juce::File bundleContents = exePath.getParentDirectory().getParentDirectory();
    if (bundleContents.getChildFile("Resources").exists()) {
        juce::File bundleAssets = bundleContents.getChildFile("Resources/Assets");
        if (bundleAssets.exists()) {
            return bundleAssets;
        }
    }

    juce::File execAssets = exePath.getParentDirectory().getChildFile("Assets");
    if (execAssets.exists()) {
        return execAssets;
    }

    juce::File parentAssets = exePath.getParentDirectory().getParentDirectory().getChildFile("Assets");
    if (parentAssets.exists()) {
        return parentAssets;
    }

    return juce::File();
}

void SFZEngine::prepare(double newSampleRate, int samplesPerBlock) {
    sampleRate = newSampleRate;
    voiceAllocator.prepare(sampleRate, samplesPerBlock);

    for (auto& [note, region] : regions) {
        for (auto& layer : region->velocityLayers) {
            if (layer.source) {
                layer.source->prepareToPlay(samplesPerBlock, sampleRate);
            }
        }
    }
}

void SFZEngine::process(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
    if (buffer.getNumChannels() == 0 || buffer.getNumSamples() == 0)
        return;

    for (const auto midi : midiMessages) {
        auto msg = midi.getMessage();

        if (msg.isNoteOn()) {
            int noteNumber = msg.getNoteNumber();
            int velocity = msg.getVelocity();
            float velocityFloat = velocity / static_cast<float>(INIConfig::Validation::MAX_MIDI_VELOCITY);

            auto regionIt = regions.find(noteNumber);
            if (regionIt != regions.end()) {
                auto& region = regionIt->second;

                auto* layer = region->getNextRoundRobinLayer(velocity);
                if (layer && layer->source) {
                    SFZVoice* voice = voiceAllocator.allocateVoice(noteNumber);
                    if (voice) {
                        voice->startNote(noteNumber, velocityFloat, sampleRate,
                                       layer->source.get(), region->adsr,
                                       INIConfig::LayoutConstants::sliderTextBoxHeight * std::log10(layer->volume));
                    }
                }
            }
        }
        else if (msg.isNoteOff()) {
            voiceAllocator.releaseVoicesForNote(msg.getNoteNumber());
        }
        else if (msg.isAllNotesOff() || msg.isAllSoundOff()) {
            voiceAllocator.releaseAllVoices();
        }
    }

    voiceAllocator.renderNextBlock(buffer);
}

void SFZEngine::release() {
    voiceAllocator.reset();
    regions.clear();
}

void SFZEngine::setSFZFolder(const juce::File& folder) {
    sfzFolder = folder;
    scanDrumkitsFolder();
    initializeDefaultPlayerDrumkits();
}

void SFZEngine::scanDrumkitsFolder() {
    availableDrumkits.clear();

    if (!sfzFolder.exists()) {
        return;
    }

    juce::Array<juce::File> subFolders;
    sfzFolder.findChildFiles(subFolders, juce::File::findDirectories, false);

    for (const auto& subFolder : subFolders) {
        DrumkitInfo kitInfo(subFolder.getFileName(), subFolder.getFullPathName());

        juce::Array<juce::File> sfzFiles;
        subFolder.findChildFiles(sfzFiles, juce::File::findFiles, false, "*.sfz");

        for (const auto& sfzFile : sfzFiles) {
            kitInfo.sfzFiles.add(sfzFile.getFileNameWithoutExtension());
        }

        if (!kitInfo.sfzFiles.isEmpty()) {
            availableDrumkits.add(kitInfo);
        }
    }
}

void SFZEngine::loadDrumkit(const juce::String& drumkitName, const juce::String& sfzFileName) {
    DrumkitInfo* targetKit = nullptr;
    for (auto& kit : availableDrumkits) {
        if (kit.name == drumkitName) {
            targetKit = &kit;
            break;
        }
    }

    if (!targetKit) {
        return;
    }

    juce::File drumkitFolder(targetKit->folderPath);
    juce::File sfzFile = drumkitFolder.getChildFile(sfzFileName + ".sfz");

    if (!sfzFile.existsAsFile()) {
        return;
    }

    currentDrumkitName = drumkitName;
    currentSFZFile = sfzFileName;

    if (currentPlayerIndex >= 0 && currentPlayerIndex < INIConfig::Defaults::MAX_PLAYERS) {
        playerSelections[currentPlayerIndex].drumkitName = drumkitName;
        playerSelections[currentPlayerIndex].sfzFileName = sfzFileName;
        playerSelections[currentPlayerIndex].enabled = true;
    }

    loadSFZFileFromPath(sfzFile);
}

juce::Array<juce::File> SFZEngine::getSFZFiles() const {
    juce::Array<juce::File> files;
    if (sfzFolder.exists()) {
        for (const auto& kitInfo : availableDrumkits) {
            juce::File kitFolder(kitInfo.folderPath);
            juce::Array<juce::File> kitSfzFiles;
            kitFolder.findChildFiles(kitSfzFiles, juce::File::findFiles, false, "*.sfz");
            files.addArray(kitSfzFiles);
        }
    }
    return files;
}

void SFZEngine::loadSFZFile(const juce::String& fileName) {
    if (!availableDrumkits.isEmpty()) {
        for (const auto& kit : availableDrumkits) {
            if (kit.sfzFiles.contains(fileName)) {
                loadDrumkit(kit.name, fileName);
                return;
            }
        }
    }

    regions.clear();
    if (!sfzFolder.exists()) {
        return;
    }

    juce::File sfzFile = sfzFolder.getChildFile(fileName + ".sfz");

    if (!sfzFile.existsAsFile() && fileName == "default") {
        createDefaultSFZMapping();
        return;
    }

    if (!sfzFile.existsAsFile()) {
        return;
    }

    loadSFZFileFromPath(sfzFile);
}

void SFZEngine::loadSFZFileFromPath(const juce::File& sfzFile) {
    regions.clear();
    voiceAllocator.reset();

    if (!sfzFile.existsAsFile()) {
        return;
    }

    auto sfzText = sfzFile.loadFileAsString();
    auto lines = juce::StringArray::fromLines(sfzText);
    juce::String currentPath = sfzFile.getParentDirectory().getFullPathName();

    std::unique_ptr<Region> currentRegion;
    std::unique_ptr<VelocityLayer> currentLayer;

    for (const auto& line : lines) {
        auto trimmedLine = line.trim();
        if (trimmedLine.isEmpty() || trimmedLine.startsWith("//"))
            continue;

        if (trimmedLine.startsWith("<region>")) {
            if (currentRegion && !currentRegion->velocityLayers.empty()) {
                int key = currentRegion->key;
                regions[key] = std::move(currentRegion);
            }

            currentRegion = std::make_unique<Region>();
            currentLayer = std::make_unique<VelocityLayer>();

            auto tokens = juce::StringArray::fromTokens(trimmedLine, " \t", "");
            for (const auto& token : tokens) {
                if (token.contains("=")) {
                    auto opcode = token.upToFirstOccurrenceOf("=", false, false).trim();
                    auto value = token.fromFirstOccurrenceOf("=", false, false).trim();
                    parseSFZOpcode(*currentRegion, opcode, value);

                    if (opcode == "sample") {
                        currentLayer->samplePath = value;
                        if (currentLayer->samplePath.startsWith("\"") &&
                            currentLayer->samplePath.endsWith("\"")) {
                            currentLayer->samplePath = currentLayer->samplePath.substring(1,
                                                     currentLayer->samplePath.length() - 1);
                        }
                        if (!juce::File::isAbsolutePath(currentLayer->samplePath)) {
                           currentLayer->samplePath = currentPath + "/" + currentLayer->samplePath;
                       }
                   }
                   else if (opcode == "volume") {
                       float vol = value.getFloatValue();
                       if (vol < 0) {
                           currentLayer->volume = juce::Decibels::decibelsToGain(vol);
                       } else if (vol > 1.0f) {
                           currentLayer->volume = vol / INIConfig::LayoutConstants::sfzOffsetMultiplier;
                       } else {
                           currentLayer->volume = vol;
                       }
                   }
                   else if (opcode == "lovel") {
                       currentLayer->loVel = value.getIntValue();
                   }
                   else if (opcode == "hivel") {
                       currentLayer->hiVel = value.getIntValue();
                   }
               }
           }

           if (!currentLayer->samplePath.isEmpty()) {
               juce::File sampleFile(currentLayer->samplePath);
               if (sampleFile.existsAsFile()) {
                   auto* reader = formatManager.createReaderFor(sampleFile);
                   if (reader) {
                       currentLayer->source = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
                       if (sampleRate > 0) {
                           currentLayer->source->prepareToPlay(INIConfig::LayoutConstants::separatorComponentDefaultWidth * INIConfig::LayoutConstants::drumKitSectionBorderThickness * INIConfig::LayoutConstants::drumKitSpacing, sampleRate);
                       }
                       currentRegion->velocityLayers.push_back(std::move(*currentLayer));
                       currentLayer = std::make_unique<VelocityLayer>();
                   }
               }
           }
       }
   }

   if (currentRegion && !currentRegion->velocityLayers.empty()) {
       int key = currentRegion->key;
       regions[key] = std::move(currentRegion);
   }
}

void SFZEngine::parseSFZOpcode(Region& region, const juce::String& opcode, const juce::String& value) {
   if (opcode == "key") {
       region.key = value.getIntValue();
       region.loKey = region.key;
       region.hiKey = region.key;
   }
   else if (opcode == "lokey") {
       region.loKey = value.getIntValue();
   }
   else if (opcode == "hikey") {
       region.hiKey = value.getIntValue();
   }
   else if (opcode == "ampeg_attack") {
       region.adsr.attackTime = value.getFloatValue();
   }
   else if (opcode == "ampeg_decay") {
       region.adsr.decayTime = value.getFloatValue();
   }
   else if (opcode == "ampeg_sustain") {
       region.adsr.sustainLevel = value.getFloatValue() / INIConfig::LayoutConstants::sfzOffsetMultiplier;
   }
   else if (opcode == "ampeg_release") {
       region.adsr.releaseTime = value.getFloatValue();
   }
   else if (opcode == "seq_length") {
       region.roundRobinCount = value.getIntValue();
   }
}

void SFZEngine::createDefaultSFZMapping() {
   if (!sfzFolder.exists()) {
       return;
   }

   juce::Array<juce::File> audioFiles;
   sfzFolder.findChildFiles(audioFiles, juce::File::findFiles, false, "*.wav;*.aif;*.aiff;*.flac;*.ogg");

   if (audioFiles.isEmpty()) {
       return;
   }

   struct DefaultMapping {
       juce::String filePattern;
       int midiNote;
       float volume;
       SFZVoice::ADSRParameters adsr;
   };

   DefaultMapping mappings[] = {
       {"kick", INIConfig::LayoutConstants::sfzBaseMidiNote, 0.8f, {0.001f, 0.5f, 0.0f, 0.1f}},
       {"snare", INIConfig::LayoutConstants::sfzBaseMidiNote + 2, 0.7f, {0.001f, 0.2f, 0.0f, 0.15f}},
       {"hihat", INIConfig::LayoutConstants::sfzBaseMidiNote + 6, 0.6f, {0.001f, 0.05f, 0.0f, 0.05f}},
       {"hat", INIConfig::LayoutConstants::sfzBaseMidiNote + 6, 0.6f, {0.001f, 0.05f, 0.0f, 0.05f}},
       {"crash", INIConfig::LayoutConstants::sfzBaseMidiNote + 13, 0.7f, {0.001f, 2.0f, 0.3f, 1.0f}},
       {"ride", INIConfig::LayoutConstants::sfzBaseMidiNote + 15, 0.6f, {0.001f, 1.0f, 0.4f, 0.8f}},
       {"tom", INIConfig::LayoutConstants::sfzBaseMidiNote + 7, 0.7f, {0.001f, 0.3f, 0.0f, 0.2f}},
       {"clap", INIConfig::LayoutConstants::sfzBaseMidiNote + 3, 0.6f, {0.001f, 0.1f, 0.0f, 0.1f}},
       {"perc", INIConfig::LayoutConstants::sfzBaseMidiNote + 1, 0.5f, {0.001f, 0.1f, 0.0f, 0.1f}}
   };

   int currentNote = INIConfig::LayoutConstants::sfzBaseMidiNote;

   for (const auto& audioFile : audioFiles) {
       auto region = std::make_unique<Region>();
       VelocityLayer layer;

       layer.samplePath = audioFile.getFullPathName();
       layer.volume = 0.8f;
       layer.loVel = 0;
       layer.hiVel = INIConfig::Validation::MAX_MIDI_VELOCITY;

       juce::String fileName = audioFile.getFileNameWithoutExtension().toLowerCase();
       bool matched = false;

       for (const auto& mapping : mappings) {
           if (fileName.contains(mapping.filePattern)) {
               region->key = mapping.midiNote;
               layer.volume = mapping.volume;
               region->adsr = mapping.adsr;
               matched = true;
               break;
           }
       }

       if (!matched) {
           region->key = currentNote++;
           if (currentNote > INIConfig::Validation::MAX_MIDI_NOTE) currentNote = INIConfig::LayoutConstants::sfzBaseMidiNote;
       }

       auto* reader = formatManager.createReaderFor(audioFile);
       if (reader) {
           layer.source = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
           if (sampleRate > 0) {
               layer.source->prepareToPlay(INIConfig::LayoutConstants::separatorComponentDefaultWidth * INIConfig::LayoutConstants::drumKitSectionBorderThickness * INIConfig::LayoutConstants::drumKitSpacing, sampleRate);
           }
           region->velocityLayers.push_back(std::move(layer));
           regions[region->key] = std::move(region);
       }
   }
}

void SFZEngine::loadSFZFile() {
   if (!availableDrumkits.isEmpty()) {
       for (const auto& kit : availableDrumkits) {
           if (kit.name == "Acoustic" && !kit.sfzFiles.isEmpty()) {
               loadDrumkit(kit.name, kit.sfzFiles[0]);
               return;
           }
       }

       const auto& firstKit = availableDrumkits.getFirst();
       if (!firstKit.sfzFiles.isEmpty()) {
           loadDrumkit(firstKit.name, firstKit.sfzFiles[0]);
       }
   }
}

void SFZEngine::setCurrentPlayer(int playerIndex) {
   if (INIConfig::isValidPlayerIndex(playerIndex)) {
       currentPlayerIndex = playerIndex;
       loadPlayerDrumkitFromState(playerIndex);
   }
}

juce::String SFZEngine::getPlayerDrumkit(int playerIndex) const {
   if (INIConfig::isValidPlayerIndex(playerIndex)) {
       return playerSelections[playerIndex].drumkitName;
   }
   return INIConfig::Defaults::DEFAULT_DRUMKIT;
}

void SFZEngine::setPlayerDrumkit(int playerIndex, const juce::String& drumkitName) {
   if (INIConfig::isValidPlayerIndex(playerIndex)) {
       juce::String sfzFileName;
       for (const auto& kit : availableDrumkits) {
           if (kit.name == drumkitName && !kit.sfzFiles.isEmpty()) {
               sfzFileName = kit.sfzFiles[0];
               break;
           }
       }

       if (sfzFileName.isEmpty()) {
           return;
       }

       playerSelections[playerIndex].drumkitName = drumkitName;
       playerSelections[playerIndex].sfzFileName = sfzFileName;
       playerSelections[playerIndex].enabled = true;

       if (playerIndex == currentPlayerIndex) {
           loadDrumkit(drumkitName, sfzFileName);
       }
   }
}

void SFZEngine::saveStates(ComponentState& state) {
   state.currentPlayer = INIConfig::clampPlayerIndex(currentPlayerIndex);

   for (int i = 0; i < INIConfig::Defaults::MAX_PLAYERS; ++i) {
       if (INIConfig::isValidPlayerIndex(i)) {
           state.playerSettings[i].selectedDrumkit = playerSelections[i].drumkitName;
           state.playerSettings[i].enabled = playerSelections[i].enabled;

           if (!playerSelections[i].sfzFileName.isEmpty()) {
               state.playerSettings[i].assignedMidiFiles[0] = playerSelections[i].sfzFileName;
           }
       }
   }
}

void SFZEngine::loadStates(const ComponentState& state) {
   currentPlayerIndex = INIConfig::clampPlayerIndex(state.currentPlayer);

   for (int i = 0; i < INIConfig::Defaults::MAX_PLAYERS; ++i) {
       if (INIConfig::isValidPlayerIndex(i)) {
           const auto& playerSetting = state.playerSettings[i];

           if (!playerSetting.selectedDrumkit.isEmpty()) {
               playerSelections[i].drumkitName = playerSetting.selectedDrumkit;
               playerSelections[i].enabled = playerSetting.enabled;

               if (!playerSetting.assignedMidiFiles[0].isEmpty()) {
                   playerSelections[i].sfzFileName = playerSetting.assignedMidiFiles[0];
               } else {
                   for (const auto& kit : availableDrumkits) {
                       if (kit.name == playerSetting.selectedDrumkit && !kit.sfzFiles.isEmpty()) {
                           playerSelections[i].sfzFileName = kit.sfzFiles[0];
                           break;
                       }
                   }
               }
           }
       }
   }

   loadPlayerDrumkitFromState(currentPlayerIndex);
}

void SFZEngine::loadPlayerDrumkitFromState(int playerIndex) {
   if (!INIConfig::isValidPlayerIndex(playerIndex)) {
       return;
   }

   const auto& selection = playerSelections[playerIndex];
   if (!selection.drumkitName.isEmpty() && !selection.sfzFileName.isEmpty()) {
       loadDrumkit(selection.drumkitName, selection.sfzFileName);
   } else {
       if (!availableDrumkits.isEmpty()) {
           const auto& defaultKit = availableDrumkits.getFirst();
           if (!defaultKit.sfzFiles.isEmpty()) {
               setPlayerDrumkit(playerIndex, defaultKit.name);
           }
       }
   }
}

void SFZEngine::initializeDefaultPlayerDrumkits() {
   if (availableDrumkits.isEmpty()) {
       return;
   }

   for (int i = 0; i < INIConfig::Defaults::MAX_PLAYERS; ++i) {
       if (playerSelections[i].drumkitName.isEmpty()) {
           juce::String defaultKitName = INIConfig::Defaults::DEFAULT_DRUMKIT;
           bool foundAcoustic = false;

           for (const auto& kit : availableDrumkits) {
               if (kit.name == defaultKitName) {
                   foundAcoustic = true;
                   if (!kit.sfzFiles.isEmpty()) {
                       playerSelections[i].drumkitName = kit.name;
                       playerSelections[i].sfzFileName = kit.sfzFiles[0];
                       playerSelections[i].enabled = true;
                   }
                   break;
               }
           }

           if (!foundAcoustic) {
               int kitIndex = i % availableDrumkits.size();
               const auto& kit = availableDrumkits[kitIndex];
               if (!kit.sfzFiles.isEmpty()) {
                   playerSelections[i].drumkitName = kit.name;
                   playerSelections[i].sfzFileName = kit.sfzFiles[0];
                   playerSelections[i].enabled = true;
               }
           }
       }
   }
}
