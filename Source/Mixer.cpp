#include "Mixer.h"
#include <cmath>
#include "INIConfig.h"
#include "ErrorHandling.h"

Mixer::Mixer() {
    loadDefaultPresets();
}

void Mixer::prepare(double newSampleRate, int samplesPerBlock) {
    sampleRate = newSampleRate;
    blockSize = samplesPerBlock;

    for (int i = 0; i < NUM_CHANNELS; ++i) {
        auto& proc = channelProcessors[i];

        proc.volumeSmoothed.reset(sampleRate, 0.01);
        proc.volumeSmoothed.setCurrentAndTargetValue(channelStates[i].volume.load());

        proc.levelFollowerLeft.reset(sampleRate, 0.1);
        proc.levelFollowerRight.reset(sampleRate, 0.1);

        updateEQCoefficients(i);
    }

    juce::dsp::Reverb::Parameters reverbParams;
    reverbParams.roomSize = reverbState.roomSize.load();
    reverbParams.damping = reverbState.damping.load();
    reverbParams.wetLevel = reverbState.mix.load();
    reverbParams.dryLevel = 1.0f - reverbState.mix.load();
    reverbParams.width = reverbState.width.load();
    reverb.setParameters(reverbParams);

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(blockSize);
    spec.numChannels = 2;

    reverbHighCut.prepare(spec);
    reverbLowCut.prepare(spec);
    delayHighCut.prepare(spec);
    delayLowCut.prepare(spec);

    reverbHighCut.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    reverbLowCut.setType(juce::dsp::StateVariableTPTFilterType::highpass);
    delayHighCut.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    delayLowCut.setType(juce::dsp::StateVariableTPTFilterType::highpass);

    delayLineLeft.prepare(spec);
    delayLineRight.prepare(spec);
    delayLineLeft.setMaximumDelayInSamples(INIConfig::Defaults::MAX_DELAY_SAMPLES);
    delayLineRight.setMaximumDelayInSamples(INIConfig::Defaults::MAX_DELAY_SAMPLES);

    compressor.prepare(spec);
    compressor.setThreshold(compressorState.threshold.load());
    compressor.setRatio(compressorState.ratio.load());
    compressor.setAttack(compressorState.attack.load());
    compressor.setRelease(compressorState.release.load());

    limiter.prepare(spec);
    limiter.setThreshold(masterState.limiterThreshold.load());
    limiter.setRelease(masterState.limiterRelease.load());

    sendBuffer.setSize(2, blockSize);
    reverbBuffer.setSize(2, blockSize);
    delayBuffer.setSize(2, blockSize);
    sidechainBuffer.setSize(2, blockSize);

    reset();
}

void Mixer::reset() {
    for (auto& proc : channelProcessors) {
        proc.lowShelf.reset();
        proc.midPeak.reset();
        proc.highShelf.reset();
        proc.panner.reset();
    }

    reverb.reset();
    delayLineLeft.reset();
    delayLineRight.reset();
    compressor.reset();
    limiter.reset();

    reverbHighCut.reset();
    reverbLowCut.reset();
    delayHighCut.reset();
    delayLowCut.reset();

    sendBuffer.clear();
    reverbBuffer.clear();
    delayBuffer.clear();
    sidechainBuffer.clear();
}

void Mixer::processBlock(juce::AudioBuffer<float>& buffer) {
    // Null-pointer safety: Validate buffer integrity
    if (buffer.getNumChannels() == 0 || buffer.getNumSamples() == 0) {
        DBG("Mixer: Invalid buffer dimensions - channels: " + juce::String(buffer.getNumChannels()) + 
            ", samples: " + juce::String(buffer.getNumSamples()));
        return;
    }

    // Null-pointer safety: Check for valid buffer data
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch) {
        if (buffer.getReadPointer(ch) == nullptr) {
            DBG("Mixer: Null buffer data pointer for channel " + juce::String(ch));
            return;
        }
    }

    const int numSamples = buffer.getNumSamples();

    try {
        // Null-pointer safety: Clear buffers with error handling
        ErrorHandler::safeExecute([&]() {
            sendBuffer.clear();
            reverbBuffer.clear();
            delayBuffer.clear();
        }, "Mixer buffer clearing");

        bool hasSolo = anySolo();

        // Null-pointer safety: Validate channel count
        int safeChannelCount = juce::jmin(NUM_CHANNELS, static_cast<int>(channelStates.size()));
        
        for (int ch = 0; ch < safeChannelCount; ++ch) {
            // Null-pointer safety: Validate channel state access
            if (ch >= static_cast<int>(channelStates.size())) {
                DBG("Mixer: Channel index out of bounds: " + juce::String(ch));
                break;
            }

            if (channelStates[ch].mute.load()) continue;
            if (hasSolo && !channelStates[ch].solo.load()) continue;

            // Null-pointer safety: Create channel buffer with validation
            try {
                juce::AudioBuffer<float> channelBuffer(2, numSamples);

                if (buffer.getNumChannels() > ch && ch >= 0) {
                    // Null-pointer safety: Validate buffer copying
                    if (channelBuffer.getNumChannels() >= 2 && 
                        channelBuffer.getNumSamples() >= numSamples) {
                        
                        channelBuffer.copyFrom(0, 0, buffer, ch, 0, numSamples);
                        channelBuffer.copyFrom(1, 0, buffer, ch, 0, numSamples);
                    } else {
                        DBG("Mixer: Invalid channel buffer dimensions");
                        continue;
                    }
                }

                // Null-pointer safety: Process channel with error handling
                ErrorHandler::safeExecute([&]() {
                    processChannel(ch, channelBuffer);
                }, "Mixer channel processing");

                // Null-pointer safety: Validate send array bounds
                if (static_cast<size_t>(ch) < channelStates.size() &&
                    static_cast<int>(SendType::Reverb) < static_cast<int>(channelStates[ch].sends.size()) &&
                    static_cast<int>(SendType::Delay) < static_cast<int>(channelStates[ch].sends.size())) {
                    
                    float reverbSend = channelStates[ch].sends[static_cast<int>(SendType::Reverb)].load();
                    float delaySend = channelStates[ch].sends[static_cast<int>(SendType::Delay)].load();
                    
                    // Validate send values
                    if (std::isfinite(reverbSend) && std::isfinite(delaySend)) {
                        // Process send effects
                        if (reverbSend > 0.0f) {
                            reverbBuffer.addFrom(0, 0, channelBuffer, 0, 0, numSamples, reverbSend);
                            reverbBuffer.addFrom(1, 0, channelBuffer, 1, 0, numSamples, reverbSend);
                        }

                        if (delaySend > 0.0f) {
                            delayBuffer.addFrom(0, 0, channelBuffer, 0, 0, numSamples, delaySend);
                            delayBuffer.addFrom(1, 0, channelBuffer, 1, 0, numSamples, delaySend);
                        }
                        
                        // Mix channel into main buffer
                        if (ch == 0) {
                            buffer.copyFrom(0, 0, channelBuffer, 0, 0, numSamples);
                            buffer.copyFrom(1, 0, channelBuffer, 1, 0, numSamples);
                        } else {
                            buffer.addFrom(0, 0, channelBuffer, 0, 0, numSamples);
                            buffer.addFrom(1, 0, channelBuffer, 1, 0, numSamples);
                        }

                        updateMetering(ch, channelBuffer);
                    } else {
                        DBG("Mixer: Invalid send values for channel " + juce::String(ch));
                    }
                } else {
                    DBG("Mixer: Invalid send array access for channel " + juce::String(ch));
                }
            } catch (const std::exception& e) {
                DBG("Mixer: Exception in channel processing - " + juce::String(e.what()));
            }
        }

        // Process global effects
        if (reverbState.enabled.load()) {
            processReverb(reverbBuffer);
            buffer.addFrom(0, 0, reverbBuffer, 0, 0, numSamples);
            buffer.addFrom(1, 0, reverbBuffer, 1, 0, numSamples);
        }

        if (delayState.enabled.load()) {
            processDelay(delayBuffer);
            buffer.addFrom(0, 0, delayBuffer, 0, 0, numSamples);
            buffer.addFrom(1, 0, delayBuffer, 1, 0, numSamples);
        }

        if (compressorState.enabled.load()) {
            processCompressor(buffer);
        }

        if (distortionState.enabled.load()) {
            processDistortion(buffer);
        }

        // Apply master volume
        float masterVol = masterState.volume.load();
        if (std::isfinite(masterVol) && masterVol >= 0.0f) {
            buffer.applyGain(masterVol);
        } else {
            DBG("Mixer: Invalid master volume, applying safety gain");
            buffer.applyGain(0.5f);
        }

        if (masterState.limiterEnabled.load()) {
            processLimiter(buffer);
        }

        updateMasterMetering(buffer);
        
    } catch (const std::exception& e) {
        DBG("Mixer: Critical exception in processBlock - " + juce::String(e.what()));
        // Emergency: Clear buffer to prevent audio artifacts
        buffer.clear();
    }
}

void Mixer::processChannel(int channel, juce::AudioBuffer<float>& buffer) {
    auto& proc = channelProcessors[channel];
    auto& state = channelStates[channel];

    const int numSamples = buffer.getNumSamples();

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    proc.lowShelf.process(context);
    proc.midPeak.process(context);
    proc.highShelf.process(context);

    proc.volumeSmoothed.setTargetValue(state.volume.load());
    buffer.applyGainRamp(0, numSamples, proc.volumeSmoothed.getCurrentValue(), proc.volumeSmoothed.getNextValue());

    float pan = state.pan.load();
    float leftGain = std::cos((pan + 1.0f) * juce::MathConstants<float>::pi * 0.25f);
    float rightGain = std::sin((pan + 1.0f) * juce::MathConstants<float>::pi * 0.25f);

    buffer.applyGain(0, 0, numSamples, leftGain);
    buffer.applyGain(1, 0, numSamples, rightGain);
}

void Mixer::processReverb(juce::AudioBuffer<float>& buffer) {
    juce::dsp::Reverb::Parameters params;
    params.roomSize = reverbState.roomSize.load();
    params.damping = reverbState.damping.load();
    params.wetLevel = reverbState.mix.load();
    params.dryLevel = 0.0f;
    params.width = reverbState.width.load();
    reverb.setParameters(params);

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    reverbLowCut.setCutoffFrequency(reverbState.lowCut.load());
    reverbHighCut.setCutoffFrequency(reverbState.highCut.load());

    reverbLowCut.process(context);
    reverbHighCut.process(context);

    reverb.process(context);
}

void Mixer::processDelay(juce::AudioBuffer<float>& buffer) {
    const int numSamples = buffer.getNumSamples();

    float delayMs = delayState.delayTime.load();
    if (delayState.syncToHost.load() && hostTempo > 0) {
        float beatLength = static_cast<float>(INIConfig::Defaults::MS_PER_MINUTE) / static_cast<float>(hostTempo);
        float division = static_cast<float>(delayState.syncDivision.load());
        delayMs = beatLength * (static_cast<float>(INIConfig::Defaults::BEATS_PER_BAR) / division);
    }

    int delaySamples = static_cast<int>(delayMs * sampleRate / static_cast<float>(INIConfig::Defaults::MS_PER_SECOND));
    delaySamples = juce::jlimit(1, INIConfig::Defaults::MAX_DELAY_SAMPLES, delaySamples);

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    delayLowCut.setCutoffFrequency(delayState.lowCut.load());
    delayHighCut.setCutoffFrequency(delayState.highCut.load());

    auto* leftIn = buffer.getReadPointer(0);
    auto* rightIn = buffer.getReadPointer(1);
    auto* leftOut = buffer.getWritePointer(0);
    auto* rightOut = buffer.getWritePointer(1);

    float feedback = delayState.feedback.load();
    float mix = delayState.mix.load();
    bool pingPong = delayState.pingPong.load();

    for (int i = 0; i < numSamples; ++i) {
        float delayedLeft = delayLineLeft.popSample(0, delaySamples);
        float delayedRight = delayLineRight.popSample(0, delaySamples);

        float leftToDelay = leftIn[i] + delayedLeft * feedback;
        float rightToDelay = rightIn[i] + delayedRight * feedback;

        if (pingPong) {
            delayLineLeft.pushSample(0, rightToDelay);
            delayLineRight.pushSample(0, leftToDelay);
        } else {
            delayLineLeft.pushSample(0, leftToDelay);
            delayLineRight.pushSample(0, rightToDelay);
        }

        leftOut[i] = leftIn[i] * (1.0f - mix) + delayedLeft * mix;
        rightOut[i] = rightIn[i] * (1.0f - mix) + delayedRight * mix;
    }

    delayLowCut.process(context);
    delayHighCut.process(context);
}

void Mixer::processCompressor(juce::AudioBuffer<float>& buffer) {
    compressor.setThreshold(compressorState.threshold.load());
    compressor.setRatio(compressorState.ratio.load());
    compressor.setAttack(compressorState.attack.load());
    compressor.setRelease(compressorState.release.load());

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    if (compressorState.sidechainEnabled.load()) {
    }

    compressor.process(context);

    float makeupGain = juce::Decibels::decibelsToGain(compressorState.makeupGain.load());
    buffer.applyGain(makeupGain);
}

void Mixer::processDistortion(juce::AudioBuffer<float>& buffer) {
    const int numSamples = buffer.getNumSamples();
    float drive = distortionState.drive.load();
    float mix = distortionState.mix.load();
    float preGain = juce::Decibels::decibelsToGain(distortionState.preGain.load());
    float postGain = juce::Decibels::decibelsToGain(distortionState.postGain.load());

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch) {
        auto* data = buffer.getWritePointer(ch);

        for (int i = 0; i < numSamples; ++i) {
            float input = data[i] * preGain;
            float distorted = applyDistortion(input, distortionState.mode, drive);
            data[i] = (data[i] * (1.0f - mix)) + (distorted * postGain * mix);
        }
    }
}

float Mixer::applyDistortion(float input, DistortionState::Mode mode, float drive) {
    switch (mode) {
        case DistortionState::Mode::Soft:
            return std::tanh(input * drive * INIConfig::Defaults::SCURVE_FACTOR) / std::tanh(drive * INIConfig::Defaults::SCURVE_FACTOR);

        case DistortionState::Mode::Hard:
            return juce::jlimit(-1.0f, 1.0f, input * drive * 5.0f);

        case DistortionState::Mode::Bit: {
            int bits = distortionState.bitDepth.load();
            float levels = std::pow(2.0f, bits);
            return std::round(input * levels) / levels;
        }

        case DistortionState::Mode::Fold: {
            float folded = input * drive * INIConfig::Defaults::BEATS_PER_BAR;
            while (folded > 1.0f) folded = 2.0f - folded;
            while (folded < -1.0f) folded = -2.0f - folded;
            return folded;
        }

        default:
            return input;
    }
}

void Mixer::processLimiter(juce::AudioBuffer<float>& buffer) {
    limiter.setThreshold(masterState.limiterThreshold.load());
    limiter.setRelease(masterState.limiterRelease.load());

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    limiter.process(context);
}

void Mixer::updateMetering(int channel, const juce::AudioBuffer<float>& buffer) {
    if (channel < 0 || channel >= NUM_CHANNELS) return;

    auto& state = channelStates[channel];
    const int numSamples = buffer.getNumSamples();

    float maxLeft = 0.0f;
    float maxRight = 0.0f;

    for (int i = 0; i < numSamples; ++i) {
        maxLeft = juce::jmax(maxLeft, std::abs(buffer.getSample(0, i)));
        maxRight = juce::jmax(maxRight, std::abs(buffer.getSample(1, i)));
    }

    state.currentLevelLeft.store(maxLeft);
    state.currentLevelRight.store(maxRight);

    if (maxLeft > state.peakLevelLeft.load()) {
        state.peakLevelLeft.store(maxLeft);
    }
    if (maxRight > state.peakLevelRight.load()) {
        state.peakLevelRight.store(maxRight);
    }
}

void Mixer::updateMasterMetering(const juce::AudioBuffer<float>& buffer) {
    const int numSamples = buffer.getNumSamples();

    float maxLeft = 0.0f;
    float maxRight = 0.0f;

    for (int i = 0; i < numSamples; ++i) {
        maxLeft = juce::jmax(maxLeft, std::abs(buffer.getSample(0, i)));
        maxRight = juce::jmax(maxRight, std::abs(buffer.getSample(1, i)));
    }

    masterState.currentLevelLeft.store(maxLeft);
    masterState.currentLevelRight.store(maxRight);

    if (maxLeft > masterState.peakLevelLeft.load()) {
        masterState.peakLevelLeft.store(maxLeft);
    }
    if (maxRight > masterState.peakLevelRight.load()) {
        masterState.peakLevelRight.store(maxRight);
    }
}

void Mixer::updateEQCoefficients(int channel) {
    if (channel < 0 || channel >= NUM_CHANNELS) return;

    auto& proc = channelProcessors[channel];
    auto& state = channelStates[channel];

    float lowGain = state.eqGains[static_cast<int>(EQBand::Low)].load();
    *proc.lowShelf.coefficients = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(
        sampleRate, 80.0f, 0.7f, juce::Decibels::decibelsToGain(lowGain));

    float midGain = state.eqGains[static_cast<int>(EQBand::Mid)].load();
    *proc.midPeak.coefficients = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
        sampleRate, 1000.0f, 0.7f, juce::Decibels::decibelsToGain(midGain));

    float highGain = state.eqGains[static_cast<int>(EQBand::High)].load();
    *proc.highShelf.coefficients = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(
        sampleRate, 8000.0f, 0.7f, juce::Decibels::decibelsToGain(highGain));
}

bool Mixer::anySolo() const {
    for (const auto& state : channelStates) {
        if (state.solo.load()) return true;
    }
    return false;
}

void Mixer::setChannelVolume(int channel, float volume) {
    if (channel >= 0 && channel < NUM_CHANNELS) {
        channelStates[channel].volume.store(juce::jlimit(0.0f, 1.0f, volume));
    }
}

void Mixer::setChannelPan(int channel, float pan) {
    if (channel >= 0 && channel < NUM_CHANNELS) {
        channelStates[channel].pan.store(juce::jlimit(-1.0f, 1.0f, pan));
    }
}

void Mixer::setChannelMute(int channel, bool mute) {
    if (channel >= 0 && channel < NUM_CHANNELS) {
        channelStates[channel].mute.store(mute);
    }
}

void Mixer::setChannelSolo(int channel, bool solo) {
    if (channel >= 0 && channel < NUM_CHANNELS) {
        channelStates[channel].solo.store(solo);
    }
}

void Mixer::setChannelEQ(int channel, EQBand band, float gain) {
    if (channel >= 0 && channel < NUM_CHANNELS) {
        channelStates[channel].eqGains[static_cast<int>(band)].store(juce::jlimit(-12.0f, 12.0f, gain));
        updateEQCoefficients(channel);
    }
}

void Mixer::setChannelSend(int channel, SendType send, float amount) {
    if (channel >= 0 && channel < NUM_CHANNELS) {
        channelStates[channel].sends[static_cast<int>(send)].store(juce::jlimit(0.0f, 1.0f, amount));
    }
}

void Mixer::setMasterVolume(float volume) {
    masterState.volume.store(juce::jlimit(0.0f, 1.2f, volume));
}

void Mixer::setLimiterEnabled(bool enabled) {
    masterState.limiterEnabled.store(enabled);
}

void Mixer::setLimiterThreshold(float threshold) {
    masterState.limiterThreshold.store(juce::jlimit(-24.0f, 0.0f, threshold));
}

void Mixer::setLimiterRelease(float release) {
    masterState.limiterRelease.store(juce::jlimit(1.0f, 1000.0f, release));
}

void Mixer::setReverbEnabled(bool enabled) {
    reverbState.enabled.store(enabled);
}

void Mixer::setReverbAlgorithm(ReverbAlgorithm algorithm) {
    reverbState.algorithm.store(algorithm);
}

void Mixer::setReverbMix(float mix) {
    reverbState.mix.store(juce::jlimit(0.0f, 1.0f, mix));
}

void Mixer::setReverbRoomSize(float size) {
    reverbState.roomSize.store(juce::jlimit(0.0f, 1.0f, size));
}

void Mixer::setReverbDamping(float damping) {
    reverbState.damping.store(juce::jlimit(0.0f, 1.0f, damping));
}

void Mixer::setReverbPredelay(float predelay) {
    reverbState.predelay.store(juce::jlimit(0.0f, 100.0f, predelay));
}

void Mixer::setReverbWidth(float width) {
    reverbState.width.store(juce::jlimit(0.0f, 1.0f, width));
}

void Mixer::setReverbHighCut(float freq) {
    reverbState.highCut.store(juce::jlimit(1000.0f, 20000.0f, freq));
}

void Mixer::setReverbLowCut(float freq) {
    reverbState.lowCut.store(juce::jlimit(20.0f, 1000.0f, freq));
}

void Mixer::setDelayEnabled(bool enabled) {
    delayState.enabled.store(enabled);
}

void Mixer::setDelaySyncToHost(bool sync) {
    delayState.syncToHost.store(sync);
}

void Mixer::setDelayTime(float timeMs) {
    delayState.delayTime.store(juce::jlimit(0.1f, 4000.0f, timeMs));
}

void Mixer::setDelaySyncDivision(int division) {
    delayState.syncDivision.store(juce::jlimit(1, 32, division));
}

void Mixer::setDelayFeedback(float feedback) {
    delayState.feedback.store(juce::jlimit(0.0f, 0.99f, feedback));
}

void Mixer::setDelayMix(float mix) {
    delayState.mix.store(juce::jlimit(0.0f, 1.0f, mix));
}

void Mixer::setDelayHighCut(float freq) {
    delayState.highCut.store(juce::jlimit(1000.0f, 20000.0f, freq));
}

void Mixer::setDelayLowCut(float freq) {
    delayState.lowCut.store(juce::jlimit(20.0f, 1000.0f, freq));
}

void Mixer::setDelayPingPong(bool enabled) {
    delayState.pingPong.store(enabled);
}

void Mixer::setDelaySpread(float spread) {
    delayState.spread.store(juce::jlimit(0.0f, 1.0f, spread));
}

void Mixer::setCompressorEnabled(bool enabled) {
    compressorState.enabled.store(enabled);
}

void Mixer::setCompressorThreshold(float threshold) {
    compressorState.threshold.store(juce::jlimit(-60.0f, 0.0f, threshold));
}

void Mixer::setCompressorRatio(float ratio) {
    compressorState.ratio.store(juce::jlimit(1.0f, 20.0f, ratio));
}

void Mixer::setCompressorAttack(float attack) {
    compressorState.attack.store(juce::jlimit(0.1f, 100.0f, attack));
}

void Mixer::setCompressorRelease(float release) {
    compressorState.release.store(juce::jlimit(1.0f, 5000.0f, release));
}

void Mixer::setCompressorMakeupGain(float gain) {
    compressorState.makeupGain.store(juce::jlimit(-12.0f, 24.0f, gain));
}

void Mixer::setCompressorKnee(float knee) {
    compressorState.knee.store(juce::jlimit(0.0f, 10.0f, knee));
}

void Mixer::setSidechainEnabled(bool enabled) {
    compressorState.sidechainEnabled.store(enabled);
}

void Mixer::setSidechainSource(int channel) {
    compressorState.sidechainSource.store(juce::jlimit(0, NUM_CHANNELS - 1, channel));
}

void Mixer::setDistortionEnabled(bool enabled) {
    distortionState.enabled.store(enabled);
}

void Mixer::setDistortionDrive(float drive) {
    distortionState.drive.store(juce::jlimit(0.0f, 1.0f, drive));
}

void Mixer::setDistortionMix(float mix) {
    distortionState.mix.store(juce::jlimit(0.0f, 1.0f, mix));
}

void Mixer::setBitDepth(int bits) {
    distortionState.bitDepth.store(juce::jlimit(1, 24, bits));
}

void Mixer::setSampleRateReduction(float factor) {
    distortionState.sampleRateReduction.store(juce::jlimit(1.0f, 100.0f, factor));
}

void Mixer::setDistortionPreGain(float gain) {
    distortionState.preGain.store(juce::jlimit(-24.0f, 24.0f, gain));
}

void Mixer::setDistortionPostGain(float gain) {
    distortionState.postGain.store(juce::jlimit(-24.0f, 24.0f, gain));
}

void Mixer::setDistortionMode(DistortionState::Mode mode) {
    distortionState.mode = mode;
}

float Mixer::getChannelVolume(int channel) const {
    if (channel >= 0 && channel < NUM_CHANNELS) {
        return channelStates[channel].volume.load();
    }
    return 0.0f;
}

float Mixer::getChannelPan(int channel) const {
    if (channel >= 0 && channel < NUM_CHANNELS) {
        return channelStates[channel].pan.load();
    }
    return 0.0f;
}

bool Mixer::isChannelMuted(int channel) const {
    if (channel >= 0 && channel < NUM_CHANNELS) {
        return channelStates[channel].mute.load();
    }
    return false;
}

bool Mixer::isChannelSoloed(int channel) const {
    if (channel >= 0 && channel < NUM_CHANNELS) {
        return channelStates[channel].solo.load();
    }
    return false;
}

float Mixer::getChannelEQ(int channel, EQBand band) const {
    if (channel >= 0 && channel < NUM_CHANNELS) {
        return channelStates[channel].eqGains[static_cast<int>(band)].load();
    }
    return 0.0f;
}

float Mixer::getChannelSend(int channel, SendType send) const {
    if (channel >= 0 && channel < NUM_CHANNELS) {
        return channelStates[channel].sends[static_cast<int>(send)].load();
    }
    return 0.0f;
}

Mixer::LevelInfo Mixer::getChannelLevels(int channel) const {
    LevelInfo info{0.0f, 0.0f};
    if (channel >= 0 && channel < NUM_CHANNELS) {
        info.left = channelStates[channel].currentLevelLeft.load();
        info.right = channelStates[channel].currentLevelRight.load();
    }
    return info;
}

Mixer::LevelInfo Mixer::getMasterLevels() const {
    return {masterState.currentLevelLeft.load(), masterState.currentLevelRight.load()};
}

void Mixer::resetPeakLevels() {
    for (auto& state : channelStates) {
        state.peakLevelLeft.store(0.0f);
        state.peakLevelRight.store(0.0f);
    }
    masterState.peakLevelLeft.store(0.0f);
    masterState.peakLevelRight.store(0.0f);
}

void Mixer::savePreset(const juce::String& name) {
    EffectPreset preset;
    preset.name = name;
    preset.reverb.copyFrom(reverbState);
    preset.delay.copyFrom(delayState);
    preset.compressor.copyFrom(compressorState);
    preset.distortion.copyFrom(distortionState);
    effectPresets.add(preset);
}

void Mixer::loadPreset(int index) {
    if (index >= 0 && index < effectPresets.size()) {
        const auto& preset = effectPresets[index];
        reverbState.copyFrom(preset.reverb);
        delayState.copyFrom(preset.delay);
        compressorState.copyFrom(preset.compressor);
        distortionState.copyFrom(preset.distortion);
    }
}

void Mixer::deletePreset(int index) {
    if (index >= 0 && index < effectPresets.size()) {
        effectPresets.remove(index);
    }
}

juce::StringArray Mixer::getPresetNames() const {
    juce::StringArray names;
    for (const auto& preset : effectPresets) {
        names.add(preset.name);
    }
    return names;
}

void Mixer::loadDefaultPresets() {
    EffectPreset clean;
    clean.name = "Clean";
    clean.reverb.enabled = false;
    clean.delay.enabled = false;
    clean.compressor.enabled = false;
    clean.distortion.enabled = false;
    effectPresets.add(clean);

    EffectPreset ambient;
    ambient.name = "Ambient";
    ambient.reverb.enabled = true;
    ambient.reverb.algorithm = ReverbAlgorithm::Hall;
    ambient.reverb.roomSize = 0.8f;
    ambient.reverb.damping = 0.3f;
    ambient.reverb.mix = 0.4f;
    ambient.delay.enabled = true;
    ambient.delay.delayTime = 375.0f;
    ambient.delay.feedback = 0.5f;
    ambient.delay.mix = 0.3f;
    effectPresets.add(ambient);

    EffectPreset punchy;
    punchy.name = "Punchy";
    punchy.compressor.enabled = true;
    punchy.compressor.threshold = -12.0f;
    punchy.compressor.ratio = INIConfig::Defaults::BEATS_PER_BAR;
    punchy.compressor.attack = 5.0f;
    punchy.compressor.release = 50.0f;
    punchy.compressor.makeupGain = INIConfig::Defaults::SCURVE_FACTOR;
    effectPresets.add(punchy);

    EffectPreset lofi;
    lofi.name = "Lo-Fi";
    lofi.distortion.enabled = true;
    lofi.distortion.mode = DistortionState::Mode::Bit;
    lofi.distortion.bitDepth = INIConfig::Defaults::MAX_PLAYERS;
    lofi.distortion.drive = 0.6f;
    lofi.distortion.mix = 0.7f;
    lofi.delay.enabled = true;
    lofi.delay.delayTime = 250.0f;
    lofi.delay.feedback = 0.3f;
    lofi.delay.highCut = 2000.0f;
    effectPresets.add(lofi);
}

void Mixer::saveState(ComponentState& state) const {
    for (int i = 0; i < NUM_CHANNELS; ++i) {
        const auto& ch = channelStates[i];
        juce::String prefix = "mixer_ch" + juce::String(i) + "_";

        state.sliderValues[prefix + "volume"] = ch.volume.load();
        state.sliderValues[prefix + "pan"] = ch.pan.load();
        state.toggleStates[1000 + i * 10] = ch.mute.load();
        state.toggleStates[1001 + i * 10] = ch.solo.load();

        state.sliderValues[prefix + "eq_low"] = ch.eqGains[0].load();
        state.sliderValues[prefix + "eq_mid"] = ch.eqGains[1].load();
        state.sliderValues[prefix + "eq_high"] = ch.eqGains[2].load();

        state.sliderValues[prefix + "send_reverb"] = ch.sends[0].load();
        state.sliderValues[prefix + "send_delay"] = ch.sends[1].load();
    }

    state.sliderValues["mixer_master_volume"] = masterState.volume.load();
    state.toggleStates[2000] = masterState.limiterEnabled.load();
    state.sliderValues["mixer_limiter_threshold"] = masterState.limiterThreshold.load();
    state.sliderValues["mixer_limiter_release"] = masterState.limiterRelease.load();

    state.toggleStates[2100] = reverbState.enabled.load();
    state.dropdownSelections["reverb_algorithm"] = static_cast<int>(reverbState.algorithm.load());
    state.sliderValues["reverb_mix"] = reverbState.mix.load();
    state.sliderValues["reverb_room_size"] = reverbState.roomSize.load();
    state.sliderValues["reverb_damping"] = reverbState.damping.load();
    state.sliderValues["reverb_predelay"] = reverbState.predelay.load();
    state.sliderValues["reverb_width"] = reverbState.width.load();
    state.sliderValues["reverb_high_cut"] = reverbState.highCut.load();
    state.sliderValues["reverb_low_cut"] = reverbState.lowCut.load();

    state.toggleStates[2200] = delayState.enabled.load();
    state.toggleStates[2201] = delayState.syncToHost.load();
    state.sliderValues["delay_time"] = delayState.delayTime.load();
    state.dropdownSelections["delay_sync_division"] = delayState.syncDivision.load();
    state.sliderValues["delay_feedback"] = delayState.feedback.load();
    state.sliderValues["delay_mix"] = delayState.mix.load();
    state.sliderValues["delay_high_cut"] = delayState.highCut.load();
    state.sliderValues["delay_low_cut"] = delayState.lowCut.load();
    state.toggleStates[2202] = delayState.pingPong.load();
    state.sliderValues["delay_spread"] = delayState.spread.load();

    state.toggleStates[2300] = compressorState.enabled.load();
    state.sliderValues["comp_threshold"] = compressorState.threshold.load();
    state.sliderValues["comp_ratio"] = compressorState.ratio.load();
    state.sliderValues["comp_attack"] = compressorState.attack.load();
    state.sliderValues["comp_release"] = compressorState.release.load();
    state.sliderValues["comp_makeup"] = compressorState.makeupGain.load();
    state.sliderValues["comp_knee"] = compressorState.knee.load();
    state.toggleStates[2301] = compressorState.sidechainEnabled.load();
    state.dropdownSelections["comp_sidechain_source"] = compressorState.sidechainSource.load();

    state.toggleStates[2400] = distortionState.enabled.load();
    state.sliderValues["dist_drive"] = distortionState.drive.load();
    state.sliderValues["dist_mix"] = distortionState.mix.load();
    state.dropdownSelections["dist_bit_depth"] = distortionState.bitDepth.load();
    state.sliderValues["dist_sr_reduction"] = distortionState.sampleRateReduction.load();
    state.sliderValues["dist_pre_gain"] = distortionState.preGain.load();
    state.sliderValues["dist_post_gain"] = distortionState.postGain.load();
    state.dropdownSelections["dist_mode"] = static_cast<int>(distortionState.mode);
}

void Mixer::loadState(const ComponentState& state) {
    for (int i = 0; i < NUM_CHANNELS; ++i) {
        juce::String prefix = "mixer_ch" + juce::String(i) + "_";

        if (state.sliderValues.count(prefix + "volume"))
            setChannelVolume(i, state.sliderValues.at(prefix + "volume"));
        if (state.sliderValues.count(prefix + "pan"))
            setChannelPan(i, state.sliderValues.at(prefix + "pan"));
        if (state.toggleStates.count(1000 + i * 10))
            setChannelMute(i, state.toggleStates.at(1000 + i * 10));
        if (state.toggleStates.count(1001 + i * 10))
            setChannelSolo(i, state.toggleStates.at(1001 + i * 10));

        if (state.sliderValues.count(prefix + "eq_low"))
            setChannelEQ(i, EQBand::Low, state.sliderValues.at(prefix + "eq_low"));
        if (state.sliderValues.count(prefix + "eq_mid"))
            setChannelEQ(i, EQBand::Mid, state.sliderValues.at(prefix + "eq_mid"));
        if (state.sliderValues.count(prefix + "eq_high"))
            setChannelEQ(i, EQBand::High, state.sliderValues.at(prefix + "eq_high"));

        if (state.sliderValues.count(prefix + "send_reverb"))
            setChannelSend(i, SendType::Reverb, state.sliderValues.at(prefix + "send_reverb"));
        if (state.sliderValues.count(prefix + "send_delay"))
            setChannelSend(i, SendType::Delay, state.sliderValues.at(prefix + "send_delay"));
    }

    if (state.sliderValues.count("mixer_master_volume"))
        setMasterVolume(state.sliderValues.at("mixer_master_volume"));
    if (state.toggleStates.count(2000))
        setLimiterEnabled(state.toggleStates.at(2000));
    if (state.sliderValues.count("mixer_limiter_threshold"))
        setLimiterThreshold(state.sliderValues.at("mixer_limiter_threshold"));
    if (state.sliderValues.count("mixer_limiter_release"))
        setLimiterRelease(state.sliderValues.at("mixer_limiter_release"));

    if (state.toggleStates.count(2100))
        setReverbEnabled(state.toggleStates.at(2100));
    if (state.dropdownSelections.count("reverb_algorithm"))
        setReverbAlgorithm(static_cast<ReverbAlgorithm>(state.dropdownSelections.at("reverb_algorithm")));
    if (state.sliderValues.count("reverb_mix"))
        setReverbMix(state.sliderValues.at("reverb_mix"));
    if (state.sliderValues.count("reverb_room_size"))
        setReverbRoomSize(state.sliderValues.at("reverb_room_size"));
    if (state.sliderValues.count("reverb_damping"))
        setReverbDamping(state.sliderValues.at("reverb_damping"));
    if (state.sliderValues.count("reverb_predelay"))
        setReverbPredelay(state.sliderValues.at("reverb_predelay"));
    if (state.sliderValues.count("reverb_width"))
        setReverbWidth(state.sliderValues.at("reverb_width"));
    if (state.sliderValues.count("reverb_high_cut"))
        setReverbHighCut(state.sliderValues.at("reverb_high_cut"));
    if (state.sliderValues.count("reverb_low_cut"))
        setReverbLowCut(state.sliderValues.at("reverb_low_cut"));

    if (state.toggleStates.count(2200))
        setDelayEnabled(state.toggleStates.at(2200));
    if (state.toggleStates.count(2201))
        setDelaySyncToHost(state.toggleStates.at(2201));
    if (state.sliderValues.count("delay_time"))
        setDelayTime(state.sliderValues.at("delay_time"));
    if (state.dropdownSelections.count("delay_sync_division"))
        setDelaySyncDivision(state.dropdownSelections.at("delay_sync_division"));
    if (state.sliderValues.count("delay_feedback"))
        setDelayFeedback(state.sliderValues.at("delay_feedback"));
    if (state.sliderValues.count("delay_mix"))
        setDelayMix(state.sliderValues.at("delay_mix"));
    if (state.sliderValues.count("delay_high_cut"))
        setDelayHighCut(state.sliderValues.at("delay_high_cut"));
    if (state.sliderValues.count("delay_low_cut"))
        setDelayLowCut(state.sliderValues.at("delay_low_cut"));
    if (state.toggleStates.count(2202))
        setDelayPingPong(state.toggleStates.at(2202));
    if (state.sliderValues.count("delay_spread"))
        setDelaySpread(state.sliderValues.at("delay_spread"));

    if (state.toggleStates.count(2300))
        setCompressorEnabled(state.toggleStates.at(2300));
    if (state.sliderValues.count("comp_threshold"))
        setCompressorThreshold(state.sliderValues.at("comp_threshold"));
    if (state.sliderValues.count("comp_ratio"))
        setCompressorRatio(state.sliderValues.at("comp_ratio"));
    if (state.sliderValues.count("comp_attack"))
        setCompressorAttack(state.sliderValues.at("comp_attack"));
    if (state.sliderValues.count("comp_release"))
        setCompressorRelease(state.sliderValues.at("comp_release"));
    if (state.sliderValues.count("comp_makeup"))
        setCompressorMakeupGain(state.sliderValues.at("comp_makeup"));
    if (state.sliderValues.count("comp_knee"))
        setCompressorKnee(state.sliderValues.at("comp_knee"));
    if (state.toggleStates.count(2301))
        setSidechainEnabled(state.toggleStates.at(2301));
    if (state.dropdownSelections.count("comp_sidechain_source"))
        setSidechainSource(state.dropdownSelections.at("comp_sidechain_source"));

    if (state.toggleStates.count(2400))
        setDistortionEnabled(state.toggleStates.at(2400));
    if (state.sliderValues.count("dist_drive"))
        setDistortionDrive(state.sliderValues.at("dist_drive"));
   if (state.sliderValues.count("dist_mix"))
       setDistortionMix(state.sliderValues.at("dist_mix"));
   if (state.dropdownSelections.count("dist_bit_depth"))
       setBitDepth(state.dropdownSelections.at("dist_bit_depth"));
   if (state.sliderValues.count("dist_sr_reduction"))
       setSampleRateReduction(state.sliderValues.at("dist_sr_reduction"));
   if (state.sliderValues.count("dist_pre_gain"))
       setDistortionPreGain(state.sliderValues.at("dist_pre_gain"));
   if (state.sliderValues.count("dist_post_gain"))
       setDistortionPostGain(state.sliderValues.at("dist_post_gain"));
   if (state.dropdownSelections.count("dist_mode"))
       setDistortionMode(static_cast<DistortionState::Mode>(state.dropdownSelections.at("dist_mode")));
}
