#pragma once

#include <JuceHeader.h>
#include "PluginEditor.h"
#include "PlatformResponsiveManager.h"
#include "PlatformAwareLookAndFeel.h"

/**
 * Enhanced Plugin Editor demonstrating integration of the new responsive system
 * with OTTO's existing architecture. Shows how to gradually migrate to responsive design.
 */
class EnhancedPluginEditor : public OTTOAudioProcessorEditor
{
public:
    EnhancedPluginEditor(OTTOAudioProcessor& processor);
    ~EnhancedPluginEditor() override;

    // Override key methods to add responsive behavior
    void paint(juce::Graphics& g) override;
    void resized() override;
    void visibilityChanged() override;

private:
    // Enhanced responsive system
    std::unique_ptr<PlatformResponsiveManager> platformResponsiveManager;
    std::unique_ptr<PlatformAwareLookAndFeel> platformLookAndFeel;
    
    // Track whether we're in responsive mode
    bool responsiveMode = true;
    bool touchOptimized = false;
    
    // Override methods to add responsive behavior
    void setupResponsiveSystem();
    void updateResponsiveLayout();
    void handleResponsiveScaleChange(const PlatformResponsiveManager::ResponsiveScale& scale);
    void handlePlatformChange(PlatformResponsiveManager::TargetPlatform platform);
    
    // Enhanced layout methods
    void calculateResponsiveLayout();
    void applyPlatformSpecificAdjustments();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EnhancedPluginEditor)
};

/**
 * Migration utility to gradually introduce responsive features
 * to existing OTTO components
 */
class ResponsiveMigrationHelper
{
public:
    // Static methods to help existing components adopt responsive features
    static void enhanceExistingComponent(juce::Component& component, 
                                       PlatformResponsiveManager& responsiveManager);
    
    static void makeButtonTouchFriendly(juce::Button& button,
                                      PlatformResponsiveManager& responsiveManager);
    
    static void makeSliderTouchFriendly(juce::Slider& slider,
                                      PlatformResponsiveManager& responsiveManager);
    
    static void adaptLayoutForPlatform(juce::Component& component,
                                     PlatformResponsiveManager& responsiveManager);
    
    // Font scaling helpers
    static juce::Font scaleFont(const juce::Font& font,
                              PlatformResponsiveManager& responsiveManager);
    
    // Color adaptation helpers
    static juce::Colour adaptColor(const juce::Colour& color,
                                 PlatformResponsiveManager& responsiveManager);

private:
    ResponsiveMigrationHelper() = delete;
};