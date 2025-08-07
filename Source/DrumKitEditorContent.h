#pragma once
#include <JuceHeader.h>
#include "SFZEngine.h"
#include "ColorScheme.h"
#include "FontManager.h"
#include "ResponsiveLayoutManager.h"
#include "INIDataManager.h"
#include "ComponentState.h"
#include "INIConfig.h"

class DrumKitWaveformDisplay : public juce::Component,
                              public juce::FileDragAndDropTarget,
                              public juce::ChangeListener
{
public:
    DrumKitWaveformDisplay();
    ~DrumKitWaveformDisplay() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void loadAudioFile(const juce::File& file);
    void clearWaveform();

    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;

    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    void setStartPosition(float position);
    void setEndPosition(float position);
    float getStartPosition() const { return startPosition; }
    float getEndPosition() const { return endPosition; }

    std::function<void(const juce::File&)> onFileDropped;
    std::function<void(float, float)> onSelectionChanged;

private:
    juce::AudioFormatManager formatManager;
    juce::AudioThumbnailCache thumbnailCache{INIConfig::Audio::WAVEFORM_THUMBNAIL_CACHE_SIZE};
    std::unique_ptr<juce::AudioThumbnail> thumbnail;

    float startPosition = INIConfig::Audio::DEFAULT_START_POSITION;
    float endPosition = INIConfig::Audio::DEFAULT_END_POSITION;
    bool isDraggingStart = false;
    bool isDraggingEnd = false;

    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;

    float getPositionFromX(int x) const;
    int getXFromPosition(float position) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DrumKitWaveformDisplay)
};

class VelocityCurveEditor : public juce::Component
{
public:
    VelocityCurveEditor();

    void paint(juce::Graphics& g) override;
    void resized() override;

    void setCurveType(int type);
    void setCurveAmount(float amount);

    std::function<void(int, float)> onCurveChanged;

private:
    enum CurveType {
        Linear = INIConfig::Defaults::ZERO_VALUE,
        Exponential = INIConfig::Defaults::ONE_VALUE,
        Logarithmic = INIConfig::Defaults::ONE_VALUE + INIConfig::Defaults::ONE_VALUE,
        SCurve = INIConfig::Defaults::ONE_VALUE + INIConfig::Defaults::ONE_VALUE + INIConfig::Defaults::ONE_VALUE
    };
    int curveType = Linear;
    float curveAmount = INIConfig::Audio::DEFAULT_CURVE_AMOUNT;
    juce::Path curvePath;

    void updateCurvePath();
    float applyCurve(float input) const;

    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VelocityCurveEditor)
};

class PadEditor : public juce::Component,
                  public juce::FileDragAndDropTarget
{
public:
    PadEditor(int padNumber);

    void paint(juce::Graphics& g) override;
    void resized() override;

    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;

    void setSample(const juce::File& file);
    void clearSample();
    bool hasSample() const { return currentSampleFile.existsAsFile(); }

    void setSelected(bool selected);
    bool isSelected() const { return isSelectedPad; }

    int getPadNumber() const { return padNumber; }
    juce::File getSampleFile() const { return currentSampleFile; }

    std::function<void(int, const juce::File&)> onSampleAssigned;
    std::function<void(int)> onPadSelected;

private:
    int padNumber;
    juce::File currentSampleFile;
    bool isSelectedPad = false;
    bool isHovering = false;

    void mouseDown(const juce::MouseEvent& e) override;
    void mouseEnter(const juce::MouseEvent& e) override;
    void mouseExit(const juce::MouseEvent& e) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PadEditor)
};

class SampleEditControls : public juce::Component,
                          public juce::Slider::Listener,
                          public juce::ComboBox::Listener
{
public:
    SampleEditControls();

    void resized() override;

    void sliderValueChanged(juce::Slider* slider) override;
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;

    void updateControlsForPad(int padNumber);
    void setSampleParameters(float start, float end, float pitch, int filterType, float filterFreq, float filterRes);

    struct SampleParameters {
        float startPosition = INIConfig::Audio::DEFAULT_START_POSITION;
        float endPosition = INIConfig::Audio::DEFAULT_END_POSITION;
        float pitchShift = INIConfig::Audio::DEFAULT_PITCH_SHIFT;
        int filterType = INIConfig::Audio::DEFAULT_FILTER_TYPE;
        float filterFrequency = INIConfig::Audio::DEFAULT_FILTER_FREQUENCY;
        float filterResonance = INIConfig::Audio::DEFAULT_FILTER_RESONANCE;
        bool reverse = false;
        bool normalize = false;
        int chokeGroup = INIConfig::Audio::DEFAULT_CHOKE_GROUP;
    };

    SampleParameters getParameters() const { return currentParameters; }

    std::function<void(const SampleParameters&)> onParametersChanged;

private:
    SampleParameters currentParameters;

    juce::Label startLabel, endLabel, pitchLabel, filterLabel, chokeLabel;
    juce::Slider startSlider, endSlider, pitchSlider, filterFreqSlider, filterResSlider;
    juce::ComboBox filterTypeCombo, chokeGroupCombo;
    juce::ToggleButton reverseButton, normalizeButton;

    void setupControls();
    void updateParametersAndNotify();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SampleEditControls)
};

class SampleBrowser : public juce::Component,
                     public juce::FileBrowserListener
{
public:
    SampleBrowser();
    ~SampleBrowser() override;

    void resized() override;

    void selectionChanged() override;
    void fileClicked(const juce::File& file, const juce::MouseEvent& e) override;
    void fileDoubleClicked(const juce::File& file) override;
    void browserRootChanged(const juce::File& newRoot) override {}

    std::function<void(const juce::File&)> onFileSelected;
    std::function<void(const juce::File&)> onFileDoubleClicked;

private:
    juce::WildcardFileFilter fileFilter;
    std::unique_ptr<juce::FileBrowserComponent> fileBrowser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SampleBrowser)
};

class DrumKitEditorContent : public juce::Component,
                            public juce::Button::Listener
{
public:
    DrumKitEditorContent(SFZEngine& sfzEngine,
                        ColorScheme& colorScheme,
                        FontManager& fontManager,
                        ResponsiveLayoutManager& layoutManager,
                        INIDataManager& iniManager);
    ~DrumKitEditorContent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void buttonClicked(juce::Button* button) override;

    void saveKit();
    void loadKit();
    void newKit();

private:
    SFZEngine& sfzEngine;
    ColorScheme& colorScheme;
    FontManager& fontManager;
    ResponsiveLayoutManager& layoutManager;
    INIDataManager& iniManager;

    std::unique_ptr<SampleBrowser> sampleBrowser;
    std::unique_ptr<DrumKitWaveformDisplay> waveformDisplay;
    std::unique_ptr<VelocityCurveEditor> velocityCurveEditor;
    std::unique_ptr<SampleEditControls> sampleEditControls;
    juce::OwnedArray<PadEditor> padEditors;

    juce::TextButton newKitButton, saveKitButton, loadKitButton;
    juce::Label kitNameLabel;
    juce::TextEditor kitNameEditor;

    int selectedPadIndex = INIConfig::MIDI::INACTIVE_PATTERN;

    struct PadSampleData {
        juce::File sampleFile;
        SampleEditControls::SampleParameters parameters;
        float velocityCurve = INIConfig::Audio::DEFAULT_CURVE_AMOUNT;
        int velocityCurveType = INIConfig::Defaults::ZERO_VALUE;
    };

    PadSampleData padData[INIConfig::Audio::NUM_DRUM_PADS];

    void setupComponents();
    void setupMainButtons();
    void connectCallbacks();
    void handlePadSelection(int padNumber);
    void handleSampleAssignment(int padNumber, const juce::File& file);
    void handleSampleParameterChange(const SampleEditControls::SampleParameters& params);
    void updateWaveformForPad(int padNumber);

    void exportKitAsSFZ(const juce::File& destination);
    void importKitFromSFZ(const juce::File& source);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DrumKitEditorContent)
};
