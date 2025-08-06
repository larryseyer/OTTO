/**
 * @file Row1Component.cpp
 * @brief Implementation of Row1Component - TopBar functionality in row-based architecture
 *
 * This file implements the Row1Component class, which provides identical functionality
 * to TopBarComponent but within the new row-based architecture. It contains all
 * transport controls, settings, presets, and branding elements from the original TopBar.
 *
 * @author OTTO Development Team
 * @version 2.0
 * @date 2024
 */

#include "JUCE8_CODING_STANDARDS.h"
#include "Row1Component.h"
#include "MidiEngine.h"
#include "INIConfig.h"
#include "INIDataManager.h"
#include "UI/Themes/ThemeManager.h"
#include "ResponsiveLayoutManager.h"
#include "PopupWindows.h"
#include "CustomLookAndFeel.h"

// OTTOSplashOverlay class for splash screen display
class OTTOSplashOverlay : public juce::Component, private juce::Timer
{
public:
    OTTOSplashOverlay(const juce::Image& image, float displayTime,
                      std::function<void()> onComplete,
                      const juce::Colour& backgroundColor)
        : splashImage(image), totalTime(displayTime),
          completionCallback(onComplete), bgColor(backgroundColor)
    {
        setOpaque(true);
        startTimer(50);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(bgColor);
        g.setOpacity(opacity);

        auto bounds = getLocalBounds();
        int imgX = (bounds.getWidth() - splashImage.getWidth()) / 2;
        int imgY = (bounds.getHeight() - splashImage.getHeight()) / 2;

        g.drawImageAt(splashImage, imgX, imgY);
    }

    void timerCallback() override
    {
        elapsed += 0.05f;

        if (elapsed >= totalTime)
        {
            overallOpacity -= 0.1f;
            if (overallOpacity <= 0.0f)
            {
                stopTimer();
                setVisible(false);
                if (completionCallback)
                    completionCallback();
                if (auto* parent = getParentComponent())
                {
                    parent->removeChildComponent(this);
                }
            }
            else
            {
                setAlpha(overallOpacity);
            }
            repaint();
        }
    }

private:
    juce::Image splashImage;
    float totalTime;
    float elapsed = 0.0f;
    float opacity = 1.0f;
    float overallOpacity = 1.0f;
    std::function<void()> completionCallback;
    juce::Colour bgColor;
};

Row1Component::Row1Component(MidiEngine& midiEngine,
                           juce::AudioProcessorValueTreeState& valueTreeState,
                           ResponsiveLayoutManager& layoutManager,
                           FontManager& fontManager,
                           ColorScheme& colorScheme)
    : ResponsiveComponent(),
      midiEngine(midiEngine), valueTreeState(valueTreeState),
      layoutManager(layoutManager), fontManager(fontManager), colorScheme(colorScheme),
      
      gearButton("gear"),
      linkButton("link"),
      cloudButton("cloud"),
      playButton("play"),
      pauseButton("pause"),
      leftChevronButton("left"),
      rightChevronButton("right"),
      recordButton("record"),
      tapTempoButton("metronome"),
      overdubButton("stack-plus"),
      loopButton("repeat"),
      
      presetsMenu(),
      presetDisplayLabel(),
      bpmLabel("bpm_label", INIConfig::Validation::MIN_TEMPO, INIConfig::Validation::MAX_TEMPO),
      ottoLabel(),
      versionLabel(),
      clockSyncLabel(),
      tapTempoLabel(),
      bottomSeparator(colorScheme),
      
      // PHASE 9D: Theme selector components
      themeButton("palette", FontManager::PhosphorWeight::Regular),
      themeSelector() {
    
    setupTopBarComponents();
    
    // PHASE 9D: Initialize theme selector
    setupThemeSelector();
}

void Row1Component::setupTopBarComponents() {
    addAndMakeVisible(gearButton);
    addAndMakeVisible(linkButton);
    addAndMakeVisible(cloudButton);
    addAndMakeVisible(playButton);
    addAndMakeVisible(pauseButton);
    addAndMakeVisible(leftChevronButton);
    addAndMakeVisible(rightChevronButton);
    addAndMakeVisible(presetsMenu);
    addAndMakeVisible(presetDisplayLabel);
    addAndMakeVisible(bpmLabel);
    addAndMakeVisible(ottoLabel);
    addAndMakeVisible(versionLabel);
    addAndMakeVisible(clockSyncLabel);
    addAndMakeVisible(bottomSeparator);
    addAndMakeVisible(recordButton);
    addAndMakeVisible(tapTempoButton);
    addAndMakeVisible(overdubButton);
    addAndMakeVisible(loopButton);
    
    // PHASE 9D: Theme selector components
    addAndMakeVisible(themeButton);
    addChildComponent(themeSelector);  // Hidden initially
    addAndMakeVisible(tapTempoLabel);

    gearButton.onClick = [this] {
        if (onGearButtonClicked) {
            onGearButtonClicked();
        }
        showSettingsWindow();
    };

    linkButton.onClick = [this] {
        showLinkSyncMenu();
    };

    cloudButton.onClick = [this] {
        showCloudMenu();
    };

    pauseButton.setVisible(false);
    playButton.onClick = [this] {
        setPlayState(true);
    };
    pauseButton.onClick = [this] {
        setPlayState(false);
    };

    recordButton.onClick = [this] {
        setLiveRecording(!liveRecording);
    };

    tapTempoButton.onClick = [this] {
        tapTempo();
    };

    overdubButton.onClick = [this] {
        setOverdubEnabled(!overdubEnabled);
    };

    // PHASE 9D: Theme button callback
    themeButton.onClick = [this] {
        toggleThemeSelector();
    };

    loopButton.onClick = [this] {
        setLoopRecordingEnabled(!loopRecordingEnabled);
    };

    leftChevronButton.onClick = [this] { handlePresetChevrons(false); };
    rightChevronButton.onClick = [this] { handlePresetChevrons(true); };

    bpmLabel.setValue(INIConfig::Defaults::DEFAULT_TEMPO);
    bpmLabel.onValueChanged = [this](float newTempo) {
        float clampedTempo = INIConfig::clampTempo(newTempo);
        midiEngine.setTempo(clampedTempo);
        if (auto* param = valueTreeState.getRawParameterValue("tempo")) {
            *param = clampedTempo;
        }
        if (onTempoChanged) {
            onTempoChanged(clampedTempo);
        }
        notifyStateChanged();
    };

    ottoLabel.setComponentID("otto_label");
    ottoLabel.setText("OTTO", juce::dontSendNotification);
    ottoLabel.setJustificationType(juce::Justification::centred);
    ottoLabel.setFont(fontManager.getFont(FontManager::FontRole::Header,
                      layoutManager.scaled(INIConfig::LayoutConstants::Row1::ottoHeight * 0.6f)));
    ottoLabel.setColour(juce::Label::textColourId, colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    ottoLabel.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    ottoLabel.addMouseListener(this, false);

    versionLabel.setComponentID("version_label");
    versionLabel.setText("Ver. 1.0", juce::dontSendNotification);
    versionLabel.setJustificationType(juce::Justification::centred);
    versionLabel.setFont(fontManager.getFont(FontManager::FontRole::Body,
                         layoutManager.scaled(INIConfig::LayoutConstants::Row1::versionHeight * 0.8f)));
    versionLabel.setColour(juce::Label::textColourId, colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));

    clockSyncLabel.setComponentID("clock_sync_label");
    clockSyncLabel.setText("", juce::dontSendNotification);
    clockSyncLabel.setJustificationType(juce::Justification::centred);
    clockSyncLabel.setFont(fontManager.getFont(FontManager::FontRole::Body,
                           layoutManager.scaled(INIConfig::LayoutConstants::Row1::clockSyncHeight * 0.8f)));
    clockSyncLabel.setColour(juce::Label::textColourId, colorScheme.getColor(ColorScheme::ColorRole::Accent));
    clockSyncLabel.setVisible(false);

    tapTempoLabel.setComponentID("tap_tempo_label");
    tapTempoLabel.setText("TAP", juce::dontSendNotification);
    tapTempoLabel.setJustificationType(juce::Justification::centred);
    tapTempoLabel.setFont(fontManager.getFont(FontManager::FontRole::Body,
                          layoutManager.scaled(INIConfig::LayoutConstants::Row1::clockSyncHeight * 0.8f)));
    tapTempoLabel.setColour(juce::Label::textColourId, colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
    tapTempoLabel.setVisible(false);

    presetDisplayLabel.setComponentID("preset_display_label");
    presetDisplayLabel.setText(currentPresetName, juce::dontSendNotification);
    presetDisplayLabel.setJustificationType(juce::Justification::centred);
    presetDisplayLabel.setFont(fontManager.getFont(FontManager::FontRole::Header,
                               layoutManager.scaled(INIConfig::LayoutConstants::Row1::ottoHeight * INIConfig::LayoutConstants::fontSizePresetLabelReduced)));
    presetDisplayLabel.setColour(juce::Label::textColourId,
                                colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));

    presetDisplayLabel.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    presetDisplayLabel.addMouseListener(this, false);

    presetsMenu.setComponentID("presets_menu");
    presetsMenu.setJustificationType(juce::Justification::left);

    setupPresets();
    presetsMenu.setTextWhenNothingSelected("Select App Preset...");
    presetsMenu.setTextWhenNoChoicesAvailable("No app presets found");

    updatePresetDisplayToggle();
    updateLinkButtonVisuals();
    updateRecordButton();
    updateCloudButtonVisuals();
}

void Row1Component::paint(juce::Graphics& g) {
    g.fillAll(colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground));
}

void Row1Component::resized() {
    ResponsiveComponent::resized(); // Call parent first
    
    auto bounds = getRowBounds();
    int currentWidth = bounds.getWidth();
    int currentHeight = bounds.getHeight();

    // Use responsive calculations instead of hardcoded values
    int iconSize = getResponsiveButtonSize();
    int spacing = getResponsiveSpacing();
    int margin = getResponsiveMargin(static_cast<int>(currentWidth * 0.015f));
    
    int iconY = (currentHeight - iconSize) / 2;

    // Update font sizes using responsive calculations
    float tempoFontSize = getResponsiveFontSize(static_cast<float>(iconSize) * 0.3f);
    bpmLabel.setFont(JUCE8_FONT(tempoFontSize));

    // Layout components using responsive spacing
    int gearX = margin;
    int linkX = gearX + iconSize + spacing;
    int cloudX = linkX + iconSize + spacing;

    gearButton.setBounds(gearX, iconY, iconSize, iconSize);
    linkButton.setBounds(linkX, iconY, iconSize, iconSize);
    cloudButton.setBounds(cloudX, iconY, iconSize, iconSize);

    int playX = (currentWidth - iconSize) / 2;
    int leftChevronX = cloudX + iconSize + spacing;

    // Preset menu area calculations using responsive sizing
    int totalPresetAreaSpace = playX - leftChevronX - spacing;
    int minPresetMenuWidth = static_cast<int>(currentWidth * 0.11f);
    int maxPresetMenuWidth = static_cast<int>(currentWidth * 0.16f);
    
    int chevronSpacing = getResponsiveSpacing();
    int availableMenuSpace = totalPresetAreaSpace - (2 * iconSize) - (2 * chevronSpacing);
    int presetMenuWidth = juce::jmax(minPresetMenuWidth, juce::jmin(maxPresetMenuWidth, availableMenuSpace));

    int totalPresetGroupWidth = iconSize + chevronSpacing + presetMenuWidth + chevronSpacing + iconSize;
    int presetGroupStartX = leftChevronX + (totalPresetAreaSpace - totalPresetGroupWidth) / 2;

    int centeredLeftChevronX = presetGroupStartX;
    int presetsMenuX = centeredLeftChevronX + iconSize + chevronSpacing;
    int rightChevronX = presetsMenuX + presetMenuWidth + chevronSpacing;

    if (rightChevronX + iconSize + spacing > playX) {
        rightChevronX = playX - spacing - iconSize;
        presetsMenuX = rightChevronX - chevronSpacing - presetMenuWidth;
        centeredLeftChevronX = presetsMenuX - chevronSpacing - iconSize;
    }

    leftChevronButton.setBounds(centeredLeftChevronX, iconY, iconSize, iconSize);
    presetsMenu.setBounds(presetsMenuX, iconY, presetMenuWidth, iconSize);
    presetDisplayLabel.setBounds(presetsMenuX, iconY, presetMenuWidth, iconSize);
    rightChevronButton.setBounds(rightChevronX, iconY, iconSize, iconSize);
    playButton.setBounds(playX, iconY, iconSize, iconSize);
    pauseButton.setBounds(playX, iconY, iconSize, iconSize);

    // Right side components using responsive sizing
    int ottoWidth = static_cast<int>(currentWidth * 0.1f);
    int ottoX = currentWidth - ottoWidth - margin;
    int ottoHeight = static_cast<int>(currentHeight * 0.6f);
    int versionHeight = static_cast<int>(currentHeight * 0.25f);

    ottoLabel.setBounds(ottoX, iconY, ottoWidth, ottoHeight);
    versionLabel.setBounds(ottoX, iconY + ottoHeight, ottoWidth, versionHeight);

    // BPM label positioning
    int bmpX = playX + iconSize + spacing;
    int bmpWidth = static_cast<int>(currentWidth * 0.08f);
    bmpWidth = juce::jmax(static_cast<int>(iconSize * 2.6f), bmpWidth);

    bpmLabel.setBounds(bmpX, iconY, bmpWidth, iconSize);
    clockSyncLabel.setBounds(bmpX, iconY + iconSize - static_cast<int>(currentHeight * 0.02f),
                           bmpWidth, static_cast<int>(currentHeight * 0.02f));

    // Right side buttons using responsive spacing
    int loopX = ottoX - margin - iconSize;
    int overdubX = loopX - iconSize - spacing;
    int recordX = overdubX - iconSize - spacing;
    int tapTempoX = recordX - iconSize - spacing;  // Position relative to record button
    int themeX = tapTempoX - iconSize - spacing;

    tapTempoButton.setBounds(tapTempoX, iconY, iconSize, iconSize);
    tapTempoLabel.setBounds(tapTempoX, iconY + iconSize - static_cast<int>(currentHeight * 0.02f),
                          iconSize, static_cast<int>(currentHeight * 0.02f));
    
    themeButton.setBounds(themeX, iconY, iconSize, iconSize);
    
    // Theme selector positioning
    if (themeSelectorVisible) {
        int selectorWidth = static_cast<int>(currentWidth * 0.12f);
        int selectorHeight = iconSize;
        int selectorX = themeX - (selectorWidth - iconSize) / 2;
        int selectorY = iconY + iconSize + spacing;
        
        themeSelector.setBounds(selectorX, selectorY, selectorWidth, selectorHeight);
    }
    
    recordButton.setBounds(recordX, iconY, iconSize, iconSize);
    overdubButton.setBounds(overdubX, iconY, iconSize, iconSize);
    loopButton.setBounds(loopX, iconY, iconSize, iconSize);

    // Bottom separator using responsive thickness
    int separatorThickness = juce::jmax(1, static_cast<int>(currentHeight * 0.05f));
    bottomSeparator.setBounds(0, currentHeight - separatorThickness, bounds.getWidth(), separatorThickness);
}

juce::Rectangle<int> Row1Component::getRowBounds() const {
    return juce::Rectangle<int>(0, INIConfig::LayoutConstants::Row1::yPosition,
                               layoutManager.getWindowWidth(), 
                               INIConfig::LayoutConstants::Row1::height);
}

void Row1Component::saveStates(ComponentState& state) {
    try {
        state.globalSettings.tempo = INIConfig::clampTempo(getTempo());
        state.globalSettings.presetID = INIConfig::clampPresetIndex(getPresetSelection());
        state.globalSettings.midiClockIn = midiClockInEnabled;
        state.globalSettings.midiClockOut = midiClockOutEnabled;

        state.playState = isPlaying;
        state.tempo = state.globalSettings.tempo;
        state.currentPreset = state.globalSettings.presetID;

        state.toggleStates[300] = liveRecording;
        state.toggleStates[301] = overdubEnabled;
        state.toggleStates[302] = loopRecordingEnabled;

        state.toggleStates[310] = cloudAuthenticated;
        state.toggleStates[311] = cloudAutoSync;
        state.toggleStates[312] = collaborationActive;
        state.dropdownSelections["cloudUsername"] = cloudAuthenticated ?
            cloudUsername.hashCode() : 0;
        state.dropdownSelections["collaborationSession"] = collaborationActive ?
            collaborationSessionName.hashCode() : 0;
    }
    catch (const std::exception& e) {
    }
}

void Row1Component::loadStates(const ComponentState& state) {
    try {
        float tempo = INIConfig::clampTempo(state.globalSettings.tempo);
        setTempo(tempo);
        midiEngine.setTempo(tempo);

        int presetIndex = INIConfig::clampPresetIndex(state.globalSettings.presetID);
        setPresetSelection(presetIndex);

        setPlayState(state.playState);

        setMidiClockInEnabled(state.globalSettings.midiClockIn);
        setMidiClockOutEnabled(state.globalSettings.midiClockOut);
        midiEngine.setReceiveMidiClock(state.globalSettings.midiClockIn);
        midiEngine.setSendMidiClock(state.globalSettings.midiClockOut);

        if (auto* param = valueTreeState.getRawParameterValue("tempo")) {
            *param = tempo;
        }

        if (state.toggleStates.count(300)) {
            setLiveRecording(state.toggleStates.at(300));
        }
        if (state.toggleStates.count(301)) {
            setOverdubEnabled(state.toggleStates.at(301));
        }
        if (state.toggleStates.count(302)) {
            setLoopRecordingEnabled(state.toggleStates.at(302));
        }

        if (state.toggleStates.count(310)) {
            cloudAuthenticated = state.toggleStates.at(310);
        }
        if (state.toggleStates.count(311)) {
            cloudAutoSync = state.toggleStates.at(311);
        }
        if (state.toggleStates.count(312)) {
            collaborationActive = state.toggleStates.at(312);
        }

        updateCloudButtonVisuals();
    }
    catch (const std::exception& e) {
        setTempo(INIConfig::Defaults::DEFAULT_TEMPO);
        setPresetSelection(INIConfig::Defaults::DEFAULT_CURRENT_PRESET);
        setPlayState(false);
    }
}

void Row1Component::updateFromState(const ComponentState& state) {
    loadStates(state);
}

int Row1Component::getPresetSelection() const {
    return getPresetIndexFromName(currentPresetName);
}

void Row1Component::setPresetSelection(int index) {
    auto allPresets = getAllPresetNames();

    if (index >= 0 && index < allPresets.size()) {
        currentPresetName = allPresets[index];
        presetsMenu.setText(currentPresetName);
        presetDisplayLabel.setText(currentPresetName, juce::dontSendNotification);
        showPresetLabel();
        notifyStateChanged();
    }
}

bool Row1Component::getPlayState() const {
    return isPlaying;
}

void Row1Component::setPlayState(bool playing) {
    if (isPlaying != playing) {
        isPlaying = playing;
        updatePlayButtons();
        if (onPlayStateChanged) {
            onPlayStateChanged(isPlaying);
        }
        notifyStateChanged();
    }
}

float Row1Component::getTempo() const {
    return bpmLabel.getValue();
}

void Row1Component::setTempo(float tempo) {
    float clampedTempo = INIConfig::clampTempo(tempo);
    bpmLabel.setValue(clampedTempo);
    notifyStateChanged();
}

void Row1Component::setMidiClockInEnabled(bool enabled) {
    midiClockInEnabled = enabled;
    updateLinkButtonVisuals();
}

void Row1Component::setMidiClockOutEnabled(bool enabled) {
    midiClockOutEnabled = enabled;
    updateLinkButtonVisuals();
}

bool Row1Component::isMidiClockInEnabled() const {
    return midiClockInEnabled;
}

bool Row1Component::isMidiClockOutEnabled() const {
    return midiClockOutEnabled;
}

void Row1Component::updateClockSyncStatus(bool isExternalSync, double externalTempo) {
    isExternalSyncActive = isExternalSync;

    if (isExternalSync && midiClockInEnabled) {
        clockSyncLabel.setText("Ext: " + juce::String(externalTempo, 1) + " BPM",
                             juce::dontSendNotification);
        clockSyncLabel.setVisible(true);
        bpmLabel.setEnabled(false);
    } else {
        clockSyncLabel.setVisible(false);
        bpmLabel.setEnabled(true);
    }
}

void Row1Component::setINIDataManager(INIDataManager* manager) {
    iniDataManager = manager;
}

juce::String Row1Component::getCurrentPresetName() const {
    return currentPresetName;
}

void Row1Component::tapTempo() {
    tapCount++;
    lastTapTime = juce::Time::getMillisecondCounterHiRes();

    midiEngine.tapTempo();

    float averagedTempo = midiEngine.getTapTempoAveraging();
    if (averagedTempo > 0) {
        setTempo(averagedTempo);
    }

    updateTapTempoDisplay();
}

bool Row1Component::isLiveRecording() const {
    return liveRecording;
}

void Row1Component::setLiveRecording(bool recording) {
    liveRecording = recording;
    midiEngine.startLiveRecording(overdubEnabled);
    updateRecordButton();
}

bool Row1Component::isOverdubEnabled() const {
    return overdubEnabled;
}

void Row1Component::setOverdubEnabled(bool enabled) {
    overdubEnabled = enabled;
    midiEngine.setOverdubMode(enabled);

    overdubButton.setToggleState(enabled, juce::dontSendNotification);
    overdubButton.repaint();
}

bool Row1Component::isLoopRecordingEnabled() const {
    return loopRecordingEnabled;
}

void Row1Component::setLoopRecordingEnabled(bool enabled) {
    loopRecordingEnabled = enabled;
    midiEngine.setLoopRecordingMode(enabled);

    loopButton.setToggleState(enabled, juce::dontSendNotification);
    loopButton.repaint();
}

void Row1Component::lookAndFeelChanged() {
    repaint();

    ottoLabel.setFont(fontManager.getFont(FontManager::FontRole::Header,
                      layoutManager.scaled(INIConfig::LayoutConstants::Row1::ottoHeight * 0.6f)));
    ottoLabel.setColour(juce::Label::textColourId, colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));

    versionLabel.setFont(fontManager.getFont(FontManager::FontRole::Body,
                         layoutManager.scaled(INIConfig::LayoutConstants::Row1::versionHeight * 0.8f)));
    versionLabel.setColour(juce::Label::textColourId, colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));

    clockSyncLabel.setFont(fontManager.getFont(FontManager::FontRole::Body,
                           layoutManager.scaled(INIConfig::LayoutConstants::Row1::clockSyncHeight * 0.8f)));
    clockSyncLabel.setColour(juce::Label::textColourId, colorScheme.getColor(ColorScheme::ColorRole::Accent));

    tapTempoLabel.setFont(fontManager.getFont(FontManager::FontRole::Body,
                          layoutManager.scaled(INIConfig::LayoutConstants::Row1::clockSyncHeight * 0.8f)));
    tapTempoLabel.setColour(juce::Label::textColourId, colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));

    presetDisplayLabel.setFont(fontManager.getFont(FontManager::FontRole::Header,
                               layoutManager.scaled(INIConfig::LayoutConstants::Row1::ottoHeight * INIConfig::LayoutConstants::fontSizePresetLabelReduced)));
    presetDisplayLabel.setColour(juce::Label::textColourId,
                                colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));

    bpmLabel.setColour(juce::Label::textColourId, colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));

    updateLinkButtonVisuals();
    updateRecordButton();
    updateCloudButtonVisuals();

    if (overdubEnabled) {
        overdubButton.setToggleState(true, juce::dontSendNotification);
    }
    if (loopRecordingEnabled) {
        loopButton.setToggleState(true, juce::dontSendNotification);
    }
}

void Row1Component::mouseDown(const juce::MouseEvent& event) {
    if (event.eventComponent == &presetDisplayLabel && showingPresetLabel) {
        togglePresetDisplay();
    } else if (event.eventComponent == &ottoLabel) {
        showSplashScreen();
    }
}

void Row1Component::refreshPresetLabelFont() {
    presetDisplayLabel.setFont(fontManager.getFont(FontManager::FontRole::Header,
                               layoutManager.scaled(INIConfig::LayoutConstants::Row1::ottoHeight * INIConfig::LayoutConstants::fontSizePresetLabelReduced)));
}

void Row1Component::setupPresets() {
    presetsMenu.setTextWhenNothingSelected("Select App Preset...");
    presetsMenu.setTextWhenNoChoicesAvailable("No app presets found");
    presetsMenu.setJustificationType(juce::Justification::left);

    presetsMenu.onPopupRequest = [this] {
        if (!showingPresetLabel) {
            // Menu is visible, clicking should show the popup
            buildHierarchicalPresetMenu();
        } else {
            // Label is visible, clicking should switch to menu mode
            showPresetMenu();
            buildHierarchicalPresetMenu();
        }
    };

    presetsMenu.onChange = [this]() {
        int selectedId = presetsMenu.getSelectedId();
        if (selectedId > 0) {
            handlePresetMenuSelection(selectedId);
            showPresetLabel();
        }
    };

    currentPresetName = "Default";
    presetsMenu.setText(currentPresetName);
}

void Row1Component::buildHierarchicalPresetMenu() {
    ensureDefaultPresetStructure();

    auto presetsDir = INIConfig::getPresetsDirectory();
    auto categoryDirs = presetsDir.findChildFiles(juce::File::findDirectories, false);
    if (categoryDirs.size() <= 1) {
        createSamplePresetStructure();
    }

    juce::PopupMenu mainMenu;

    presetMenuMapping.clear();
    int currentMenuId = 1;

    auto categories = getPresetCategoriesFromFilesystem();

    for (const auto& categoryName : categories) {
        juce::PopupMenu subMenu;

        auto presetsInCategory = getPresetsInCategory(categoryName);

        bool categoryContainsCurrentPreset = false;

        for (const auto& preset : presetsInCategory) {
            bool isCurrentSelection = (preset == currentPresetName);
            if (isCurrentSelection) {
                categoryContainsCurrentPreset = true;
            }

            juce::String presetCopy = preset;
            juce::String categoryCopy = categoryName;

            juce::String displayName = isCurrentSelection ?
                INIConfig::UI::MENU_SELECTION_INDICATOR + " " + presetCopy :
                INIConfig::UI::MENU_NON_SELECTION_PADDING + presetCopy;

            subMenu.addItem(currentMenuId, displayName, true, false);

            presetMenuMapping.add({presetCopy, categoryCopy, currentMenuId});
            currentMenuId++;
        }

        if (presetsInCategory.size() > 0) {
            juce::String categoryDisplayName = categoryContainsCurrentPreset ?
                INIConfig::UI::MENU_SELECTION_INDICATOR + " " + categoryName :
                INIConfig::UI::MENU_NON_SELECTION_PADDING + categoryName;

            mainMenu.addSubMenu(categoryDisplayName, subMenu);
        }
    }

    mainMenu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(&presetsMenu),
                          [this](int result) {
                              handlePresetMenuSelection(result);
                          });
}

void Row1Component::handlePresetMenuSelection(int result) {
    if (result == 0) {
        showPresetLabel();
        return;
    }

    for (const auto& mapping : presetMenuMapping) {
        if (mapping.menuId == result) {
            currentPresetName = mapping.presetName;
            presetsMenu.setText(currentPresetName);
            presetDisplayLabel.setText(currentPresetName, juce::dontSendNotification);

            showPresetLabel();

            int presetIndex = getPresetIndexFromName(currentPresetName);
            if (onPresetChanged && presetIndex >= 0) {
                onPresetChanged(presetIndex);
            }
            notifyStateChanged();
            break;
        }
    }
}

int Row1Component::getPresetIndexFromName(const juce::String& presetName) const {
    auto allPresets = getAllPresetNames();
    return allPresets.indexOf(presetName);
}

juce::StringArray Row1Component::getAllPresetNames() const {
    if (iniDataManager) {
        auto presets = iniDataManager->getAvailablePresetNames();
        if (presets.size() > 0) {
            return presets;
        }
    }

    return {"Default"};
}

juce::Array<juce::String> Row1Component::getPresetCategoriesFromFilesystem() const {
    juce::Array<juce::String> categories;

    auto presetsDir = INIConfig::getPresetsDirectory();
    if (!presetsDir.exists()) {
        categories.add("Defaults");
        return categories;
    }

    bool hasDefaults = false;

    for (auto& file : presetsDir.findChildFiles(juce::File::findDirectories, false)) {
        juce::String categoryName = file.getFileName();
        if (categoryName == "Defaults") {
            hasDefaults = true;
        } else {
            categories.add(categoryName);
        }
    }

    if (hasDefaults) {
        categories.insert(0, "Defaults");
    } else {
        categories.insert(0, "Defaults");
    }

    return categories;
}

juce::StringArray Row1Component::getPresetsInCategory(const juce::String& categoryName) const {
    juce::StringArray presets;

    auto presetsDir = INIConfig::getPresetsDirectory();
    auto categoryDir = presetsDir.getChildFile(categoryName);

    if (!categoryDir.exists()) {
        if (categoryName == "Defaults") {
            if (iniDataManager) {
                iniDataManager->createDefaultPreset();
                presets.add("Default");
            }
        }
        return presets;
    }

    for (auto& file : categoryDir.findChildFiles(juce::File::findFiles, false, "*.ini")) {
        juce::String presetName = file.getFileNameWithoutExtension();
        presets.add(presetName);
    }

    if (presets.isEmpty() && categoryName == "Defaults") {
        if (iniDataManager) {
            iniDataManager->createDefaultPreset();
            presets.add("Default");
        }
    }

    if (presets.contains("Default")) {
        presets.removeString("Default");
        presets.sort(false);
        presets.insert(0, "Default");
    } else {
        presets.sort(false);
    }

    return presets;
}

void Row1Component::ensureDefaultPresetStructure() const {
    auto presetsDir = INIConfig::getPresetsDirectory();

    if (!presetsDir.exists()) {
        presetsDir.createDirectory();
    }

    auto defaultCategoryDir = presetsDir.getChildFile("Defaults");
    if (!defaultCategoryDir.exists()) {
        defaultCategoryDir.createDirectory();
    }

    auto defaultPresetFile = defaultCategoryDir.getChildFile("Default.ini");
    if (!defaultPresetFile.existsAsFile() && iniDataManager) {
        iniDataManager->createDefaultPreset();
    }
}

void Row1Component::createSamplePresetStructure() const {
    if (!iniDataManager) return;

    auto presetsDir = INIConfig::getPresetsDirectory();

    struct SampleCategory {
        juce::String categoryName;
        juce::StringArray presets;
    };

    SampleCategory sampleCategories[] = {
        {"Basic", {"Acoustic", "Electronic"}},
        {"Vintage", {"Bathroom", "Blues", "Brush"}},
        {"Modern", {"Claps", "Funk", "Rock"}},
        {"Special", {"Noise Makers", "Percs", "Rods & Shakers", "Tamborine"}}
    };

    for (const auto& category : sampleCategories) {
        auto categoryDir = presetsDir.getChildFile(category.categoryName);
        if (!categoryDir.exists()) {
            categoryDir.createDirectory();
        }

        for (const auto& presetName : category.presets) {
            auto presetFile = categoryDir.getChildFile(presetName + ".ini");
            if (!presetFile.existsAsFile()) {
                ComponentState sampleState;
                sampleState.tempo = INIConfig::Defaults::DEFAULT_TEMPO;
                sampleState.sliderValues["masterVolume"] = INIConfig::Defaults::DEFAULT_MASTER_VOLUME;

                for (int i = 0; i < INIConfig::LayoutConstants::playerTabsCount; ++i) {
                    auto& player = sampleState.playerSettings[i];
                    player.enabled = (i < 4);
                    player.selectedDrumkit = (presetName.contains("Electronic")) ? "Electronic" : "Acoustic";
                    player.swingValue = INIConfig::Defaults::SWING;
                    player.energyValue = INIConfig::Defaults::ENERGY;
                    player.volume = INIConfig::Defaults::VOLUME;
                }

                iniDataManager->savePreset(presetName, sampleState);

                auto userDir = presetsDir.getChildFile("User");
                auto wrongFile = userDir.getChildFile(presetName + ".ini");
                if (wrongFile.existsAsFile() && category.categoryName != "User") {
                    wrongFile.moveFileTo(presetFile);
                }
            }
        }
    }
}

void Row1Component::handlePresetChevrons(bool isRight) {
    auto allPresets = getAllPresetNames();

    int currentIndex = allPresets.indexOf(currentPresetName);
    if (currentIndex >= 0) {
        if (isRight) {
            currentIndex = (currentIndex + 1) % allPresets.size();
        } else {
            currentIndex = (currentIndex - 1 + allPresets.size()) % allPresets.size();
        }

        currentPresetName = allPresets[currentIndex];
        presetsMenu.setText(currentPresetName);
        presetDisplayLabel.setText(currentPresetName, juce::dontSendNotification);

        showPresetLabel();

        if (onPresetChanged) {
            onPresetChanged(currentIndex);
        }
        notifyStateChanged();
    }
}

void Row1Component::updatePlayButtons() {
    playButton.setVisible(!isPlaying);
    pauseButton.setVisible(isPlaying);
}

void Row1Component::updateTapTempoDisplay() {
    tapTempoLabel.setVisible(true);
    tapTempoLabel.setText("TAP " + juce::String(tapCount), juce::dontSendNotification);

    juce::Timer::callAfterDelay(INIConfig::LayoutConstants::tapTempoDisplayMs, [this] {
        tapTempoLabel.setVisible(false);
    });
}

void Row1Component::togglePresetDisplay() {
    showingPresetLabel = !showingPresetLabel;
    updatePresetDisplayToggle();
}

void Row1Component::showPresetLabel() {
    showingPresetLabel = true;
    updatePresetDisplayToggle();
}

void Row1Component::showPresetMenu() {
    showingPresetLabel = false;
    updatePresetDisplayToggle();
}

void Row1Component::updatePresetDisplayToggle() {
    presetDisplayLabel.setVisible(showingPresetLabel);
    presetsMenu.setVisible(!showingPresetLabel);
}

void Row1Component::updateRecordButton() {
    recordButton.setToggleState(liveRecording, juce::dontSendNotification);
    recordButton.repaint();
}

void Row1Component::updateLinkButtonVisuals() {
    if (midiClockInEnabled || midiClockOutEnabled) {
        linkButton.setToggleState(true, juce::dontSendNotification);
    } else {
        linkButton.setToggleState(false, juce::dontSendNotification);
    }
    linkButton.repaint();
}

void Row1Component::showLinkSyncMenu() {
    juce::PopupMenu menu;
    menu.setLookAndFeel(&getLookAndFeel());

    menu.addItem("No Sync", !midiClockInEnabled && !midiClockOutEnabled, false, [this] {
        setMidiClockInEnabled(false);
        setMidiClockOutEnabled(false);
        midiEngine.setReceiveMidiClock(false);
        midiEngine.setSendMidiClock(false);
        updateLinkButtonVisuals();
        notifyStateChanged();
    });

    menu.addSeparator();

    menu.addItem("MIDI Clock In", midiClockInEnabled, false, [this] {
        setMidiClockInEnabled(!midiClockInEnabled);
        setMidiClockOutEnabled(false);
        midiEngine.setReceiveMidiClock(!midiClockInEnabled);
        midiEngine.setSendMidiClock(false);
        updateLinkButtonVisuals();
        notifyStateChanged();
    });

    menu.addItem("MIDI Clock Out", midiClockOutEnabled, false, [this] {
        setMidiClockInEnabled(false);
        setMidiClockOutEnabled(!midiClockOutEnabled);
        midiEngine.setReceiveMidiClock(false);
        midiEngine.setSendMidiClock(!midiClockOutEnabled);
        updateLinkButtonVisuals();
        notifyStateChanged();
    });

    menu.addSeparator();

    menu.addItem("MIDI Learn...", false, false, [this] {
        if (onLinkButtonClicked) {
            onLinkButtonClicked();
        }
    });

    menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(&linkButton));
}

void Row1Component::showCloudMenu() {
    juce::PopupMenu menu;
    menu.setLookAndFeel(&getLookAndFeel());

    bool isAuthenticated = cloudAuthenticated;

    if (!isAuthenticated) {
        menu.addItem("Sign In to Cloud...", true, false, [this] {
            showCloudAuthDialog();
        });
    } else {
        menu.addItem("Signed in as: " + cloudUsername, false, false, nullptr);
        menu.addItem("Sign Out", true, false, [this] {
            cloudAuthenticated = false;
            cloudUsername.clear();
            updateCloudButtonVisuals();
            notifyStateChanged();
        });
    }

    menu.addSeparator();

    menu.addItem("Browse Community Patterns", true, false, [this] {
        if (onCloudButtonClicked) {
            onCloudButtonClicked();
        }
    });

    menu.addItem("Share Current Pattern...", isAuthenticated && isPlaying, false, [this] {
        shareCurrentPattern();
    });

    menu.addItem("Share Drum Kit...", isAuthenticated, false, [this] {
        shareCurrentDrumKit();
    });

    menu.addSeparator();

    menu.addItem("Start Collaboration Session...", isAuthenticated, false, [this] {
        startCollaborationSession();
    });

    menu.addItem("Join Session...", isAuthenticated, false, [this] {
        joinCollaborationSession();
    });

    if (collaborationActive) {
        menu.addItem("Leave Session", true, false, [this] {
            leaveCollaborationSession();
        });
    }

    menu.addSeparator();

    menu.addItem("Backup to Cloud", isAuthenticated, false, [this] {
        backupToCloud();
    });

    menu.addItem("Restore from Cloud...", isAuthenticated, false, [this] {
        restoreFromCloud();
    });

    menu.addItem("Auto-sync", isAuthenticated, cloudAutoSync, [this] {
        cloudAutoSync = !cloudAutoSync;
        updateCloudButtonVisuals();
        notifyStateChanged();
    });

    menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(&cloudButton));
}

void Row1Component::updateCloudButtonVisuals() {
    if (cloudAuthenticated) {
        cloudButton.setToggleState(true, juce::dontSendNotification);
    } else {
        cloudButton.setToggleState(false, juce::dontSendNotification);
    }
    cloudButton.repaint();
}

void Row1Component::showCloudAuthDialog() {
}

void Row1Component::shareCurrentPattern() {
}

void Row1Component::shareCurrentDrumKit() {
}

void Row1Component::startCollaborationSession() {
}

void Row1Component::joinCollaborationSession() {
}

void Row1Component::leaveCollaborationSession() {
}

void Row1Component::backupToCloud() {
}

void Row1Component::restoreFromCloud() {
}

void Row1Component::showShareSuccessMessage() {
}

void Row1Component::showBackupSuccessMessage() {
}

void Row1Component::showCollaborationCode() {
}

juce::String Row1Component::generateShareCode() {
    return juce::String();
}

void Row1Component::notifyStateChanged() {
}

//==============================================================================
// PHASE 9D: Theme Management Implementation
//==============================================================================

void Row1Component::setThemeManager(ThemeManager* manager) {
    themeManager = manager;
    if (themeManager) {
        populateThemeSelector();
    }
}

void Row1Component::showThemeSelector(bool show) {
    themeSelectorVisible = show;
    themeSelector.setVisible(show);
    if (show) {
        updateThemeSelector();
    }
}

bool Row1Component::isThemeSelectorVisible() const {
    return themeSelectorVisible && themeSelector.isVisible();
}

void Row1Component::setupThemeSelector() {
    // Configure theme selector appearance
    themeSelector.setTextWhenNothingSelected("Select Theme");
    themeSelector.setJustificationType(juce::Justification::centredLeft);
    
    // Set up theme selector callback
    themeSelector.onChange = [this] {
        int selectedId = themeSelector.getSelectedId();
        if (selectedId > 0) {
            handleThemeSelection(selectedId);
        }
    };
    
    // Initially hidden
    themeSelector.setVisible(false);
}

void Row1Component::updateThemeSelector() {
    if (!themeManager) return;
    
    // Update current selection based on active theme
    juce::String currentTheme = themeManager->getCurrentThemeName();
    
    // Find and select current theme in dropdown
    for (int i = 1; i <= themeSelector.getNumItems(); ++i) {
        if (themeSelector.getItemText(i - 1) == currentTheme) {
            themeSelector.setSelectedId(i, juce::dontSendNotification);
            break;
        }
    }
}

void Row1Component::handleThemeSelection(int themeId) {
    if (!themeManager || themeId <= 0) return;
    
    // Get theme name from selector
    juce::String themeName = themeSelector.getItemText(themeId - 1);
    
    if (themeName.isNotEmpty()) {
        // Apply theme through ThemeManager
        themeManager->setCurrentTheme(themeName);
        
        // Hide selector after selection
        showThemeSelector(false);
    }
}

void Row1Component::toggleThemeSelector() {
    showThemeSelector(!isThemeSelectorVisible());
}

void Row1Component::populateThemeSelector() {
    if (!themeManager) return;
    
    themeSelector.clear();
    
    // Get available themes from ThemeManager
    auto availableThemes = themeManager->getAvailableThemes();
    
    int itemId = 1;
    for (const auto& themeName : availableThemes) {
        themeSelector.addItem(themeName, itemId++);
    }
    
    // Set current selection
    updateThemeSelector();
}

//==============================================================================
// ResponsiveComponent Implementation
//==============================================================================

void Row1Component::updateResponsiveLayout() {
    auto category = getCurrentDeviceCategory();
    
    // Device-specific adjustments
    switch (category) {
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::Mobile:
            // Mobile: Larger touch targets, simplified layout
            break;
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::Tablet:
            // Tablet: Medium touch targets, balanced layout
            break;
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::Desktop:
            // Desktop: Standard layout with mouse precision
            break;
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::LargeDesktop:
            // Large Desktop: Expanded layout, more information density
            break;
        default:
            break;
    }
    
    // Layout update will happen naturally through component hierarchy
    // Do not call resized() here as it creates infinite recursion
}

int Row1Component::getResponsiveButtonSize() const {
    auto category = getCurrentDeviceCategory();
    auto rules = getCurrentLayoutRules();
    
    // Base size from INI config
    int baseSize = static_cast<int>(getHeight() * 0.6f); // 60% of row height
    
    // Apply device-specific adjustments
    switch (category) {
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::Mobile:
            return juce::jmax(static_cast<int>(rules.sizing.minTouchTarget), baseSize);
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::Tablet:
            return juce::jmax(static_cast<int>(rules.sizing.minTouchTarget * 0.9f), baseSize);
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::Desktop:
            return juce::jmax(30, baseSize);
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::LargeDesktop:
            return juce::jmax(35, static_cast<int>(baseSize * 1.1f));
        default:
            return juce::jmax(30, baseSize);
    }
}

int Row1Component::getResponsiveSpacing() const {
    auto category = getCurrentDeviceCategory();
    auto rules = getCurrentLayoutRules();
    
    // Base spacing
    int baseSpacing = rules.spacing.defaultSpacing;
    
    // Apply device-specific adjustments
    switch (category) {
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::Mobile:
            return juce::jmax(8, baseSpacing);
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::Tablet:
            return juce::jmax(6, baseSpacing);
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::Desktop:
            return juce::jmax(4, baseSpacing);
        case OTTO::UI::Layout::BreakpointManager::DeviceCategory::LargeDesktop:
            return juce::jmax(5, baseSpacing);
        default:
            return baseSpacing;
    }
}

float Row1Component::getResponsiveFontSize(float baseSize) const {
    return ResponsiveComponent::getResponsiveFontSize(baseSize);
}

void Row1Component::showSettingsWindow() {
    // Check if iniDataManager is valid before proceeding
    if (iniDataManager == nullptr) {
        juce::AlertWindow::showMessageBoxAsync(
            juce::MessageBoxIconType::WarningIcon,
            "Settings Error",
            "Settings manager is not available. Please try again later.",
            "OK"
        );
        return;
    }
    
    // Create a modal dialog window to contain the settings panel
    auto settingsPanel = std::make_unique<SettingsPanelWindow>(
        fontManager, 
        colorScheme, 
        layoutManager, 
        *iniDataManager,
        &midiEngine
    );
    
    // Set proper size using layout manager scaling
    int settingsWidth = layoutManager.scaled(800);
    int settingsHeight = layoutManager.scaled(600);
    settingsPanel->setSize(settingsWidth, settingsHeight);
    
    // Create a dialog window to hold the settings panel
    juce::DialogWindow::LaunchOptions options;
    options.content.setOwned(settingsPanel.release());
    options.dialogTitle = "OTTO Settings";
    options.dialogBackgroundColour = colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground);
    options.escapeKeyTriggersCloseButton = true;
    options.useNativeTitleBar = true;
    options.resizable = false;
    options.useBottomRightCornerResizer = false;
    
    // Center the dialog on screen
    options.content->centreWithSize(settingsWidth, settingsHeight);
    
    options.launchAsync();
}

void Row1Component::showSplashScreen() {
    // Get the splash screen image from CustomLookAndFeel (same as app startup)
    auto* customLookAndFeel = dynamic_cast<CustomLookAndFeel*>(&getLookAndFeel());
    juce::Image splashImage;
    
    if (customLookAndFeel) {
        splashImage = customLookAndFeel->getSplashImage();
    }
    
    if (splashImage.isValid()) {
        // Get the top-level component (main window)
        auto* topLevelComponent = getTopLevelComponent();
        if (topLevelComponent) {
            // Use the same splash screen display time as startup (3 seconds)
            float splashTime = 3.0f;
            
            // Get background color from color scheme
            juce::Colour appBgColor = colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground);
            
            // Create the splash overlay using the same approach as startup
            auto splashOverlay = std::make_unique<OTTOSplashOverlay>(
                splashImage,
                splashTime,
                [topLevelComponent](){ 
                    // Cleanup callback - the overlay removes itself
                    topLevelComponent->repaint(); 
                },
                appBgColor
            );
            
            // Add to top-level component and show
            splashOverlay->setBounds(topLevelComponent->getLocalBounds());
            topLevelComponent->addAndMakeVisible(splashOverlay.get());
            splashOverlay->toFront(false);
            
            // Release ownership - the overlay will clean itself up
            splashOverlay.release();
        }
    } else {
        // Fallback to text message if image not found
        juce::AlertWindow::showMessageBoxAsync(
            juce::MessageBoxIconType::InfoIcon,
            "OTTO Drum Machine",
            "OTTO v1.0\n\nAdvanced Drum Machine & Sequencer\n\nBuilt with JUCE 8",
            "OK"
        );
    }
}
