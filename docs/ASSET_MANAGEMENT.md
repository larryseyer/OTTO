# OTTO Asset Management Documentation

This document explains how OTTO handles assets (graphics, fonts, audio samples) and the recent changes to the asset loading system.

## Overview

OTTO uses a hybrid asset management approach:
- **UI Assets**: Embedded in binary via JUCE BinaryData for reliability
- **User Content**: File-based for customization (drum samples, patterns, etc.)

## Asset Categories

### 🎨 **Embedded Assets (BinaryData)**

These assets are compiled into the application binary and are always available:

#### **Graphics**
- `Button080.png` - Small button graphics
- `Button100.png` - Medium button graphics  
- `Slider100.png` - Slider track graphics
- `SliderKnob.png` - Slider knob graphics
- `OTTO_Splash_Screen.png` - Application splash screen
- `OTTO_Icon.png` - Application icon
- `OTTO_GLYPH.png` - Logo glyph

#### **Fonts**
- **Roboto Family**: `RobotoVariableFont_wdthwght.ttf`
- **Roboto Condensed**: `RobotoCondensedVariableFont_wght.ttf`
- **Montserrat**: `MontserratVariableFont_wght.ttf`
- **Orbitron**: `OrbitronVariableFont_wght.ttf`
- **Playfair Display**: `PlayfairDisplayVariableFont_wght.ttf`
- **Open Sans**: `OpenSansVariableFont_wdthwght.ttf`

#### **Icon Fonts (Phosphor)**
- `Phosphor.ttf` - Regular weight icons
- `PhosphorThin.ttf` - Thin weight icons
- `PhosphorLight.ttf` - Light weight icons
- `PhosphorBold.ttf` - Bold weight icons
- `PhosphorFill.ttf` - Filled icons
- `PhosphorDuotone.ttf` - Duotone icons

### 📁 **File-Based Assets (User Customizable)**

These assets remain as files to allow user customization:

#### **Audio Content**
- **Drum Samples**: WAV files in `Assets/DrumSamples/[KitName]/`
- **SFZ Definitions**: Drum kit mappings in `Assets/DrumKits/[KitName].sfz`
- **MIDI Patterns**: Beat patterns in `Assets/Patterns/[PatternName].mid`

#### **User Data**
- **Presets**: User-created plugin settings
- **Custom Kits**: User-imported drum kits
- **Recorded Patterns**: User-recorded MIDI patterns

## Technical Implementation

### BinaryData Integration

The ProJucer automatically generates BinaryData from assets in the project. Our CMake system includes all generated files:

```cmake
# Auto-detect all BinaryData files (ProJucer may split into multiple files)
file(GLOB BINARY_DATA_FILES "${CMAKE_CURRENT_SOURCE_DIR}/JuceLibraryCode/BinaryData*.cpp")
if(BINARY_DATA_FILES)
    target_sources(OTTO PRIVATE ${BINARY_DATA_FILES})
    list(LENGTH BINARY_DATA_FILES BINARY_DATA_COUNT)
    message(STATUS "Using ${BINARY_DATA_COUNT} BinaryData files with embedded assets")
endif()
```

### Graphics Loading

Graphics are loaded from BinaryData using JUCE's memory-based loading:

```cpp
// Modern approach - loads from embedded binary data
buttonImage = juce::ImageCache::getFromMemory(
    BinaryData::Button080_png, 
    BinaryData::Button080_pngSize
);
```

**Fallback Strategy:**
1. Try loading from BinaryData (primary)
2. If BinaryData not available, attempt file loading (backup)
3. Use default graphics if both fail

### Font Loading

Fonts are loaded using JUCE 8's modern font API:

```cpp
// Load typeface from binary data
robotoTypeface = juce::Typeface::createSystemTypefaceFor(
    BinaryData::RobotoVariableFont_wdthwght_ttf,
    BinaryData::RobotoVariableFont_wdthwght_ttfSize
);

// Create font instance
juce::Font robotoFont = juce::Font(robotoTypeface).withHeight(16.0f);
```

**Phosphor Icon Usage:**
```cpp
// Get icon string for UI
juce::String playIcon = fontManager->getIconString("play");
juce::Font iconFont = fontManager->getPhosphorFont(FontManager::PhosphorWeight::Regular, 20.0f);

// Draw icon
g.setFont(iconFont);
g.drawText(playIcon, bounds, juce::Justification::centred);
```

### File-Based Asset Loading

Audio and user content continues to use file-based loading:

```cpp
// Load drum sample
juce::File sampleFile = assetsPath.getChildFile("DrumSamples").getChildFile(kitName).getChildFile("kick.wav");
if (sampleFile.existsAsFile()) {
    audioFormatManager.createReaderFor(sampleFile);
}

// Load SFZ definition
juce::File sfzFile = assetsPath.getChildFile("DrumKits").getChildFile(kitName + ".sfz");
if (sfzFile.existsAsFile()) {
    sfzEngine.loadSfzFile(sfzFile);
}
```

## Error Handling

### Font Loading Errors

The FontManager provides debugging information:

```cpp
// Check if fonts loaded successfully
if (fontManager->hasError()) {
    DBG("FontManager Error: " + fontManager->getError());
}

if (!fontManager->arePhosphorFontsLoaded()) {
    DBG("Warning: No Phosphor fonts loaded!");
}

// List loaded fonts
auto loadedFonts = fontManager->getLoadedFontNames();
DBG("Loaded fonts: " + loadedFonts.joinIntoString(", "));
```

### Graphics Loading Errors

Graphics loading uses fallback strategies:

```cpp
// Try BinaryData first
auto image = juce::ImageCache::getFromMemory(BinaryData::Button080_png, BinaryData::Button080_pngSize);

if (!image.isValid()) {
    // Fallback to file loading
    auto file = assetsPath.getChildFile("Button080.png");
    if (file.existsAsFile()) {
        image = juce::ImageFileFormat::loadFrom(file);
    }
}

if (!image.isValid()) {
    // Use default/generated graphics
    image = createDefaultButtonImage();
}
```

## Asset Organization

### Project Structure
```
OTTO/
├── Assets/                    # File-based assets (user customizable)
│   ├── DrumSamples/          # WAV files organized by kit
│   ├── DrumKits/             # SFZ definition files
│   ├── Patterns/             # MIDI pattern files
│   └── Presets/              # User preset files
├── JuceLibraryCode/          # Generated by ProJucer
│   ├── BinaryData.h          # Asset declarations
│   ├── BinaryData.cpp        # Asset data (part 1)
│   ├── BinaryData2.cpp       # Asset data (part 2)
│   ├── BinaryData3.cpp       # Asset data (part 3)
│   ├── BinaryData4.cpp       # Asset data (part 4)
│   └── BinaryData5.cpp       # Asset data (part 5)
└── Source/
    ├── FontManager.cpp       # Font loading and management
    ├── CustomLookAndFeel.cpp # Graphics loading and UI theming
    └── PluginEditor.cpp      # Main UI initialization
```

### ProJucer Integration

Assets to be embedded are added to the ProJucer project:

1. **Add files to ProJucer**: Drag assets into the file browser
2. **Set Binary Resource**: Mark files as "Binary Resource" in ProJucer
3. **Regenerate**: Save ProJucer project to regenerate BinaryData files
4. **Build**: CMake automatically includes all BinaryData*.cpp files

## Migration Guide

### From File-Based to BinaryData

If you need to embed additional assets:

1. **Add to ProJucer**: Add file as Binary Resource
2. **Regenerate**: Save ProJucer project
3. **Update Code**: Change loading code from file-based to BinaryData

**Before:**
```cpp
auto file = assetsPath.getChildFile("MyImage.png");
auto image = juce::ImageFileFormat::loadFrom(file);
```

**After:**
```cpp
auto image = juce::ImageCache::getFromMemory(BinaryData::MyImage_png, BinaryData::MyImage_pngSize);
```

### From BinaryData to File-Based

If you need to make assets user-customizable:

1. **Move to Assets folder**: Place file in appropriate Assets subfolder
2. **Update loading code**: Use file-based loading with fallbacks
3. **Update ProJucer**: Remove from Binary Resources

## Best Practices

### Asset Selection Criteria

**Embed in BinaryData if:**
- ✅ Critical for basic UI functionality
- ✅ Should be consistent across installations  
- ✅ Not expected to be customized by users
- ✅ Relatively small file size

**Keep as files if:**
- ✅ Users should be able to customize/replace
- ✅ Large file sizes (audio samples)
- ✅ Frequently updated content
- ✅ Optional/plugin content

### Performance Considerations

- **BinaryData**: Faster loading, no disk I/O, but increases binary size
- **File-Based**: Slower loading, requires disk I/O, but allows customization
- **Caching**: Use JUCE's built-in caching for frequently accessed assets

### Development Workflow

1. **Design assets** in external tools
2. **Add to ProJucer** as Binary Resources for UI assets
3. **Place in Assets folder** for user content
4. **Update loading code** with appropriate method
5. **Test both approaches** (embedded and fallback)
6. **Build and verify** assets load correctly

## Troubleshooting

### Common Issues

1. **Assets not found**
   - Check BinaryData files are included in CMake
   - Verify ProJucer regenerated BinaryData correctly
   - Check asset file names match BinaryData symbols

2. **Fonts not displaying**
   - Ensure all BinaryData*.cpp files are compiled
   - Check FontManager error messages
   - Verify font loading is called during initialization

3. **Large binary size**
   - Consider moving large assets to file-based loading
   - Compress assets before embedding
   - Use appropriate asset formats (PNG for graphics, TTF for fonts)

### Debug Information

Enable debugging to see asset loading status:

```cpp
#ifdef JUCE_DEBUG
    DBG("Loading assets...");
    if (fontManager->hasError()) {
        DBG("Font error: " + fontManager->getError());
    }
    auto fonts = fontManager->getLoadedFontNames();
    DBG("Loaded fonts: " + fonts.joinIntoString(", "));
#endif
```

This documentation ensures developers understand the current asset management system and can maintain or extend it appropriately.