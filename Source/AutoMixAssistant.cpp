#include "AutoMixAssistant.h"
#include "INIConfig.h"
#include "SFZEngine.h"

AutoMixAssistant::AutoMixAssistant(Mixer& mixerRef, SFZEngine& sfzEngineRef)
    : mixer(mixerRef), sfzEngine(sfzEngineRef) {
    initializePresets();
}

AutoMixAssistant::~AutoMixAssistant() {
}

AutoMixAssistant::MixSuggestion AutoMixAssistant::analyzeMix() {
    MixSuggestion suggestion;

    auto analysis = analyzeCurrentMix();

    suggestion = generateSuggestions(analysis);

    suggestion.confidence = calculateConfidence(analysis);

    return suggestion;
}

void AutoMixAssistant::applyMixSuggestion(const MixSuggestion& suggestion, float blendAmount) {
    for (int channel = 0; channel < INIConfig::Defaults::MAX_PLAYERS; ++channel) {
        float currentVolume = mixer.getChannelVolume(channel);
        float targetVolume = suggestion.channelVolumes[channel];
        float blendedVolume = currentVolume + (targetVolume - currentVolume) * blendAmount;
        mixer.setChannelVolume(channel, blendedVolume);

        float currentPan = mixer.getChannelPan(channel);
        float targetPan = suggestion.channelPans[channel];
        float blendedPan = currentPan + (targetPan - currentPan) * blendAmount;
        mixer.setChannelPan(channel, blendedPan);

        for (int band = 0; band < 3; ++band) {
            float currentGain = mixer.getChannelEQ(channel, static_cast<Mixer::EQBand>(band));
            float targetGain = suggestion.eqSettings[channel][band];
            float blendedGain = currentGain + (targetGain - currentGain) * blendAmount;
            mixer.setChannelEQ(channel, static_cast<Mixer::EQBand>(band), blendedGain);
        }

    }
}

AutoMixAssistant::MixPreset AutoMixAssistant::getCurrentMixAsPreset(const juce::String& name) {
    MixPreset preset;
    preset.name = name;
    preset.createdTime = juce::Time::getCurrentTime();

    for (int channel = 0; channel < INIConfig::Defaults::MAX_PLAYERS; ++channel) {
        preset.channelVolumes[channel] = mixer.getChannelVolume(channel);
        preset.channelPans[channel] = mixer.getChannelPan(channel);
        preset.channelMutes[channel] = mixer.isChannelMuted(channel);
        preset.channelSolos[channel] = mixer.isChannelSoloed(channel);

        for (int band = 0; band < 3; ++band) {
            preset.eqSettings[channel][band] = mixer.getChannelEQ(channel, static_cast<Mixer::EQBand>(band));
        }

        preset.sendLevels[channel][0] = mixer.getChannelSend(channel, Mixer::SendType::Reverb);
        preset.sendLevels[channel][1] = mixer.getChannelSend(channel, Mixer::SendType::Delay);
    }

    preset.masterVolume = mixer.getMasterVolume();
    preset.masterEQ[0] = 0.0f;
    preset.masterEQ[1] = 0.0f;
    preset.masterEQ[2] = 0.0f;

    return preset;
}

void AutoMixAssistant::loadMixPreset(const MixPreset& preset) {
    for (int channel = 0; channel < INIConfig::Defaults::MAX_PLAYERS; ++channel) {
        mixer.setChannelVolume(channel, preset.channelVolumes[channel]);
        mixer.setChannelPan(channel, preset.channelPans[channel]);
        mixer.setChannelMute(channel, preset.channelMutes[channel]);
        mixer.setChannelSolo(channel, preset.channelSolos[channel]);

        for (int band = 0; band < 3; ++band) {
            mixer.setChannelEQ(channel, static_cast<Mixer::EQBand>(band), preset.eqSettings[channel][band]);
        }

        mixer.setChannelSend(channel, Mixer::SendType::Reverb, preset.sendLevels[channel][0]);
        mixer.setChannelSend(channel, Mixer::SendType::Delay, preset.sendLevels[channel][1]);
    }

    mixer.setMasterVolume(preset.masterVolume);
}

float AutoMixAssistant::analyzeMixBalance() {
    float leftSum = 0.0f;
    float rightSum = 0.0f;
    int activeChannels = 0;

    for (int channel = 0; channel < INIConfig::Defaults::MAX_PLAYERS; ++channel) {
        if (mixer.isChannelMuted(channel)) continue;

        auto levels = mixer.getChannelLevels(channel);
        if (levels.left > 0.01f || levels.right > 0.01f) {
            activeChannels++;

            float pan = mixer.getChannelPan(channel);
            float leftGain = 1.0f - (pan > 0 ? pan : 0);
            float rightGain = 1.0f + (pan < 0 ? pan : 0);

            leftSum += levels.left * leftGain;
            rightSum += levels.right * rightGain;
        }
    }

    if (activeChannels == 0) return 1.0f;

    float balance = 1.0f - std::abs(leftSum - rightSum) / (leftSum + rightSum + 0.0001f);
    return balance;
}

void AutoMixAssistant::createSpace() {
    struct ChannelFreqInfo {
        int channel;
        float lowEnergy;
        float midEnergy;
        float highEnergy;
        float dominantBand;
    };

    juce::Array<ChannelFreqInfo> channelInfo;

    for (int channel = 0; channel < INIConfig::Defaults::MAX_PLAYERS; ++channel) {
        if (mixer.isChannelMuted(channel)) continue;

        auto levels = mixer.getChannelLevels(channel);
        if (levels.left < 0.01f && levels.right < 0.01f) continue;

        ChannelFreqInfo info;
        info.channel = channel;

        if (channel == 0 || channel == 1) {
            info.lowEnergy = 0.8f;
            info.midEnergy = 0.2f;
            info.highEnergy = 0.1f;
            info.dominantBand = 0;
        } else if (channel == 2 || channel == 3) {
            info.lowEnergy = 0.2f;
            info.midEnergy = 0.7f;
            info.highEnergy = 0.5f;
            info.dominantBand = 1;
        } else {
            info.lowEnergy = 0.1f;
            info.midEnergy = 0.3f;
            info.highEnergy = 0.8f;
            info.dominantBand = 2;
        }

        channelInfo.add(info);
    }

    for (const auto& info : channelInfo) {
        mixer.setChannelEQ(info.channel, static_cast<Mixer::EQBand>(static_cast<int>(info.dominantBand)), 3.0f);

        for (int band = 0; band < 3; ++band) {
            if (band != static_cast<int>(info.dominantBand)) {
                float currentGain = mixer.getChannelEQ(info.channel, static_cast<Mixer::EQBand>(band));
                mixer.setChannelEQ(info.channel, static_cast<Mixer::EQBand>(band), currentGain - 2.0f);
            }
        }
    }
}

void AutoMixAssistant::preventFrequencyMasking() {

    for (int ch1 = 0; ch1 < INIConfig::Defaults::MAX_PLAYERS - 1; ++ch1) {
        if (mixer.isChannelMuted(ch1)) continue;

        for (int ch2 = ch1 + 1; ch2 < INIConfig::Defaults::MAX_PLAYERS; ++ch2) {
            if (mixer.isChannelMuted(ch2)) continue;

            auto levels1 = mixer.getChannelLevels(ch1);
            auto levels2 = mixer.getChannelLevels(ch2);

            if ((levels1.left + levels1.right) > 0.01f &&
                (levels2.left + levels2.right) > 0.01f) {

                float currentPan1 = mixer.getChannelPan(ch1);
                float currentPan2 = mixer.getChannelPan(ch2);

                if (std::abs(currentPan1 - currentPan2) < 0.2f) {
                    mixer.setChannelPan(ch1, currentPan1 - 0.1f);
                    mixer.setChannelPan(ch2, currentPan2 + 0.1f);
                }

                for (int band = 0; band < 3; ++band) {
                    float gain1 = mixer.getChannelEQ(ch1, static_cast<Mixer::EQBand>(band));
                    float gain2 = mixer.getChannelEQ(ch2, static_cast<Mixer::EQBand>(band));

                    if (std::abs(gain1 - gain2) < 1.0f) {
                        mixer.setChannelEQ(ch1, static_cast<Mixer::EQBand>(band), gain1 + 1.0f);
                        mixer.setChannelEQ(ch2, static_cast<Mixer::EQBand>(band), gain2 - 1.0f);
                    }
                }
            }
        }
    }
}

void AutoMixAssistant::applyGenreSpecificMixing(const juce::String& genre) {
    if (genre == "Rock") {
        for (int ch = 0; ch < INIConfig::Defaults::MAX_PLAYERS; ++ch) {
            if (ch < 2) {
                mixer.setChannelEQ(ch, Mixer::EQBand::Low, 4.0f);
                mixer.setChannelEQ(ch, Mixer::EQBand::Mid, -2.0f);
            } else if (ch < 4) {
                mixer.setChannelEQ(ch, Mixer::EQBand::Mid, 3.0f);
                mixer.setChannelEQ(ch, Mixer::EQBand::High, 2.0f);
            }
        }
    } else if (genre == "Electronic") {
        for (int ch = 0; ch < INIConfig::Defaults::MAX_PLAYERS; ++ch) {
            if (ch < 2) {
                mixer.setChannelEQ(ch, Mixer::EQBand::Low, 6.0f);
            }
            if (ch >= 4) {
                float pan = (ch % 2) ? 0.5f : -0.5f;
                mixer.setChannelPan(ch, pan);
            }
        }
    } else if (genre == "Jazz") {
        for (int ch = 0; ch < INIConfig::Defaults::MAX_PLAYERS; ++ch) {
            mixer.setChannelEQ(ch, Mixer::EQBand::Low, 0.0f);
            mixer.setChannelEQ(ch, Mixer::EQBand::Mid, 0.0f);
            mixer.setChannelEQ(ch, Mixer::EQBand::High, 0.0f);
        }
    }
}

void AutoMixAssistant::optimizeHeadroom() {
    float peakLevel = 0.0f;

    for (int channel = 0; channel < INIConfig::Defaults::MAX_PLAYERS; ++channel) {
        if (mixer.isChannelMuted(channel)) continue;

        auto levels = mixer.getChannelLevels(channel);
        float channelPeak = juce::jmax(levels.left, levels.right);
        peakLevel = juce::jmax(peakLevel, channelPeak);
    }

    float targetPeak = 0.5f;
    if (peakLevel > targetPeak) {
        float reductionFactor = targetPeak / peakLevel;

        for (int channel = 0; channel < INIConfig::Defaults::MAX_PLAYERS; ++channel) {
            float currentVolume = mixer.getChannelVolume(channel);
            mixer.setChannelVolume(channel, currentVolume * reductionFactor);
        }
    }
}

void AutoMixAssistant::initializePresets() {
    MixPreset balanced;
    balanced.name = "Balanced";
    balanced.type = "Default";

    for (int ch = 0; ch < INIConfig::Defaults::MAX_PLAYERS; ++ch) {
        balanced.channelVolumes[ch] = 0.7f;
        balanced.channelPans[ch] = 0.0f;
        balanced.channelMutes[ch] = false;
        balanced.channelSolos[ch] = false;

        for (int band = 0; band < 3; ++band) {
            balanced.eqSettings[ch][band] = 0.0f;
        }
    }

    balanced.masterVolume = 0.8f;
    mixPresets.add(balanced);

}

AutoMixAssistant::MixAnalysis AutoMixAssistant::analyzeCurrentMix() {
    MixAnalysis analysis;

    for (int ch = 0; ch < INIConfig::Defaults::MAX_PLAYERS; ++ch) {
        auto levels = mixer.getChannelLevels(ch);
        float level = (levels.left + levels.right) * 0.5f;

        if (ch < 2) {
            analysis.frequencyBalance.low += level;
        } else if (ch < 4) {
            analysis.frequencyBalance.mid += level;
        } else {
            analysis.frequencyBalance.high += level;
        }

        analysis.channelActivity[ch] = level > 0.01f;
    }

    float total = analysis.frequencyBalance.low +
                  analysis.frequencyBalance.mid +
                  analysis.frequencyBalance.high;

    if (total > 0.0f) {
        analysis.frequencyBalance.low /= total;
        analysis.frequencyBalance.mid /= total;
        analysis.frequencyBalance.high /= total;
    }

    analysis.stereoWidth = calculateStereoWidth();

    analysis.dynamicRange = calculateDynamicRange();

    analysis.activeChannels = 0;
    for (int ch = 0; ch < INIConfig::Defaults::MAX_PLAYERS; ++ch) {
        if (analysis.channelActivity[ch]) {
            analysis.activeChannels++;
        }
    }

    return analysis;
}

AutoMixAssistant::MixSuggestion AutoMixAssistant::generateSuggestions(const MixAnalysis& analysis) {
    MixSuggestion suggestion;

    float idealLow = 0.4f;
    float idealMid = 0.35f;
    float idealHigh = 0.25f;

    float lowError = analysis.frequencyBalance.low - idealLow;
    float midError = analysis.frequencyBalance.mid - idealMid;
    float highError = analysis.frequencyBalance.high - idealHigh;

    for (int ch = 0; ch < INIConfig::Defaults::MAX_PLAYERS; ++ch) {
        suggestion.channelVolumes[ch] = mixer.getChannelVolume(ch);

        if (ch < 2 && lowError > 0.1f) {
            suggestion.channelVolumes[ch] *= 0.9f;
        } else if (ch >= 2 && ch < 4 && midError > 0.1f) {
            suggestion.channelVolumes[ch] *= 0.9f;
        } else if (ch >= 4 && highError > 0.1f) {
            suggestion.channelVolumes[ch] *= 0.9f;
        }
    }

    if (analysis.stereoWidth < 0.3f) {
        for (int ch = 0; ch < INIConfig::Defaults::MAX_PLAYERS; ++ch) {
            if (ch >= 4) {
                suggestion.channelPans[ch] = (ch % 2) ? 0.3f : -0.3f;
            } else {
                suggestion.channelPans[ch] = 0.0f;
            }
        }
    } else {
        for (int ch = 0; ch < INIConfig::Defaults::MAX_PLAYERS; ++ch) {
            suggestion.channelPans[ch] = mixer.getChannelPan(ch);
        }
    }

    for (int ch = 0; ch < INIConfig::Defaults::MAX_PLAYERS; ++ch) {
        for (int band = 0; band < 3; ++band) {
            suggestion.eqSettings[ch][band] = 0.0f;
        }

        if (ch < 2) {
            suggestion.eqSettings[ch][0] = -lowError * 5.0f;
        } else if (ch < 4) {
            suggestion.eqSettings[ch][1] = -midError * 5.0f;
        } else {
            suggestion.eqSettings[ch][2] = -highError * 5.0f;
        }
    }

    if (analysis.dynamicRange < 0.3f) {
        for (int ch = 0; ch < INIConfig::Defaults::MAX_PLAYERS; ++ch) {
            suggestion.compressionSettings[ch] = 0.2f;
        }
    } else if (analysis.dynamicRange > 0.7f) {
        for (int ch = 0; ch < INIConfig::Defaults::MAX_PLAYERS; ++ch) {
            suggestion.compressionSettings[ch] = 0.5f;
        }
    } else {
        for (int ch = 0; ch < INIConfig::Defaults::MAX_PLAYERS; ++ch) {
            suggestion.compressionSettings[ch] = 0.3f;
        }
    }

    return suggestion;
}

float AutoMixAssistant::calculateConfidence(const MixAnalysis& analysis) {
    float confidence = 1.0f;

    if (analysis.activeChannels < 3) {
        confidence *= 0.7f;
    }

    float freqImbalance = std::abs(analysis.frequencyBalance.low - 0.4f) +
                         std::abs(analysis.frequencyBalance.mid - 0.35f) +
                         std::abs(analysis.frequencyBalance.high - 0.25f);

    confidence *= (1.0f - juce::jmin(freqImbalance, 1.0f));

    if (analysis.stereoWidth < 0.1f || analysis.stereoWidth > 0.9f) {
        confidence *= 0.8f;
    }

    return juce::jmax(0.0f, confidence);
}

float AutoMixAssistant::calculateStereoWidth() {
    float leftSum = 0.0f;
    float rightSum = 0.0f;
    float monoSum = 0.0f;

    for (int ch = 0; ch < INIConfig::Defaults::MAX_PLAYERS; ++ch) {
        if (mixer.isChannelMuted(ch)) continue;

        auto levels = mixer.getChannelLevels(ch);
        float pan = mixer.getChannelPan(ch);

        float leftGain = 1.0f - (pan > 0 ? pan : 0);
        float rightGain = 1.0f + (pan < 0 ? pan : 0);

        leftSum += levels.left * leftGain;
        rightSum += levels.right * rightGain;
        monoSum += (levels.left + levels.right) * 0.5f;
    }

    if (monoSum < 0.001f) return 0.0f;

    float width = 1.0f - (std::abs(leftSum - rightSum) / (leftSum + rightSum + 0.001f));
    return juce::jlimit(0.0f, 1.0f, width);
}

float AutoMixAssistant::calculateDynamicRange() {
    float minLevel = 1.0f;
    float maxLevel = 0.0f;

    for (int ch = 0; ch < INIConfig::Defaults::MAX_PLAYERS; ++ch) {
        if (mixer.isChannelMuted(ch)) continue;

        auto levels = mixer.getChannelLevels(ch);
        float level = (levels.left + levels.right) * 0.5f;

        if (level > 0.001f) {
            minLevel = juce::jmin(minLevel, level);
            maxLevel = juce::jmax(maxLevel, level);
        }
    }

    if (maxLevel < 0.001f) return 0.0f;

    float range = (maxLevel - minLevel) / maxLevel;
    return juce::jlimit(0.0f, 1.0f, range);
}

void AutoMixAssistant::adaptMixToRoom(const RoomAnalysis& roomAnalysis) {

    for (int ch = 0; ch < INIConfig::Defaults::MAX_PLAYERS; ++ch) {
        if (ch < 2) {
            if (roomAnalysis.lowFreqResonance > 0.7f) {
                float currentLowGain = mixer.getChannelEQ(ch, Mixer::EQBand::Low);
                mixer.setChannelEQ(ch, Mixer::EQBand::Low, currentLowGain - 3.0f);
            }
        }
    }

    if (roomAnalysis.highFreqAbsorption > 0.7f) {
        for (int ch = 4; ch < INIConfig::Defaults::MAX_PLAYERS; ++ch) {
            float currentHighGain = mixer.getChannelEQ(ch, Mixer::EQBand::High);
            mixer.setChannelEQ(ch, Mixer::EQBand::High, currentHighGain + 2.0f);
        }
    }

    if (roomAnalysis.roomSize < 0.3f) {
        for (int ch = 0; ch < INIConfig::Defaults::MAX_PLAYERS; ++ch) {
            float currentPan = mixer.getChannelPan(ch);
            mixer.setChannelPan(ch, currentPan * 0.7f);
        }
    }
}

void AutoMixAssistant::learnFromUserAdjustments() {
    MixPreset currentState = getCurrentMixAsPreset("User Adjustment");

    if (userAdjustmentHistory.size() > 0) {
        const auto& lastState = userAdjustmentHistory.getLast();

        for (int ch = 0; ch < INIConfig::Defaults::MAX_PLAYERS; ++ch) {
            float volumeDiff = currentState.channelVolumes[ch] - lastState.channelVolumes[ch];
            float panDiff = currentState.channelPans[ch] - lastState.channelPans[ch];

            if (std::abs(volumeDiff) > 0.05f) {
                userPreferences.volumeTendencies[ch] += volumeDiff * 0.1f;
            }

            if (std::abs(panDiff) > 0.05f) {
                userPreferences.panTendencies[ch] += panDiff * 0.1f;
            }
        }
    }

    userAdjustmentHistory.add(currentState);

    if (userAdjustmentHistory.size() > static_cast<int>(INIConfig::LayoutConstants::drumButtonRows * INIConfig::LayoutConstants::drumKitEditorPadColumns)) {
        userAdjustmentHistory.remove(0);
    }
}
