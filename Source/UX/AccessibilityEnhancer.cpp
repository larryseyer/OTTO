#include "AccessibilityEnhancer.h"

AccessibilityEnhancer::AccessibilityEnhancer() {
}

void AccessibilityEnhancer::enhanceComponent(juce::Component& component, const juce::String& role) {
    AccessibilityHelper::setupAccessibility(&component);
    component.setAccessible(true);
}

void AccessibilityEnhancer::setAccessibleName(juce::Component& component, const juce::String& name) {
    AccessibilityHelper::setAccessibleName(&component, name);
}

void AccessibilityEnhancer::setAccessibleDescription(juce::Component& component, const juce::String& description) {
    AccessibilityHelper::setAccessibleDescription(&component, description);
}

void AccessibilityEnhancer::setupTabOrder(const juce::Array<juce::Component*>& components) {
    AccessibilityHelper::setupTabOrder(components);
}

bool AccessibilityEnhancer::isScreenReaderActive() const {
    return AccessibilityHelper::isScreenReaderActive();
}

bool AccessibilityEnhancer::isHighContrastMode() const {
    return false;
}

bool AccessibilityEnhancer::isReducedMotionEnabled() const {
    return false;
}

void AccessibilityEnhancer::announceChange(const juce::String& message) {
    AccessibilityHelper::announceChange(message);
}
