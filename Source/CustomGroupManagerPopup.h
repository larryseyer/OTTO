#pragma once
#include <JuceHeader.h>
#include "UtilityComponents.h"
#include "ResponsiveLayoutManager.h"
#include "FontManager.h"
#include "ColorScheme.h"

class ComponentState;

class CustomGroupManagerPopup : public juce::Component {
public:
    enum class Mode {
        CreateGroup,
        DeleteGroup,
        EditGroup
    };

    CustomGroupManagerPopup(Mode mode,
                           const juce::String& currentGroupName,
                           ResponsiveLayoutManager& layoutManager,
                           FontManager& fontManager,
                           ColorScheme& colorScheme,
                           ComponentState& componentState);

    void paint(juce::Graphics&) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& event) override;

    std::function<void(const juce::String&)> onCreateGroup;
    std::function<void()> onDeleteGroup;
    std::function<void()> onEditGroup;
    std::function<void()> onCancel;

    void saveState(ComponentState& state);
    void loadState(const ComponentState& state);

    static void showCreateGroupPopup(juce::Component* parent,
                                    const juce::Rectangle<int>& bounds,
                                    ResponsiveLayoutManager& layoutManager,
                                    FontManager& fontManager,
                                    ColorScheme& colorScheme,
                                    ComponentState& componentState,
                                    std::function<void(const juce::String&)> onCreateGroup,
                                    std::function<void()> onCancel);

    static void showDeleteGroupPopup(juce::Component* parent,
                                    const juce::Rectangle<int>& bounds,
                                    const juce::String& groupName,
                                    ResponsiveLayoutManager& layoutManager,
                                    FontManager& fontManager,
                                    ColorScheme& colorScheme,
                                    ComponentState& componentState,
                                    std::function<void()> onDeleteGroup,
                                    std::function<void()> onCancel);

    static void showEditGroupPopup(juce::Component* parent,
                                  const juce::Rectangle<int>& bounds,
                                  const juce::String& groupName,
                                  ResponsiveLayoutManager& layoutManager,
                                  FontManager& fontManager,
                                  ColorScheme& colorScheme,
                                  ComponentState& componentState,
                                  std::function<void()> onEditGroup,
                                  std::function<void()> onCancel);

private:
    Mode mode;
    juce::String currentGroupName;
    ResponsiveLayoutManager& layoutManager;
    FontManager& fontManager;
    ColorScheme& colorScheme;
    ComponentState& componentState;

    juce::Label titleLabel;
    juce::Label messageLabel;
    juce::TextEditor groupNameEditor;
    juce::TextButton confirmButton;
    juce::TextButton cancelButton;
    PhosphorIconButton closeButton;

    void setupComponents();
    void handleConfirm();
    void handleCancel();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CustomGroupManagerPopup)
};
