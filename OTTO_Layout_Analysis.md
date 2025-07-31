# OTTO JUCE 8 Layout Analysis & Sizing Guide

## SVG Design Reference Analysis

**Source File:** `HISE OTTO Recent.svg`  
**Original Dimensions:** 8067 × 4589 pixels  
**Base Interface:** 1024 × 768 pixels (minimum system requirement)  
**Layout System:** 6-Row Percentage-based Responsive Design

---

## 6-Row Layout Structure (Percentage-Based)

### Row Height Distribution

| Row | Purpose | Height % | Actual Height @ 768px | Content |
|-----|---------|----------|---------------------|---------|
| **Row 1** | TopBar Component | 10.0% | 77px | Transport, Tempo, Gear |
| **Row 2** | Player Buttons/Tabs | 8.0% | 61px | 8 Player Tab Buttons |
| **Row 3** | DrumKit Menu Controls | 12.0% | 92px | Edit, Chevrons, DrumKit Dropdown, Mute, Mixer |
| **Row 4** | Pattern Group Controls | 12.0% | 92px | Group Label, Pattern Dropdown, Status, Actions |
| **Row 5** | Main Content Matrix | 50.0% | 384px | 4×4 Drum Matrix + Controls (65%/35% split) |
| **Row 6** | Loop Controls | 8.0% | 61px | Loop Start/End Labels + Position Slider |
| **TOTAL** | | **100.0%** | **768px** | Complete Interface |

---

## Font Sizes (Percentage-Based Scaling)

### Primary Font Sizes
```cpp
// Base font sizes from INIConfig::LayoutConstants
constexpr float fontSizeBody = 16.0f;           // Standard text
constexpr float fontSizeBodyLarge = 18.0f;      // Emphasized text  
constexpr float fontSizeHeader = 18.0f;         // Section headers
constexpr float fontSizeNumeric = 24.0f;        // BPM, values
constexpr float fontSizeBrand = 48.0f;          // OTTO logo
constexpr float fontSizeButton = 14.0f;         // Button text
constexpr float fontSizeIcon = 24.0f;           // Icon fonts
constexpr float fontSizeLabel = 12.0f;          // Small labels
constexpr float fontSizePresetLabel = 18.0f;    // Preset names
constexpr float fontSizePresetMenu = 32.0f;     // Preset dropdown
```

### Row-Specific Font Sizing

#### Row 1 (TopBar) - 10% Height
- **BPM Display:** 24.0px (fontSizeNumeric)
- **OTTO Brand:** 48.0px (fontSizeBrand) 
- **Version Info:** 12.0px (fontSizeLabel)
- **Button Text:** 14.0px (fontSizeButton)

#### Row 2 (Player Tabs) - 8% Height  
- **Tab Labels:** 14.0px (fontSizeButton)
- **Player Numbers:** 18.0px (fontSizeHeader)

#### Row 3 (DrumKit Controls) - 12% Height
- **Large Player Number:** 70% of content height = ~64px
- **Button Text:** 14.0px (fontSizeButton)
- **Dropdown Text:** 16.0px (fontSizeBody)

#### Row 4 (Pattern Groups) - 12% Height
- **Group Labels:** 16.0px (fontSizeBody)
- **Dropdown Text:** 16.0px (fontSizeBody)
- **Status Text:** 14.0px (fontSizeButton)

#### Row 5 (Main Matrix) - 50% Height
- **Matrix Button Numbers:** 18.0px (fontSizeHeader)
- **Control Labels:** 12.0px (fontSizeLabel)
- **Slider Values:** 14.0px (fontSizeButton)

#### Row 6 (Loop Controls) - 8% Height
- **Loop Labels:** 12.0px (fontSizeLabel)
- **Position Values:** 14.0px (fontSizeButton)

---

## Icon Sizes (Responsive Scaling)

### Primary Icon Dimensions
```cpp
// Base icon sizing from INIConfig::LayoutConstants
constexpr int iconButtonSize = static_cast<int>((DEFAULT_INTERFACE_HEIGHT * 0.10f) * 0.95f);
// = 73px @ 768px height (95% of Row 1 height for proper fit)

constexpr int topBarIconSpacing = static_cast<int>(DEFAULT_INTERFACE_WIDTH * 0.02f);
// = 20px @ 1024px width (2% spacing maintains proportion)
```

### Row-Specific Icon Sizing

#### Row 1 (TopBar Icons)
- **Primary Icons:** 73px × 73px (gear, link, cloud, play, record, etc.)
- **Icon Spacing:** 20px between icons
- **Icon Y Position:** Vertically centered in 77px row

#### Row 2 (Player Tabs)
- **Tab Height:** 49px (contentHeight after padding)
- **Tab Width:** 92px (9% of interface width)
- **No icons** - text-based tabs

#### Row 3 (DrumKit Controls)  
- **Control Buttons:** 69px × 69px (75% of 92px content height)
- **Edit/Chevron/Mute/Mixer:** Square buttons, touch-friendly
- **Spacing:** 8px between buttons

#### Row 4 (Pattern Groups)
- **Action Buttons:** 69px × 69px (75% of content height) 
- **Add/Delete/Menu icons:** Standard control icons

#### Row 5 (Main Matrix)
- **4×4 Drum Pads:** Calculated from 65% section width (~100px each)
- **Control Knobs:** Varies by control type (32-48px typical)
- **Toggle Buttons:** Touch-friendly sizing (44px minimum)

#### Row 6 (Loop Controls)
- **Minimal Icons:** 16px for small transport controls
- **Focus on slider control** rather than icon buttons

---

## Component Spacing & Margins

### Global Spacing Constants
```cpp
constexpr int defaultMargin = static_cast<int>(DEFAULT_INTERFACE_WIDTH * 0.0167f);    // ~17px
constexpr int defaultPadding = static_cast<int>(DEFAULT_INTERFACE_WIDTH * 0.0083f);   // ~8px  
constexpr int defaultSpacing = static_cast<int>(DEFAULT_INTERFACE_WIDTH * 0.0125f);   // ~13px
```

### Row-Specific Spacing

#### Horizontal Spacing
- **Row margins:** 17px from screen edges
- **Component spacing:** 13px between related elements  
- **Icon spacing:** 20px between toolbar icons
- **Section separation:** 8px padding within components

#### Vertical Spacing  
- **Row padding:** 8px top/bottom within each row
- **Component centering:** Vertical center alignment within available height
- **Content margins:** Calculated to maintain touch targets (44px minimum)

---

## Touch Accessibility Compliance

### Minimum Touch Targets
```cpp
constexpr int MIN_TOUCH_TARGET_PX = 44;                     // iOS/Android standard
constexpr float MIN_TOUCH_TARGET_HEIGHT_PERCENT = 5.73f;    // 44px / 768px  
constexpr float MIN_TOUCH_TARGET_WIDTH_PERCENT = 4.30f;     // 44px / 1024px
```

### Touch-Optimized Components
- **All primary buttons:** ≥44px height/width
- **Sliders:** 44px touch area minimum  
- **Dropdown menus:** ≥44px height
- **Tab buttons:** Row 2 height ensures accessibility
- **Matrix pads:** Large enough for finger interaction

---

## Responsive Scaling Factors

### Interface Scaling
- **Base Interface:** 1024×768 (100% scale)
- **Scale Factor:** `DEFAULT_INTERFACE_SCALE = 1.0f`  
- **DPI Awareness:** `responsiveLayoutBaseDPI = 96.0f`
- **Aspect Ratio:** `16:9 default, adaptable to various screen ratios`

### Platform Adaptations
- **Desktop:** Full feature set, mouse + keyboard
- **Tablet:** Touch-optimized, larger buttons
- **Phone:** Compact mode, essential controls
- **Plugin:** Resizable within DAW constraints

---

## Implementation Guidelines

### Using INIConfig Constants

```cpp
// ✅ CORRECT - Use percentage-based constants
int buttonHeight = INIConfig::LayoutConstants::Row3::buttonSize;
int spacing = INIConfig::LayoutConstants::defaultSpacing;

// ❌ INCORRECT - Never use hardcoded values  
int buttonHeight = 69;  // DON'T DO THIS
int spacing = 8;        // DON'T DO THIS
```

### FontManager Integration
```cpp
// ✅ CORRECT - Use FontManager for all text
auto font = FontManager::getFont(FontManager::FontType::Body, 
                                INIConfig::LayoutConstants::fontSizeBody);

// ✅ CORRECT - Scale fonts responsively  
float scaledSize = INIConfig::LayoutConstants::Row3::largePlayerFontSize;
auto largeFont = FontManager::getFont(FontManager::FontType::Numeric, scaledSize);
```

### ColorScheme Integration
```cpp
// ✅ CORRECT - Use ColorScheme for all colors
auto bgColor = ColorScheme::getColor(ColorScheme::ColorRole::WindowBackground);
auto buttonColor = ColorScheme::getColor(ColorScheme::ColorRole::ButtonBackground);
```

---

## Summary of Key Measurements

| Element | Percentage | Pixels @ 1024×768 | Usage |
|---------|------------|-------------------|-------|
| **Row Heights** | 10/8/12/12/50/8% | 77/61/92/92/384/61px | Structure |
| **Icon Size** | 9.5% of Row 1 | 73×73px | Primary actions |
| **Font Sizes** | 1.6-6.3% of height | 12-48px | Text hierarchy |
| **Touch Targets** | 5.7% minimum | 44px minimum | Accessibility |
| **Margins** | 1.67% of width | 17px | Screen edges |
| **Spacing** | 1.25% of width | 13px | Component gaps |

This percentage-based system ensures consistent, responsive design across all target platforms while maintaining accessibility standards and visual hierarchy.