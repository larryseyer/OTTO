#include "SceneLauncherComponent.h"
#include "MidiEngine.h"
#include "INIConfig.h"

SceneLauncherComponent::SceneLauncherComponent(ResponsiveLayoutManager& layoutManager,
                                             FontManager& fontManager,
                                             ColorScheme& colorScheme)
    : layoutManager(layoutManager),
      fontManager(fontManager), colorScheme(colorScheme) {

    setupComponents();
}

SceneLauncherComponent::~SceneLauncherComponent() {
    stopTimer();
}

void SceneLauncherComponent::initialise() {
    if (midiEngine) {
        for (int i = 0; i < INIConfig::LayoutConstants::numScenes; ++i) {
            auto scene = midiEngine->getScene(i);
            sceneButtons[i]->isEmpty = scene.name.isEmpty();
            if (!sceneButtons[i]->isEmpty) {
                sceneButtons[i]->sceneName = scene.name;
            }
        }
        updateSceneStates();
        startTimerHz(INIConfig::LayoutConstants::sceneLauncherTimerHz);
    }
}

void SceneLauncherComponent::setupComponents() {
    addAndMakeVisible(titleLabel);
    titleLabel.setText("SCENES", juce::dontSendNotification);
    titleLabel.setColour(juce::Label::textColourId, colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
    titleLabel.setJustificationType(juce::Justification::centred);

    for (int i = 0; i < INIConfig::LayoutConstants::numScenes; ++i) {
        auto* button = sceneButtons.add(new SceneButton(i));
        addAndMakeVisible(button);
        button->colorScheme = &colorScheme;
        button->sceneName = "Scene " + juce::String(i + 1);

        button->onClick = [this, i] { triggerScene(i); };
        button->onDoubleClick = [this, i] { renameScene(i); };
    }

    addAndMakeVisible(masterButton);
    masterButton.setButtonText("MASTER");
    masterButton.onClick = [this] {
        setActiveScene(-1);
    };
}

void SceneLauncherComponent::triggerScene(int sceneIndex) {
    if (midiEngine && sceneIndex >= 0 && sceneIndex < INIConfig::LayoutConstants::numScenes) {
        midiEngine->queueSceneChange(sceneIndex, midiEngine->getQuantize());

        if (onSceneTriggered) {
            onSceneTriggered(sceneIndex);
        }

        updateSceneStates();
    }
}

void SceneLauncherComponent::renameScene(int sceneIndex) {
    if (!midiEngine || sceneIndex < 0 || sceneIndex >= INIConfig::LayoutConstants::numScenes) return;

    auto* dialog = new juce::AlertWindow("Rename Scene",
                                        "Enter new name for " + sceneButtons[sceneIndex]->sceneName,
                                        juce::AlertWindow::NoIcon);

    dialog->addTextEditor("name", sceneButtons[sceneIndex]->sceneName);
    dialog->addButton("OK", 1);
    dialog->addButton("Cancel", 0);

    dialog->enterModalState(true, juce::ModalCallbackFunction::create(
        [this, sceneIndex, dialog](int result) {
            if (result == 1) {
                juce::String newName = dialog->getTextEditor("name")->getText();
                if (newName.isNotEmpty()) {
                    sceneButtons[sceneIndex]->sceneName = newName;
                    if (midiEngine) {
                        midiEngine->saveScene(sceneIndex, newName);
                    }

                    if (onSceneRenamed) {
                        onSceneRenamed(sceneIndex, newName);
                    }
                }
            }
            delete dialog;
        }), true);
}

void SceneLauncherComponent::setActiveScene(int sceneIndex) {
    activeSceneIndex = sceneIndex;
    updateSceneStates();
}

void SceneLauncherComponent::updateSceneStates() {
    if (!midiEngine) return;

    int currentActiveScene = midiEngine->getActiveSceneIndex();

    for (int i = 0; i < sceneButtons.size(); ++i) {
        sceneButtons[i]->isActive = (i == currentActiveScene);
        sceneButtons[i]->repaint();
    }
}

void SceneLauncherComponent::updateQueuedStates() {
    if (!midiEngine) return;

    const auto& queuedChanges = midiEngine->getQueuedChanges();

    for (auto* button : sceneButtons) {
        button->isQueued = false;
    }

    for (const auto& change : queuedChanges) {
        if (change.type == MidiEngine::QueuedChange::Scene) {
            if (change.targetIndex >= 0 && change.targetIndex < sceneButtons.size()) {
                sceneButtons[change.targetIndex]->isQueued = true;
            }
        }
    }

    for (auto* button : sceneButtons) {
        button->repaint();
    }
}

void SceneLauncherComponent::timerCallback() {
    updateQueuedStates();
}

void SceneLauncherComponent::paint(juce::Graphics& g) {
    g.fillAll(colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground));

    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::Separator));
    g.drawRect(getLocalBounds(), INIConfig::LayoutConstants::sceneLauncherBorderThickness);
}

void SceneLauncherComponent::resized() {
    auto bounds = getLocalBounds().reduced(layoutManager.scaled(INIConfig::LayoutConstants::sceneLauncherPadding));

    int titleHeight = layoutManager.scaled(INIConfig::LayoutConstants::sceneLauncherTitleHeight);
    titleLabel.setBounds(bounds.removeFromTop(titleHeight));

    bounds.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::sceneLauncherButtonSpacing));

    int buttonHeight = layoutManager.scaled(INIConfig::LayoutConstants::sceneLauncherButtonHeight);
    int spacing = layoutManager.scaled(INIConfig::LayoutConstants::sceneLauncherButtonSpacing);

    for (auto* button : sceneButtons) {
        button->setBounds(bounds.removeFromTop(buttonHeight));
        bounds.removeFromTop(spacing);
    }

    bounds.removeFromBottom(layoutManager.scaled(INIConfig::LayoutConstants::sceneLauncherBottomMargin));
    masterButton.setBounds(bounds);
}

void SceneLauncherComponent::SceneButton::paint(juce::Graphics& g) {
    if (!colorScheme) return;

    auto bounds = getLocalBounds().toFloat();

    juce::Colour bgColor;
    if (isActive) {
        bgColor = colorScheme->getColor(ColorScheme::ColorRole::Accent);
    } else if (isQueued) {
        bgColor = colorScheme->getColor(ColorScheme::ColorRole::AccentHover);
    } else if (isEmpty) {
        bgColor = colorScheme->getColor(ColorScheme::ColorRole::ButtonBackground).withAlpha(INIConfig::LayoutConstants::sceneLauncherInactiveAlpha);
    } else {
        bgColor = colorScheme->getColor(ColorScheme::ColorRole::ButtonBackground);
    }

    g.setColour(bgColor);
    g.fillRoundedRectangle(bounds, INIConfig::LayoutConstants::sceneLauncherButtonCornerRadius);

    if (isActive || isQueued) {
        g.setColour(colorScheme->getColor(ColorScheme::ColorRole::Accent));
        g.drawRoundedRectangle(bounds.reduced(INIConfig::LayoutConstants::sceneLauncherBorderReduction),
                              INIConfig::LayoutConstants::sceneLauncherButtonCornerRadius,
                              INIConfig::LayoutConstants::sceneLauncherButtonBorderWidth);
    }

    g.setColour(isActive ? colorScheme->getColor(ColorScheme::ColorRole::ComponentBackground) :
                          colorScheme->getColor(ColorScheme::ColorRole::ButtonText));
    g.drawText(sceneName, bounds, juce::Justification::centred);
}

void SceneLauncherComponent::SceneButton::mouseDown(const juce::MouseEvent& e) {
    if (e.mods.isLeftButtonDown() && onClick) {
        onClick();
    }
}

void SceneLauncherComponent::SceneButton::mouseDoubleClick(const juce::MouseEvent& e) {
    if (e.mods.isLeftButtonDown() && onDoubleClick) {
        onDoubleClick();
    }
}
