#include "TransitionEffects.h"

void TransitionEffects::fadeTransition(juce::Component& component, float targetAlpha, int durationMs) {
    auto startAlpha = component.getAlpha();
    auto timer = std::make_unique<TransitionTimer>(
        [&component, startAlpha, targetAlpha](float progress) {
            float currentAlpha = startAlpha + (targetAlpha - startAlpha) * EasingCurves::easeInOutQuad(progress);
            component.setAlpha(currentAlpha);
        },
        []() {},
        durationMs
    );
    timer->startTimerHz(60);
}

void TransitionEffects::slideTransition(juce::Component& component, juce::Point<int> targetPosition, int durationMs) {
    auto startPosition = component.getPosition();
    auto timer = std::make_unique<TransitionTimer>(
        [&component, startPosition, targetPosition](float progress) {
            auto currentX = static_cast<int>(startPosition.x + (targetPosition.x - startPosition.x) * EasingCurves::easeInOutQuad(progress));
            auto currentY = static_cast<int>(startPosition.y + (targetPosition.y - startPosition.y) * EasingCurves::easeInOutQuad(progress));
            component.setTopLeftPosition(currentX, currentY);
        },
        []() {},
        durationMs
    );
    timer->startTimerHz(60);
}

void TransitionEffects::scaleTransition(juce::Component& component, float targetScale, int durationMs) {
    auto startBounds = component.getBounds();
    auto timer = std::make_unique<TransitionTimer>(
        [&component, startBounds, targetScale](float progress) {
            float currentScale = 1.0f + (targetScale - 1.0f) * EasingCurves::easeInOutQuad(progress);
            auto scaledBounds = startBounds.withSizeKeepingCentre(
                static_cast<int>(startBounds.getWidth() * currentScale),
                static_cast<int>(startBounds.getHeight() * currentScale)
            );
            component.setBounds(scaledBounds);
        },
        []() {},
        durationMs
    );
    timer->startTimerHz(60);
}

void TransitionEffects::colorTransition(juce::Component& component, juce::Colour targetColor, int durationMs) {
    juce::ignoreUnused(component, targetColor, durationMs);
}

TransitionEffects::TransitionTimer::TransitionTimer(std::function<void(float)> updateCallback, 
                                                   std::function<void()> completionCallback,
                                                   int durationMs)
    : updateCallback(updateCallback), completionCallback(completionCallback), duration(durationMs) {
    startTime = juce::Time::getMillisecondCounterHiRes();
}

void TransitionEffects::TransitionTimer::timerCallback() {
    auto currentTime = juce::Time::getMillisecondCounterHiRes();
    auto elapsed = currentTime - startTime;
    
    if (elapsed >= duration) {
        updateCallback(1.0f);
        completionCallback();
        stopTimer();
        return;
    }
    
    float progress = static_cast<float>(elapsed) / static_cast<float>(duration);
    updateCallback(progress);
}
