#include "UtilityComponents.h"
#include "ResponsiveLayoutManager.h"
#include "INIConfig.h"

AnimatedComponent::AnimatedComponent() {
    setInterceptsMouseClicks(true, true);
}

AnimatedComponent::~AnimatedComponent() {
    stopTimer();
}

void AnimatedComponent::fadeIn(int durationMs) {
    targetOpacity = 1.0f;
    animationDuration = durationMs;
    animationTime = 0;
    startTimerHz(INIConfig::LayoutConstants::animatedComponentTimerHz);
}

void AnimatedComponent::fadeOut(int durationMs) {
    targetOpacity = 0.0f;
    animationDuration = durationMs;
    animationTime = 0;
    startTimerHz(INIConfig::LayoutConstants::animatedComponentTimerHz);
}

void AnimatedComponent::slideIn(juce::Point<int> from, int durationMs) {
    position = from.toFloat();
    targetPosition = getBounds().getPosition().toFloat();
    animationDuration = durationMs;
    animationTime = 0;
    startTimerHz(INIConfig::LayoutConstants::animatedComponentTimerHz);
}

void AnimatedComponent::slideOut(juce::Point<int> to, int durationMs) {
    targetPosition = to.toFloat();
    animationDuration = durationMs;
    animationTime = 0;
    startTimerHz(INIConfig::LayoutConstants::animatedComponentTimerHz);
}

void AnimatedComponent::timerCallback() {
    animationTime += 1000 / INIConfig::LayoutConstants::animatedComponentTimerHz;

    if (animationTime >= animationDuration) {
        opacity = targetOpacity;
        position = targetPosition;
        stopTimer();

        if (opacity == 0.0f) {
            setVisible(false);
        }
    } else {
        float progress = static_cast<float>(animationTime) / static_cast<float>(animationDuration);
        progress = progress < INIConfig::LayoutConstants::animatedComponentProgressThreshold ?
                   INIConfig::LayoutConstants::animatedComponentQuadraticFactor * progress * progress :
                   1.0f - std::pow(-INIConfig::LayoutConstants::animatedComponentQuadraticFactor * progress + INIConfig::LayoutConstants::animatedComponentQuadraticFactor, INIConfig::LayoutConstants::animatedComponentProgressPower) / INIConfig::LayoutConstants::animatedComponentQuadraticFactor;

        opacity = opacity + (targetOpacity - opacity) * progress;
        position = position + (targetPosition - position) * progress;

        setTopLeftPosition(position.toInt());
    }

    repaint();
}

void AnimatedComponent::paint(juce::Graphics& g) {
    g.saveState();
    g.setOpacity(opacity);
    paintContent(g);
    g.restoreState();
}

std::map<juce::Component*, juce::String> TooltipManager::tooltips;

TooltipManager::TooltipManager() {
    tooltipWindow = std::make_unique<juce::TooltipWindow>();
}

void TooltipManager::addTooltip(juce::Component* component, const juce::String& tooltip) {
    if (component != nullptr) {
        tooltips[component] = tooltip;
        component->setHelpText(tooltip);
    }
}

void TooltipManager::removeTooltip(juce::Component* component) {
    if (component != nullptr) {
        tooltips.erase(component);
        component->setHelpText("");
    }
}

juce::String TooltipManager::getTooltip(juce::Component* component) {
    auto it = tooltips.find(component);
    return (it != tooltips.end()) ? it->second : juce::String();
}

void TooltipManager::showTooltipAt(const juce::String& text, juce::Point<int> screenPos) {
    if (tooltipWindow != nullptr) {
        tooltipWindow->displayTip(screenPos, text);
    }
}

void TooltipManager::hideTooltip() {
    if (tooltipWindow != nullptr) {
        tooltipWindow->hideTip();
    }
}

KeyboardShortcutsOverlay::KeyboardShortcutsOverlay(FontManager& fontManager, ColorScheme& colorScheme)
    : fontManager(fontManager), colorScheme(colorScheme) {
    setVisible(false);
    setAlwaysOnTop(true);

    addShortcut("Space", "Play/Pause");
    addShortcut("Tab", "Next Player");
    addShortcut("Shift+Tab", "Previous Player");
    addShortcut("Ctrl/Cmd+S", "Save Preset");
    addShortcut("Ctrl/Cmd+O", "Load Preset");
    addShortcut("1-8", "Select Player");
    addShortcut("Q-P", "Trigger Pattern");
    addShortcut("Ctrl/Cmd+Z", "Undo");
    addShortcut("Ctrl/Cmd+Y", "Redo");
    addShortcut("F11", "Toggle Full Screen");
    addShortcut("?", "Show This Help");
}

void KeyboardShortcutsOverlay::addShortcut(const juce::String& key, const juce::String& description) {
    shortcuts.add({key, description});
}

void KeyboardShortcutsOverlay::clearShortcuts() {
    shortcuts.clear();
}

void KeyboardShortcutsOverlay::show() {
    setVisible(true);
    fadeIn(INIConfig::LayoutConstants::keyboardOverlayFadeDuration);
}

void KeyboardShortcutsOverlay::hide() {
    fadeOut(INIConfig::LayoutConstants::keyboardOverlayFadeDuration);
}

void KeyboardShortcutsOverlay::paintContent(juce::Graphics& g) {
    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::WindowBackground).withAlpha(INIConfig::LayoutConstants::keyboardOverlayAlpha));
    g.fillRoundedRectangle(getLocalBounds().toFloat(), INIConfig::LayoutConstants::keyboardOverlayCornerRadius);

    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::Separator));
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(INIConfig::LayoutConstants::keyboardOverlayBorderReduction),
                          INIConfig::LayoutConstants::keyboardOverlayCornerRadius, INIConfig::LayoutConstants::keyboardOverlayBorderWidth);

    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    g.setFont(fontManager.getFont(FontManager::FontRole::Header, INIConfig::LayoutConstants::keyboardOverlayTitleFontSize));
    g.drawText("Keyboard Shortcuts", getLocalBounds().removeFromTop(INIConfig::LayoutConstants::keyboardOverlayTitleHeight),
               juce::Justification::centred);

    int y = INIConfig::LayoutConstants::keyboardOverlayStartY;
    const int lineHeight = INIConfig::LayoutConstants::keyboardOverlayLineHeight;
    g.setFont(fontManager.getFont(FontManager::FontRole::Body, INIConfig::LayoutConstants::keyboardOverlayBodyFontSize));

    for (const auto& shortcut : shortcuts) {
        g.setColour(colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
        juce::Rectangle<int> keyBounds(INIConfig::LayoutConstants::keyboardOverlayKeyX, y,
                                      INIConfig::LayoutConstants::keyboardOverlayKeyWidth, lineHeight);
      g.drawText(shortcut.key, keyBounds, juce::Justification::centredLeft);

      g.setColour(colorScheme.getColor(ColorScheme::ColorRole::SecondaryText).brighter(INIConfig::LayoutConstants::customLookFeelBrighterAmount));
      juce::Rectangle<int> descBounds(INIConfig::LayoutConstants::keyboardOverlayDescX, y,
                                     INIConfig::LayoutConstants::keyboardOverlayDescWidth, lineHeight);
      g.drawText(shortcut.description, descBounds, juce::Justification::centredLeft);

     y += lineHeight;
 }
}

void KeyboardShortcutsOverlay::resized() {
 if (auto* parent = getParentComponent()) {
     setBounds(parent->getLocalBounds().reduced(INIConfig::LayoutConstants::keyboardOverlayReduction));
 }
}

juce::Rectangle<int> MultiMonitorManager::getDisplayAreaForComponent(juce::Component* component) {
 if (component == nullptr) return {};

 auto& displays = juce::Desktop::getInstance().getDisplays();
 auto componentBounds = component->getScreenBounds();

 auto display = displays.getDisplayForRect(componentBounds);
 return display != nullptr ? display->userArea : juce::Rectangle<int>();
}

juce::Array<juce::Rectangle<int>> MultiMonitorManager::getAllDisplayAreas() {
 juce::Array<juce::Rectangle<int>> areas;
 auto& displays = juce::Desktop::getInstance().getDisplays();

 for (int i = 0; i < displays.displays.size(); ++i) {
     areas.add(displays.displays[i].userArea);
 }

 return areas;
}

int MultiMonitorManager::getDisplayIndexForComponent(juce::Component* component) {
 if (component == nullptr) return 0;

 auto& displays = juce::Desktop::getInstance().getDisplays();
 auto componentBounds = component->getScreenBounds();

 for (int i = 0; i < displays.displays.size(); ++i) {
     if (displays.displays[i].totalArea.intersects(componentBounds)) {
         return i;
     }
 }

 return 0;
}

void MultiMonitorManager::moveToDisplay(juce::Component* component, int displayIndex) {
 if (component == nullptr) return;

 auto& displays = juce::Desktop::getInstance().getDisplays();
 if (displayIndex >= 0 && displayIndex < displays.displays.size()) {
     auto targetDisplay = displays.displays[displayIndex];
     component->setCentrePosition(targetDisplay.userArea.getCentre());
 }
}

bool MultiMonitorManager::isFullScreenAvailable() {
 return juce::Desktop::getInstance().getKioskModeComponent() == nullptr;
}

void MultiMonitorManager::toggleFullScreen(juce::Component* component) {
 if (component == nullptr) return;

 auto& desktop = juce::Desktop::getInstance();

 if (desktop.getKioskModeComponent() == component) {
     desktop.setKioskModeComponent(nullptr, false);
 } else {
     desktop.setKioskModeComponent(component, false);
 }
}

void AccessibilityHelper::setupAccessibility(juce::Component* component) {
 if (component == nullptr) return;

 component->setWantsKeyboardFocus(true);
 component->setExplicitFocusOrder(0);
}

void AccessibilityHelper::setAccessibleName(juce::Component* component, const juce::String& name) {
 if (component == nullptr) return;

 component->setTitle(name);
 component->setName(name);
}

void AccessibilityHelper::setAccessibleDescription(juce::Component* component, const juce::String& description) {
 if (component == nullptr) return;

 component->setDescription(description);
}

void AccessibilityHelper::announceChange(const juce::String& message) {
#if JUCE_WINDOWS
#elif JUCE_MAC
#endif

}

bool AccessibilityHelper::isScreenReaderActive() {
#if JUCE_WINDOWS
#elif JUCE_MAC
#endif
 return false;
}

void AccessibilityHelper::setupTabOrder(const juce::Array<juce::Component*>& components) {
 for (int i = 0; i < components.size(); ++i) {
     if (components[i] != nullptr) {
         components[i]->setExplicitFocusOrder(i + 1);
     }
 }
}

void AccessibilityHelper::focusNext(juce::Component* currentComponent) {
 if (currentComponent == nullptr) return;

 if (auto* parent = currentComponent->getParentComponent()) {
     parent->moveKeyboardFocusToSibling(true);
 }
}

void AccessibilityHelper::focusPrevious(juce::Component* currentComponent) {
 if (currentComponent == nullptr) return;

 if (auto* parent = currentComponent->getParentComponent()) {
     parent->moveKeyboardFocusToSibling(false);
 }
}

PhosphorIconButton::PhosphorIconButton(const juce::String& iconName,
                                    FontManager::PhosphorWeight weight)
  : juce::TextButton(""), iconName(iconName), phosphorWeight(weight) {
  setButtonText("");
  setSize(INIConfig::LayoutConstants::phosphorIconButtonDefaultSize, INIConfig::LayoutConstants::phosphorIconButtonDefaultSize);
}

void PhosphorIconButton::setPhosphorWeight(FontManager::PhosphorWeight weight) {
 phosphorWeight = weight;
 repaint();
}

void PhosphorIconButton::paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) {
    getLookAndFeel().drawButtonBackground(g, *this, findColour(buttonColourId),
                                         shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
    getLookAndFeel().drawButtonText(g, *this, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
}

void PhosphorIconButton::saveState(ComponentState& state) const {
 state.phosphorIconWeight = INIConfig::clampPhosphorWeight(static_cast<int>(phosphorWeight));
}

void PhosphorIconButton::loadState(const ComponentState& state) {
 int weightValue = INIConfig::clampPhosphorWeight(state.phosphorIconWeight);
 phosphorWeight = static_cast<FontManager::PhosphorWeight>(weightValue);
 repaint();
}

void PhosphorIconButton::mouseEnter(const juce::MouseEvent& e) {
 Button::mouseEnter(e);
 if (animationEnabled) {
     startTimerHz(INIConfig::LayoutConstants::animatedComponentTimerHz);
 }
}

void PhosphorIconButton::mouseExit(const juce::MouseEvent& e) {
 Button::mouseExit(e);
 if (animationEnabled) {
     startTimerHz(INIConfig::LayoutConstants::animatedComponentTimerHz);
 }
}

void PhosphorIconButton::timerCallback() {
 const float targetHover = isMouseOver() ? 1.0f : 0.0f;
 const float animSpeed = INIConfig::LayoutConstants::phosphorIconButtonAnimSpeed;

 hoverAmount += (targetHover - hoverAmount) * animSpeed;

 if (std::abs(hoverAmount - targetHover) < INIConfig::LayoutConstants::phosphorIconButtonAnimThreshold) {
     hoverAmount = targetHover;
     stopTimer();
 }

 repaint();
}

void HierarchicalComboBox::mouseDown(const juce::MouseEvent& event) {
 if (onPopupRequest && event.mods.isLeftButtonDown()) {
     onPopupRequest();
 } else {
     juce::ComboBox::mouseDown(event);
 }
}

void HierarchicalComboBox::showPopup() {
 if (onPopupRequest) {
     onPopupRequest();
 }
}

void HierarchicalComboBox::saveState(ComponentState& state) const {
 juce::String componentId = getComponentID();

 if (componentId.contains("player")) {
     int playerIndex = componentId.getTrailingIntValue();
     if (INIConfig::isValidPlayerIndex(playerIndex)) {
         if (componentId.contains("drumkit")) {
             state.playerSettings[playerIndex].selectedDrumkit = getText();
         }
     }
 } else if (componentId.contains("theme")) {
     state.globalSettings.themeID = getSelectedId();
 }
}

void HierarchicalComboBox::loadState(const ComponentState& state) {
 juce::String componentId = getComponentID();

 if (componentId.contains("player")) {
     int playerIndex = componentId.getTrailingIntValue();
     if (INIConfig::isValidPlayerIndex(playerIndex)) {
         if (componentId.contains("drumkit")) {
             setText(state.playerSettings[playerIndex].selectedDrumkit);
         }
     }
 } else if (componentId.contains("theme")) {
     setSelectedId(state.globalSettings.themeID);
 }
}

EditableNumericLabel::EditableNumericLabel(const juce::String& componentId, float minValue, float maxValue)
 : minValue(minValue), maxValue(maxValue), currentValue(minValue) {

 setComponentID(componentId);
 setText(juce::String(currentValue, INIConfig::LayoutConstants::editableNumericDecimalPlaces), juce::dontSendNotification);
 setEditable(false, true, true);

 onTextChange = [this] {
     juce::String text = getText().trim();
     if (text.isEmpty()) {
         setValue(this->minValue);
         return;
     }

     bool isValid = true;
     int decimalCount = 0;
     for (auto c : text) {
         if (c == '.') {
             decimalCount++;
             if (decimalCount > 1) {
                 isValid = false;
                 break;
             }
         } else if (!juce::CharacterFunctions::isDigit(c)) {
             isValid = false;
             break;
         }
     }

     if (text.length() > INIConfig::LayoutConstants::editableNumericMaxLength || !isValid) {
         setValue(currentValue);
         return;
     }

     float newValue = text.getFloatValue();
     if (newValue >= this->minValue && newValue <= this->maxValue) {
         currentValue = newValue;
         if (onValueChanged) {
             onValueChanged(currentValue);
         }
         setText(juce::String(currentValue, INIConfig::LayoutConstants::editableNumericDecimalPlaces), juce::dontSendNotification);
     } else {
         setValue(currentValue);
     }
 };

 onEditorHide = [this] {
     validateAndSetText();
 };
}

void EditableNumericLabel::setValueRange(float min, float max) {
 minValue = min;
 maxValue = max;
 if (currentValue < minValue || currentValue > maxValue) {
     setValue(juce::jlimit(minValue, maxValue, currentValue));
 }
}

void EditableNumericLabel::setValue(float value) {
 currentValue = juce::jlimit(minValue, maxValue, value);
 setText(juce::String(currentValue, INIConfig::LayoutConstants::editableNumericDecimalPlaces), juce::dontSendNotification);
}

float EditableNumericLabel::getValue() const {
 return currentValue;
}

void EditableNumericLabel::validateAndSetText() {
 juce::String text = getText().trim();
 float newValue = text.getFloatValue();
 if (text.isEmpty() || newValue < minValue || newValue > maxValue) {
     setValue(currentValue);
 }
}

void EditableNumericLabel::saveState(ComponentState& state) const {
 juce::String componentId = getComponentID();

 if (componentId.contains("tempo")) {
     state.globalSettings.tempo = INIConfig::clampTempo(static_cast<int>(currentValue));
 } else if (componentId.contains("swing") && componentId.contains("player")) {
     int playerIndex = componentId.getTrailingIntValue();
     if (INIConfig::isValidPlayerIndex(playerIndex)) {
         state.playerSettings[playerIndex].swingValue = INIConfig::clampSwing(currentValue);
     }
 } else if (componentId.contains("energy") && componentId.contains("player")) {
     int playerIndex = componentId.getTrailingIntValue();
     if (INIConfig::isValidPlayerIndex(playerIndex)) {
         state.playerSettings[playerIndex].energyValue = INIConfig::clampEnergy(currentValue);
     }
 } else if (componentId.contains("volume") && componentId.contains("player")) {
     int playerIndex = componentId.getTrailingIntValue();
     if (INIConfig::isValidPlayerIndex(playerIndex)) {
         state.playerSettings[playerIndex].volumeValue = INIConfig::clampVolume(currentValue);
     }
 } else if (componentId.contains("metronome") && componentId.contains("volume")) {
     state.globalSettings.metronomeVolume = INIConfig::clampVolume(currentValue);
 } else if (componentId.contains("scale")) {
     state.globalSettings.interfaceScale = INIConfig::clampScale(currentValue);
 }
}

void EditableNumericLabel::loadState(const ComponentState& state) {
 juce::String componentId = getComponentID();

 if (componentId.contains("tempo")) {
     setValue(static_cast<float>(state.globalSettings.tempo));
 } else if (componentId.contains("swing") && componentId.contains("player")) {
     int playerIndex = componentId.getTrailingIntValue();
     if (INIConfig::isValidPlayerIndex(playerIndex)) {
         setValue(state.playerSettings[playerIndex].swingValue);
     }
 } else if (componentId.contains("energy") && componentId.contains("player")) {
     int playerIndex = componentId.getTrailingIntValue();
     if (INIConfig::isValidPlayerIndex(playerIndex)) {
         setValue(state.playerSettings[playerIndex].energyValue);
     }
 } else if (componentId.contains("volume") && componentId.contains("player")) {
     int playerIndex = componentId.getTrailingIntValue();
     if (INIConfig::isValidPlayerIndex(playerIndex)) {
         setValue(state.playerSettings[playerIndex].volumeValue);
     }
 } else if (componentId.contains("metronome") && componentId.contains("volume")) {
     setValue(state.globalSettings.metronomeVolume);
 } else if (componentId.contains("scale")) {
     setValue(state.globalSettings.interfaceScale);
 }
}

ScaledSlider::ScaledSlider(const ResponsiveLayoutManager& layoutManager)
 : layoutManager(layoutManager) {
 setSliderStyle(juce::Slider::LinearVertical);
 setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
}

void ScaledSlider::resized() {
 juce::Slider::resized();
}

void ScaledSlider::saveState(ComponentState& state) const {
 juce::String componentId = getComponentID();
 float value = static_cast<float>(getValue());

 if (!componentId.isEmpty()) {
     state.sliderValues[componentId] = value;
 }

 if (componentId.contains("volume") && componentId.contains("player")) {
     int playerIndex = componentId.getTrailingIntValue();
     if (INIConfig::isValidPlayerIndex(playerIndex)) {
         state.playerSettings[playerIndex].volume = INIConfig::clampVolume(value);
     }
 } else if (componentId.contains("pan") && componentId.contains("player")) {
     int playerIndex = componentId.getTrailingIntValue();
     if (INIConfig::isValidPlayerIndex(playerIndex)) {
         state.playerSettings[playerIndex].pan = INIConfig::clampPan(value);
     }
 } else if (componentId.contains("master") && componentId.contains("volume")) {
     state.globalSettings.metronomeVolume = INIConfig::clampVolume(value);
 }
}

void ScaledSlider::loadState(const ComponentState& state) {
 juce::String componentId = getComponentID();

 if (!componentId.isEmpty() && state.sliderValues.find(componentId) != state.sliderValues.end()) {
     setValue(state.sliderValues.at(componentId));
     return;
 }

 if (componentId.contains("volume") && componentId.contains("player")) {
     int playerIndex = componentId.getTrailingIntValue();
     if (INIConfig::isValidPlayerIndex(playerIndex)) {
         setValue(state.playerSettings[playerIndex].volume);
     }
 } else if (componentId.contains("pan") && componentId.contains("player")) {
     int playerIndex = componentId.getTrailingIntValue();
     if (INIConfig::isValidPlayerIndex(playerIndex)) {
         setValue(state.playerSettings[playerIndex].pan);
     }
 } else if (componentId.contains("master") && componentId.contains("volume")) {
     setValue(state.globalSettings.metronomeVolume);
 }
}

SeparatorComponent::SeparatorComponent(ColorScheme& colorScheme, float thickness)
 : colorScheme(colorScheme), thickness(thickness) {
 setSize(INIConfig::LayoutConstants::separatorComponentDefaultWidth, static_cast<int>(thickness));
}

void SeparatorComponent::paint(juce::Graphics& g) {
 g.setColour(colorScheme.getColor(ColorScheme::ColorRole::Separator));
 g.fillRect(getLocalBounds().toFloat());
}

void SeparatorComponent::saveState(ComponentState& state) const {
 juce::ignoreUnused(state);
}

void SeparatorComponent::loadState(const ComponentState& state) {
 juce::ignoreUnused(state);
}

MeterComponent::MeterComponent(ColorScheme& colorScheme, ResponsiveLayoutManager& layoutManager)
 : colorScheme(colorScheme), layoutManager(layoutManager) {
 startTimerHz(INIConfig::LayoutConstants::meterComponentTimerHz);
}

MeterComponent::~MeterComponent() {
 stopTimer();
}

void MeterComponent::paint(juce::Graphics& g) {
 auto bounds = getLocalBounds().toFloat();

 g.setColour(colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground));
 g.fillRoundedRectangle(bounds, INIConfig::LayoutConstants::meterComponentCornerRadius);

 const float padding = INIConfig::LayoutConstants::meterComponentPadding;
 const float barSpacing = INIConfig::LayoutConstants::meterComponentBarSpacing;

 if (isStereo) {
     float barWidth = (bounds.getWidth() - padding * INIConfig::LayoutConstants::customLookFeelThumbDivisor - barSpacing) / INIConfig::LayoutConstants::customLookFeelThumbDivisor;
     float barHeight = bounds.getHeight() - padding * INIConfig::LayoutConstants::customLookFeelThumbDivisor;

     float leftBarHeight = leftDisplayLevel * barHeight;
     juce::Rectangle<float> leftBar(padding, bounds.getHeight() - padding - leftBarHeight, barWidth, leftBarHeight);

     float rightBarHeight = rightDisplayLevel * barHeight;
     juce::Rectangle<float> rightBar(padding + barWidth + barSpacing, bounds.getHeight() - padding - rightBarHeight, barWidth, rightBarHeight);

     drawMeterBar(g, leftBar, leftDisplayLevel);
     drawMeterBar(g, rightBar, rightDisplayLevel);

     if (leftPeak > 0.0f) {
         float peakY = bounds.getHeight() - padding - (leftPeak * barHeight);
         g.setColour(colorScheme.getColor(ColorScheme::ColorRole::MeterHigh));
         g.fillRect(padding, peakY - INIConfig::LayoutConstants::meterComponentPeakHeight, barWidth, INIConfig::LayoutConstants::meterComponentPeakHeight);
     }

     if (rightPeak > 0.0f) {
         float peakY = bounds.getHeight() - padding - (rightPeak * barHeight);
         g.setColour(colorScheme.getColor(ColorScheme::ColorRole::MeterHigh));
         g.fillRect(padding + barWidth + barSpacing, peakY - INIConfig::LayoutConstants::meterComponentPeakHeight, barWidth, INIConfig::LayoutConstants::meterComponentPeakHeight);
     }
 } else {
     float barHeight = (leftDisplayLevel + rightDisplayLevel) * INIConfig::LayoutConstants::alphaDisabled * (bounds.getHeight() - padding * INIConfig::LayoutConstants::customLookFeelThumbDivisor);
     juce::Rectangle<float> bar(padding, bounds.getHeight() - padding - barHeight,
                               bounds.getWidth() - padding * INIConfig::LayoutConstants::customLookFeelThumbDivisor, barHeight);
     drawMeterBar(g, bar, (leftDisplayLevel + rightDisplayLevel) * INIConfig::LayoutConstants::alphaDisabled);
 }
}

void MeterComponent::drawMeterBar(juce::Graphics& g, const juce::Rectangle<float>& bar, float level) {
 if (bar.getHeight() <= 0) return;

 juce::ColourGradient gradient;

 if (level < INIConfig::LayoutConstants::meterComponentGreenThreshold) {
     gradient = juce::ColourGradient(colorScheme.getColor(ColorScheme::ColorRole::MeterLow),
                                    bar.getBottomLeft(),
                                    colorScheme.getColor(ColorScheme::ColorRole::MeterMid),
                                    bar.getTopLeft(),
                                    false);
 } else {
     gradient = juce::ColourGradient(colorScheme.getColor(ColorScheme::ColorRole::MeterMid),
                                    bar.getBottomLeft(),
                                    colorScheme.getColor(ColorScheme::ColorRole::MeterHigh),
                                    bar.getTopLeft(),
                                    false);
 }

 g.setGradientFill(gradient);
 g.fillRoundedRectangle(bar, INIConfig::LayoutConstants::meterComponentBarCornerRadius);
}

void MeterComponent::timerCallback() {
 const float decay = INIConfig::LayoutConstants::meterComponentDecay;
 const float attack = INIConfig::LayoutConstants::meterComponentAttack;

 leftDisplayLevel = leftLevel > leftDisplayLevel ?
                   leftDisplayLevel + (leftLevel - leftDisplayLevel) * attack :
                   leftDisplayLevel * decay;

 rightDisplayLevel = rightLevel > rightDisplayLevel ?
                    rightDisplayLevel + (rightLevel - rightDisplayLevel) * attack :
                    rightDisplayLevel * decay;

 if (peakHoldTime > 0) {
     peakHoldTime--;
 } else {
     leftPeak *= INIConfig::LayoutConstants::meterComponentPeakDecay;
     rightPeak *= INIConfig::LayoutConstants::meterComponentPeakDecay;
 }

 repaint();
}

void MeterComponent::setLevel(float left, float right) {
 leftLevel = juce::jlimit(0.0f, 1.0f, left);
 rightLevel = juce::jlimit(0.0f, 1.0f, right);
}

void MeterComponent::setPeakLevel(float left, float right) {
 if (left > leftPeak) {
     leftPeak = left;
     peakHoldTime = peakHoldDuration;
 }
 if (right > rightPeak) {
     rightPeak = right;
     peakHoldTime = peakHoldDuration;
 }
}

void MeterComponent::resetPeak() {
 leftPeak = 0.0f;
 rightPeak = 0.0f;
 peakHoldTime = 0;
}

EffectButton::EffectButton(const juce::String& buttonText, ColorScheme& colorScheme)
 : TextButton(buttonText), colorScheme(colorScheme) {
}

void EffectButton::paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted,
                           bool shouldDrawButtonAsDown) {
 auto bounds = getLocalBounds().toFloat();

 juce::Colour bgColor;
 if (shouldDrawButtonAsDown) {
     bgColor = colorScheme.getColor(ColorScheme::ColorRole::ButtonBackgroundPressed);
 } else if (shouldDrawButtonAsHighlighted) {
     bgColor = colorScheme.getColor(ColorScheme::ColorRole::ButtonBackgroundHover);
 } else {
     bgColor = colorScheme.getColor(ColorScheme::ColorRole::ButtonBackground);
 }

 if (isBypassed) {
     bgColor = bgColor.darker(INIConfig::LayoutConstants::effectButtonDarkenFactor);
 }

 g.setColour(bgColor);
 g.fillRoundedRectangle(bounds, INIConfig::LayoutConstants::effectButtonCornerRadius);

 g.setColour(colorScheme.getColor(ColorScheme::ColorRole::Separator));
 g.drawRoundedRectangle(bounds.reduced(INIConfig::LayoutConstants::effectButtonBorderReduction),
                       INIConfig::LayoutConstants::effectButtonCornerRadius, INIConfig::LayoutConstants::effectButtonBorderWidth);

 g.setColour(isBypassed ? colorScheme.getColor(ColorScheme::ColorRole::SecondaryText).withAlpha(INIConfig::LayoutConstants::effectButtonBypassAlpha)
                        : colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
 g.setFont(juce::Font(INIConfig::LayoutConstants::effectButtonFontSize));
 g.drawText(getButtonText(), bounds, juce::Justification::centred);

 if (isBypassed) {
     g.setColour(colorScheme.getColor(ColorScheme::ColorRole::Error));
     g.drawLine(bounds.getX() + INIConfig::LayoutConstants::effectButtonBypassLineMargin, bounds.getCentreY(),
               bounds.getRight() - INIConfig::LayoutConstants::effectButtonBypassLineMargin, bounds.getCentreY(),
               INIConfig::LayoutConstants::effectButtonBypassLineWidth);
 }
}
