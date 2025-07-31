∑∑∑∑# OTTO - Minimum System Requirements

## Display Requirements

### **Minimum Screen Resolution: 1024x768**

OTTO requires a minimum display resolution of **1024x768** (XGA) to ensure proper functionality and accessibility compliance.

#### Why 1024x768?

- **Touch Accessibility:** All interactive elements meet the 44px minimum touch target requirement
- **Pattern Matrix:** The core pattern editing area requires 300px minimum height  
- **Component Spacing:** Buttons, dropdowns, and controls have adequate breathing room
- **Text Readability:** Font sizes remain clear and readable across all UI elements
- **Cross-Platform:** Universally supported resolution across desktop, tablet, and embedded displays

#### Resolution Validation

At 1024x768, OTTO provides:
- **Touch Targets:** 79px (80% larger than 44px minimum) ✅
- **Pattern Matrix:** 345px (15% larger than 300px minimum) ✅
- **Component Layout:** All 6 rows fit comfortably with proper proportions ✅

## Supported Display Configurations

### ✅ **Fully Supported**
- **1024x768** (XGA) - Minimum supported
- **1280x720** (720p HD) - Recommended minimum
- **1366x768** (WXGA) - Common laptop resolution
- **1920x1080** (1080p Full HD) - Default/optimal
- **2560x1440** (1440p) - High resolution
- **3840x2160** (4K) - Ultra high resolution

### ❌ **Not Supported**
- **800x600** (SVGA) - Height insufficient for pattern matrix
- **1024x600** (Netbook) - Height insufficient for touch targets
- Any resolution smaller than 1024x768

## Supported Device Specifications

### **iOS Devices** ✅

#### **iPads (All Models Supported)**
- **iPad (10th gen, 2022+):** 2360 x 1640 → 1640 x 1178 landscape ✅
- **iPad Air (11", M2, 2024+):** 2360 x 1640 → 1640 x 1178 landscape ✅ 
- **iPad Air (13", M2, 2024+):** 2752 x 2064 → 2064 x 1536 landscape ✅
- **iPad Pro (11", M4, 2024+):** 2420 x 1668 → 1668 x 1246 landscape ✅
- **iPad Pro (13", M4, 2024+):** 2752 x 2064 → 2064 x 1536 landscape ✅
- **iPad mini (6th gen, A17 Pro, 2024+):** 2266 x 1488 → 1488 x 1114 landscape ✅

*All current iPad models exceed the 1024x768 minimum in landscape orientation.*

#### **iPhones (Limited Support)**
- **iPhone 16 Pro Max:** 2868 x 1320 → 1320 x 896 landscape ✅
- **iPhone 16 Pro:** 2556 x 1179 → 1179 x 852 landscape ✅
- **iPhone 16/Plus:** 2556 x 1179 → 1179 x 852 landscape ✅

*iPhones support OTTO but iPad experience is recommended for optimal usability.*

### **Android Devices** ✅

#### **Tablets (Recommended)**
- **Samsung Galaxy Tab S9/S9+/S9 Ultra (2023+):** All models exceed 1024x768 ✅
- **Google Pixel Tablet (2023+):** 2560 x 1600 → 1600 x 1200 landscape ✅
- **OnePlus Pad (2023+):** 2800 x 2000 → 2000 x 1500 landscape ✅
- **Xiaomi Pad 6 Series (2023+):** 2880 x 1800 → 1800 x 1350 landscape ✅
- **Lenovo Tab P11/P12 Series (2022+):** All models exceed requirements ✅

#### **Large Phones (Basic Support)**
- **Samsung Galaxy S24 Ultra:** 3120 x 1440 → 1440 x 1080 landscape ✅
- **Google Pixel 8 Pro:** 2992 x 1344 → 1344 x 1008 landscape ✅
- **OnePlus 12:** 3168 x 1440 → 1440 x 1080 landscape ✅

*Large Android phones (6.5"+ displays) can run OTTO but tablet experience is optimal.*

### **Minimum Device Requirements**

#### **Physical Screen Size**
- **Minimum:** 7.9" diagonal (iPad mini baseline)
- **Recommended:** 10.1"+ diagonal for comfortable use
- **Optimal:** 11"+ diagonal for professional workflow

#### **Resolution Requirements**
- **Absolute Minimum:** 1024x768 pixels (landscape)
- **Touch Target Safety:** Ensures 44px minimum interactive elements
- **Pattern Matrix:** Guarantees 300px minimum editing space

## Platform-Specific Notes

### **Desktop (Windows/Mac/Linux)**
- Window can be resized above minimum
- Maintains aspect ratio and component proportions  
- Responsive scaling for larger displays
- External monitor support for extended workspace

### **iOS**
- **Recommended:** iPad Air 11" or larger
- **Minimum:** iPad mini (7.9") for basic functionality
- **iPhone Support:** iPhone 16 series and newer (landscape only)
- Native Metal rendering for optimal performance

### **Android**
- **Recommended:** 10.1"+ tablets with 1920x1200+ resolution
- **Minimum:** 8"+ tablets with 1280x800+ resolution
- **Phone Support:** 6.5"+ displays with 1080p+ resolution
- Vulkan API support preferred for graphics acceleration

### **Embedded/Kiosk**
- Fixed 1024x768 displays fully supported
- Touch screen compatibility verified (USB HID, I2C)
- ARM/x86 architecture support
- Performance optimized for embedded hardware

## Technical Implementation

All layout calculations are **percentage-based** and defined in `INIConfig.h`:

```cpp
// Minimum display dimensions
static const int DEFAULT_INTERFACE_WIDTH = 1024;
static const int DEFAULT_INTERFACE_HEIGHT = 768;

// Touch accessibility requirements @ 1024x768
constexpr float MIN_TOUCH_TARGET_HEIGHT_PERCENT = 5.73f;  // 44px / 768px
constexpr float MIN_TOUCH_TARGET_WIDTH_PERCENT = 4.30f;   // 44px / 1024px
```

This ensures consistent usability across all supported resolutions while maintaining accessibility standards.

## Device Compatibility Summary

### ✅ **Fully Supported (Optimal Experience)**
- All current iPad models (2022+)
- Android tablets 10"+ (2022+) 
- Desktop/laptop displays 1024x768+
- Embedded displays 1024x768+

### ⚠️ **Basic Support (Functional but Limited)**
- iPad mini (7.9" - minimum viable)
- iPhone 16 series (landscape only)
- Large Android phones 6.5"+ (landscape only)
- Small Android tablets 8-10"

### ❌ **Not Recommended**
- Phones under 6.5" diagonal
- Any device under 1024x768 resolution
- Portrait-only usage on small devices

---

**OTTO Phase 2.3.5 - INI-Driven Layout System**  
*Minimum Resolution: 1024x768 | Smallest Device: iPad mini (7.9")*