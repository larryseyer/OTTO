#include "PopupWindows.h"
#include "Mixer.h"
#include "SFZEngine.h"
#include "INIConfig.h"

class DrumKitMixerWindow::MixerContent : public juce::Component,
                                         public juce::Timer {
public:
    class DoubleClickFader : public juce::Slider {
    public:
        DoubleClickFader(SliderStyle style, TextEntryBoxPosition textBoxPosition, double defaultVal = INIConfig::Defaults::DEFAULT_CHANNEL_VOLUME)
            : juce::Slider(style, textBoxPosition), defaultValue(defaultVal) {
        }

        void mouseDoubleClick(const juce::MouseEvent& event) override {
            setValue(defaultValue, juce::sendNotificationSync);
        }

        void setDefaultValue(double value) {
            defaultValue = value;
        }

    private:
        double defaultValue;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DoubleClickFader)
    };
    class ChannelStrip : public juce::Component {
    public:
        ChannelStrip(int channelIndex, ColorScheme& colorScheme,
                    ResponsiveLayoutManager& layoutManager)
            : index(channelIndex), colorScheme(colorScheme),
              layoutManager(layoutManager),
              volumeFader(juce::Slider::LinearVertical, juce::Slider::TextBoxBelow, INIConfig::Defaults::DEFAULT_CHANNEL_VOLUME),
              panKnob(juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow),
              muteButton("M"), soloButton("S"),
              highEQ(juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox),
              midEQ(juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox),
              lowEQ(juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox),
              reverbSend(juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox),
              delaySend(juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox) {

            setupComponents();
        }

        void paint(juce::Graphics& g) override {
            g.setColour(colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground));
            g.fillRoundedRectangle(getLocalBounds().toFloat(), layoutManager.scaled(INIConfig::LayoutConstants::mixerStripCornerRadius));

            g.setColour(colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
            g.setFont(layoutManager.scaled(INIConfig::LayoutConstants::mixerStripTitleFontSize));
            g.drawText("CH " + juce::String(index + 1),
                      getLocalBounds().removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::mixerStripTitleHeight)),
                      juce::Justification::centred);

            drawMeter(g);
        }

        void resized() override {
            auto bounds = getLocalBounds();

            bounds.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::mixerStripTitleHeight));

            auto meterBounds = bounds.removeFromRight(layoutManager.scaled(INIConfig::LayoutConstants::mixerStripMeterWidth));
            meterBounds = meterBounds.reduced(layoutManager.scaled(INIConfig::LayoutConstants::mixerStripMeterMargin));
            meterArea = meterBounds;

            auto controlsBounds = bounds.reduced(layoutManager.scaled(INIConfig::LayoutConstants::mixerStripControlsMargin));

            auto eqSection = controlsBounds.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::mixerStripEQSection));
            auto eqKnobSize = layoutManager.scaled(INIConfig::LayoutConstants::mixerStripEQKnobSize);
            auto eqY = layoutManager.scaled(INIConfig::LayoutConstants::mixerStripEQYOffset);

            highEQ.setBounds(eqSection.getX(), eqY, eqKnobSize, eqKnobSize);
            midEQ.setBounds(eqSection.getX() + layoutManager.scaled(INIConfig::LayoutConstants::mixerStripEQSpacing), eqY, eqKnobSize, eqKnobSize);
            lowEQ.setBounds(eqSection.getX(), eqY + layoutManager.scaled(INIConfig::LayoutConstants::mixerStripEQRow2Offset), eqKnobSize, eqKnobSize);

            auto sendY = eqY + layoutManager.scaled(INIConfig::LayoutConstants::mixerStripSendYOffset);
            reverbSend.setBounds(eqSection.getX(), sendY, eqKnobSize, eqKnobSize);
            delaySend.setBounds(eqSection.getX() + layoutManager.scaled(INIConfig::LayoutConstants::mixerStripEQSpacing), sendY, eqKnobSize, eqKnobSize);

            controlsBounds.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::mixerStripSectionSpacing));
            auto buttonSection = controlsBounds.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::mixerStripButtonHeight));
            auto buttonWidth = layoutManager.scaled(INIConfig::LayoutConstants::mixerStripButtonWidth);
            muteButton.setBounds(buttonSection.getX(), buttonSection.getY(), buttonWidth, buttonSection.getHeight());
            soloButton.setBounds(buttonSection.getX() + buttonWidth + layoutManager.scaled(INIConfig::LayoutConstants::mixerStripButtonSpacing),
                                buttonSection.getY(), buttonWidth, buttonSection.getHeight());

            controlsBounds.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::mixerStripSectionSpacing));
            auto panSection = controlsBounds.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::mixerStripPanKnobHeight));
            panKnob.setBounds(panSection.withSizeKeepingCentre(layoutManager.scaled(INIConfig::LayoutConstants::mixerStripPanKnobSize), layoutManager.scaled(INIConfig::LayoutConstants::mixerStripPanKnobHeight)));

            controlsBounds.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::mixerStripSectionSpacing));
            volumeFader.setBounds(controlsBounds.withTrimmedTop(layoutManager.scaled(INIConfig::LayoutConstants::mixerStripFaderTopMargin)));
        }

        void updateMetering(float leftLevel, float rightLevel) {
            currentLevel = juce::jmax(leftLevel, rightLevel);

            if (currentLevel > peakLevel) {
                peakLevel = currentLevel;
                peakHoldCounter = 0;
            } else {
                peakHoldCounter++;
                if (peakHoldCounter > INIConfig::LayoutConstants::mixerStripPeakHoldCount) {
                    peakLevel *= INIConfig::LayoutConstants::mixerStripPeakDecay;
                }
            }

            repaint(meterArea);
        }

        juce::Slider& getVolumeFader() { return volumeFader; }
        juce::Slider& getPanKnob() { return panKnob; }
        juce::TextButton& getMuteButton() { return muteButton; }
        juce::TextButton& getSoloButton() { return soloButton; }
        juce::Slider& getHighEQ() { return highEQ; }
        juce::Slider& getMidEQ() { return midEQ; }
        juce::Slider& getLowEQ() { return lowEQ; }
        juce::Slider& getReverbSend() { return reverbSend; }
        juce::Slider& getDelaySend() { return delaySend; }

    private:
        void setupComponents() {
            addAndMakeVisible(volumeFader);
            volumeFader.setRange(0.0, 1.0);
            volumeFader.setValue(INIConfig::Defaults::DEFAULT_CHANNEL_VOLUME);
            volumeFader.setTextValueSuffix(" dB");
            volumeFader.setColour(juce::Slider::backgroundColourId,
                                 colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground).darker());
            volumeFader.setColour(juce::Slider::trackColourId,
                                 colorScheme.getColor(ColorScheme::ColorRole::Accent));

            addAndMakeVisible(panKnob);
            panKnob.setRange(-1.0, 1.0);
            panKnob.setValue(0.0);
            panKnob.setTextValueSuffix("");

            addAndMakeVisible(muteButton);
            addAndMakeVisible(soloButton);
            muteButton.setClickingTogglesState(true);
            soloButton.setClickingTogglesState(true);

            muteButton.setColour(juce::TextButton::buttonOnColourId, colorScheme.getColor(ColorScheme::ColorRole::Error));
            soloButton.setColour(juce::TextButton::buttonOnColourId, colorScheme.getColor(ColorScheme::ColorRole::Warning));

            addAndMakeVisible(highEQ);
            addAndMakeVisible(midEQ);
            addAndMakeVisible(lowEQ);

            highEQ.setRange(-12.0, 12.0);
            midEQ.setRange(-12.0, 12.0);
            lowEQ.setRange(-12.0, 12.0);
            highEQ.setValue(0.0);
            midEQ.setValue(0.0);
            lowEQ.setValue(0.0);

            addAndMakeVisible(reverbSend);
            addAndMakeVisible(delaySend);

            reverbSend.setRange(0.0, 1.0);
            delaySend.setRange(0.0, 1.0);
            reverbSend.setValue(0.0);
            delaySend.setValue(0.0);

            auto knobColor = colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground).darker();
            auto thumbColor = colorScheme.getColor(ColorScheme::ColorRole::Accent);

            for (auto* slider : { &panKnob, &highEQ, &midEQ, &lowEQ, &reverbSend, &delaySend }) {
                slider->setColour(juce::Slider::rotarySliderFillColourId, thumbColor);
                slider->setColour(juce::Slider::rotarySliderOutlineColourId, knobColor);
            }
        }

        void drawMeter(juce::Graphics& g) {
            if (meterArea.isEmpty()) return;

            g.setColour(colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground).darker(0.8f));
            g.fillRect(meterArea);

            float dbLevel = 20.0f * std::log10(juce::jmax(0.0001f, currentLevel));
            float normalizedLevel = juce::jmap(dbLevel, INIConfig::LayoutConstants::mixerStripMeterMinDb, INIConfig::LayoutConstants::mixerStripMeterMaxDb, 0.0f, 1.0f);

            int meterHeight = static_cast<int>(meterArea.getHeight() * normalizedLevel);
            auto meterRect = meterArea.removeFromBottom(meterHeight);

            if (!meterRect.isEmpty()) {
                juce::ColourGradient gradient(colorScheme.getColor(ColorScheme::ColorRole::MeterLow), 0, meterArea.getBottom(),
                                            colorScheme.getColor(ColorScheme::ColorRole::MeterHigh), 0, meterArea.getY(), false);
                gradient.addColour(0.7, colorScheme.getColor(ColorScheme::ColorRole::MeterMid));
                g.setGradientFill(gradient);
                g.fillRect(meterRect);
            }

            float peakDb = 20.0f * std::log10(juce::jmax(0.0001f, peakLevel));
            float peakNormalized = juce::jmap(peakDb, INIConfig::LayoutConstants::mixerStripMeterMinDb, INIConfig::LayoutConstants::mixerStripMeterMaxDb, 0.0f, 1.0f);
            int peakY = meterArea.getBottom() - static_cast<int>(meterArea.getHeight() * peakNormalized);

            g.setColour(colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
            g.drawHorizontalLine(peakY, meterArea.getX(), meterArea.getRight());

            g.setColour(colorScheme.getColor(ColorScheme::ColorRole::SecondaryText).withAlpha(0.5f));
            for (int db = 0; db >= INIConfig::LayoutConstants::mixerStripMeterMinDb; db -= static_cast<int>(INIConfig::LayoutConstants::mixerStripMeterDbStep)) {
                float y = meterArea.getBottom() - (meterArea.getHeight() * (db + 60) / 60.0f);
                g.drawHorizontalLine(static_cast<int>(y), meterArea.getX() - INIConfig::LayoutConstants::mixerStripMeterTickLength, meterArea.getX());
            }
        }

        int index;
        ColorScheme& colorScheme;
        ResponsiveLayoutManager& layoutManager;

        DoubleClickFader volumeFader;
        juce::Slider panKnob;
        juce::TextButton muteButton, soloButton;
        juce::Slider highEQ, midEQ, lowEQ;
        juce::Slider reverbSend, delaySend;

        juce::Rectangle<int> meterArea;
        float currentLevel = 0.0f;
        float peakLevel = 0.0f;
        int peakHoldCounter = 0;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChannelStrip)
    };

    class MasterSection : public juce::Component {
    public:
        MasterSection(ColorScheme& colorScheme, ResponsiveLayoutManager& layoutManager)
            : colorScheme(colorScheme), layoutManager(layoutManager),
              masterFader(juce::Slider::LinearVertical, juce::Slider::TextBoxBelow, INIConfig::Defaults::DEFAULT_MASTER_VOLUME),
              limiterButton("Limiter"), limiterThreshold(juce::Slider::LinearHorizontal, juce::Slider::TextBoxRight) {

            setupComponents();
        }

        void paint(juce::Graphics& g) override {
            g.setColour(colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground));
            g.fillRoundedRectangle(getLocalBounds().toFloat(), layoutManager.scaled(INIConfig::LayoutConstants::mixerStripCornerRadius));

            g.setColour(colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
            g.setFont(layoutManager.scaled(INIConfig::LayoutConstants::mixerMasterTitleFontSize));
            g.drawText("MASTER", getLocalBounds().removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::mixerMasterTitleHeight)),
                      juce::Justification::centred);

            drawMasterMeter(g);
        }

        void resized() override {
            auto bounds = getLocalBounds();
            bounds.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::mixerMasterTitleHeight));

            auto leftMeter = bounds.removeFromLeft(layoutManager.scaled(INIConfig::LayoutConstants::mixerMasterMeterWidth)).reduced(INIConfig::LayoutConstants::mixerMasterMeterMargin);
            auto rightMeter = bounds.removeFromRight(layoutManager.scaled(INIConfig::LayoutConstants::mixerMasterMeterWidth)).reduced(INIConfig::LayoutConstants::mixerMasterMeterMargin);
            leftMeterArea = leftMeter;
            rightMeterArea = rightMeter;

            auto controlBounds = bounds.reduced(layoutManager.scaled(INIConfig::LayoutConstants::mixerMasterControlsMargin));

            auto limiterSection = controlBounds.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::mixerMasterLimiterHeight));
            limiterButton.setBounds(limiterSection.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::mixerMasterLimiterButtonHeight)));
            limiterSection.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::mixerMasterLimiterSpacing));
            limiterThreshold.setBounds(limiterSection);

            controlBounds.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::mixerMasterFaderTopMargin));
            masterFader.setBounds(controlBounds);
        }

        void updateMetering(float leftLevel, float rightLevel) {
            leftChannelLevel = leftLevel;
            rightChannelLevel = rightLevel;

            if (leftLevel > leftPeakLevel) {
                leftPeakLevel = leftLevel;
                leftPeakCounter = 0;
            } else {
                leftPeakCounter++;
                if (leftPeakCounter > INIConfig::LayoutConstants::mixerStripPeakHoldCount) {
                    leftPeakLevel *= INIConfig::LayoutConstants::mixerStripPeakDecay;
                }
            }

            if (rightLevel > rightPeakLevel) {
                rightPeakLevel = rightLevel;
                rightPeakCounter = 0;
            } else {
                rightPeakCounter++;
                if (rightPeakCounter > INIConfig::LayoutConstants::mixerStripPeakHoldCount) {
                    rightPeakLevel *= INIConfig::LayoutConstants::mixerStripPeakDecay;
                }
            }

            repaint();
        }

        juce::Slider& getMasterFader() { return masterFader; }
        juce::TextButton& getLimiterButton() { return limiterButton; }
        juce::Slider& getLimiterThreshold() { return limiterThreshold; }

    private:
        void setupComponents() {
            addAndMakeVisible(masterFader);
            masterFader.setRange(0.0, 1.2);
            masterFader.setValue(INIConfig::Defaults::DEFAULT_MASTER_VOLUME);
            masterFader.setSkewFactorFromMidPoint(0.5);

            addAndMakeVisible(limiterButton);
            limiterButton.setClickingTogglesState(true);
            limiterButton.setColour(juce::TextButton::buttonOnColourId,
                                   colorScheme.getColor(ColorScheme::ColorRole::Accent));

            addAndMakeVisible(limiterThreshold);
            limiterThreshold.setRange(-12.0, 0.0);
            limiterThreshold.setValue(-3.0);
            limiterThreshold.setTextValueSuffix(" dB");
        }

        void drawMasterMeter(juce::Graphics& g) {
            drawChannelMeter(g, leftMeterArea, leftChannelLevel, leftPeakLevel);
            drawChannelMeter(g, rightMeterArea, rightChannelLevel, rightPeakLevel);
        }

        void drawChannelMeter(juce::Graphics& g, const juce::Rectangle<int>& area,
                             float level, float peak) {
            if (area.isEmpty()) return;

            g.setColour(colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground).darker(0.8f));
            g.fillRect(area);

            float dbLevel = 20.0f * std::log10(juce::jmax(0.0001f, level));
            float normalizedLevel = juce::jmap(dbLevel, INIConfig::LayoutConstants::mixerStripMeterMinDb, INIConfig::LayoutConstants::mixerStripMeterClipDb, 0.0f, 1.0f);

            int meterHeight = static_cast<int>(area.getHeight() * normalizedLevel);

            auto areaCopy = area;
            auto meterRect = areaCopy.removeFromBottom(meterHeight);

            if (!meterRect.isEmpty()) {
                juce::ColourGradient gradient(colorScheme.getColor(ColorScheme::ColorRole::MeterLow), 0, area.getBottom(),
                                            colorScheme.getColor(ColorScheme::ColorRole::MeterHigh), 0, area.getY(), false);
                gradient.addColour(0.7, colorScheme.getColor(ColorScheme::ColorRole::MeterMid));
                gradient.addColour(0.9, colorScheme.getColor(ColorScheme::ColorRole::Warning));
                g.setGradientFill(gradient);
                g.fillRect(meterRect);
            }

            float peakDb = 20.0f * std::log10(juce::jmax(0.0001f, peak));
            float peakNormalized = juce::jmap(peakDb, INIConfig::LayoutConstants::mixerStripMeterMinDb, INIConfig::LayoutConstants::mixerStripMeterClipDb, 0.0f, 1.0f);
            int peakY = area.getBottom() - static_cast<int>(area.getHeight() * peakNormalized);

            g.setColour(colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
            g.drawHorizontalLine(peakY, area.getX(), area.getRight());
        }

        ColorScheme& colorScheme;
        ResponsiveLayoutManager& layoutManager;

        DoubleClickFader masterFader;
        juce::TextButton limiterButton;
        juce::Slider limiterThreshold;

        juce::Rectangle<int> leftMeterArea, rightMeterArea;
        float leftChannelLevel = 0.0f, rightChannelLevel = 0.0f;
        float leftPeakLevel = 0.0f, rightPeakLevel = 0.0f;
        int leftPeakCounter = 0, rightPeakCounter = 0;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MasterSection)
    };

public:
    MixerContent(Mixer& mixer, SFZEngine& sfzEngine, ColorScheme& colorScheme,
                FontManager& fontManager, ResponsiveLayoutManager& layoutManager)
        : mixer(mixer), sfzEngine(sfzEngine), colorScheme(colorScheme),
          fontManager(fontManager), layoutManager(layoutManager) {

        for (int i = 0; i < INIConfig::LayoutConstants::playerTabsCount; ++i) {
            auto strip = std::make_unique<ChannelStrip>(i, colorScheme, layoutManager);
            setupChannelCallbacks(strip.get(), i);
            addAndMakeVisible(strip.get());
            channelStrips.add(strip.release());
        }

        masterSection = std::make_unique<MasterSection>(colorScheme, layoutManager);
        setupMasterCallbacks();
        addAndMakeVisible(masterSection.get());

        startTimerHz(INIConfig::LayoutConstants::mixerTimerHz);
    }

    ~MixerContent() override {
        stopTimer();
    }

    void paint(juce::Graphics& g) override {
        g.fillAll(colorScheme.getColor(ColorScheme::ColorRole::WindowBackground));
    }

    void resized() override {
        auto bounds = getLocalBounds();

        bounds.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::mixerTopMargin));

        int masterWidth = layoutManager.scaled(INIConfig::LayoutConstants::mixerMasterSectionWidth);
        auto masterBounds = bounds.removeFromRight(masterWidth);
        masterSection->setBounds(masterBounds.reduced(layoutManager.scaled(INIConfig::LayoutConstants::mixerMasterSectionMargin)));

        int stripWidth = bounds.getWidth() / INIConfig::LayoutConstants::mixerChannelStripWidth;
        for (int i = 0; i < INIConfig::LayoutConstants::playerTabsCount; ++i) {
            channelStrips[i]->setBounds(bounds.removeFromLeft(stripWidth).reduced(layoutManager.scaled(INIConfig::LayoutConstants::mixerChannelStripMargin)));
        }
    }

    void timerCallback() override {
        for (int i = 0; i < INIConfig::LayoutConstants::playerTabsCount; ++i) {
            auto levels = mixer.getChannelLevels(i);
            channelStrips[i]->updateMetering(levels.left, levels.right);
        }

        auto masterLevels = mixer.getMasterLevels();
        masterSection->updateMetering(masterLevels.left, masterLevels.right);
    }

private:
    void setupChannelCallbacks(ChannelStrip* strip, int channelIndex) {
        strip->getVolumeFader().onValueChange = [this, channelIndex, strip] {
            float value = static_cast<float>(strip->getVolumeFader().getValue());
            mixer.setChannelVolume(channelIndex, value);
        };

        strip->getPanKnob().onValueChange = [this, channelIndex, strip] {
            float value = static_cast<float>(strip->getPanKnob().getValue());
            mixer.setChannelPan(channelIndex, value);
        };

        strip->getMuteButton().onClick = [this, channelIndex, strip] {
            bool muted = strip->getMuteButton().getToggleState();
            mixer.setChannelMute(channelIndex, muted);
        };

        strip->getSoloButton().onClick = [this, channelIndex, strip] {
            bool soloed = strip->getSoloButton().getToggleState();
            mixer.setChannelSolo(channelIndex, soloed);
        };

        strip->getHighEQ().onValueChange = [this, channelIndex, strip] {
            float gain = static_cast<float>(strip->getHighEQ().getValue());
            mixer.setChannelEQ(channelIndex, Mixer::EQBand::High, gain);
        };

        strip->getMidEQ().onValueChange = [this, channelIndex, strip] {
            float gain = static_cast<float>(strip->getMidEQ().getValue());
            mixer.setChannelEQ(channelIndex, Mixer::EQBand::Mid, gain);
        };

        strip->getLowEQ().onValueChange = [this, channelIndex, strip] {
            float gain = static_cast<float>(strip->getLowEQ().getValue());
            mixer.setChannelEQ(channelIndex, Mixer::EQBand::Low, gain);
        };

        strip->getReverbSend().onValueChange = [this, channelIndex, strip] {
            float value = static_cast<float>(strip->getReverbSend().getValue());
            mixer.setChannelSend(channelIndex, Mixer::SendType::Reverb, value);
        };

        strip->getDelaySend().onValueChange = [this, channelIndex, strip] {
            float value = static_cast<float>(strip->getDelaySend().getValue());
            mixer.setChannelSend(channelIndex, Mixer::SendType::Delay, value);
        };
    }

    void setupMasterCallbacks() {
        masterSection->getMasterFader().onValueChange = [this] {
            float value = static_cast<float>(masterSection->getMasterFader().getValue());
            mixer.setMasterVolume(value);
        };

        masterSection->getLimiterButton().onClick = [this] {
            bool enabled = masterSection->getLimiterButton().getToggleState();
            mixer.setLimiterEnabled(enabled);
        };

        masterSection->getLimiterThreshold().onValueChange = [this] {
            float threshold = static_cast<float>(masterSection->getLimiterThreshold().getValue());
            mixer.setLimiterThreshold(threshold);
        };
    }

    Mixer& mixer;
    SFZEngine& sfzEngine;
    ColorScheme& colorScheme;
    FontManager& fontManager;
    ResponsiveLayoutManager& layoutManager;

    juce::OwnedArray<ChannelStrip> channelStrips;
    std::unique_ptr<MasterSection> masterSection;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerContent)
};

DrumKitMixerWindow::DrumKitMixerWindow(Mixer& mixer,
                                       SFZEngine& sfzEngine,
                                       ColorScheme& colorScheme,
                                       FontManager& fontManager,
                                       ResponsiveLayoutManager& layoutManager,
                                       INIDataManager& iniManager)
    : juce::DocumentWindow("DrumKit Mixer", colorScheme.getColor(ColorScheme::ColorRole::WindowBackground), DocumentWindow::closeButton),
      mixer(mixer), sfzEngine(sfzEngine), colorScheme(colorScheme),
      fontManager(fontManager), layoutManager(layoutManager), iniManager(iniManager) {

    content = std::make_unique<MixerContent>(mixer, sfzEngine, colorScheme, fontManager, layoutManager);
    setContentOwned(content.release(), false);
    setResizable(true, true);
    setUsingNativeTitleBar(true);

    ComponentState state;
    if (iniManager.loadAllSettings(state)) {
        loadStates(state);
    } else {
        setBounds(INIConfig::LayoutConstants::editorWindowDefaultX, INIConfig::LayoutConstants::editorWindowDefaultY,
                  INIConfig::LayoutConstants::mixerWindowDefaultWidth, INIConfig::LayoutConstants::mixerWindowDefaultHeight);
    }

    setBackgroundColour(colorScheme.getColor(ColorScheme::ColorRole::WindowBackground));
}

DrumKitMixerWindow::~DrumKitMixerWindow() {
}

void DrumKitMixerWindow::closeButtonPressed() {
    ComponentState state;
    if (iniManager.loadAllSettings(state)) {
        saveStates(state);
        if (!iniManager.saveAllSettings(state)) {
        }
    }
    setVisible(false);
}

void DrumKitMixerWindow::saveStates(ComponentState& state) {
    auto bounds = getBounds();
    state.sliderValues["mixerWindowX"] = static_cast<float>(INIConfig::clampWindowSize(bounds.getX()));
    state.sliderValues["mixerWindowY"] = static_cast<float>(INIConfig::clampWindowSize(bounds.getY()));
    state.sliderValues["mixerWindowWidth"] = static_cast<float>(INIConfig::clampWindowSize(bounds.getWidth()));
    state.sliderValues["mixerWindowHeight"] = static_cast<float>(INIConfig::clampWindowSize(bounds.getHeight()));
}

void DrumKitMixerWindow::loadStates(const ComponentState& state) {
    int windowX = INIConfig::LayoutConstants::editorWindowDefaultX;
    int windowY = INIConfig::LayoutConstants::editorWindowDefaultY;
    int windowWidth = INIConfig::LayoutConstants::mixerWindowDefaultWidth;
    int windowHeight = INIConfig::LayoutConstants::mixerWindowDefaultHeight;

    auto xIt = state.sliderValues.find("mixerWindowX");
    if (xIt != state.sliderValues.end()) {
        windowX = INIConfig::clampWindowSize(static_cast<int>(xIt->second));
    }

    auto yIt = state.sliderValues.find("mixerWindowY");
    if (yIt != state.sliderValues.end()) {
        windowY = INIConfig::clampWindowSize(static_cast<int>(yIt->second));
    }

    auto widthIt = state.sliderValues.find("mixerWindowWidth");
    if (widthIt != state.sliderValues.end()) {
        windowWidth = INIConfig::clampWindowSize(static_cast<int>(widthIt->second));
    }

    auto heightIt = state.sliderValues.find("mixerWindowHeight");
    if (heightIt != state.sliderValues.end()) {
        windowHeight = INIConfig::clampWindowSize(static_cast<int>(heightIt->second));
    }

    if (windowWidth < INIConfig::LayoutConstants::mixerWindowMinWidth) windowWidth = INIConfig::LayoutConstants::mixerWindowDefaultWidth;
    if (windowHeight < INIConfig::LayoutConstants::mixerWindowMinHeight) windowHeight = INIConfig::LayoutConstants::mixerWindowDefaultHeight;

    setBounds(windowX, windowY, windowWidth, windowHeight);
}

void DrumKitMixerWindow::setCurrentPlayerIndex(int playerIndex)
{
    currentPlayerIndex = playerIndex;
    setTitle(juce::String("Mixer - Player ") + juce::String(playerIndex + 1));
    repaint();
}
