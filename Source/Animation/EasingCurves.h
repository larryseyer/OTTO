#pragma once
#include <JuceHeader.h>

class EasingCurves {
public:
    static float linear(float t);
    static float easeInQuad(float t);
    static float easeOutQuad(float t);
    static float easeInOutQuad(float t);
    static float easeInCubic(float t);
    static float easeOutCubic(float t);
    static float easeInOutCubic(float t);
    static float easeInQuart(float t);
    static float easeOutQuart(float t);
    static float easeInOutQuart(float t);
    static float easeInSine(float t);
    static float easeOutSine(float t);
    static float easeInOutSine(float t);
    static float easeInExpo(float t);
    static float easeOutExpo(float t);
    static float easeInOutExpo(float t);
    static float easeInCirc(float t);
    static float easeOutCirc(float t);
    static float easeInOutCirc(float t);
    static float easeInBack(float t);
    static float easeOutBack(float t);
    static float easeInOutBack(float t);
    static float easeInElastic(float t);
    static float easeOutElastic(float t);
    static float easeInOutElastic(float t);
    static float easeInBounce(float t);
    static float easeOutBounce(float t);
    static float easeInOutBounce(float t);
};
