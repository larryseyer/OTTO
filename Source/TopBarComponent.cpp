/**
 * @file TopBarComponent.cpp
 * @brief Implementation of OTTO's top navigation bar and transport controls
 *
 * This file implements the TopBarComponent class, which provides the primary
 * navigation and transport interface for OTTO. It occupies Row 1 of the main
 * layout (10% of interface height) and contains essential controls for
 * settings, presets, playback, and branding elements.
 *
 * LAYOUT ARCHITECTURE (Row 1 - 10% of interface height):
 * =======================================================
 * LEFT SECTION: Settings and Navigation
 * - Gear button: Opens settings panel
 * - Link button: Ableton Link sync control
 * - Cloud button: Cloud service connectivity
 * - Chevron buttons: Preset navigation (previous/next)
 * - Presets menu: Dropdown for preset selection
 *
 * CENTER SECTION: Transport Controls
 * - Play/Pause buttons: Primary transport control
 * - BPM display: Tempo with tap tempo functionality
 * - Record button: Pattern recording
 * - Overdub button: Layer additional patterns
 * - Loop button: Loop mode toggle
 *
 * RIGHT SECTION: Branding and Status
 * - OTTO label: Application branding
 * - Version label: Current software version
 * - Clock sync indicator: External sync status
 *
 * RESPONSIVE DESIGN FEATURES:
 * ===========================
 * - Uses INIConfig::LayoutConstants::Row1 for all positioning
 * - Phosphor icon integration for scalable vector graphics
 * - FontManager integration for consistent typography
 * - ColorScheme integration for theme-consistent styling
 * - ResponsiveLayoutManager for real-time scaling
 *
 * INTEGRATION POINTS:
 * ===================
 * - MidiEngine.cpp: Transport control and tempo management
 * - INIConfig.h: Layout constants and default values
 * - FontManager.cpp: Typography and icon rendering
 * - ColorScheme.cpp: Theme-consistent visual styling
 * - ResponsiveLayoutManager.cpp: Dynamic scaling and positioning
 * - SettingsPanel.cpp: Settings panel activation
 * - PresetManager.cpp: Preset selection and navigation
 *
 * @author OTTO Development Team
 * @version 2.0
 * @date 2024
 * @see INIConfig.h for Row1 layout constants
 * @see ResponsiveLayoutManager.cpp for scaling implementation
 * @see MidiEngine.cpp for transport integration
 */

#include "TopBarComponent.h"
#include "MidiEngine.h"
#include "INIConfig.h"
#include "INIDataManager.h"

/**
 * @brief TopBarComponent constructor - initialize top navigation bar with all controls
 *
 * Sets up the complete top bar interface using Row 1 layout specifications from
 * INIConfig. Initializes all Phosphor icon buttons, creates preset management
 * components, and establishes integration with core application systems.
 *
 * COMPONENT INITIALIZATION STRATEGY:
 * 1. Store references to all required subsystems
 * 2. Initialize Phosphor icon buttons with appropriate icons
 * 3. Set up BPM label with tempo validation limits
 * 4. Create bottom separator for visual row division
 * 5. Configure all components for immediate layout application
 *
 * PHOSPHOR ICON ASSIGNMENTS:
 * - "gear": Settings access (universal settings icon)
 * - "link": Ableton Link connectivity (chain link metaphor)
 * - "cloud": Cloud service status (cloud storage metaphor)
 * - "play"/"pause": Transport control (standard media icons)
 * - "left"/"right": Preset navigation (directional arrows)
 * - "record": Pattern recording (standard record symbol)
 * - "metronome": Tap tempo (timing/rhythm symbol)
 * - "stack-plus": Overdub mode (layering metaphor)
 * - "repeat": Loop mode (cycling/repetition symbol)
 *
 * BPM VALIDATION:
 * - Uses INIConfig::Validation::MIN_TEMPO and MAX_TEMPO for input limits
 * - Ensures tempo values remain within musical and technical constraints
 * - Provides real-time feedback for tempo changes
 *
 * @param midiEngine Reference to MIDI processing and transport system
 * @param valueTreeState JUCE parameter automation and state management
 * @param layoutManager Responsive layout calculation and scaling system
 * @param fontManager Typography and Phosphor icon font management
 * @param colorScheme Theme and color management for consistent styling
 *
 * Called by: MainComponent.cpp or PluginEditor.cpp during interface construction
 * References: INIConfig.h for validation limits, FontManager for icon rendering
 */
TopBarComponent::TopBarComponent(MidiEngine& midiEngine,
                                juce::AudioProcessorValueTreeState& valueTreeState,
                                ResponsiveLayoutManager& layoutManager,
                                FontManager& fontManager,
                                ColorScheme& colorScheme)
    : midiEngine(midiEngine), valueTreeState(valueTreeState), layoutManager(layoutManager),
      fontManager(fontManager), colorScheme(colorScheme),

      // LEFT SECTION BUTTONS: Settings and service connectivity controls
      gearButton("gear"),          // Settings panel access - universal settings metaphor
      linkButton("link"),          // Ableton Link sync - chain connection visual
      cloudButton("cloud"),        // Cloud service status - cloud storage metaphor

      // TRANSPORT CONTROLS: Primary playback and recording functionality
      playButton("play"),          // Start playback - standard media play symbol
      pauseButton("pause"),        // Pause playback - standard media pause symbol

      // PRESET NAVIGATION: Chevron buttons for browsing presets
      leftChevronButton("left"),   // Previous preset - left-pointing directional arrow
      rightChevronButton("right"), // Next preset - right-pointing directional arrow

      // ADVANCED TRANSPORT: Recording and performance controls
      recordButton("record"),      // Pattern recording - standard red record symbol
      tapTempoButton("metronome"), // Tap tempo input - metronome/timing symbol
      overdubButton("stack-plus"), // Overdub mode - layering/stacking metaphor
      loopButton("repeat"),        // Loop mode toggle - repeat/cycle symbol

      // TEMPO DISPLAY: BPM input with validation constraints
      // Uses INIConfig tempo limits to ensure valid musical range (typically 60-200 BPM)
      bpmLabel("bpm_label", INIConfig::Validation::MIN_TEMPO, INIConfig::Validation::MAX_TEMPO),

      // VISUAL SEPARATOR: Bottom border for Row 1 definition
      // Uses ColorScheme for theme-consistent styling
      bottomSeparator(colorScheme) {

    setupComponents();
}

void TopBarComponent::setupComponents() {
    addAndMakeVisible(gearButton);
    addAndMakeVisible(linkButton);
    addAndMakeVisible(cloudButton);

    // TEMPORARY: Debug rectangles will be drawn in paint() method to show boundaries
    addAndMakeVisible(playButton);
    addAndMakeVisible(pauseButton);
    addAndMakeVisible(leftChevronButton);
    addAndMakeVisible(rightChevronButton);
    addAndMakeVisible(presetsMenu);
    addAndMakeVisible(presetDisplayLabel);
    addAndMakeVisible(bpmLabel);
    // Right-side branding elements
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


    // Configure labels with FontManager and ColorScheme integration
    ottoLabel.setComponentID("otto_label");
    ottoLabel.setText("OTTO", juce::dontSendNotification);
    ottoLabel.setJustificationType(juce::Justification::centred);
    ottoLabel.setFont(fontManager.getFont(FontManager::FontRole::Header,
                      layoutManager.scaled(INIConfig::LayoutConstants::Row1::ottoHeight * 0.6f)));
    ottoLabel.setColour(juce::Label::textColourId, colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));

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

    // Configure preset display label with Playfair Display font - make it prominent
    presetDisplayLabel.setComponentID("preset_display_label");
    presetDisplayLabel.setText(currentPresetName, juce::dontSendNotification);
    presetDisplayLabel.setJustificationType(juce::Justification::centred);
    presetDisplayLabel.setFont(fontManager.getFont(FontManager::FontRole::Header,
                               layoutManager.scaled(INIConfig::LayoutConstants::Row1::ottoHeight * 2.0f)));
    presetDisplayLabel.setColour(juce::Label::textColourId,
                                colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));

    // Make preset display label clickable
    presetDisplayLabel.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    presetDisplayLabel.addMouseListener(this, false);

    // Configure presets menu with component ID for font styling and left alignment
    presetsMenu.setComponentID("presets_menu");
    presetsMenu.setJustificationType(juce::Justification::left);

    setupPresets();
    presetsMenu.setTextWhenNothingSelected("Select App Preset...");
    presetsMenu.setTextWhenNoChoicesAvailable("No app presets found");

    // Initialize preset display toggle state
    updatePresetDisplayToggle();

    updateLinkButtonVisuals();
    updateRecordButton();
    updateCloudButtonVisuals();
}

void TopBarComponent::lookAndFeelChanged() {
    repaint();

    // Update fonts and colors when look and feel changes
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

    // Update preset display label font and color - make it prominent
    presetDisplayLabel.setFont(fontManager.getFont(FontManager::FontRole::Header,
                               layoutManager.scaled(INIConfig::LayoutConstants::Row1::ottoHeight * 2.4f)));
    presetDisplayLabel.setColour(juce::Label::textColourId,
                                colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));

    // Set BPM label to use Roboto Condensed (Version role) for compact tempo display
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
    auto window = std::make_unique<juce::DialogWindow>("Cloud Sign In",
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

    auto content = std::make_unique<AuthContent>(colorScheme);
    content->setSize(INIConfig::LayoutConstants::authDialogWidth, INIConfig::LayoutConstants::authDialogHeight);

    // Capture raw pointer for lambda - window will manage its own lifetime
    auto* windowPtr = window.get();

    content->signInButton.onClick = [this, windowPtr] {
        if (auto* contentPtr = dynamic_cast<AuthContent*>(windowPtr->getContentComponent())) {
            cloudAuthenticated = true;
            cloudUsername = contentPtr->emailInput.getText();
            updateCloudButtonVisuals();
        }
        windowPtr->exitModalState(1);
        notifyStateChanged();
    };

    content->cancelButton.onClick = [windowPtr] {
        windowPtr->exitModalState(0);
    };

    window->setContentOwned(content.release(), true); // Transfer ownership to window
    window->centreWithSize(INIConfig::LayoutConstants::authDialogWidth, INIConfig::LayoutConstants::authDialogHeight);
    window->setVisible(true);

    // JUCE automatically manages modal window lifecycle - release from smart pointer
    window.release()->enterModalState(true, nullptr, true); // deleteWhenFinished = true
}

void TopBarComponent::shareCurrentPattern() {
    auto window = std::make_unique<juce::DialogWindow>("Share Pattern",
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

    auto content = std::make_unique<ShareContent>(colorScheme);
    content->setSize(INIConfig::LayoutConstants::shareDialogWidth, INIConfig::LayoutConstants::shareDialogHeight);

    // Capture raw pointer for lambda - window will manage its own lifetime
    auto* windowPtr = window.get();

    content->shareButton.onClick = [this, windowPtr] {
        windowPtr->exitModalState(1);
        showShareSuccessMessage();
    };

    content->cancelButton.onClick = [windowPtr] {
        windowPtr->exitModalState(0);
    };

    window->setContentOwned(content.release(), true);
    window->centreWithSize(INIConfig::LayoutConstants::shareDialogWidth, INIConfig::LayoutConstants::shareDialogHeight);
    window->setVisible(true);

    // JUCE automatically manages modal window lifecycle - release from smart pointer
    window.release()->enterModalState(true, nullptr, true); // deleteWhenFinished = true
}

void TopBarComponent::shareCurrentDrumKit() {
    juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon,
                                          "Share Drum Kit",
                                          "Drum kit sharing will be implemented soon!");
}

void TopBarComponent::startCollaborationSession() {
    auto window = std::make_unique<juce::DialogWindow>("Start Collaboration",
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

    auto content = std::make_unique<CollabContent>(colorScheme);
    content->setSize(INIConfig::LayoutConstants::collabDialogWidth, INIConfig::LayoutConstants::collabDialogHeight);

    // Capture raw pointer for lambda - window will manage its own lifetime
    auto* windowPtr = window.get();

    content->startButton.onClick = [this, windowPtr] {
        if (auto* contentPtr = dynamic_cast<CollabContent*>(windowPtr->getContentComponent())) {
            collaborationActive = true;
            collaborationSessionName = contentPtr->nameInput.getText();
            updateCloudButtonVisuals();
        }
        windowPtr->exitModalState(1);
        showCollaborationCode();
    };

    content->cancelButton.onClick = [windowPtr] {
        windowPtr->exitModalState(0);
    };

    window->setContentOwned(content.release(), true);
    window->centreWithSize(INIConfig::LayoutConstants::collabDialogWidth, INIConfig::LayoutConstants::collabDialogHeight);
    window->setVisible(true);

    // JUCE automatically manages modal window lifecycle - release from smart pointer
    window.release()->enterModalState(true, nullptr, true); // deleteWhenFinished = true
}

void TopBarComponent::joinCollaborationSession() {
    auto window = std::make_unique<juce::DialogWindow>("Join Collaboration",
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

    auto content = std::make_unique<JoinContent>(colorScheme);
    content->setSize(INIConfig::LayoutConstants::collabDialogWidth, INIConfig::LayoutConstants::collabDialogHeight);

    // Capture raw pointer for lambda - window will manage its own lifetime
    auto* windowPtr = window.get();

    content->joinButton.onClick = [this, windowPtr] {
        if (auto* contentPtr = dynamic_cast<JoinContent*>(windowPtr->getContentComponent())) {
            collaborationActive = true;
            collaborationSessionName = "Session " + contentPtr->codeInput.getText();
            updateCloudButtonVisuals();
        }
        windowPtr->exitModalState(1);
    };

    content->cancelButton.onClick = [windowPtr] {
        windowPtr->exitModalState(0);
    };

    window->setContentOwned(content.release(), true);
    window->centreWithSize(INIConfig::LayoutConstants::collabDialogWidth, INIConfig::LayoutConstants::collabDialogHeight);
    window->setVisible(true);

    // JUCE automatically manages modal window lifecycle - release from smart pointer
    window.release()->enterModalState(true, nullptr, true); // deleteWhenFinished = true
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
    presetsMenu.setJustificationType(juce::Justification::left);

    presetsMenu.onPopupRequest = [this] {
        buildHierarchicalPresetMenu();
    };

    currentPresetName = "Default";
    presetsMenu.setText(currentPresetName);
}

void TopBarComponent::buildHierarchicalPresetMenu() {
    // Ensure default preset structure exists
    ensureDefaultPresetStructure();

    // Create sample preset structure for demonstration (only if directories are empty)
    auto presetsDir = INIConfig::getPresetsDirectory();
    auto categoryDirs = presetsDir.findChildFiles(juce::File::findDirectories, false);
    if (categoryDirs.size() <= 1) { // Only Default category exists
        createSamplePresetStructure();
    }

    juce::PopupMenu mainMenu;

    // Don't set custom LookAndFeel to avoid font issues with Unicode characters
    // mainMenu.setLookAndFeel(&getLookAndFeel());

    presetMenuMapping.clear();
    int currentMenuId = 1;

    // Get categories from filesystem
    auto categories = getPresetCategoriesFromFilesystem();

    for (const auto& categoryName : categories) {
        juce::PopupMenu subMenu;
        // Don't set custom LookAndFeel to avoid font issues with Unicode characters
        // subMenu.setLookAndFeel(&getLookAndFeel());

        // Get presets in this category
        auto presetsInCategory = getPresetsInCategory(categoryName);

        for (const auto& preset : presetsInCategory) {
            bool isCurrentSelection = (preset == currentPresetName);
            // Create a clean copy of the preset name
            juce::String presetCopy = preset;
            juce::String categoryCopy = categoryName;
            
            // Use INI-defined selection indicator to avoid JUCE's Unicode issues
            juce::String displayName = isCurrentSelection ? 
                INIConfig::UI::MENU_SELECTION_INDICATOR + " " + presetCopy : 
                INIConfig::UI::MENU_NON_SELECTION_PADDING + presetCopy;
            
            // Add item without JUCE's built-in selection to avoid corruption
            subMenu.addItem(currentMenuId, displayName, true, false);

            presetMenuMapping.add({presetCopy, categoryCopy, currentMenuId});
            currentMenuId++;
        }

        // Only add submenu if it has presets
        if (presetsInCategory.size() > 0) {
            mainMenu.addSubMenu(categoryName, subMenu);
        }
    }

    mainMenu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(&presetsMenu),
                          [this](int result) {
                              handlePresetMenuSelection(result);
                          });
}

void TopBarComponent::handlePresetMenuSelection(int result) {
    if (result == 0) {
        // User cancelled - show label again
        showPresetLabel();
        return;
    }

    for (const auto& mapping : presetMenuMapping) {
        if (mapping.menuId == result) {
            currentPresetName = mapping.presetName;
            presetsMenu.setText(currentPresetName);
            presetDisplayLabel.setText(currentPresetName, juce::dontSendNotification);

            // Hide menu and show label with new selection
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

int TopBarComponent::getPresetIndexFromName(const juce::String& presetName) const {
    auto allPresets = getAllPresetNames();
    return allPresets.indexOf(presetName);
}

juce::StringArray TopBarComponent::getAllPresetNames() const {
    // Dynamically read presets from disk using the .ini storage system
    if (iniDataManager) {
        auto presets = iniDataManager->getAvailablePresetNames();
        if (presets.size() > 0) {
            return presets;
        }
    }

    // Fallback to just Default if system is not ready
    return {"Default"};
}

juce::Array<juce::String> TopBarComponent::getPresetCategoriesFromFilesystem() const {
    juce::Array<juce::String> categories;

    auto presetsDir = INIConfig::getPresetsDirectory();
    if (!presetsDir.exists()) {
        categories.add("Defaults");
        return categories;
    }

    // Always ensure "Defaults" category exists and comes first
    bool hasDefaults = false;

    // Scan for subdirectories in the Presets folder
    for (auto& file : presetsDir.findChildFiles(juce::File::findDirectories, false)) {
        juce::String categoryName = file.getFileName();
        if (categoryName == "Defaults") {
            hasDefaults = true;
        } else {
            categories.add(categoryName);
        }
    }

    // Ensure Defaults is first
    if (hasDefaults) {
        categories.insert(0, "Defaults");
    } else {
        categories.insert(0, "Defaults");
    }

    return categories;
}

juce::StringArray TopBarComponent::getPresetsInCategory(const juce::String& categoryName) const {
    juce::StringArray presets;

    auto presetsDir = INIConfig::getPresetsDirectory();
    auto categoryDir = presetsDir.getChildFile(categoryName);

    if (!categoryDir.exists()) {
        // For Defaults category, create it and add default preset
        if (categoryName == "Defaults") {
            if (iniDataManager) {
                iniDataManager->createDefaultPreset();
                presets.add("Default");
            }
        }
        return presets;
    }

    // Scan for preset files in the category directory
    // Look for .ini files (presets are stored as INI files)
    for (auto& file : categoryDir.findChildFiles(juce::File::findFiles, false, "*.ini")) {
        juce::String presetName = file.getFileNameWithoutExtension();
        presets.add(presetName);
    }

    // If no presets found but it's Defaults category, create and add Default preset
    if (presets.isEmpty() && categoryName == "Defaults") {
        if (iniDataManager) {
            iniDataManager->createDefaultPreset();
            presets.add("Default");
        }
    }

    // Sort presets, but ensure "Default" comes first if it exists
    if (presets.contains("Default")) {
        presets.removeString("Default");
        presets.sort(false);
        presets.insert(0, "Default");
    } else {
        presets.sort(false);
    }

    return presets;
}

void TopBarComponent::ensureDefaultPresetStructure() const {
    auto presetsDir = INIConfig::getPresetsDirectory();

    // Ensure main presets directory exists
    if (!presetsDir.exists()) {
        presetsDir.createDirectory();
    }

    // Ensure Defaults category directory exists
    auto defaultCategoryDir = presetsDir.getChildFile("Defaults");
    if (!defaultCategoryDir.exists()) {
        defaultCategoryDir.createDirectory();
    }

    // Ensure Default preset file exists - use INIDataManager to create it properly
    auto defaultPresetFile = defaultCategoryDir.getChildFile("Default.ini");
    if (!defaultPresetFile.existsAsFile() && iniDataManager) {
        iniDataManager->createDefaultPreset();
    }
}

void TopBarComponent::createSamplePresetStructure() const {
    if (!iniDataManager) return;

    auto presetsDir = INIConfig::getPresetsDirectory();

    // Create sample categories and presets to match the original hardcoded structure
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
                // Create a basic sample preset state
                ComponentState sampleState;
                sampleState.tempo = INIConfig::Defaults::DEFAULT_TEMPO;
                sampleState.sliderValues["masterVolume"] = INIConfig::Defaults::DEFAULT_MASTER_VOLUME;

                // Set sample player settings with variations based on preset name
                for (int i = 0; i < INIConfig::LayoutConstants::playerTabsCount; ++i) {
                    auto& player = sampleState.playerSettings[i];
                    player.enabled = (i < 4); // Enable first 4 players for sample presets
                    player.selectedDrumkit = (presetName.contains("Electronic")) ? "Electronic" : "Acoustic";
                    player.swingValue = INIConfig::Defaults::SWING;
                    player.energyValue = INIConfig::Defaults::ENERGY;
                    player.volume = INIConfig::Defaults::VOLUME;
                }

                // Use INIDataManager to save the preset properly
                // First save with a temporary name and then rename the file to correct category
                iniDataManager->savePreset(presetName, sampleState);

                // Move the preset file to correct category if it was created in the wrong location
                auto userDir = presetsDir.getChildFile("User");
                auto wrongFile = userDir.getChildFile(presetName + ".ini");
                if (wrongFile.existsAsFile() && category.categoryName != "User") {
                    wrongFile.moveFileTo(presetFile);
                }
            }
        }
    }
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
        presetDisplayLabel.setText(currentPresetName, juce::dontSendNotification);

        // Ensure label is shown when using chevrons
        showPresetLabel();

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
        presetDisplayLabel.setText(currentPresetName, juce::dontSendNotification);

        // Show label by default when preset is programmatically set
        showPresetLabel();

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

    // REMOVED: using namespace to prevent conflicts with responsive spacing calculations
    // Each INI constant will be explicitly qualified when needed

    // ==================================================================================
    // TRULY RESPONSIVE DESIGN - Cross-Platform Multi-Device Layout System
    // ==================================================================================
    // This system calculates ALL dimensions based on CURRENT interface size for:
    // ✅ Smooth scaling at any interface size (no more "too small" icons when shrunk)
    // ✅ Proportional spacing that scales with interface width
    // ✅ Cross-platform compatibility (mobile, tablet, desktop)
    // ✅ Multi-device responsive behavior (phone to ultra-wide monitors)
    // ==================================================================================

    int currentWidth = bounds.getWidth();
    int currentHeight = bounds.getHeight();

    // UNIFIED RESPONSIVE SYSTEM: Calculate balanced scale using both width and height with safeguards
    int actualInterfaceWidth = layoutManager.getWindowWidth();
    int actualInterfaceHeight = layoutManager.getWindowHeight();

    // Calculate responsive scale factor using MINIMUM of width/height ratios to prevent extreme sizing
    float widthScale = static_cast<float>(actualInterfaceWidth) / 1000.0f;  // Normalize to 1000px base
    float heightScale = static_cast<float>(actualInterfaceHeight) / 750.0f; // Normalize to 750px base
    float responsiveScale = juce::jmin(widthScale, heightScale); // Use minimum to prevent oversizing

    // Apply safeguards to prevent extreme scaling
    responsiveScale = juce::jlimit(0.6f, 2.0f, responsiveScale); // Clamp between 60% and 200%

    // UNIFIED ICON SIZE: Smaller boxes for tighter layout
    int baseIconSize = 45; // Reduced from 80px - make icon boxes smaller
    int iconSize = static_cast<int>(baseIconSize * responsiveScale);
    iconSize = juce::jlimit(28, 70, iconSize); // Smaller range - tighter icon boxes
    int iconY = (currentHeight - iconSize) / 2;

    // Update BPM label font with smaller sizing, especially at minimum scale
    // Reduced from 0.4f to 0.3f for better fit in constrained width
    float tempoFontScale = responsiveScale < 0.8f ? 0.25f : 0.3f; // Even smaller at very small scales
    bpmLabel.setFont(fontManager.getFont(FontManager::FontRole::Version,
                     layoutManager.scaled(static_cast<float>(iconSize) * tempoFontScale)));

    // RESPONSIVE MARGIN: Smaller at minimum scales to maximize icon space
    int margin = static_cast<int>(actualInterfaceWidth * 0.015f);
    margin = juce::jlimit(4, 40, margin); // Reduced minimum from 8px to 4px for tight layouts

    // LAYOUT VALIDATION: ALL elements must remain visible at all times!
    // Use our actual small spacing for layout validation (not percentage-based)
    int estimatedSpacing = 2; // Use same small spacing as we'll actually use
    int allIconCount = 10; // All icon buttons that must remain visible
    int totalAllIconsWidth = allIconCount * iconSize;
    int totalAllSpacingWidth = (allIconCount + 2) * estimatedSpacing; // +2 for spacing around preset menu and BPM

    // Add widths for non-icon elements that must be visible
    int estimatedPresetsMenuWidth = static_cast<int>(actualInterfaceWidth * 0.11f); // 11% for preset menu (33% increase)
    int estimatedBmpWidth = static_cast<int>(actualInterfaceWidth * 0.053f);        // 5.3% for BPM display (33% increase)
    int estimatedOttoWidth = static_cast<int>(actualInterfaceWidth * 0.08f);        // 8% for OTTO logo

    int totalRequiredWidth = totalAllIconsWidth + totalAllSpacingWidth +
                            estimatedPresetsMenuWidth + estimatedBmpWidth + estimatedOttoWidth + (2 * margin);

    // If layout doesn't fit, reduce icon size proportionally
    if (totalRequiredWidth > currentWidth) {
        float reductionFactor = static_cast<float>(currentWidth) / static_cast<float>(totalRequiredWidth);
        reductionFactor = juce::jmax(0.65f, reductionFactor); // Don't reduce below 65% (less aggressive)

        iconSize = static_cast<int>(iconSize * reductionFactor);
        iconSize = juce::jmax(24, iconSize); // Smaller minimum for tighter boxes
        iconY = (currentHeight - iconSize) / 2; // Recalculate Y position
    }

    // ICON-WIDTH-BASED SPACING: Calculate AFTER any icon size adjustments
    // This ensures spacing is always proportional to the FINAL icon size
    int iconWidth = iconSize; // Icons are square, so width = final iconSize
    int iconSpacing = 2; // FIXED SMALL SPACING - test if percentage calculation is the problem

    // Left side controls: Gear, Link, Cloud buttons - positioned with responsive calculations
    int gearX = margin;
    int linkX = gearX + iconSize + iconSpacing;
    int cloudX = linkX + iconSize + iconSpacing;



    gearButton.setBounds(gearX, iconY, iconSize, iconSize);
    linkButton.setBounds(linkX, iconY, iconSize, iconSize);
    cloudButton.setBounds(cloudX, iconY, iconSize, iconSize);

    // ✅ PHOSPHOR ICON FONT SCALING - COMPLETED via LookAndFeel system
    // In JUCE 8, button fonts are controlled by the LookAndFeel, not directly by setFont()
    // FIXED: Implemented in CustomLookAndFeel.cpp drawIconButton() with box-based compensation

    // Center transport controls - responsive centering (calculate first for reference)
    int playX = (currentWidth - iconSize) / 2;

    // Preset navigation controls - centered dropdown with ultra-tight chevron spacing
    int leftChevronX = cloudX + iconSize + iconSpacing;

    // Calculate total available space for preset area (left chevron to play button)
    int totalPresetAreaSpace = playX - leftChevronX - iconSpacing;

    // Define preset menu width constraints based on actual interface width - 1/3 wider
    int minPresetMenuWidth = static_cast<int>(actualInterfaceWidth * 0.11f);  // 11% minimum for "Default" (33% increase from 8%)
    int maxPresetMenuWidth = static_cast<int>(actualInterfaceWidth * 0.16f);  // 16% maximum (33% increase from 12%)

    // Calculate optimal preset menu width (total space minus 2 chevrons and ultra-tight spacing)
    int chevronSpacing = static_cast<int>(actualInterfaceWidth * 0.0005f);  // 0.05% ultra-tight chevron spacing
    int chevronSpacing2 = juce::jmax(1, chevronSpacing); // Ensure at least 1px

    int availableMenuSpace = totalPresetAreaSpace - (2 * iconSize) - (2 * chevronSpacing2);
    int presetMenuWidth = juce::jmax(minPresetMenuWidth, juce::jmin(maxPresetMenuWidth, availableMenuSpace));

    // Center the entire preset group (left chevron + menu + right chevron) in available space
    int totalPresetGroupWidth = iconSize + chevronSpacing2 + presetMenuWidth + chevronSpacing2 + iconSize;
    int presetGroupStartX = leftChevronX + (totalPresetAreaSpace - totalPresetGroupWidth) / 2;

    // Calculate final positions with centering
    int centeredLeftChevronX = presetGroupStartX;
    int presetsMenuX = centeredLeftChevronX + iconSize + chevronSpacing2;
    int rightChevronX = presetsMenuX + presetMenuWidth + chevronSpacing2;

    // Ensure we don't overlap with play button
    if (rightChevronX + iconSize + iconSpacing > playX) {
        // Fallback: right-align to play button with minimum spacing
        rightChevronX = playX - iconSpacing - iconSize;
        presetsMenuX = rightChevronX - chevronSpacing2 - presetMenuWidth;
        centeredLeftChevronX = presetsMenuX - chevronSpacing2 - iconSize;
    }

    leftChevronButton.setBounds(centeredLeftChevronX, iconY, iconSize, iconSize);
    presetsMenu.setBounds(presetsMenuX, iconY, presetMenuWidth, iconSize);
    presetDisplayLabel.setBounds(presetsMenuX, iconY, presetMenuWidth, iconSize);
    rightChevronButton.setBounds(rightChevronX, iconY, iconSize, iconSize);
    playButton.setBounds(playX, iconY, iconSize, iconSize);
    pauseButton.setBounds(playX, iconY, iconSize, iconSize);

    // ✅ PHOSPHOR ICON FONT SCALING for transport buttons - COMPLETED via LookAndFeel

    // Right side branding - calculate first to prevent overlap (responsive positioning with proper width for full "OTTO" text)
    int ottoWidth = static_cast<int>(actualInterfaceWidth * 0.1f); // 10% of actual interface width (balanced for readability)
    int ottoX = currentWidth - ottoWidth - margin;
    int ottoHeight = static_cast<int>(currentHeight * 0.6f); // 60% of row height
    int versionHeight = static_cast<int>(currentHeight * 0.25f); // 25% of row height

    ottoLabel.setBounds(ottoX, iconY, ottoWidth, ottoHeight);
    versionLabel.setBounds(ottoX, iconY + ottoHeight, ottoWidth, versionHeight);

    // Tempo controls - responsive layout that respects OTTO logo space
    int bpmX = playX + iconSize + iconSpacing;
    int bpmHeight = iconSize; // Same height as icons for consistency

    // Calculate available space between play button and OTTO logo for all right-side controls
    int totalAvailableRightSpace = ottoX - bpmX - iconSpacing;

    // BPM width: 1/3 wider allocation for tempo decimals
    int absoluteMinBpmWidth = static_cast<int>(iconSize * 2.6f);  // 260% of icon size - 33% increase from 200%
    int preferredMinBmpWidth = static_cast<int>(actualInterfaceWidth * 0.08f);   // 8% of actual interface width (33% increase from 6%)
    int maxBmpWidth = static_cast<int>(actualInterfaceWidth * 0.133f);   // 13.3% maximum - 33% increase from 10%

    // Use the larger of absolute minimum or preferred minimum, but cap at maximum
    int bpmWidth = juce::jmax(absoluteMinBpmWidth, preferredMinBmpWidth);
    bpmWidth = juce::jmin(bpmWidth, maxBmpWidth);

    // Final safety - ensure tempo is always readable
    bpmWidth = juce::jmax(bpmWidth, absoluteMinBpmWidth);

    bpmLabel.setBounds(bpmX, iconY, bpmWidth, bpmHeight);
    clockSyncLabel.setBounds(bpmX, iconY + iconSize - static_cast<int>(currentHeight * 0.01875f),
                           bpmWidth, static_cast<int>(currentHeight * 0.01875f));

    // Right-side icons positioned to be right-aligned to OTTO logo (right-aligned layout)
    // This gives tempo maximum space and creates cleaner visual alignment
    int loopX = ottoX - margin - iconSize;  // Start from OTTO logo and work backwards
    int overdubX = loopX - iconSize - iconSpacing;
    int recordX = overdubX - iconSize - iconSpacing;
    int tapTempoX = recordX - iconSize - iconSpacing;

    // Position ALL icons to "hug" the tempo display, then control visibility based on space
    tapTempoButton.setBounds(tapTempoX, iconY, iconSize, iconSize);
    tapTempoLabel.setBounds(tapTempoX, iconY + iconSize - static_cast<int>(currentHeight * 0.01875f),
                          iconSize, static_cast<int>(currentHeight * 0.01875f));
    recordButton.setBounds(recordX, iconY, iconSize, iconSize);
    overdubButton.setBounds(overdubX, iconY, iconSize, iconSize);
    loopButton.setBounds(loopX, iconY, iconSize, iconSize);

    // ✅ PHOSPHOR ICON FONT SCALING for recording buttons - COMPLETED via LookAndFeel

    // ALL elements must remain visible at all times - no visibility controls needed
    // Layout validation above ensures everything fits through proportional scaling

    // Bottom separator - responsive thickness
    int separatorThickness = juce::jmax(1, static_cast<int>(currentHeight * 0.05f)); // 5% of row height, min 1px
    bottomSeparator.setBounds(0, currentHeight - separatorThickness, bounds.getWidth(), separatorThickness);

    // Force ComboBox to refresh its font after resize - fixes font reverting bug
    presetsMenu.lookAndFeelChanged();
}

void TopBarComponent::togglePresetDisplay() {
    showingPresetLabel = !showingPresetLabel;
    updatePresetDisplayToggle();
}

void TopBarComponent::showPresetLabel() {
    showingPresetLabel = true;
    updatePresetDisplayToggle();
}

void TopBarComponent::showPresetMenu() {
    showingPresetLabel = false;
    updatePresetDisplayToggle();

    // Show the preset menu dropdown
    buildHierarchicalPresetMenu();
}

void TopBarComponent::updatePresetDisplayToggle() {
    if (showingPresetLabel) {
        // Show large preset label, hide dropdown menu
        presetDisplayLabel.setVisible(true);
        presetsMenu.setVisible(false);
    } else {
        // Show dropdown menu, hide large preset label
        presetDisplayLabel.setVisible(false);
        presetsMenu.setVisible(true);
    }
}

void TopBarComponent::mouseDown(const juce::MouseEvent& event) {
    // Check if click was on the preset display label
    if (event.eventComponent == &presetDisplayLabel && showingPresetLabel) {
        // User clicked the preset label - show menu
        showPresetMenu();
    }
}

void TopBarComponent::refreshPresetLabelFont() {
    // Force refresh the preset label font - make it prominent and readable
    presetDisplayLabel.setFont(fontManager.getFont(FontManager::FontRole::Header,
                               layoutManager.scaled(INIConfig::LayoutConstants::Row1::ottoHeight * 2.4f)));

    // Also refresh the color to ensure consistency
    presetDisplayLabel.setColour(juce::Label::textColourId,
                                colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));

    // Force a repaint to show the changes
    presetDisplayLabel.repaint();
}
