#pragma once

#include <JuceHeader.h>
#include "ResponsiveLayoutManager.h"
#include "FontManager.h"
#include "ColorScheme.h"
#include "INIConfig.h"

class MidiEngine;

class SceneLauncherComponent : public juce::Component, public juce::Timer {
public:
    SceneLauncherComponent(ResponsiveLayoutManager& layoutManager,
                          FontManager& fontManager,
                          ColorScheme& colorScheme);
    ~SceneLauncherComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

    void setMidiEngine(MidiEngine* engine) { midiEngine = engine; }
    void updateSceneStates();
    void setActiveScene(int sceneIndex);
    void initialise();

    std::function<void(int)> onSceneTriggered;
    std::function<void(int, const juce::String&)> onSceneRenamed;

private:
    MidiEngine* midiEngine = nullptr;
    ResponsiveLayoutManager& layoutManager;
    FontManager& fontManager;
    ColorScheme& colorScheme;

    struct SceneButton : public juce::Component {
        SceneButton(int index) : sceneIndex(index) {}

        void paint(juce::Graphics& g) override;
        void mouseDown(const juce::MouseEvent& e) override;
        void mouseDoubleClick(const juce::MouseEvent& e) override;

        int sceneIndex;
        juce::String sceneName;
        bool isActive = false;
        bool isQueued = false;
        bool isEmpty = true;
        ColorScheme* colorScheme = nullptr;

        std::function<void()> onClick;
        std::function<void()> onDoubleClick;
    };

    juce::OwnedArray<SceneButton> sceneButtons;
    juce::Label titleLabel;
    juce::TextButton masterButton;

    int activeSceneIndex = INIConfig::MIDI::INACTIVE_SCENE;

    void setupComponents();
    void triggerScene(int sceneIndex);
    void renameScene(int sceneIndex);
    void updateQueuedStates();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SceneLauncherComponent)
};
