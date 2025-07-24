#include "SFZVoiceAllocator.h"
#include <algorithm>
#include "INIConfig.h"

SFZVoiceAllocator::SFZVoiceAllocator() {
    voices.reserve(MAX_VOICES);
    for (int i = 0; i < MAX_VOICES; ++i) {
        voices.push_back(std::make_unique<SFZVoice>());
    }

    roundRobinCounters.resize(INIConfig::LayoutConstants::maxMidiVelocity + 1, 0);
}

void SFZVoiceAllocator::prepare(double sr, int samplesPerBlock) {
    sampleRate = sr;
    juce::ignoreUnused(samplesPerBlock);
}

void SFZVoiceAllocator::reset() {
    for (auto& voice : voices) {
        voice->reset();
    }
    std::fill(roundRobinCounters.begin(), roundRobinCounters.end(), 0);
}

SFZVoice* SFZVoiceAllocator::allocateVoice(int midiNote) {
    if (midiNote < 0 || midiNote > INIConfig::LayoutConstants::maxMidiVelocity) return nullptr;

    SFZVoice* voice = findIdleVoice();

    if (!voice && getActiveVoiceCount() >= maxPolyphony) {
        voice = stealVoice();
    }

    if (voice) {
        roundRobinCounters[midiNote] = (roundRobinCounters[midiNote] + 1) % MAX_VOICES_PER_NOTE;
    }

    return voice;
}

void SFZVoiceAllocator::releaseVoicesForNote(int midiNote) {
    for (auto& voice : voices) {
        if (voice->getCurrentNote() == midiNote && voice->isActive()) {
            voice->stopNote();
        }
    }
}

void SFZVoiceAllocator::releaseAllVoices() {
    for (auto& voice : voices) {
        if (voice->isActive()) {
            voice->stopNote();
        }
    }
}

void SFZVoiceAllocator::renderNextBlock(juce::AudioBuffer<float>& buffer) {
    for (auto& voice : voices) {
        if (voice->isActive()) {
            voice->renderNextBlock(buffer, 0, buffer.getNumSamples());
        }
    }
}

int SFZVoiceAllocator::getActiveVoiceCount() const {
    auto count = std::count_if(voices.begin(), voices.end(),
                              [](const auto& voice) { return voice->isActive(); });
    jassert(count <= std::numeric_limits<int>::max());
    return static_cast<int>(count);
}

SFZVoice* SFZVoiceAllocator::findIdleVoice() {
    for (auto& voice : voices) {
        if (!voice->isActive()) {
            return voice.get();
        }
    }
    return nullptr;
}

SFZVoice* SFZVoiceAllocator::stealVoice() {
    for (auto& voice : voices) {
        if (voice->isReleasing()) {
            voice->reset();
            return voice.get();
        }
    }

    SFZVoice* oldestVoice = findOldestVoice();
    if (oldestVoice && oldestVoice->canBeStolen()) {
        oldestVoice->reset();
        return oldestVoice;
    }

    SFZVoice* quietestVoice = findQuietestVoice();
    if (quietestVoice) {
        quietestVoice->reset();
        return quietestVoice;
    }

    for (auto& voice : voices) {
        if (voice->isActive()) {
            voice->reset();
            return voice.get();
        }
    }

    return nullptr;
}

SFZVoice* SFZVoiceAllocator::findOldestVoice() {
    SFZVoice* oldest = nullptr;
    int64_t oldestTime = std::numeric_limits<int64_t>::max();

    for (auto& voice : voices) {
        if (voice->isActive() && voice->getStartTime() < oldestTime) {
            oldest = voice.get();
            oldestTime = voice->getStartTime();
        }
    }

    return oldest;
}

SFZVoice* SFZVoiceAllocator::findQuietestVoice() {
    SFZVoice* quietest = nullptr;
    float lowestVelocity = 1.0f;

    for (auto& voice : voices) {
        if (voice->isActive() && voice->getVelocity() < lowestVelocity) {
            quietest = voice.get();
            lowestVelocity = voice->getVelocity();
        }
    }

    return quietest;
}
