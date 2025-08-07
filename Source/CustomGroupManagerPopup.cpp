#include "CustomGroupManagerPopup.h"
#include "INIConfig.h"

CustomGroupManagerPopup::CustomGroupManagerPopup(Mode mode,
                                               const juce::String& currentGroupName,
                                               ResponsiveLayoutManager& layoutManager,
                                               FontManager& fontManager,
                                               ColorScheme& colorScheme,
                                               ComponentState& componentState)
    : mode(mode), currentGroupName(currentGroupName),
      layoutManager(layoutManager), fontManager(fontManager), colorScheme(colorScheme),
      componentState(componentState), closeButton("close") {
    setupComponents();
}

void CustomGroupManagerPopup::setupComponents() {
    addAndMakeVisible(titleLabel);
    addAndMakeVisible(messageLabel);
    addAndMakeVisible(confirmButton);
    addAndMakeVisible(cancelButton);
    addAndMakeVisible(closeButton);

    titleLabel.setColour(juce::Label::textColourId, colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    titleLabel.setJustificationType(juce::Justification::centred);

    messageLabel.setColour(juce::Label::textColourId, colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
    messageLabel.setJustificationType(juce::Justification::centred);

    confirmButton.setColour(juce::TextButton::buttonColourId, colorScheme.getColor(ColorScheme::ColorRole::ButtonBackground));
    confirmButton.setColour(juce::TextButton::textColourOnId, colorScheme.getColor(ColorScheme::ColorRole::ButtonText));
    confirmButton.setColour(juce::TextButton::textColourOffId, colorScheme.getColor(ColorScheme::ColorRole::ButtonText));

    cancelButton.setColour(juce::TextButton::buttonColourId, colorScheme.getColor(ColorScheme::ColorRole::ButtonBackground));
    cancelButton.setColour(juce::TextButton::textColourOnId, colorScheme.getColor(ColorScheme::ColorRole::ButtonText));
    cancelButton.setColour(juce::TextButton::textColourOffId, colorScheme.getColor(ColorScheme::ColorRole::ButtonText));

    closeButton.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
    closeButton.setColour(juce::TextButton::textColourOnId, colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
    closeButton.setColour(juce::TextButton::textColourOffId, colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));

    switch (mode) {
        case Mode::CreateGroup:
            titleLabel.setText("Create New MIDI Group", juce::dontSendNotification);
            messageLabel.setText("Enter a name for the new MIDI group:", juce::dontSendNotification);
            confirmButton.setButtonText("Create");

            addAndMakeVisible(groupNameEditor);
            groupNameEditor.setColour(juce::TextEditor::backgroundColourId, colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground));
            groupNameEditor.setColour(juce::TextEditor::textColourId, colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
            groupNameEditor.setColour(juce::TextEditor::outlineColourId, colorScheme.getColor(ColorScheme::ColorRole::Separator));
            groupNameEditor.setColour(juce::TextEditor::focusedOutlineColourId, colorScheme.getColor(ColorScheme::ColorRole::Accent));
            groupNameEditor.setTextToShowWhenEmpty("Group Name", colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
            groupNameEditor.onReturnKey = [this] { handleConfirm(); };
            groupNameEditor.onEscapeKey = [this] { handleCancel(); };
            break;

        case Mode::DeleteGroup:
            titleLabel.setText("Delete MIDI Group", juce::dontSendNotification);
            messageLabel.setText(juce::String("Are you sure you want to delete \"") + currentGroupName + juce::String("\"?"), juce::dontSendNotification);
            confirmButton.setButtonText("Delete");
            confirmButton.setColour(juce::TextButton::buttonColourId, colorScheme.getColor(ColorScheme::ColorRole::Error));
            break;

        case Mode::EditGroup:
            titleLabel.setText("Update MIDI Group", juce::dontSendNotification);
            messageLabel.setText(juce::String("Update \"") + currentGroupName + juce::String("\" with current MIDI assignments?"), juce::dontSendNotification);
            confirmButton.setButtonText("Update");
            break;
    }

    cancelButton.setButtonText("Cancel");

    confirmButton.onClick = [this] { handleConfirm(); };
    cancelButton.onClick = [this] { handleCancel(); };
    closeButton.onClick = [this] { handleCancel(); };

    if (mode == Mode::CreateGroup) {
        juce::Timer::callAfterDelay(INIConfig::LayoutConstants::customGroupTimerDelay, [this] {
            groupNameEditor.grabKeyboardFocus();
        });
    }
}

void CustomGroupManagerPopup::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0x60000000));

    auto bounds = getLocalBounds();
    auto panelBounds = bounds.withSizeKeepingCentre(layoutManager.scaled(INIConfig::LayoutConstants::customGroupPopupWidth), layoutManager.scaled(INIConfig::LayoutConstants::customGroupPopupHeight));

    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::WindowBackground));
    g.fillRoundedRectangle(panelBounds.toFloat(), layoutManager.scaled(INIConfig::LayoutConstants::customGroupCornerRadius));

    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::Separator));
    g.drawRoundedRectangle(panelBounds.toFloat(), layoutManager.scaled(INIConfig::LayoutConstants::customGroupCornerRadius), layoutManager.scaled(INIConfig::LayoutConstants::customGroupBorderThickness));
}

void CustomGroupManagerPopup::resized() {
    auto bounds = getLocalBounds();
    auto panelBounds = bounds.withSizeKeepingCentre(layoutManager.scaled(INIConfig::LayoutConstants::customGroupPopupWidth), layoutManager.scaled(INIConfig::LayoutConstants::customGroupPopupHeight));

    int margin = layoutManager.scaled(INIConfig::LayoutConstants::customGroupContentMargin);
    auto contentBounds = panelBounds.reduced(margin);

    int closeButtonSize = layoutManager.scaled(INIConfig::LayoutConstants::customGroupCloseButtonSize);
    closeButton.setBounds(panelBounds.getRight() - layoutManager.scaled(INIConfig::LayoutConstants::customGroupCloseButtonX),
                         panelBounds.getY() + layoutManager.scaled(INIConfig::LayoutConstants::customGroupCloseButtonY),
                         closeButtonSize, closeButtonSize);

    titleLabel.setBounds(contentBounds.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::customGroupTitleHeight)));
    contentBounds.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::customGroupTitleSpacing));

    messageLabel.setBounds(contentBounds.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::customGroupMessageHeight)));
    contentBounds.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::customGroupButtonSpacing));

    if (mode == Mode::CreateGroup) {
        groupNameEditor.setBounds(contentBounds.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::customGroupEditorHeight)));
        contentBounds.removeFromTop(layoutManager.scaled(INIConfig::LayoutConstants::customGroupEditorSpacing));
    }

    auto buttonArea = contentBounds.removeFromBottom(layoutManager.scaled(INIConfig::LayoutConstants::customGroupButtonAreaHeight));
    int buttonWidth = layoutManager.scaled(INIConfig::LayoutConstants::customGroupButtonWidth);
    int buttonSpacing = layoutManager.scaled(INIConfig::LayoutConstants::customGroupButtonSpacing);

    confirmButton.setBounds(buttonArea.removeFromRight(buttonWidth));
    buttonArea.removeFromRight(buttonSpacing);
    cancelButton.setBounds(buttonArea.removeFromRight(buttonWidth));
}

void CustomGroupManagerPopup::mouseDown(const juce::MouseEvent& event) {
    auto bounds = getLocalBounds();
    auto panelBounds = bounds.withSizeKeepingCentre(layoutManager.scaled(INIConfig::LayoutConstants::customGroupPopupWidth), layoutManager.scaled(INIConfig::LayoutConstants::customGroupPopupHeight));

    if (!panelBounds.contains(event.getPosition())) {
        handleCancel();
    }
}

void CustomGroupManagerPopup::handleConfirm() {
    switch (mode) {
        case Mode::CreateGroup:
            if (onCreateGroup) {
                juce::String groupName = groupNameEditor.getText().trim();
                if (groupName.isNotEmpty()) {
                    onCreateGroup(groupName);
                }
            }
            break;

        case Mode::DeleteGroup:
            if (onDeleteGroup) {
                onDeleteGroup();
            }
            break;

        case Mode::EditGroup:
            if (onEditGroup) {
                onEditGroup();
            }
            break;
    }
}

void CustomGroupManagerPopup::handleCancel() {
    if (onCancel) {
        onCancel();
    }
}

void CustomGroupManagerPopup::saveState(ComponentState& state) {
    state.customGroupManagerState.pendingGroupName = groupNameEditor.getText();
    state.customGroupManagerState.currentEditingGroup = currentGroupName;
    state.customGroupManagerState.hasUnsavedChanges = (mode == Mode::CreateGroup && groupNameEditor.getText().isNotEmpty());
}

void CustomGroupManagerPopup::loadState(const ComponentState& state) {
    if (mode == Mode::CreateGroup && state.customGroupManagerState.pendingGroupName.isNotEmpty()) {
        groupNameEditor.setText(state.customGroupManagerState.pendingGroupName);
    }
}

void CustomGroupManagerPopup::showCreateGroupPopup(juce::Component* parent,
                                                  const juce::Rectangle<int>& bounds,
                                                  ResponsiveLayoutManager& layoutManager,
                                                  FontManager& fontManager,
                                                  ColorScheme& colorScheme,
                                                  ComponentState& componentState,
                                                  std::function<void(const juce::String&)> onCreateGroup,
                                                  std::function<void()> onCancel) {
    // Remove any existing popup first
    for (int i = parent->getNumChildComponents() - 1; i >= 0; --i) {
        if (auto* existingPopup = dynamic_cast<CustomGroupManagerPopup*>(parent->getChildComponent(i))) {
            parent->removeChildComponent(existingPopup);
            delete existingPopup;
            break;
        }
    }
    
    auto popup = std::make_unique<CustomGroupManagerPopup>(Mode::CreateGroup, "", layoutManager, fontManager, colorScheme, componentState);
    popup->setBounds(bounds);
    popup->onCreateGroup = onCreateGroup;
    popup->onCancel = onCancel;
    popup->setName("CustomGroupManagerPopup");
    
    // Transfer ownership to parent component
    parent->addAndMakeVisible(popup.release());
}

void CustomGroupManagerPopup::showDeleteGroupPopup(juce::Component* parent,
                                                  const juce::Rectangle<int>& bounds,
                                                  const juce::String& groupName,
                                                  ResponsiveLayoutManager& layoutManager,
                                                  FontManager& fontManager,
                                                  ColorScheme& colorScheme,
                                                  ComponentState& componentState,
                                                  std::function<void()> onDeleteGroup,
                                                  std::function<void()> onCancel) {
    // Remove any existing popup first
    for (int i = parent->getNumChildComponents() - 1; i >= 0; --i) {
        if (auto* existingPopup = dynamic_cast<CustomGroupManagerPopup*>(parent->getChildComponent(i))) {
            parent->removeChildComponent(existingPopup);
            delete existingPopup;
            break;
        }
    }
    
    auto popup = std::make_unique<CustomGroupManagerPopup>(Mode::DeleteGroup, groupName, layoutManager, fontManager, colorScheme, componentState);
    popup->setBounds(bounds);
    popup->onDeleteGroup = onDeleteGroup;
    popup->onCancel = onCancel;
    popup->setName("CustomGroupManagerPopup");
    
    // Transfer ownership to parent component
    parent->addAndMakeVisible(popup.release());
}

void CustomGroupManagerPopup::showEditGroupPopup(juce::Component* parent,
                                                const juce::Rectangle<int>& bounds,
                                                const juce::String& groupName,
                                                ResponsiveLayoutManager& layoutManager,
                                                FontManager& fontManager,
                                                ColorScheme& colorScheme,
                                                ComponentState& componentState,
                                                std::function<void()> onEditGroup,
                                                std::function<void()> onCancel) {
    // Remove any existing popup first
    for (int i = parent->getNumChildComponents() - 1; i >= 0; --i) {
        if (auto* existingPopup = dynamic_cast<CustomGroupManagerPopup*>(parent->getChildComponent(i))) {
            parent->removeChildComponent(existingPopup);
            delete existingPopup;
            break;
        }
    }
    
    auto popup = std::make_unique<CustomGroupManagerPopup>(Mode::EditGroup, groupName, layoutManager, fontManager, colorScheme, componentState);
    popup->setBounds(bounds);
    popup->onEditGroup = onEditGroup;
    popup->onCancel = onCancel;
    popup->setName("CustomGroupManagerPopup");
    
    // Transfer ownership to parent component
    parent->addAndMakeVisible(popup.release());
}
