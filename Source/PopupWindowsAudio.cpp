#include "PopupWindowsAudio.h"
#include "INIConfig.h"

AudioSettingsTab::AudioSettingsTab(ColorScheme& colorScheme,
                                  ResponsiveLayoutManager& layoutManager,
                                  FontManager& fontManager,
                                  juce::AudioDeviceManager* deviceManager)
    : colorScheme(colorScheme), layoutManager(layoutManager),
      fontManager(fontManager), deviceManager(deviceManager) {
    setupComponents();
}

void AudioSettingsTab::setupComponents() {
    addAndMakeVisible(deviceLabel);
    addAndMakeVisible(sampleRateLabel);
    addAndMakeVisible(bufferSizeLabel);

    deviceLabel.setText("Audio Device:", juce::dontSendNotification);
    sampleRateLabel.setText("Sample Rate:", juce::dontSendNotification);
    bufferSizeLabel.setText("Buffer Size:", juce::dontSendNotification);

    for (auto* label : {&deviceLabel, &sampleRateLabel, &bufferSizeLabel}) {
        label->setColour(juce::Label::textColourId, colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
        label->setFont(fontManager.getFont(FontManager::FontRole::Body));
    }

    addAndMakeVisible(deviceCombo);
    addAndMakeVisible(sampleRateCombo);
    addAndMakeVisible(bufferSizeCombo);

    for (auto* combo : {&deviceCombo, &sampleRateCombo, &bufferSizeCombo}) {
        combo->addListener(this);
        combo->setColour(juce::ComboBox::backgroundColourId,
                        colorScheme.getColor(ColorScheme::ColorRole::ControlBackground));
        combo->setColour(juce::ComboBox::textColourId,
                        colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    }

    #if JUCE_MAC || JUCE_IOS
        sampleRateCombo.addItem("44100 Hz", 1);
        sampleRateCombo.addItem("48000 Hz", 2);
        sampleRateCombo.addItem("88200 Hz", 3);
        sampleRateCombo.addItem("96000 Hz", 4);
        sampleRateCombo.addItem("176400 Hz", 5);
        sampleRateCombo.addItem("192000 Hz", 6);
    #elif JUCE_WINDOWS
        sampleRateCombo.addItem("44100 Hz", 1);
        sampleRateCombo.addItem("48000 Hz", 2);
        sampleRateCombo.addItem("88200 Hz", 3);
        sampleRateCombo.addItem("96000 Hz", 4);
    #elif JUCE_LINUX
        sampleRateCombo.addItem("44100 Hz", 1);
        sampleRateCombo.addItem("48000 Hz", 2);
        sampleRateCombo.addItem("96000 Hz", 3);
    #elif JUCE_ANDROID
        sampleRateCombo.addItem("44100 Hz", 1);
        sampleRateCombo.addItem("48000 Hz", 2);
    #else
        sampleRateCombo.addItem("44100 Hz", 1);
        sampleRateCombo.addItem("48000 Hz", 2);
        sampleRateCombo.addItem("88200 Hz", 3);
        sampleRateCombo.addItem("96000 Hz", 4);
        sampleRateCombo.addItem("192000 Hz", 5);
    #endif

    #if JUCE_MAC || JUCE_IOS
        bufferSizeCombo.addItem("64 samples", 1);
        bufferSizeCombo.addItem("128 samples", 2);
        bufferSizeCombo.addItem("256 samples", 3);
        bufferSizeCombo.addItem("512 samples", 4);
    #elif JUCE_WINDOWS
        bufferSizeCombo.addItem("128 samples", 1);
        bufferSizeCombo.addItem("256 samples", 2);
        bufferSizeCombo.addItem("512 samples", 3);
        bufferSizeCombo.addItem("1024 samples", 4);
    #elif JUCE_LINUX
        bufferSizeCombo.addItem("256 samples", 1);
        bufferSizeCombo.addItem("512 samples", 2);
        bufferSizeCombo.addItem("1024 samples", 3);
        bufferSizeCombo.addItem("2048 samples", 4);
    #elif JUCE_ANDROID
        bufferSizeCombo.addItem("512 samples", 1);
        bufferSizeCombo.addItem("1024 samples", 2);
        bufferSizeCombo.addItem("2048 samples", 3);
    #else
        bufferSizeCombo.addItem("32 samples", 1);
        bufferSizeCombo.addItem("64 samples", 2);
        bufferSizeCombo.addItem("128 samples", 3);
        bufferSizeCombo.addItem("256 samples", 4);
        bufferSizeCombo.addItem("512 samples", 5);
        bufferSizeCombo.addItem("1024 samples", 6);
        bufferSizeCombo.addItem("2048 samples", 7);
    #endif

    addAndMakeVisible(asioToggle);
    addAndMakeVisible(multiCoreToggle);

    asioToggle.setButtonText("Enable ASIO");
    multiCoreToggle.setButtonText("Multi-core Processing");

    for (auto* toggle : {&asioToggle, &multiCoreToggle}) {
        toggle->addListener(this);
        toggle->setColour(juce::ToggleButton::textColourId,
                         colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    }

    addAndMakeVisible(testAudioButton);
    testAudioButton.setButtonText("Test Audio");
    testAudioButton.addListener(this);

    addAndMakeVisible(statusLabel);
    statusLabel.setColour(juce::Label::textColourId,
                         colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
    statusLabel.setJustificationType(juce::Justification::centred);

    updateDeviceList();
}

void AudioSettingsTab::updateDeviceList() {
    deviceCombo.clear();

    if (deviceManager) {
        auto& types = deviceManager->getAvailableDeviceTypes();

        int itemId = 1;
        for (auto* type : types) {
            auto deviceNames = type->getDeviceNames();
            for (const auto& name : deviceNames) {
                deviceCombo.addItem(name, itemId++);
            }
        }

        if (auto* currentDevice = deviceManager->getCurrentAudioDevice()) {
            deviceCombo.setText(currentDevice->getName());
        }
    } else {
        deviceCombo.addItem("Default Audio Device", 1);
        deviceCombo.setSelectedId(1);
    }
}

void AudioSettingsTab::resized() {
    auto bounds = getLocalBounds().reduced(layoutManager.scaled(INIConfig::LayoutConstants::defaultMargin));

    auto rowHeight = layoutManager.scaled(INIConfig::LayoutConstants::audioTabRowHeight);
    auto labelWidth = layoutManager.scaled(INIConfig::LayoutConstants::audioTabLabelWidth);
    auto spacing = layoutManager.scaled(INIConfig::LayoutConstants::defaultSpacing);

    auto row = bounds.removeFromTop(rowHeight);
    deviceLabel.setBounds(row.removeFromLeft(labelWidth));
    row.removeFromLeft(spacing);
    deviceCombo.setBounds(row);

    bounds.removeFromTop(spacing);

    row = bounds.removeFromTop(rowHeight);
    sampleRateLabel.setBounds(row.removeFromLeft(labelWidth));
    row.removeFromLeft(spacing);
    sampleRateCombo.setBounds(row.removeFromLeft(layoutManager.scaled(INIConfig::LayoutConstants::audioTabComboWidth)));

    bounds.removeFromTop(spacing);

    row = bounds.removeFromTop(rowHeight);
    bufferSizeLabel.setBounds(row.removeFromLeft(labelWidth));
    row.removeFromLeft(spacing);
    bufferSizeCombo.setBounds(row.removeFromLeft(layoutManager.scaled(INIConfig::LayoutConstants::audioTabComboWidth)));

    bounds.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::defaultMargin));

    asioToggle.setBounds(bounds.removeFromTop(rowHeight));
    bounds.removeFromTop(spacing);
    multiCoreToggle.setBounds(bounds.removeFromTop(rowHeight));

    bounds.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::dialogRowHeight));

    testAudioButton.setBounds(bounds.removeFromTop(rowHeight)
                             .withSizeKeepingCentre(layoutManager.scaled(INIConfig::LayoutConstants::audioTabButtonWidth), rowHeight));

    bounds.removeFromTop(spacing);

    statusLabel.setBounds(bounds.removeFromTop(rowHeight));
}

void AudioSettingsTab::comboBoxChanged(juce::ComboBox* comboBox) {
    if (comboBox == &deviceCombo && deviceManager) {
        auto selectedDevice = deviceCombo.getText();
        statusLabel.setText("Device selected: " + selectedDevice, juce::dontSendNotification);
    }
    else if (comboBox == &sampleRateCombo) {
        int sampleRates[] = {44100, 48000, 88200, 96000, 192000};
        int index = sampleRateCombo.getSelectedId() - 1;
        if (index >= 0 && index < 5) {
            statusLabel.setText("Sample rate: " + juce::String(sampleRates[index]) + " Hz",
                              juce::dontSendNotification);
        }
    }
    else if (comboBox == &bufferSizeCombo) {
        int bufferSizes[] = {32, 64, 128, 256, 512, 1024, 2048};
        int index = bufferSizeCombo.getSelectedId() - 1;
        if (index >= 0 && index < 7) {
            statusLabel.setText("Buffer size: " + juce::String(bufferSizes[index]) + " samples",
                              juce::dontSendNotification);
        }
    }
}

void AudioSettingsTab::buttonClicked(juce::Button* button) {
    if (button == &testAudioButton) {
        statusLabel.setText("Testing audio...", juce::dontSendNotification);
    }
}

void AudioSettingsTab::saveToAudioSettings(AudioSettings& settings) {
    settings.audioDeviceName = deviceCombo.getText();

    int sampleRates[] = {44100, 48000, 88200, 96000, 192000};
    int srIndex = sampleRateCombo.getSelectedId() - 1;
    if (srIndex >= 0 && srIndex < 5) {
        settings.sampleRate = sampleRates[srIndex];
    }

    int bufferSizes[] = {32, 64, 128, 256, 512, 1024, 2048};
    int bsIndex = bufferSizeCombo.getSelectedId() - 1;
    if (bsIndex >= 0 && bsIndex < 7) {
        settings.bufferSize = bufferSizes[bsIndex];
    }

    settings.enableASIO = asioToggle.getToggleState();
    settings.enableMultiCore = multiCoreToggle.getToggleState();
}

void AudioSettingsTab::loadFromAudioSettings(const AudioSettings& settings) {
    deviceCombo.setText(settings.audioDeviceName);

    switch (settings.sampleRate) {
        case 44100: sampleRateCombo.setSelectedId(1); break;
        case 48000: sampleRateCombo.setSelectedId(2); break;
        case 88200: sampleRateCombo.setSelectedId(3); break;
        case 96000: sampleRateCombo.setSelectedId(4); break;
        case 192000: sampleRateCombo.setSelectedId(5); break;
        default: sampleRateCombo.setSelectedId(2); break;
    }

    switch (settings.bufferSize) {
        case 32: bufferSizeCombo.setSelectedId(1); break;
        case 64: bufferSizeCombo.setSelectedId(2); break;
        case 128: bufferSizeCombo.setSelectedId(3); break;
        case 256: bufferSizeCombo.setSelectedId(4); break;
        case 512: bufferSizeCombo.setSelectedId(5); break;
        case 1024: bufferSizeCombo.setSelectedId(6); break;
        case 2048: bufferSizeCombo.setSelectedId(7); break;
        default: bufferSizeCombo.setSelectedId(4); break;
    }

    asioToggle.setToggleState(settings.enableASIO, juce::dontSendNotification);
    multiCoreToggle.setToggleState(settings.enableMultiCore, juce::dontSendNotification);
}
