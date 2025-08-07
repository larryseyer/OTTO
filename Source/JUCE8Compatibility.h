#pragma once

#include <JuceHeader.h>

/**
 * JUCE 8 Compatibility Layer for OTTO Proportional Scaling
 * Ensures all methods use JUCE 8 APIs and protocols
 */

//==============================================================================
// Phase 2.2 Safety Feature Definitions
//==============================================================================

#define OTTO_ENHANCED_NULL_SAFETY 1
#define OTTO_MEMORY_SAFETY_CHECKS 1
#define OTTO_AUDIO_PROCESSING_SAFETY 1
#define OTTO_MIDI_SAFETY_FEATURES 1
#define OTTO_UI_THREAD_SAFETY 1

// Phase 2.3.x Platform Optimization Flags
#define OTTO_PHASE_2_3_X_OPTIMIZATIONS 1
#define OTTO_CROSS_PLATFORM_ROBUSTNESS 1

namespace JUCE8Compatibility
{
    //==============================================================================
    // JUCE 8 Display and DPI Management
    //==============================================================================
    
    struct DisplayInfo
    {
        float scale = 1.0f;
        juce::Rectangle<int> userArea;
        juce::Rectangle<int> totalArea;
        juce::Rectangle<int> safeArea;
        bool isMain = false;
        int dpi = 96;
    };
    
    class DisplayManager
    {
    public:
        static DisplayInfo getPrimaryDisplayInfo()
        {
            DisplayInfo info;
            
            auto& desktop = juce::Desktop::getInstance();
            const auto& displays = desktop.getDisplays();
            
            if (displays.displays.size() > 0)
            {
                auto primaryDisplay = displays.displays[0]; // First display is main
                info.scale = primaryDisplay.scale;
                info.userArea = primaryDisplay.userArea;
                info.totalArea = primaryDisplay.totalArea;
                // Convert BorderSize to Rectangle for safeArea
                info.safeArea = juce::Rectangle<int>(primaryDisplay.safeAreaInsets.getLeft(),
                                                   primaryDisplay.safeAreaInsets.getTop(),
                                                   primaryDisplay.totalArea.getWidth() - primaryDisplay.safeAreaInsets.getLeftAndRight(),
                                                   primaryDisplay.totalArea.getHeight() - primaryDisplay.safeAreaInsets.getTopAndBottom());
                info.isMain = true; // First display is main
                info.dpi = primaryDisplay.dpi;
            }
            
            return info;
        }
        
        static std::vector<DisplayInfo> getAllDisplays()
        {
            std::vector<DisplayInfo> displayInfos;
            
            auto& desktop = juce::Desktop::getInstance();
            const auto& displays = desktop.getDisplays();
            
            for (int i = 0; i < displays.displays.size(); ++i)
            {
                DisplayInfo info;
                auto display = displays.displays[i];
                info.scale = display.scale;
                info.userArea = display.userArea;
                info.totalArea = display.totalArea;
                // Convert BorderSize to Rectangle for safeArea
                info.safeArea = juce::Rectangle<int>(display.safeAreaInsets.getLeft(),
                                                   display.safeAreaInsets.getTop(),
                                                   display.totalArea.getWidth() - display.safeAreaInsets.getLeftAndRight(),
                                                   display.totalArea.getHeight() - display.safeAreaInsets.getTopAndBottom());
                info.isMain = (display.totalArea == displays.displays[0].totalArea); // First display is main
                info.dpi = display.dpi;
                displayInfos.push_back(info);
            }
            
            return displayInfos;
        }
        
        static float getSystemDPIScale()
        {
            auto info = getPrimaryDisplayInfo();
            
            #if JUCE_MAC
                // macOS uses logical scale factor
                return info.scale;
            #elif JUCE_WINDOWS
                // Windows per-monitor DPI scaling (JUCE 8)
                return info.scale;
            #elif JUCE_LINUX
                // Linux X11 DPI detection
                return jmax(1.0f, info.scale);
            #elif JUCE_IOS || JUCE_ANDROID
                // Mobile device pixel ratio
                return info.scale;
            #else
                return 1.0f;
            #endif
        }
    };
    
    //==============================================================================
    // JUCE 8 Component and Layout Management
    //==============================================================================
    
    class ComponentHelpers
    {
    public:
        // JUCE 8 safe component bounds setting
        static void setSafeBounds(juce::Component* component, const juce::Rectangle<int>& bounds)
        {
            if (component && bounds.getWidth() > 0 && bounds.getHeight() > 0)
            {
                component->setBounds(bounds);
            }
        }
        
        // JUCE 8 component bounds constraining
        static juce::Rectangle<int> constrainBounds(const juce::Rectangle<int>& bounds, 
                                                   const juce::Rectangle<int>& parentBounds)
        {
            return bounds.constrainedWithin(parentBounds);
        }
        
        // JUCE 8 animation-safe component positioning
        static void setAnimatedBounds(juce::Component* component, 
                                     const juce::Rectangle<int>& newBounds,
                                     int animationDurationMs = 200)
        {
            if (!component) return;
            
            auto& animator = juce::Desktop::getInstance().getAnimator();
            animator.animateComponent(component, newBounds, 1.0f, animationDurationMs, false, 0.0, 0.0);
        }
        
        // JUCE 8 component hierarchy traversal
        template<typename ComponentType>
        static ComponentType* findParentComponentOfClass(juce::Component* component)
        {
            if (!component) return nullptr;
            
            auto* parent = component->getParentComponent();
            while (parent)
            {
                if (auto* typedParent = dynamic_cast<ComponentType*>(parent))
                    return typedParent;
                parent = parent->getParentComponent();
            }
            return nullptr;
        }
    };
    
    //==============================================================================
    // JUCE 8 Font and Text Rendering
    //==============================================================================
    
    class FontHelpers
    {
    public:
        // JUCE 8 safe font scaling with FontOptions
        static juce::Font scaledFont(const juce::Font& baseFont, float scaleFactor)
        {
            float newHeight = baseFont.getHeight() * scaleFactor;
            // JUCE 8 minimum font size enforcement
            newHeight = jmax(8.0f, newHeight);
            
            return baseFont.withHeight(newHeight);
        }
        
        // JUCE 8 DPI-aware font scaling
        static juce::Font dpiScaledFont(const juce::Font& baseFont, float dpiScale)
        {
            float scaledHeight = baseFont.getHeight() * dpiScale;
            return baseFont.withHeight(scaledHeight);
        }
        
        // JUCE 8 create font with FontOptions
        static juce::Font createFont(float height, int styleFlags = juce::Font::plain)
        {
            juce::FontOptions options = juce::FontOptions().withHeight(height);
            
            // Convert style flags to JUCE 8 style
            if (styleFlags & juce::Font::bold)
                options = options.withStyle("Bold");
            if (styleFlags & juce::Font::italic)
                options = options.withStyle("Italic");
                
            return juce::Font(options);
        }
        
        // JUCE 8 create font with name and FontOptions
        static juce::Font createFont(const juce::String& fontName, float height, int styleFlags = juce::Font::plain)
        {
            juce::FontOptions options = juce::FontOptions().withName(fontName).withHeight(height);
            
            // Convert style flags to JUCE 8 style
            if (styleFlags & juce::Font::bold)
                options = options.withStyle("Bold");
            if (styleFlags & juce::Font::italic)
                options = options.withStyle("Italic");
                
            return juce::Font(options);
        }
        
        // JUCE 8 text bounds calculation
        static juce::Rectangle<int> getTextBounds(const juce::Font& font, 
                                                 const juce::String& text,
                                                 int maximumWidth = 0)
        {
            if (maximumWidth > 0)
            {
                // Use GlyphArrangement for width calculation
                juce::GlyphArrangement glyphs;
                glyphs.addFittedText(font, text, 0, 0, static_cast<float>(maximumWidth), 
                                   font.getHeight(), juce::Justification::left, 1);
                return glyphs.getBoundingBox(0, glyphs.getNumGlyphs(), true).toNearestInt();
            }
            else
            {
                juce::GlyphArrangement glyphs;
                glyphs.addLineOfText(font, text, 0, 0);
                return glyphs.getBoundingBox(0, glyphs.getNumGlyphs(), true).toNearestInt();
            }
        }
    };
    
    //==============================================================================
    // JUCE 8 Touch and Input Handling
    //==============================================================================
    
    class TouchHelpers
    {
    public:
        // JUCE 8 touch target size validation for iOS/Android compliance
        static bool isValidTouchTarget(const juce::Rectangle<int>& bounds)
        {
            #if JUCE_IOS
                return bounds.getWidth() >= 44 && bounds.getHeight() >= 44;
            #elif JUCE_ANDROID
                return bounds.getWidth() >= 48 && bounds.getHeight() >= 48;
            #else
                return bounds.getWidth() >= 20 && bounds.getHeight() >= 20;
            #endif
        }
        
        // JUCE 8 ensure minimum touch target size
        static juce::Rectangle<int> ensureMinimumTouchTarget(const juce::Rectangle<int>& bounds)
        {
            auto result = bounds;
            
            #if JUCE_IOS
                int minSize = 44;
            #elif JUCE_ANDROID
                int minSize = 48;
            #else
                int minSize = 20;
            #endif
            
            if (result.getWidth() < minSize)
            {
                int expansion = (minSize - result.getWidth()) / 2;
                result = result.expanded(expansion, 0);
            }
            
            if (result.getHeight() < minSize)
            {
                int expansion = (minSize - result.getHeight()) / 2;
                result = result.expanded(0, expansion);
            }
            
            return result;
        }
        
        // JUCE 8 touch input detection
        static bool hasTouchInput()
        {
            #if JUCE_IOS || JUCE_ANDROID
                return true;
            #else
                // JUCE 8 desktop touch detection
                auto& desktop = juce::Desktop::getInstance();
                return desktop.getMainMouseSource().isTouch();
            #endif
        }
    };
    
    //==============================================================================
    // JUCE 8 Platform Detection
    //==============================================================================
    
    class PlatformHelpers
    {
    public:
        enum class Platform
        {
            macOS_Standalone,
            macOS_VST3,
            macOS_AU,
            macOS_CLAP,
            Windows_Standalone,
            Windows_VST3,
            Windows_CLAP,
            Linux_Standalone,
            Linux_VST3,
            Linux_CLAP,
            iOS_Standalone,
            iOS_AUv3,
            Android_Standalone,
            Unknown
        };
        
        static Platform getCurrentPlatform()
        {
            #if JUCE_MAC
                #if JUCE_PLUGINHOST_VST3
                    return Platform::macOS_VST3;
                #elif JUCE_PLUGINHOST_AU
                    return Platform::macOS_AU;
                #else
                    return Platform::macOS_Standalone;
                #endif
            #elif JUCE_WINDOWS
                #if JUCE_PLUGINHOST_VST3
                    return Platform::Windows_VST3;
                #else
                    return Platform::Windows_Standalone;
                #endif
            #elif JUCE_LINUX
                #if JUCE_PLUGINHOST_VST3
                    return Platform::Linux_VST3;
                #else
                    return Platform::Linux_Standalone;
                #endif
            #elif JUCE_IOS
                return Platform::iOS_AUv3; // Assume AUv3 for iOS
            #elif JUCE_ANDROID
                return Platform::Android_Standalone;
            #else
                return Platform::Unknown;
            #endif
        }
        
        static juce::String getPlatformName()
        {
            switch (getCurrentPlatform())
            {
                case Platform::macOS_Standalone:   return "macOS Standalone";
                case Platform::macOS_VST3:        return "macOS VST3";
                case Platform::macOS_AU:          return "macOS AU";
                case Platform::macOS_CLAP:        return "macOS CLAP";
                case Platform::Windows_Standalone: return "Windows Standalone";
                case Platform::Windows_VST3:      return "Windows VST3";
                case Platform::Windows_CLAP:      return "Windows CLAP";
                case Platform::Linux_Standalone:  return "Linux Standalone";
                case Platform::Linux_VST3:        return "Linux VST3";
                case Platform::Linux_CLAP:        return "Linux CLAP";
                case Platform::iOS_Standalone:    return "iOS Standalone";
                case Platform::iOS_AUv3:          return "iOS AUv3";
                case Platform::Android_Standalone: return "Android Standalone";
                default:                          return "Unknown Platform";
            }
        }
        
        static bool isMobilePlatform()
        {
            auto platform = getCurrentPlatform();
            return platform == Platform::iOS_Standalone || 
                   platform == Platform::iOS_AUv3 || 
                   platform == Platform::Android_Standalone;
        }
        
        static bool isDesktopPlatform()
        {
            return !isMobilePlatform();
        }
    };
    
    //==============================================================================
    // JUCE 8 Color and Graphics Helpers
    //==============================================================================
    
    class GraphicsHelpers
    {
    public:
        // JUCE 8 high-DPI aware drawing
        static void drawScaledRoundedRectangle(juce::Graphics& g, 
                                              const juce::Rectangle<float>& bounds,
                                              float cornerRadius,
                                              float dpiScale = 1.0f)
        {
            float scaledRadius = cornerRadius * dpiScale;
            g.fillRoundedRectangle(bounds, scaledRadius);
        }
        
        // JUCE 8 platform-appropriate drop shadow
        static void drawPlatformDropShadow(juce::Graphics& g, 
                                          const juce::Rectangle<int>& bounds,
                                          juce::Colour shadowColour = juce::Colours::black.withAlpha(0.3f))
        {
            #if JUCE_MAC
                // macOS style shadow
                auto shadowBounds = bounds.toFloat().translated(0, 1);
                g.setColour(shadowColour);
                g.fillRoundedRectangle(shadowBounds, 3.0f);
            #elif JUCE_WINDOWS
                // Windows style shadow
                auto shadowBounds = bounds.toFloat().translated(1, 1);
                g.setColour(shadowColour);
                g.fillRoundedRectangle(shadowBounds, 1.0f);
            #else  
                // Generic shadow
                auto shadowBounds = bounds.toFloat().translated(1, 1);
                g.setColour(shadowColour);
                g.fillRoundedRectangle(shadowBounds, 2.0f);
            #endif
        }
    };
    
    //==============================================================================
    // JUCE 8 Plugin Host Integration
    //==============================================================================
    
    class PluginHostHelpers
    {
    public:
        // JUCE 8 plugin format detection
        static juce::String detectPluginFormat(const juce::AudioProcessor& processor)
        {
            switch (processor.wrapperType)
            {
                case juce::AudioProcessor::wrapperType_VST3:
                    return "VST3";
                case juce::AudioProcessor::wrapperType_AudioUnit:
                    return "AU";
                case juce::AudioProcessor::wrapperType_Standalone:
                    return "Standalone";
                default:
                    return "Unknown";
            }
        }
        
        // JUCE 8 host capability detection
        static bool hostSupportsResizing(const juce::AudioProcessor& processor)
        {
            // Most modern hosts support resizing, but check specific cases
            auto format = detectPluginFormat(processor);
            
            if (format == "VST3") return true;  // VST3 supports resizing
            if (format == "AU") return true;    // AU supports resizing on macOS
            if (format == "Standalone") return true; // Standalone always supports resizing
            
            return false; // Conservative fallback
        }
        
        // JUCE 8 optimal plugin dimensions
        static juce::Rectangle<int> getOptimalPluginSize(const juce::AudioProcessor& processor)
        {
            auto format = detectPluginFormat(processor);
            
            if (format == "VST3")
                return juce::Rectangle<int>(0, 0, 800, 600);
            else if (format == "AU")
                return juce::Rectangle<int>(0, 0, 1024, 768);
            else if (format == "Standalone")
                return juce::Rectangle<int>(0, 0, 1000, 750);
            else
                return juce::Rectangle<int>(0, 0, 800, 600);
        }
    };
}

//==============================================================================
// Convenience macros for JUCE 8 compatibility
//==============================================================================

#define JUCE8_SAFE_BOUNDS(component, bounds) \
    JUCE8Compatibility::ComponentHelpers::setSafeBounds(component, bounds)

#define JUCE8_SCALED_FONT(font, scale) \
    JUCE8Compatibility::FontHelpers::scaledFont(font, scale)

#define JUCE8_DPI_FONT(font, dpiScale) \
    JUCE8Compatibility::FontHelpers::dpiScaledFont(font, dpiScale)

#define JUCE8_ENSURE_TOUCH_TARGET(bounds) \
    JUCE8Compatibility::TouchHelpers::ensureMinimumTouchTarget(bounds)

#define JUCE8_IS_MOBILE() \
    JUCE8Compatibility::PlatformHelpers::isMobilePlatform()

#define JUCE8_CURRENT_PLATFORM() \
    JUCE8Compatibility::PlatformHelpers::getCurrentPlatform()

#define JUCE8_PLATFORM_NAME() \
    JUCE8Compatibility::PlatformHelpers::getPlatformName()

#define JUCE8_SYSTEM_DPI() \
    JUCE8Compatibility::DisplayManager::getSystemDPIScale()

#define JUCE8_PRIMARY_DISPLAY() \
    JUCE8Compatibility::DisplayManager::getPrimaryDisplayInfo()

#define JUCE8_HAS_TOUCH() \
    JUCE8Compatibility::TouchHelpers::hasTouchInput()

#define JUCE8_PLUGIN_FORMAT(processor) \
    JUCE8Compatibility::PluginHostHelpers::detectPluginFormat(processor)

#define JUCE8_OPTIMAL_PLUGIN_SIZE(processor) \
    JUCE8Compatibility::PluginHostHelpers::getOptimalPluginSize(processor)

#define JUCE8_CREATE_FONT(...) \
    JUCE8Compatibility::FontHelpers::createFont(__VA_ARGS__)

#define JUCE8_CREATE_NAMED_FONT(name, height, style) \
    JUCE8Compatibility::FontHelpers::createFont(name, height, style)