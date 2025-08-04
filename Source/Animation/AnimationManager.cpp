#include "AnimationManager.h"

AnimationManager::AnimationManager(PlatformResponsiveManager& platformManager)
    : platformManager(platformManager) {
}

float AnimationManager::easeInOut(float t) {
    return t < 0.5f ? 2.0f * t * t : -1.0f + (4.0f - 2.0f * t) * t;
}

float AnimationManager::easeSpring(float t) {
    return 1.0f - std::cos(t * juce::MathConstants<float>::pi * 0.5f);
}

float AnimationManager::easeElastic(float t) {
    if (t == 0.0f) return 0.0f;
    if (t == 1.0f) return 1.0f;
    
    float p = 0.3f;
    float s = p / 4.0f;
    return std::pow(2.0f, -10.0f * t) * std::sin((t - s) * (2.0f * juce::MathConstants<float>::pi) / p) + 1.0f;
}

void AnimationManager::animateSliderValue(juce::Slider& slider, float targetValue, int durationMs) {
    if (!shouldUseAnimations()) {
        slider.setValue(targetValue, juce::dontSendNotification);
        return;
    }
    
    auto startValue = static_cast<float>(slider.getValue());
    auto startTime = juce::Time::getMillisecondCounterHiRes();
    auto duration = getOptimalDuration(durationMs);
    
    auto timer = std::make_unique<juce::Timer>();
    timer->onTimer = [&slider, startValue, targetValue, startTime, duration, timer = timer.get()]() {
        auto currentTime = juce::Time::getMillisecondCounterHiRes();
        auto elapsed = currentTime - startTime;
        
        if (elapsed >= duration) {
            slider.setValue(targetValue, juce::dontSendNotification);
            timer->stopTimer();
            return;
        }
        
        float progress = static_cast<float>(elapsed) / static_cast<float>(duration);
        progress = easeInOut(progress);
        
        float currentValue = startValue + (targetValue - startValue) * progress;
        slider.setValue(currentValue, juce::dontSendNotification);
    };
    
    timer->startTimerHz(60);
}

void AnimationManager::animateButtonPress(juce::Button& button, int durationMs) {
    if (!shouldUseAnimations()) return;
    
    auto originalAlpha = button.getAlpha();
    button.setAlpha(0.7f);
    
    juce::Timer::callAfterDelay(getOptimalDuration(durationMs), [&button, originalAlpha]() {
        button.setAlpha(originalAlpha);
    });
}

void AnimationManager::animatePatternSwitch(juce::Component& component, int durationMs) {
    if (!shouldUseAnimations()) return;
    
    auto originalAlpha = component.getAlpha();
    component.setAlpha(0.7f);
    
    juce::Timer::callAfterDelay(getOptimalDuration(durationMs / 2), [&component, originalAlpha, durationMs, this]() {
        juce::Timer::callAfterDelay(getOptimalDuration(durationMs / 2), [&component, originalAlpha]() {
            component.setAlpha(originalAlpha);
        });
    });
}

bool AnimationManager::shouldUseAnimations() const {
    return platformManager.shouldUseAnimations();
}

int AnimationManager::getOptimalDuration(int baseDuration) const {
    return platformManager.getAnimationDuration(baseDuration);
}
