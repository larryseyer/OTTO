#pragma once

#include <JuceHeader.h>
#include "FontManager.h"
#include "ColorScheme.h"
#include "ComponentState.h"
#include "INIConfig.h"

class ResponsiveLayoutManager;

class AnimatedComponent : public juce::Component, private juce::Timer {
public:
    AnimatedComponent();
    ~AnimatedComponent() override;

    void fadeIn(int durationMs = INIConfig::LayoutConstants::animatedComponentDefaultDuration);
    void fadeOut(int durationMs = INIConfig::LayoutConstants::animatedComponentDefaultDuration);
    void slideIn(juce::Point<int> from, int durationMs = INIConfig::LayoutConstants::animatedComponentDefaultDuration);
    void slideOut(juce::Point<int> to, int durationMs = INIConfig::LayoutConstants::animatedComponentDefaultDuration);

    void paint(juce::Graphics& g) override;

protected:
    void timerCallback() override;
    virtual void paintContent(juce::Graphics& g) = 0;

private:
    float opacity = INIConfig::Defaults::DEFAULT_ALPHA_FULL;
    float targetOpacity = INIConfig::Defaults::DEFAULT_ALPHA_FULL;
    juce::Point<float> position;
    juce::Point<float> targetPosition;
    int animationDuration = INIConfig::Defaults::ZERO_VALUE;
    int animationTime = INIConfig::Defaults::ZERO_VALUE;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnimatedComponent)
};

class TooltipManager : public juce::Component {
public:
    TooltipManager();

    static void addTooltip(juce::Component* component, const juce::String& tooltip);
    static void removeTooltip(juce::Component* component);
    static juce::String getTooltip(juce::Component* component);

    void showTooltipAt(const juce::String& text, juce::Point<int> screenPos);
    void hideTooltip();

private:
    static std::map<juce::Component*, juce::String> tooltips;
    std::unique_ptr<juce::TooltipWindow> tooltipWindow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TooltipManager)
};

class KeyboardShortcutsOverlay : public AnimatedComponent {
public:
    KeyboardShortcutsOverlay(FontManager& fontManager, ColorScheme& colorScheme);

    void addShortcut(const juce::String& key, const juce::String& description);
    void clearShortcuts();
    void show();
    void hide();

protected:
    void paintContent(juce::Graphics& g) override;
    void resized() override;

private:
    FontManager& fontManager;
    ColorScheme& colorScheme;

    struct Shortcut {
        juce::String key;
        juce::String description;
    };

    juce::Array<Shortcut> shortcuts;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KeyboardShortcutsOverlay)
};

class MultiMonitorManager {
public:
    static juce::Rectangle<int> getDisplayAreaForComponent(juce::Component* component);
    static juce::Array<juce::Rectangle<int>> getAllDisplayAreas();
    static int getDisplayIndexForComponent(juce::Component* component);
    static void moveToDisplay(juce::Component* component, int displayIndex);
    static bool isFullScreenAvailable();
    static void toggleFullScreen(juce::Component* component);
};

class AccessibilityHelper {
public:
    static void setupAccessibility(juce::Component* component);
    static void setAccessibleName(juce::Component* component, const juce::String& name);
    static void setAccessibleDescription(juce::Component* component, const juce::String& description);
    static void announceChange(const juce::String& message);
    static bool isScreenReaderActive();

    static void setupTabOrder(const juce::Array<juce::Component*>& components);
    static void focusNext(juce::Component* currentComponent);
    static void focusPrevious(juce::Component* currentComponent);
};

class PhosphorIconButton : public juce::TextButton, private juce::Timer {
public:
    PhosphorIconButton(const juce::String& iconName,
                      FontManager::PhosphorWeight weight = FontManager::PhosphorWeight::Regular);

    void setPhosphorWeight(FontManager::PhosphorWeight weight);
    FontManager::PhosphorWeight getPhosphorWeight() const { return phosphorWeight; }
    juce::String getIconName() const { return iconName; }
    void setIconName(const juce::String& newIconName);

    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    void saveState(ComponentState& state) const;
    void loadState(const ComponentState& state);

    void setAnimationEnabled(bool enabled) { animationEnabled = enabled; }
    void mouseEnter(const juce::MouseEvent& e) override;
    void mouseExit(const juce::MouseEvent& e) override;

    void setColorScheme(ColorScheme* scheme) { colorScheme = scheme; }

private:
    juce::String iconName;
    FontManager::PhosphorWeight phosphorWeight;
    ColorScheme* colorScheme = nullptr;
    bool animationEnabled = true;
    float hoverAmount = INIConfig::Validation::MIN_VOLUME;

    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PhosphorIconButton)
};

class HierarchicalComboBox : public juce::ComboBox {
public:
    HierarchicalComboBox() = default;

    void mouseDown(const juce::MouseEvent& event) override;
    void showPopup() override;

    std::function<void()> onPopupRequest;

    void saveState(ComponentState& state) const;
    void loadState(const ComponentState& state);

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HierarchicalComboBox)
};

class EditableNumericLabel : public juce::Label {
public:
    EditableNumericLabel(const juce::String& componentId, float minValue, float maxValue);

    void setValueRange(float min, float max);
    void setValue(float value);
    float getValue() const;

    std::function<void(float)> onValueChanged;

    void saveState(ComponentState& state) const;
    void loadState(const ComponentState& state);

private:
    float minValue;
    float maxValue;
    float currentValue;

    void validateAndSetText();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EditableNumericLabel)
};

class ScaledSlider : public juce::Slider {
public:
    ScaledSlider(const ResponsiveLayoutManager& layoutManager);

    void resized() override;

    void saveState(ComponentState& state) const;
    void loadState(const ComponentState& state);

private:
    const ResponsiveLayoutManager& layoutManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScaledSlider)
};

class SeparatorComponent : public juce::Component {
public:
    SeparatorComponent(ColorScheme& colorScheme, float thickness = INIConfig::LayoutConstants::separatorComponentDefaultThickness);

    void paint(juce::Graphics& g) override;

    void saveState(ComponentState& state) const;
    void loadState(const ComponentState& state);

private:
    ColorScheme& colorScheme;
    float thickness;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeparatorComponent)
};

class MeterComponent : public juce::Component, public juce::Timer {
public:
    MeterComponent(ColorScheme& colorScheme, ResponsiveLayoutManager& layoutManager);
    ~MeterComponent() override;

    void paint(juce::Graphics& g) override;
    void timerCallback() override;

    void setLevel(float left, float right);
    void setPeakLevel(float left, float right);
    void resetPeak();

    void setStereo(bool stereo) { isStereo = stereo; }
    bool getStereo() const { return isStereo; }

private:
    ColorScheme& colorScheme;
    ResponsiveLayoutManager& layoutManager;

    float leftLevel = INIConfig::Validation::MIN_VOLUME;
    float rightLevel = INIConfig::Validation::MIN_VOLUME;
    float leftPeak = INIConfig::Validation::MIN_VOLUME;
    float rightPeak = INIConfig::Validation::MIN_VOLUME;
    float leftDisplayLevel = INIConfig::Validation::MIN_VOLUME;
    float rightDisplayLevel = INIConfig::Validation::MIN_VOLUME;

    bool isStereo = true;
    int peakHoldTime = INIConfig::Defaults::ZERO_VALUE;
    static constexpr int peakHoldDuration = INIConfig::LayoutConstants::meterComponentPeakHoldDuration;

    void drawMeterBar(juce::Graphics& g, const juce::Rectangle<float>& bar, float level);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MeterComponent)
};

class EffectButton : public juce::TextButton {
public:
    EffectButton(const juce::String& buttonText, ColorScheme& colorScheme);

    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted,
                    bool shouldDrawButtonAsDown) override;

    void setBypass(bool bypassed) {
        isBypassed = bypassed;
        repaint();
    }

    bool getBypass() const { return isBypassed; }

private:
    ColorScheme& colorScheme;
    bool isBypassed = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EffectButton)
};

inline void drawBeveledRect(juce::Graphics& g, const juce::Rectangle<float>& bounds,
                           const juce::Colour& baseColour, float bevelSize = INIConfig::LayoutConstants::beveledRectDefaultSize,
                           bool raised = true) {
    auto lightColour = baseColour.brighter(INIConfig::LayoutConstants::beveledRectLightBrightness);
    auto darkColour = baseColour.darker(INIConfig::LayoutConstants::beveledRectDarkBrightness);

    if (!raised) std::swap(lightColour, darkColour);

    g.setColour(lightColour);
    g.fillRect(bounds.getX(), bounds.getY(), bounds.getWidth(), bevelSize);
    g.fillRect(bounds.getX(), bounds.getY(), bevelSize, bounds.getHeight());

    g.setColour(darkColour);
    g.fillRect(bounds.getX(), bounds.getBottom() - bevelSize, bounds.getWidth(), bevelSize);
    g.fillRect(bounds.getRight() - bevelSize, bounds.getY(), bevelSize, bounds.getHeight());
}
