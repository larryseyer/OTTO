#pragma once

#include <JuceHeader.h>
#include <array>
#include <atomic>
#include "ComponentState.h"
#include "INIConfig.h"

class Mixer {
public:
    enum class EQBand {
        Low = INIConfig::Defaults::ZERO_VALUE,
        Mid = INIConfig::Defaults::ONE_VALUE,
        High = INIConfig::Defaults::ONE_VALUE + INIConfig::Defaults::ONE_VALUE
    };

    enum class SendType {
        Reverb = INIConfig::Defaults::ZERO_VALUE,
        Delay = INIConfig::Defaults::ONE_VALUE
    };

    enum class ReverbAlgorithm {
        Hall = INIConfig::Defaults::ZERO_VALUE,
        Room = INIConfig::Defaults::ONE_VALUE,
        Plate = INIConfig::Defaults::ONE_VALUE + INIConfig::Defaults::ONE_VALUE,
        Spring = INIConfig::Defaults::ONE_VALUE + INIConfig::Defaults::ONE_VALUE + INIConfig::Defaults::ONE_VALUE,
        Shimmer = INIConfig::Defaults::ONE_VALUE + INIConfig::Defaults::ONE_VALUE + INIConfig::Defaults::ONE_VALUE + INIConfig::Defaults::ONE_VALUE
    };

    struct ChannelState {
        std::atomic<float> volume{INIConfig::Defaults::DEFAULT_KICK_VOLUME};
        std::atomic<float> pan{INIConfig::Audio::DEFAULT_PAN};
        std::atomic<bool> mute{INIConfig::Audio::DEFAULT_MUTE};
        std::atomic<bool> solo{INIConfig::Audio::DEFAULT_SOLO};
        std::array<std::atomic<float>, INIConfig::Audio::NUM_EQ_BANDS> eqGains{{INIConfig::Audio::EQ_ATOMIC_INIT, INIConfig::Audio::EQ_ATOMIC_INIT, INIConfig::Audio::EQ_ATOMIC_INIT}};
        std::array<std::atomic<float>, INIConfig::Audio::NUM_SEND_TYPES> sends{{INIConfig::Audio::SEND_ATOMIC_INIT, INIConfig::Audio::SEND_ATOMIC_INIT}};

        std::atomic<float> currentLevelLeft{INIConfig::Defaults::DEFAULT_LEVELLEFT};
        std::atomic<float> currentLevelRight{INIConfig::Defaults::DEFAULT_LEVELRIGHT};
        std::atomic<float> peakLevelLeft{INIConfig::Defaults::DEFAULT_PEAKLEFT};
        std::atomic<float> peakLevelRight{INIConfig::Defaults::DEFAULT_PEAKRIGHT};
    };

    struct MasterState {
        std::atomic<float> volume{INIConfig::Defaults::DEFAULT_MASTER_VOLUME};
        std::atomic<bool> limiterEnabled{true};
        std::atomic<float> limiterThreshold{INIConfig::Defaults::DEFAULT_LIMITER_THRESHOLD};
        std::atomic<float> limiterRelease{INIConfig::Defaults::DEFAULT_LIMITER_RELEASE};

        std::atomic<float> currentLevelLeft{INIConfig::Defaults::DEFAULT_LEVELLEFT};
        std::atomic<float> currentLevelRight{INIConfig::Defaults::DEFAULT_LEVELRIGHT};
        std::atomic<float> peakLevelLeft{INIConfig::Defaults::DEFAULT_PEAKLEFT};
        std::atomic<float> peakLevelRight{INIConfig::Defaults::DEFAULT_PEAKRIGHT};
    };

    struct ReverbState {
        std::atomic<bool> enabled{true};
        std::atomic<ReverbAlgorithm> algorithm{ReverbAlgorithm::Hall};
        std::atomic<float> mix{INIConfig::Defaults::DEFAULT_REVERB_MIX};
        std::atomic<float> roomSize{INIConfig::Defaults::DEFAULT_ROOM_SIZE};
        std::atomic<float> damping{INIConfig::Defaults::DEFAULT_DAMPING};
        std::atomic<float> predelay{INIConfig::Defaults::DEFAULT_PREDELAY};
        std::atomic<float> width{INIConfig::Defaults::DEFAULT_WIDTH};
        std::atomic<float> highCut{INIConfig::Defaults::DEFAULT_REVERB_HIGH_CUT};
        std::atomic<float> lowCut{INIConfig::Defaults::DEFAULT_REVERB_LOW_CUT};

        void copyFrom(const ReverbState& other) {
            enabled.store(other.enabled.load());
            algorithm.store(other.algorithm.load());
            mix.store(other.mix.load());
            roomSize.store(other.roomSize.load());
            damping.store(other.damping.load());
            predelay.store(other.predelay.load());
            width.store(other.width.load());
            highCut.store(other.highCut.load());
            lowCut.store(other.lowCut.load());
        }
    };

    struct DelayState {
        std::atomic<bool> enabled{true};
        std::atomic<bool> syncToHost{true};
        std::atomic<float> delayTime{INIConfig::Defaults::DEFAULT_DELAY_TIME};
        std::atomic<int> syncDivision{INIConfig::Audio::DEFAULT_SYNC_DIVISION};
        std::atomic<float> feedback{INIConfig::Defaults::DEFAULT_FEEDBACK};
        std::atomic<float> mix{INIConfig::Defaults::DEFAULT_DELAY_MIX};
        std::atomic<float> highCut{INIConfig::Defaults::DEFAULT_DELAY_HIGH_CUT};
        std::atomic<float> lowCut{INIConfig::Defaults::DEFAULT_DELAY_LOW_CUT};
        std::atomic<bool> pingPong{INIConfig::Defaults::DEFAULT_PINGPONG};
        std::atomic<float> spread{INIConfig::Defaults::DEFAULT_SPREAD};

        void copyFrom(const DelayState& other) {
            enabled.store(other.enabled.load());
            syncToHost.store(other.syncToHost.load());
            delayTime.store(other.delayTime.load());
            syncDivision.store(other.syncDivision.load());
            feedback.store(other.feedback.load());
            mix.store(other.mix.load());
            highCut.store(other.highCut.load());
            lowCut.store(other.lowCut.load());
            pingPong.store(other.pingPong.load());
            spread.store(other.spread.load());
        }
    };

    struct CompressorState {
        std::atomic<bool> enabled{false};
        std::atomic<float> threshold{INIConfig::Defaults::DEFAULT_COMPRESSOR_THRESHOLD};
        std::atomic<float> ratio{INIConfig::Defaults::DEFAULT_COMPRESSOR_RATIO};
        std::atomic<float> attack{INIConfig::Defaults::DEFAULT_COMPRESSOR_ATTACK};
        std::atomic<float> release{INIConfig::Defaults::DEFAULT_COMPRESSOR_RELEASE};
        std::atomic<float> makeupGain{INIConfig::Defaults::DEFAULT_MAKEUPGAIN};
        std::atomic<float> knee{INIConfig::Defaults::DEFAULT_COMPRESSOR_KNEE};
        std::atomic<bool> sidechainEnabled{INIConfig::Defaults::DEFAULT_SIDECHAIN_ENABLED};
        std::atomic<int> sidechainSource{INIConfig::Audio::DEFAULT_SIDECHAIN_SOURCE};

        void copyFrom(const CompressorState& other) {
            enabled.store(other.enabled.load());
            threshold.store(other.threshold.load());
            ratio.store(other.ratio.load());
            attack.store(other.attack.load());
            release.store(other.release.load());
            makeupGain.store(other.makeupGain.load());
            knee.store(other.knee.load());
            sidechainEnabled.store(other.sidechainEnabled.load());
            sidechainSource.store(other.sidechainSource.load());
        }
    };

    struct DistortionState {
        std::atomic<bool> enabled{false};
        std::atomic<float> drive{INIConfig::Defaults::DEFAULT_DRIVE};
        std::atomic<float> mix{INIConfig::Defaults::DEFAULT_MIX};
        std::atomic<int> bitDepth{INIConfig::Audio::BIT_DEPTH_16};
        std::atomic<float> sampleRateReduction{INIConfig::Audio::DEFAULT_SAMPLERATE_REDUCTION};
        std::atomic<float> preGain{INIConfig::Audio::DEFAULT_PRE_GAIN};
        std::atomic<float> postGain{INIConfig::Audio::DEFAULT_POST_GAIN};
        enum class Mode { Soft, Hard, Bit, Fold } mode{Mode::Soft};

        void copyFrom(const DistortionState& other) {
            enabled.store(other.enabled.load());
            drive.store(other.drive.load());
            mix.store(other.mix.load());
            bitDepth.store(other.bitDepth.load());
            sampleRateReduction.store(other.sampleRateReduction.load());
            preGain.store(other.preGain.load());
            postGain.store(other.postGain.load());
            mode = other.mode;
        }
    };

    struct EffectPreset {
        juce::String name;
        ReverbState reverb;
        DelayState delay;
        CompressorState compressor;
        DistortionState distortion;

        EffectPreset() = default;

        EffectPreset(const EffectPreset& other)
            : name(other.name) {
            reverb.copyFrom(other.reverb);
            delay.copyFrom(other.delay);
            compressor.copyFrom(other.compressor);
            distortion.copyFrom(other.distortion);
        }

        EffectPreset& operator=(const EffectPreset& other) {
            if (this != &other) {
                name = other.name;
                reverb.copyFrom(other.reverb);
                delay.copyFrom(other.delay);
                compressor.copyFrom(other.compressor);
                distortion.copyFrom(other.distortion);
            }
            return *this;
        }
    };

    Mixer();
    ~Mixer() = default;

    void prepare(double sampleRate, int samplesPerBlock);
    void processBlock(juce::AudioBuffer<float>& buffer);
    void reset();

    void setChannelVolume(int channel, float volume);
    void setChannelPan(int channel, float pan);
    void setChannelMute(int channel, bool mute);
    void setChannelSolo(int channel, bool solo);
    void setChannelEQ(int channel, EQBand band, float gain);
    void setChannelSend(int channel, SendType send, float amount);

    void setMasterVolume(float volume);
    void setLimiterEnabled(bool enabled);
    void setLimiterThreshold(float threshold);
    void setLimiterRelease(float release);

    void setReverbEnabled(bool enabled);
    void setReverbAlgorithm(ReverbAlgorithm algorithm);
    void setReverbMix(float mix);
    void setReverbRoomSize(float size);
    void setReverbDamping(float damping);
    void setReverbPredelay(float predelay);
    void setReverbWidth(float width);
    void setReverbHighCut(float freq);
    void setReverbLowCut(float freq);

    void setDelayEnabled(bool enabled);
    void setDelaySyncToHost(bool sync);
    void setDelayTime(float timeMs);
    void setDelaySyncDivision(int division);
    void setDelayFeedback(float feedback);
    void setDelayMix(float mix);
    void setDelayHighCut(float freq);
    void setDelayLowCut(float freq);
    void setDelayPingPong(bool enabled);
    void setDelaySpread(float spread);

    void setCompressorEnabled(bool enabled);
    void setCompressorThreshold(float threshold);
    void setCompressorRatio(float ratio);
    void setCompressorAttack(float attack);
    void setCompressorRelease(float release);
    void setCompressorMakeupGain(float gain);
    void setCompressorKnee(float knee);
    void setSidechainEnabled(bool enabled);
    void setSidechainSource(int channel);

    void setDistortionEnabled(bool enabled);
    void setDistortionDrive(float drive);
    void setDistortionMix(float mix);
    void setBitDepth(int bits);
    void setSampleRateReduction(float factor);
    void setDistortionPreGain(float gain);
    void setDistortionPostGain(float gain);
    void setDistortionMode(DistortionState::Mode mode);

    void savePreset(const juce::String& name);
    void loadPreset(int index);
    void deletePreset(int index);
    juce::StringArray getPresetNames() const;
    int getNumPresets() const { return effectPresets.size(); }

    struct LevelInfo {
        float left;
        float right;
    };

    LevelInfo getChannelLevels(int channel) const;
    LevelInfo getMasterLevels() const;
    void resetPeakLevels();

    void saveState(ComponentState& state) const;
    void loadState(const ComponentState& state);

    float getChannelVolume(int channel) const;
    float getChannelPan(int channel) const;
    bool isChannelMuted(int channel) const;
    bool isChannelSoloed(int channel) const;
    float getChannelEQ(int channel, EQBand band) const;
    float getChannelSend(int channel, SendType send) const;

    float getMasterVolume() const { return masterState.volume.load(); }
    bool isLimiterEnabled() const { return masterState.limiterEnabled.load(); }
    float getLimiterThreshold() const { return masterState.limiterThreshold.load(); }

    const ReverbState& getReverbState() const { return reverbState; }
    const DelayState& getDelayState() const { return delayState; }
    const CompressorState& getCompressorState() const { return compressorState; }
    const DistortionState& getDistortionState() const { return distortionState; }

    void setHostTempo(double tempo) { hostTempo = tempo; }

private:
    static constexpr int NUM_CHANNELS = INIConfig::Defaults::MAX_PLAYERS;

    std::array<ChannelState, NUM_CHANNELS> channelStates;
    MasterState masterState;
    ReverbState reverbState;
    DelayState delayState;
    CompressorState compressorState;
    DistortionState distortionState;

    struct ChannelProcessors {
        juce::dsp::IIR::Filter<float> lowShelf;
        juce::dsp::IIR::Filter<float> midPeak;
        juce::dsp::IIR::Filter<float> highShelf;
        juce::dsp::Panner<float> panner;
        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> volumeSmoothed;
        juce::LinearSmoothedValue<float> levelFollowerLeft;
        juce::LinearSmoothedValue<float> levelFollowerRight;
    };

    std::array<ChannelProcessors, NUM_CHANNELS> channelProcessors;

    juce::dsp::Reverb reverb;
    juce::dsp::DelayLine<float> delayLineLeft{INIConfig::Defaults::MAX_DELAY_SAMPLES};
    juce::dsp::DelayLine<float> delayLineRight{INIConfig::Defaults::MAX_DELAY_SAMPLES};
    juce::dsp::Compressor<float> compressor;
    juce::dsp::Limiter<float> limiter;

    juce::dsp::StateVariableTPTFilter<float> reverbHighCut;
    juce::dsp::StateVariableTPTFilter<float> reverbLowCut;
    juce::dsp::StateVariableTPTFilter<float> delayHighCut;
    juce::dsp::StateVariableTPTFilter<float> delayLowCut;

    juce::AudioBuffer<float> sendBuffer;
    juce::AudioBuffer<float> reverbBuffer;
    juce::AudioBuffer<float> delayBuffer;
    juce::AudioBuffer<float> sidechainBuffer;

    double sampleRate = static_cast<double>(INIConfig::Defaults::DEFAULT_SAMPLE_RATE);
    int blockSize = INIConfig::Defaults::DEFAULT_BUFFER_SIZE;
    double hostTempo = INIConfig::Defaults::DEFAULT_TEMPO;
    int delayBufferPos = INIConfig::Defaults::ZERO_VALUE;

    juce::Array<EffectPreset> effectPresets;

    void processChannel(int channel, juce::AudioBuffer<float>& buffer);
    void processReverb(juce::AudioBuffer<float>& buffer);
    void processDelay(juce::AudioBuffer<float>& buffer);
    void processCompressor(juce::AudioBuffer<float>& buffer);
    void processDistortion(juce::AudioBuffer<float>& buffer);
    void processLimiter(juce::AudioBuffer<float>& buffer);
    void updateMetering(int channel, const juce::AudioBuffer<float>& buffer);
    void updateMasterMetering(const juce::AudioBuffer<float>& buffer);

    float applyDistortion(float input, DistortionState::Mode mode, float drive);
    void updateEQCoefficients(int channel);
    void updateDelayTime();

    bool anySolo() const;

    void loadDefaultPresets();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Mixer)
};
