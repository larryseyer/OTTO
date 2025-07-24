#include "SFZVoice.h"
#include <algorithm>
#include "INIConfig.h"

SFZVoice::SFZVoice() {
    sourceBuffer.setSize(INIConfig::LayoutConstants::defaultOutputChannels, INIConfig::LayoutConstants::defaultBufferSize);
}

void SFZVoice::startNote(int midiNote, float vel, double sr,
                        juce::AudioFormatReaderSource* source,
                        const ADSRParameters& adsr, float volumeDb) {
    if (!source) return;

    currentNote = midiNote;
    velocity = vel;
    sampleRate = sr;
    audioSource = source;
    adsrParams = adsr;
    baseVolume = juce::Decibels::decibelsToGain(volumeDb);

    sourcePosition = 0;
    audioSource->setNextReadPosition(0);
    currentEnvelopeValue = 0.0f;

    state = State::Attack;
    targetEnvelopeValue = 1.0f;
    calculateEnvelopeIncrement(targetEnvelopeValue, adsrParams.attackTime);

    startTime = juce::Time::currentTimeMillis();
}

void SFZVoice::stopNote() {
    if (state != State::Idle && state != State::Finished) {
        state = State::Release;
        targetEnvelopeValue = 0.0f;
        calculateEnvelopeIncrement(targetEnvelopeValue, adsrParams.releaseTime);
    }
}

void SFZVoice::reset() {
    state = State::Idle;
    currentNote = -1;
    velocity = 0.0f;
    sourcePosition = 0;
    currentEnvelopeValue = 0.0f;
    audioSource = nullptr;
}

void SFZVoice::renderNextBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples) {
    if (state == State::Idle || state == State::Finished || !audioSource) {
        return;
    }

    int samplesToRead = std::min(numSamples, sourceBuffer.getNumSamples());
    sourceBuffer.setSize(buffer.getNumChannels(), samplesToRead, false, false, true);
    sourceBuffer.clear();

    juce::AudioSourceChannelInfo info(&sourceBuffer, 0, samplesToRead);
    audioSource->getNextAudioBlock(info);

    for (int sample = 0; sample < samplesToRead; ++sample) {
        updateEnvelope();

        float gain = currentEnvelopeValue * velocity * baseVolume;

        for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
            if (channel < sourceBuffer.getNumChannels()) {
                buffer.addSample(channel, startSample + sample,
                                sourceBuffer.getSample(channel, sample) * gain);
            }
        }
    }

    sourcePosition += samplesToRead;

    if (state == State::Finished ||
        (audioSource->getTotalLength() > 0 && sourcePosition >= audioSource->getTotalLength())) {
        reset();
    }
}

void SFZVoice::updateEnvelope() {
    switch (state) {
        case State::Attack:
            currentEnvelopeValue += envelopeIncrement;
            if (currentEnvelopeValue >= targetEnvelopeValue) {
                currentEnvelopeValue = targetEnvelopeValue;
                state = State::Decay;
                targetEnvelopeValue = adsrParams.sustainLevel;
                calculateEnvelopeIncrement(targetEnvelopeValue, adsrParams.decayTime);
            }
            break;

        case State::Decay:
            currentEnvelopeValue += envelopeIncrement;
            if (currentEnvelopeValue <= targetEnvelopeValue) {
                currentEnvelopeValue = targetEnvelopeValue;
                state = State::Sustain;
                envelopeIncrement = 0.0f;
            }
            break;

        case State::Sustain:
            break;

        case State::Release:
            currentEnvelopeValue += envelopeIncrement;
            if (currentEnvelopeValue <= 0.0f) {
                currentEnvelopeValue = 0.0f;
                state = State::Finished;
            }
            break;

        default:
            break;
    }
}

void SFZVoice::calculateEnvelopeIncrement(float targetValue, float timeInSeconds) {
    if (timeInSeconds <= 0.0f) {
        currentEnvelopeValue = targetValue;
        envelopeIncrement = 0.0f;
    } else {
        float numSamples = timeInSeconds * sampleRate;
        envelopeIncrement = (targetValue - currentEnvelopeValue) / numSamples;
    }
}

bool SFZVoice::canBeStolen() const {
    return state == State::Release ||
           (state != State::Idle &&
            juce::Time::currentTimeMillis() - startTime > INIConfig::LayoutConstants::sfzVoiceStealThreshold);
}
