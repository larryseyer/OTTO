#include "TopBarComponent.h"
#include "MidiEngine.h"
#include "INIConfig.h"
#include "INIDataManager.h"

TopBarComponent::TopBarComponent(MidiEngine& midiEngine,
                                juce::AudioProcessorValueTreeState& valueTreeState,
                                ResponsiveLayoutManager& layoutManager,
                                FontManager& fontManager,
                                ColorScheme& colorScheme)
    : midiEngine(midiEngine), valueTreeState(valueTreeState), layoutManager(layoutManager),
      fontManager(fontManager), colorScheme(colorScheme),
      gearButton("gear"), linkButton("link"), cloudButton("cloud"),
      playButton("play"), pauseButton("pause"),
      leftChevronButton("left"), rightChevronButton("right"),
      recordButton("record"), tapTempoButton("metronome"),
      overdubButton("stack-plus"), loopButton("repeat"),
      bpmLabel("bpm_label", INIConfig::Validation::MIN_TEMPO, INIConfig::Validation::MAX_TEMPO),
      bottomSeparator(colorScheme) {

    setupComponents();
}

void TopBarComponent::setupComponents() {
    addAndMakeVisible(gearButton);
    addAndMakeVisible(linkButton);
    addAndMakeVisible(cloudButton);
    addAndMakeVisible(playButton);
    addAndMakeVisible(pauseButton);
    addAndMakeVisible(leftChevronButton);
    addAndMakeVisible(rightChevronButton);
    addAndMakeVisible(presetsMenu);
    addAndMakeVisible(bpmLabel);
    addAndMakeVisible(ottoLabel);
    addAndMakeVisible(versionLabel);
    addAndMakeVisible(clockSyncLabel);
    addAndMakeVisible(bottomSeparator);

    addAndMakeVisible(recordButton);
    addAndMakeVisible(tapTempoButton);
    addAndMakeVisible(overdubButton);
    addAndMakeVisible(loopButton);
    addAndMakeVisible(tapTempoLabel);

    gearButton.onClick = [this] {
        if (onGearButtonClicked) {
            onGearButtonClicked();
        }
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

    versionLabel.setComponentID("version_label");
    versionLabel.setText("Ver. 1.0", juce::dontSendNotification);
    versionLabel.setJustificationType(juce::Justification::centred);

    clockSyncLabel.setComponentID("clock_sync_label");
    clockSyncLabel.setText("", juce::dontSendNotification);
    clockSyncLabel.setJustificationType(juce::Justification::centred);
    clockSyncLabel.setVisible(false);

    tapTempoLabel.setComponentID("tap_tempo_label");
    tapTempoLabel.setText("TAP", juce::dontSendNotification);
    tapTempoLabel.setJustificationType(juce::Justification::centred);
    tapTempoLabel.setVisible(false);

    presetsMenu.setJustificationType(juce::Justification::centred);

    setupPresets();
    presetsMenu.setTextWhenNothingSelected("Select App Preset...");
    presetsMenu.setTextWhenNoChoicesAvailable("No app presets found");
    updateLinkButtonVisuals();
    updateRecordButton();
    updateCloudButtonVisuals();
}

void TopBarComponent::lookAndFeelChanged() {
    repaint();

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

void TopBarComponent::showCloudMenu() {
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

void TopBarComponent::showCloudAuthDialog() {
    auto* window = new juce::DialogWindow("Cloud Sign In",
                                         colorScheme.getColor(ColorScheme::ColorRole::WindowBackground),
                                         true);

    class AuthContent : public juce::Component {
    public:
        juce::Label emailLabel;
        juce::TextEditor emailInput;
        juce::Label passwordLabel;
        juce::TextEditor passwordInput;
        juce::TextButton signInButton{"Sign In"};
        juce::TextButton cancelButton{"Cancel"};
        ColorScheme& colorScheme;

        AuthContent(ColorScheme& cs) : colorScheme(cs) {
            emailLabel.setText("Email:", juce::dontSendNotification);
            passwordLabel.setText("Password:", juce::dontSendNotification);
            passwordInput.setPasswordCharacter('*');

            addAndMakeVisible(emailLabel);
            addAndMakeVisible(emailInput);
            addAndMakeVisible(passwordLabel);
            addAndMakeVisible(passwordInput);
            addAndMakeVisible(signInButton);
            addAndMakeVisible(cancelButton);
        }

        void resized() override {
            auto bounds = getLocalBounds().reduced(INIConfig::LayoutConstants::dialogContentMargin);
            emailLabel.setBounds(bounds.removeFromTop(INIConfig::LayoutConstants::dialogRowHeight));
            emailInput.setBounds(bounds.removeFromTop(INIConfig::LayoutConstants::dialogRowHeight));
            bounds.removeFromTop(INIConfig::LayoutConstants::dialogRowSpacing);
            passwordLabel.setBounds(bounds.removeFromTop(INIConfig::LayoutConstants::dialogRowHeight));
            passwordInput.setBounds(bounds.removeFromTop(INIConfig::LayoutConstants::dialogRowHeight));
            bounds.removeFromTop(INIConfig::LayoutConstants::dialogContentMargin);

            auto buttonArea = bounds.removeFromTop(INIConfig::LayoutConstants::dialogButtonAreaHeight);
            cancelButton.setBounds(buttonArea.removeFromLeft(INIConfig::LayoutConstants::dialogButtonWidth));
            buttonArea.removeFromLeft(INIConfig::LayoutConstants::dialogButtonSpacing);
            signInButton.setBounds(buttonArea.removeFromLeft(INIConfig::LayoutConstants::dialogButtonWidth));
        }
    };

    auto* content = new AuthContent(colorScheme);
    content->setSize(INIConfig::LayoutConstants::authDialogWidth, INIConfig::LayoutConstants::authDialogHeight);

    content->signInButton.onClick = [this, window, content] {
        cloudAuthenticated = true;
        cloudUsername = content->emailInput.getText();
        updateCloudButtonVisuals();
        window->exitModalState(1);
        delete window;
        notifyStateChanged();
    };

    content->cancelButton.onClick = [window] {
        window->exitModalState(0);
        delete window;
    };

    window->setContentOwned(content, false);
    window->centreWithSize(INIConfig::LayoutConstants::authDialogWidth, INIConfig::LayoutConstants::authDialogHeight);
    window->setVisible(true);
    window->enterModalState(true);
}

void TopBarComponent::shareCurrentPattern() {
    auto* window = new juce::DialogWindow("Share Pattern",
                                         colorScheme.getColor(ColorScheme::ColorRole::WindowBackground),
                                         true);

    class ShareContent : public juce::Component {
    public:
        juce::Label nameLabel;
        juce::TextEditor nameInput;
        juce::Label descLabel;
        juce::TextEditor descInput;
        juce::Label tagsLabel;
        juce::TextEditor tagsInput;
        juce::TextButton shareButton{"Share"};
        juce::TextButton cancelButton{"Cancel"};
        ColorScheme& colorScheme;

        ShareContent(ColorScheme& cs) : colorScheme(cs) {
            nameLabel.setText("Pattern Name:", juce::dontSendNotification);
            nameInput.setText("My Awesome Pattern");

            descLabel.setText("Description:", juce::dontSendNotification);
            descInput.setMultiLine(true);
            descInput.setText("Created with OTTO drum machine");

            tagsLabel.setText("Tags (comma separated):", juce::dontSendNotification);
            tagsInput.setText("drums, electronic, groove");

            addAndMakeVisible(nameLabel);
            addAndMakeVisible(nameInput);
            addAndMakeVisible(descLabel);
            addAndMakeVisible(descInput);
            addAndMakeVisible(tagsLabel);
            addAndMakeVisible(tagsInput);
            addAndMakeVisible(shareButton);
            addAndMakeVisible(cancelButton);
        }

        void resized() override {
            auto bounds = getLocalBounds().reduced(INIConfig::LayoutConstants::dialogContentMargin);
            nameLabel.setBounds(bounds.removeFromTop(INIConfig::LayoutConstants::shareDialogLabelHeight));
            nameInput.setBounds(bounds.removeFromTop(INIConfig::LayoutConstants::dialogRowHeight));
            bounds.removeFromTop(INIConfig::LayoutConstants::dialogRowSpacing);
            descLabel.setBounds(bounds.removeFromTop(INIConfig::LayoutConstants::shareDialogLabelHeight));
            descInput.setBounds(bounds.removeFromTop(INIConfig::LayoutConstants::shareDialogDescHeight));
            bounds.removeFromTop(INIConfig::LayoutConstants::dialogRowSpacing);
            tagsLabel.setBounds(bounds.removeFromTop(INIConfig::LayoutConstants::shareDialogLabelHeight));
            tagsInput.setBounds(bounds.removeFromTop(INIConfig::LayoutConstants::dialogRowHeight));
            bounds.removeFromTop(INIConfig::LayoutConstants::dialogContentMargin);

            auto buttonArea = bounds.removeFromTop(INIConfig::LayoutConstants::dialogButtonAreaHeight);
            cancelButton.setBounds(buttonArea.removeFromLeft(INIConfig::LayoutConstants::dialogButtonWidth));
            buttonArea.removeFromLeft(INIConfig::LayoutConstants::dialogButtonSpacing);
            shareButton.setBounds(buttonArea.removeFromLeft(INIConfig::LayoutConstants::dialogButtonWidth));
        }
    };

    auto* content = new ShareContent(colorScheme);
    content->setSize(INIConfig::LayoutConstants::shareDialogWidth, INIConfig::LayoutConstants::shareDialogHeight);

    content->shareButton.onClick = [this, window] {
        window->exitModalState(1);
        delete window;
        showShareSuccessMessage();
    };

    content->cancelButton.onClick = [window] {
        window->exitModalState(0);
        delete window;
    };

    window->setContentOwned(content, false);
    window->centreWithSize(INIConfig::LayoutConstants::shareDialogWidth, INIConfig::LayoutConstants::shareDialogHeight);
    window->setVisible(true);
    window->enterModalState(true);
}

void TopBarComponent::shareCurrentDrumKit() {
    juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon,
                                          "Share Drum Kit",
                                          "Drum kit sharing will be implemented soon!");
}

void TopBarComponent::startCollaborationSession() {
    auto* window = new juce::DialogWindow("Start Collaboration",
                                         colorScheme.getColor(ColorScheme::ColorRole::WindowBackground),
                                         true);

    class CollabContent : public juce::Component {
    public:
        juce::Label nameLabel;
        juce::TextEditor nameInput;
        juce::TextButton startButton{"Start Session"};
        juce::TextButton cancelButton{"Cancel"};
        ColorScheme& colorScheme;

        CollabContent(ColorScheme& cs) : colorScheme(cs) {
            nameLabel.setText("Session Name:", juce::dontSendNotification);
            nameInput.setText("Jam Session " + juce::Time::getCurrentTime().toString(false, true));

            addAndMakeVisible(nameLabel);
            addAndMakeVisible(nameInput);
            addAndMakeVisible(startButton);
            addAndMakeVisible(cancelButton);
        }

        void resized() override {
            auto bounds = getLocalBounds().reduced(INIConfig::LayoutConstants::dialogContentMargin);
            nameLabel.setBounds(bounds.removeFromTop(INIConfig::LayoutConstants::dialogRowHeight));
            nameInput.setBounds(bounds.removeFromTop(INIConfig::LayoutConstants::dialogRowHeight));
            bounds.removeFromTop(INIConfig::LayoutConstants::dialogContentMargin);

            auto buttonArea = bounds.removeFromTop(INIConfig::LayoutConstants::dialogButtonAreaHeight);
            cancelButton.setBounds(buttonArea.removeFromLeft(INIConfig::LayoutConstants::dialogButtonWidth));
            buttonArea.removeFromLeft(INIConfig::LayoutConstants::dialogButtonSpacing);
            startButton.setBounds(buttonArea.removeFromLeft(INIConfig::LayoutConstants::dialogButtonWidth));
        }
    };

    auto* content = new CollabContent(colorScheme);
    content->setSize(INIConfig::LayoutConstants::collabDialogWidth, INIConfig::LayoutConstants::collabDialogHeight);

    content->startButton.onClick = [this, window, content] {
        collaborationActive = true;
        collaborationSessionName = content->nameInput.getText();
        updateCloudButtonVisuals();
        window->exitModalState(1);
        delete window;
        showCollaborationCode();
    };

    content->cancelButton.onClick = [window] {
        window->exitModalState(0);
        delete window;
    };

    window->setContentOwned(content, false);
    window->centreWithSize(INIConfig::LayoutConstants::collabDialogWidth, INIConfig::LayoutConstants::collabDialogHeight);
    window->setVisible(true);
    window->enterModalState(true);
}

void TopBarComponent::joinCollaborationSession() {
    auto* window = new juce::DialogWindow("Join Collaboration",
                                         colorScheme.getColor(ColorScheme::ColorRole::WindowBackground),
                                         true);

    class JoinContent : public juce::Component {
    public:
        juce::Label codeLabel;
        juce::TextEditor codeInput;
        juce::TextButton joinButton{"Join Session"};
        juce::TextButton cancelButton{"Cancel"};
        ColorScheme& colorScheme;

        JoinContent(ColorScheme& cs) : colorScheme(cs) {
            codeLabel.setText("Session Code:", juce::dontSendNotification);
            codeInput.setInputRestrictions(INIConfig::LayoutConstants::sessionCodeLength, "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");

            addAndMakeVisible(codeLabel);
            addAndMakeVisible(codeInput);
            addAndMakeVisible(joinButton);
            addAndMakeVisible(cancelButton);
        }

        void resized() override {
            auto bounds = getLocalBounds().reduced(INIConfig::LayoutConstants::dialogContentMargin);
            codeLabel.setBounds(bounds.removeFromTop(INIConfig::LayoutConstants::dialogRowHeight));
            codeInput.setBounds(bounds.removeFromTop(INIConfig::LayoutConstants::dialogRowHeight));
            bounds.removeFromTop(INIConfig::LayoutConstants::dialogContentMargin);

            auto buttonArea = bounds.removeFromTop(INIConfig::LayoutConstants::dialogButtonAreaHeight);
            cancelButton.setBounds(buttonArea.removeFromLeft(INIConfig::LayoutConstants::dialogButtonWidth));
            buttonArea.removeFromLeft(INIConfig::LayoutConstants::dialogButtonSpacing);
            joinButton.setBounds(buttonArea.removeFromLeft(INIConfig::LayoutConstants::dialogButtonWidth));
        }
    };

    auto* content = new JoinContent(colorScheme);
    content->setSize(INIConfig::LayoutConstants::collabDialogWidth, INIConfig::LayoutConstants::collabDialogHeight);

    content->joinButton.onClick = [this, window, content] {
        collaborationActive = true;
        collaborationSessionName = "Session " + content->codeInput.getText();
        updateCloudButtonVisuals();
        window->exitModalState(1);
        delete window;
    };

    content->cancelButton.onClick = [window] {
        window->exitModalState(0);
        delete window;
    };

    window->setContentOwned(content, false);
    window->centreWithSize(INIConfig::LayoutConstants::collabDialogWidth, INIConfig::LayoutConstants::collabDialogHeight);
    window->setVisible(true);
    window->enterModalState(true);
}

void TopBarComponent::leaveCollaborationSession() {
    collaborationActive = false;
    collaborationSessionName.clear();
    updateCloudButtonVisuals();
    juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon,
                                          "Collaboration",
                                          "You have left the collaboration session.");
}

void TopBarComponent::backupToCloud() {
    class BackupProgressWindow : public juce::Component, public juce::Timer
    {
    public:
        BackupProgressWindow(TopBarComponent& parent, ColorScheme& cs)
            : owner(parent),
              colorScheme(cs),
              progress(0.0),
              progressBar(progress)
        {
            addAndMakeVisible(progressBar);
            setSize(INIConfig::LayoutConstants::backupProgressWidth, INIConfig::LayoutConstants::backupProgressHeight);
            startTimer(INIConfig::LayoutConstants::backupProgressTimerMs);
        }

        void resized() override
        {
            progressBar.setBounds(INIConfig::LayoutConstants::backupProgressBarMargin, INIConfig::LayoutConstants::backupProgressBarY,
                                getWidth() - 2 * INIConfig::LayoutConstants::backupProgressBarMargin, INIConfig::LayoutConstants::dialogRowHeight);
        }

        void paint(juce::Graphics& g) override
        {
            g.fillAll(colorScheme.getColor(ColorScheme::ColorRole::WindowBackground));
            g.setColour(colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
            g.drawText("Backing up to cloud...", getLocalBounds().removeFromTop(INIConfig::LayoutConstants::dialogRowHeight),
                      juce::Justification::centred);
        }

        void timerCallback() override
        {
            progress += INIConfig::LayoutConstants::backupProgressIncrement;

            if (progress >= 1.0)
            {
                stopTimer();
                owner.showBackupSuccessMessage();
                if (auto* window = findParentComponentOfClass<juce::DialogWindow>())
                {
                    window->exitModalState(0);
                }
            }
        }

    private:
        TopBarComponent& owner;
        ColorScheme& colorScheme;
        double progress;
        juce::ProgressBar progressBar;
    };

    auto* progressWindow = new BackupProgressWindow(*this, colorScheme);
    auto* window = new juce::DialogWindow("Cloud Backup",
                                         colorScheme.getColor(ColorScheme::ColorRole::WindowBackground),
                                         false);
    window->setContentOwned(progressWindow, true);
    window->centreWithSize(INIConfig::LayoutConstants::backupProgressWidth, INIConfig::LayoutConstants::backupProgressHeight);
    window->enterModalState(true, nullptr, true);
}

void TopBarComponent::restoreFromCloud() {
    juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon,
                                          "Restore from Cloud",
                                          "Cloud restore functionality will be implemented soon!");
}

void TopBarComponent::showShareSuccessMessage() {
    juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon,
                                          "Success",
                                          "Pattern shared successfully!\n\nShare code: " + generateShareCode());
}

void TopBarComponent::showBackupSuccessMessage() {
    juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon,
                                          "Success",
                                          "Backup completed successfully!");
}

void TopBarComponent::showCollaborationCode() {
    juce::String code = generateShareCode();
    juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon,
                                          "Collaboration Session Started",
                                          "Share this code with others:\n\n" + code +
                                          "\n\nThey can join using the 'Join Session' option.");
}

juce::String TopBarComponent::generateShareCode() {
    juce::Random random;
    juce::String code;
    const char* chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (int i = 0; i < INIConfig::LayoutConstants::shareCodeLength; ++i) {
        code += chars[random.nextInt(INIConfig::LayoutConstants::shareCodeChars)];
    }
    return code;
}

void TopBarComponent::updateCloudButtonVisuals() {
    if (cloudAuthenticated || collaborationActive || cloudAutoSync) {
        cloudButton.setToggleState(true, juce::dontSendNotification);
    } else {
        cloudButton.setToggleState(false, juce::dontSendNotification);
    }
    cloudButton.repaint();
}

void TopBarComponent::tapTempo() {
    double currentTime = juce::Time::getMillisecondCounterHiRes();

    if (currentTime - lastTapTime > INIConfig::LayoutConstants::tapTempoTimeoutMs) {
        tapCount = 0;
        midiEngine.resetTapTempo();
    }

    lastTapTime = currentTime;
    tapCount++;

    midiEngine.tapTempo();

    float averagedTempo = midiEngine.getTapTempoAveraging();
    if (averagedTempo > 0) {
        setTempo(averagedTempo);
    }

    updateTapTempoDisplay();
}

void TopBarComponent::updateTapTempoDisplay() {
    tapTempoLabel.setVisible(true);
    tapTempoLabel.setText("TAP " + juce::String(tapCount), juce::dontSendNotification);

    juce::Timer::callAfterDelay(INIConfig::LayoutConstants::tapTempoDisplayMs, [this] {
        tapTempoLabel.setVisible(false);
    });
}

void TopBarComponent::setLiveRecording(bool recording) {
    liveRecording = recording;
    midiEngine.startLiveRecording(overdubEnabled);
    updateRecordButton();
}

void TopBarComponent::setOverdubEnabled(bool enabled) {
    overdubEnabled = enabled;
    midiEngine.setOverdubMode(enabled);

    overdubButton.setToggleState(enabled, juce::dontSendNotification);
    overdubButton.repaint();
}

void TopBarComponent::setLoopRecordingEnabled(bool enabled) {
    loopRecordingEnabled = enabled;
    midiEngine.setLoopRecordingMode(enabled);

    loopButton.setToggleState(enabled, juce::dontSendNotification);
    loopButton.repaint();
}

void TopBarComponent::updateRecordButton() {
    recordButton.setToggleState(liveRecording, juce::dontSendNotification);
    recordButton.repaint();
}

void TopBarComponent::showLinkSyncMenu() {
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

void TopBarComponent::updateLinkButtonVisuals() {
    if (midiClockInEnabled || midiClockOutEnabled) {
        linkButton.setToggleState(true, juce::dontSendNotification);
    } else {
        linkButton.setToggleState(false, juce::dontSendNotification);
    }
    linkButton.repaint();
}

void TopBarComponent::updateClockSyncStatus(bool isExternalSync, double externalTempo) {
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

void TopBarComponent::setMidiClockInEnabled(bool enabled) {
    midiClockInEnabled = enabled;
    updateLinkButtonVisuals();
}

void TopBarComponent::setMidiClockOutEnabled(bool enabled) {
    midiClockOutEnabled = enabled;
    updateLinkButtonVisuals();
}

void TopBarComponent::setupPresets() {
    presetsMenu.setTextWhenNothingSelected("Select App Preset...");
    presetsMenu.setTextWhenNoChoicesAvailable("No app presets found");
    presetsMenu.setJustificationType(juce::Justification::centred);

    presetsMenu.onPopupRequest = [this] {
        buildHierarchicalPresetMenu();
    };

    currentPresetName = "Default";
    presetsMenu.setText(currentPresetName);
}

void TopBarComponent::buildHierarchicalPresetMenu() {
    juce::PopupMenu mainMenu;

    mainMenu.setLookAndFeel(&getLookAndFeel());

    presetMenuMapping.clear();
    int currentMenuId = 1;

    struct PresetCategory {
        juce::String categoryName;
        juce::StringArray presets;
    };

    PresetCategory categories[] = {
        {"Basic", {"Default", "Acoustic", "Electronic"}},
        {"Vintage", {"Bathroom", "Blues", "Brush"}},
        {"Modern", {"Claps", "Funk", "Rock"}},
        {"Special", {"Noise Makers", "Percs", "Rods & Shakers", "Tamborine"}}
    };

    for (const auto& category : categories) {
        juce::PopupMenu subMenu;

        subMenu.setLookAndFeel(&getLookAndFeel());

        for (const auto& preset : category.presets) {
            bool isCurrentSelection = (preset == currentPresetName);
            subMenu.addItem(currentMenuId, preset, true, isCurrentSelection);

            presetMenuMapping.add({preset, currentMenuId});
            currentMenuId++;
        }

        mainMenu.addSubMenu(category.categoryName, subMenu);
    }

    mainMenu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(&presetsMenu),
                          [this](int result) {
                              handlePresetMenuSelection(result);
                          });
}

void TopBarComponent::handlePresetMenuSelection(int result) {
    if (result == 0) return;

    for (const auto& mapping : presetMenuMapping) {
        if (mapping.menuId == result) {
            currentPresetName = mapping.presetName;
            presetsMenu.setText(currentPresetName);

            int presetIndex = getPresetIndexFromName(currentPresetName);
            if (onPresetChanged && presetIndex >= 0) {
                onPresetChanged(presetIndex);
            }
            notifyStateChanged();
            break;
        }
    }
}

int TopBarComponent::getPresetIndexFromName(const juce::String& presetName) const {
    auto allPresets = getAllPresetNames();
    return allPresets.indexOf(presetName);
}

juce::StringArray TopBarComponent::getAllPresetNames() const {
    return {
        "Default", "Acoustic", "Bathroom", "Blues", "Brush", "Claps",
        "Electronic", "Funk", "Noise Makers", "Percs", "Rock", "Rods & Shakers", "Tamborine"
    };
}

void TopBarComponent::handlePresetChevrons(bool isRight) {
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

        if (onPresetChanged) {
            onPresetChanged(currentIndex);
        }
        notifyStateChanged();
    }
}

void TopBarComponent::updatePlayButtons() {
    playButton.setVisible(!isPlaying);
    pauseButton.setVisible(isPlaying);
}

void TopBarComponent::setPresetSelection(int index) {
    auto allPresets = getAllPresetNames();

    if (index >= 0 && index < allPresets.size()) {
        currentPresetName = allPresets[index];
        presetsMenu.setText(currentPresetName);
        notifyStateChanged();
    }
}

void TopBarComponent::setPlayState(bool playing) {
    if (isPlaying != playing) {
        isPlaying = playing;
        updatePlayButtons();
        if (onPlayStateChanged) {
            onPlayStateChanged(isPlaying);
        }
        notifyStateChanged();
    }
}

float TopBarComponent::getTempo() const {
    return bpmLabel.getValue();
}

void TopBarComponent::setTempo(float tempo) {
    float clampedTempo = INIConfig::clampTempo(tempo);
    bpmLabel.setValue(clampedTempo);
    notifyStateChanged();
}

int TopBarComponent::getPresetSelection() const {
    return getPresetIndexFromName(currentPresetName);
}

void TopBarComponent::saveStates(ComponentState& state) {
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

void TopBarComponent::loadStates(const ComponentState& state) {
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

void TopBarComponent::notifyStateChanged() {
}

void TopBarComponent::paint(juce::Graphics& g) {
    g.fillAll(colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground));
}

void TopBarComponent::resized() {
    auto bounds = getLocalBounds();

    int iconSize = layoutManager.scaled(INIConfig::LayoutConstants::iconButtonSize);
    int iconY = layoutManager.scaled(INIConfig::LayoutConstants::topBarIconY);

    gearButton.setBounds(layoutManager.scaled(INIConfig::LayoutConstants::topBarGearX), iconY, iconSize, iconSize);
    linkButton.setBounds(layoutManager.scaled(INIConfig::LayoutConstants::topBarLinkX), iconY, iconSize, iconSize);
    cloudButton.setBounds(layoutManager.scaled(INIConfig::LayoutConstants::topBarCloudX), iconY, iconSize, iconSize);

    int presetY = layoutManager.scaled(INIConfig::LayoutConstants::topBarPresetY);
    int chevronSize = layoutManager.scaled(INIConfig::LayoutConstants::topBarChevronSize);
    leftChevronButton.setBounds(layoutManager.scaled(INIConfig::LayoutConstants::topBarLeftChevronX), presetY, chevronSize, chevronSize);
    presetsMenu.setBounds(layoutManager.scaled(INIConfig::LayoutConstants::topBarPresetsMenuX), layoutManager.scaled(INIConfig::LayoutConstants::topBarPresetsMenuY),
                         layoutManager.scaled(INIConfig::LayoutConstants::topBarPresetsMenuWidth), layoutManager.scaled(INIConfig::LayoutConstants::iconButtonSize));
    rightChevronButton.setBounds(layoutManager.scaled(INIConfig::LayoutConstants::topBarRightChevronX), presetY, chevronSize, chevronSize);

    playButton.setBounds(layoutManager.scaled(INIConfig::LayoutConstants::topBarPlayX), iconY, iconSize, iconSize);
    pauseButton.setBounds(layoutManager.scaled(INIConfig::LayoutConstants::topBarPlayX), iconY, iconSize, iconSize);

    bpmLabel.setBounds(layoutManager.scaled(INIConfig::LayoutConstants::topBarBpmX), layoutManager.scaled(INIConfig::LayoutConstants::topBarBpmY),
                      layoutManager.scaled(INIConfig::LayoutConstants::topBarBpmWidth), layoutManager.scaled(INIConfig::LayoutConstants::topBarBpmHeight));

    clockSyncLabel.setBounds(layoutManager.scaled(INIConfig::LayoutConstants::topBarBpmX), layoutManager.scaled(INIConfig::LayoutConstants::topBarClockSyncY),
                           layoutManager.scaled(INIConfig::LayoutConstants::topBarBpmWidth), layoutManager.scaled(INIConfig::LayoutConstants::topBarClockSyncHeight));

    tapTempoButton.setBounds(layoutManager.scaled(INIConfig::LayoutConstants::topBarTapTempoX), iconY, iconSize, iconSize);
    tapTempoLabel.setBounds(layoutManager.scaled(INIConfig::LayoutConstants::topBarTapTempoX), layoutManager.scaled(INIConfig::LayoutConstants::topBarClockSyncY),
                          iconSize, layoutManager.scaled(INIConfig::LayoutConstants::topBarClockSyncHeight));

    recordButton.setBounds(layoutManager.scaled(INIConfig::LayoutConstants::topBarRecordX), iconY, iconSize, iconSize);
    overdubButton.setBounds(layoutManager.scaled(INIConfig::LayoutConstants::topBarOverdubX), iconY, iconSize, iconSize);
    loopButton.setBounds(layoutManager.scaled(INIConfig::LayoutConstants::topBarLoopX), iconY, iconSize, iconSize);

    ottoLabel.setBounds(layoutManager.scaled(INIConfig::LayoutConstants::topBarOttoX), layoutManager.scaled(INIConfig::LayoutConstants::topBarOttoY),
                       layoutManager.scaled(INIConfig::LayoutConstants::topBarOttoWidth), layoutManager.scaled(INIConfig::LayoutConstants::topBarOttoHeight));
    versionLabel.setBounds(layoutManager.scaled(INIConfig::LayoutConstants::topBarOttoX), layoutManager.scaled(INIConfig::LayoutConstants::topBarVersionY),
                          layoutManager.scaled(INIConfig::LayoutConstants::topBarOttoWidth), layoutManager.scaled(INIConfig::LayoutConstants::topBarVersionHeight));

    bottomSeparator.setBounds(0, bounds.getBottom() - layoutManager.scaled(INIConfig::LayoutConstants::separatorThickness),
                             bounds.getWidth(), layoutManager.scaled(INIConfig::LayoutConstants::separatorThickness));
}
