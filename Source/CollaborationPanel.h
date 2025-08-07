#pragma once
#include <JuceHeader.h>
#include "CloudServiceManager.h"
#include "MidiEngine.h"
#include "INIConfig.h"

class CollaborationPanel : public juce::Component,
                          public juce::Timer {
public:
    struct Collaborator {
        juce::String userId;
        juce::String username;
        juce::Colour color;
        int activePlayer = INIConfig::MIDI::INACTIVE_PATTERN;
        bool isActive = true;
    };

    struct SessionInfo {
        juce::String sessionId;
        juce::String sessionName;
        juce::String hostId;
        juce::Array<Collaborator> collaborators;
        bool isActive = false;
    };

    CollaborationPanel(CloudServiceManager& cloudService,
                      MidiEngine& midiEngine,
                      ResponsiveLayoutManager& layoutManager,
                      FontManager& fontManager,
                      ColorScheme& colorScheme);

    ~CollaborationPanel() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;

    void createNewSession(const juce::String& name);
    void joinSession(const juce::String& sessionId);
    void leaveSession();

    void sendPlayerUpdate(int playerIndex);
    void sendPatternChange(int playerIndex, const juce::MidiMessageSequence& pattern);
    void sendMixerChange(int channel, float volume, float pan);

    std::function<void(const Collaborator&, int playerIndex)> onCollaboratorActivity;
    std::function<void(const juce::String&)> onChatMessageReceived;

private:
    CloudServiceManager& cloudService;
    MidiEngine& midiEngine;
    ResponsiveLayoutManager& layoutManager;
    FontManager& fontManager;
    ColorScheme& colorScheme;

    juce::Label sessionLabel;
    juce::TextButton createSessionButton;
    juce::TextButton joinSessionButton;
    juce::TextEditor sessionIdInput;

    class CollaboratorList : public juce::Component {
    public:
        void updateCollaborators(const juce::Array<Collaborator>& collaborators);
        void paint(juce::Graphics& g) override;
    private:
        juce::Array<Collaborator> currentCollaborators;
    };

    CollaboratorList collaboratorList;

    juce::TextEditor chatDisplay;
    juce::TextEditor chatInput;
    juce::TextButton sendButton;

    SessionInfo currentSession;

    void handleCollaborationData(const juce::String& data);
    void updateSessionDisplay();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CollaborationPanel)
};
