#include "EasingCurves.h"

float EasingCurves::linear(float t) {
    return t;
}

float EasingCurves::easeInQuad(float t) {
    return t * t;
}

float EasingCurves::easeOutQuad(float t) {
    return 1.0f - (1.0f - t) * (1.0f - t);
}

float EasingCurves::easeInOutQuad(float t) {
    return t < 0.5f ? 2.0f * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 2.0f) / 2.0f;
}

float EasingCurves::easeInCubic(float t) {
    return t * t * t;
}

float EasingCurves::easeOutCubic(float t) {
    return 1.0f - std::pow(1.0f - t, 3.0f);
}

float EasingCurves::easeInOutCubic(float t) {
    return t < 0.5f ? 4.0f * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 3.0f) / 2.0f;
}

float EasingCurves::easeInQuart(float t) {
    return t * t * t * t;
}

float EasingCurves::easeOutQuart(float t) {
    return 1.0f - std::pow(1.0f - t, 4.0f);
}

float EasingCurves::easeInOutQuart(float t) {
    return t < 0.5f ? 8.0f * t * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 4.0f) / 2.0f;
}

float EasingCurves::easeInSine(float t) {
    return 1.0f - std::cos((t * juce::MathConstants<float>::pi) / 2.0f);
}

float EasingCurves::easeOutSine(float t) {
    return std::sin((t * juce::MathConstants<float>::pi) / 2.0f);
}

float EasingCurves::easeInOutSine(float t) {
    return -(std::cos(juce::MathConstants<float>::pi * t) - 1.0f) / 2.0f;
}

float EasingCurves::easeInExpo(float t) {
    return t == 0.0f ? 0.0f : std::pow(2.0f, 10.0f * (t - 1.0f));
}

float EasingCurves::easeOutExpo(float t) {
    return t == 1.0f ? 1.0f : 1.0f - std::pow(2.0f, -10.0f * t);
}

float EasingCurves::easeInOutExpo(float t) {
    if (t == 0.0f) return 0.0f;
    if (t == 1.0f) return 1.0f;
    return t < 0.5f ? std::pow(2.0f, 20.0f * t - 10.0f) / 2.0f
                    : (2.0f - std::pow(2.0f, -20.0f * t + 10.0f)) / 2.0f;
}

float EasingCurves::easeInCirc(float t) {
    return 1.0f - std::sqrt(1.0f - std::pow(t, 2.0f));
}

float EasingCurves::easeOutCirc(float t) {
    return std::sqrt(1.0f - std::pow(t - 1.0f, 2.0f));
}

float EasingCurves::easeInOutCirc(float t) {
    return t < 0.5f
        ? (1.0f - std::sqrt(1.0f - std::pow(2.0f * t, 2.0f))) / 2.0f
        : (std::sqrt(1.0f - std::pow(-2.0f * t + 2.0f, 2.0f)) + 1.0f) / 2.0f;
}

float EasingCurves::easeInBack(float t) {
    const float c1 = 1.70158f;
    const float c3 = c1 + 1.0f;
    return c3 * t * t * t - c1 * t * t;
}

float EasingCurves::easeOutBack(float t) {
    const float c1 = 1.70158f;
    const float c3 = c1 + 1.0f;
    return 1.0f + c3 * std::pow(t - 1.0f, 3.0f) + c1 * std::pow(t - 1.0f, 2.0f);
}

float EasingCurves::easeInOutBack(float t) {
    const float c1 = 1.70158f;
    const float c2 = c1 * 1.525f;
    return t < 0.5f
        ? (std::pow(2.0f * t, 2.0f) * ((c2 + 1.0f) * 2.0f * t - c2)) / 2.0f
        : (std::pow(2.0f * t - 2.0f, 2.0f) * ((c2 + 1.0f) * (t * 2.0f - 2.0f) + c2) + 2.0f) / 2.0f;
}

float EasingCurves::easeInElastic(float t) {
    const float c4 = (2.0f * juce::MathConstants<float>::pi) / 3.0f;
    if (t == 0.0f) return 0.0f;
    if (t == 1.0f) return 1.0f;
    return -std::pow(2.0f, 10.0f * (t - 1.0f)) * std::sin((t - 1.0f) * c4);
}

float EasingCurves::easeOutElastic(float t) {
    const float c4 = (2.0f * juce::MathConstants<float>::pi) / 3.0f;
    if (t == 0.0f) return 0.0f;
    if (t == 1.0f) return 1.0f;
    return std::pow(2.0f, -10.0f * t) * std::sin(t * c4) + 1.0f;
}

float EasingCurves::easeInOutElastic(float t) {
    const float c5 = (2.0f * juce::MathConstants<float>::pi) / 4.5f;
    if (t == 0.0f) return 0.0f;
    if (t == 1.0f) return 1.0f;
    return t < 0.5f
        ? -(std::pow(2.0f, 20.0f * t - 10.0f) * std::sin((20.0f * t - 11.125f) * c5)) / 2.0f
        : (std::pow(2.0f, -20.0f * t + 10.0f) * std::sin((20.0f * t - 11.125f) * c5)) / 2.0f + 1.0f;
}

float EasingCurves::easeInBounce(float t) {
    return 1.0f - easeOutBounce(1.0f - t);
}

float EasingCurves::easeOutBounce(float t) {
    const float n1 = 7.5625f;
    const float d1 = 2.75f;
    
    if (t < 1.0f / d1) {
        return n1 * t * t;
    } else if (t < 2.0f / d1) {
        return n1 * (t -= 1.5f / d1) * t + 0.75f;
    } else if (t < 2.5f / d1) {
        return n1 * (t -= 2.25f / d1) * t + 0.9375f;
    } else {
        return n1 * (t -= 2.625f / d1) * t + 0.984375f;
    }
}

float EasingCurves::easeInOutBounce(float t) {
    return t < 0.5f
        ? (1.0f - easeOutBounce(1.0f - 2.0f * t)) / 2.0f
        : (1.0f + easeOutBounce(2.0f * t - 1.0f)) / 2.0f;
}
