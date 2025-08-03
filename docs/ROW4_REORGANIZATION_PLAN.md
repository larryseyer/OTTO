# Row 4 Component Reorganization Plan

## Overview
This document outlines the complete plan to reorganize OTTO's UI components to match the correct Row 4 layout as specified in INIConfig.h. The INIConfig.h Row 4 comment is correct, and all other components need to be moved to implement this layout properly.

## Current State Analysis
**INIConfig.h Row 4 Comment (CORRECT):**
```
// Layout: [Drum pattern group edit icon], [left chevron], [drum pattern group menu/label combo], [right chevron], [favorite icon], [toggles buttons label], [fills buttons label], [swing slider label], [Energy slider label], [Volume slider label]
```

**Current Implementation Issues:**
- Row 4 only has: patternGroupLabel, patternGroupDropdown, patternStatusLabel, patternAddButton, patternDeleteButton
- Missing components are currently in LeftSection and RightSection (Row 5)
- Row 5 contains both interactive controls AND labels that should be in Row 4

## Correct Row Assignments

### Row 4 Should Contain (Labels/Navigation):
1. Edit/Pencil icon for Pattern groups → `patternGroupEditButton`
2. Left chevron → `patternGroupLeftChevron`
3. Pattern groups dropdown menu/label combo → `patternGroupDropdown` ✅ (already there)
4. Right chevron → `patternGroupRightChevron`
5. Favorites group icon → `patternGroupFavoriteButton`
6. Toggles buttons label → `togglesLabel`
7. Fills buttons label → `fillsLabel`
8. Swing slider label → `swingLabel`
9. Energy slider label → `energyLabel`
10. Volume slider label → `volumeLabel`

### Row 5 Should Contain (Interactive Controls):
1. 4x4 16 button midi drum pattern selectors → `drumButtons[16]`
2. 5 toggle buttons → `toggleButtons[5]`
3. 5 fills buttons → `fillButtons[5]`
4. Swing slider → `swingSlider`
5. Energy slider → `energySlider`
6. Volume slider → `volumeSlider`

---

## Phase 1: Analysis & Planning ✅ COMPLETED
**Status:** COMPLETED
**Objective:** Understand current state vs desired state and create implementation plan

**What was accomplished:**
- Analyzed INIConfig.h Row 4 comment vs current implementation
- Identified all components that need to move from LeftSection/RightSection to Row 4
- Confirmed Row 5 should only contain interactive controls (sliders, buttons)
- Created this comprehensive plan document

---

## Phase 2: Update INIConfig.h Row4 Constants
**Status:** PENDING
**Objective:** Replace current Row4 namespace with correct constants for all 10 components

### Files to Modify:
- `/Users/larryseyer/AudioDevelopment/OTTO/Source/INIConfig.h`

### Tasks:
1. Replace Row4 namespace component width percentages (currently has 5, needs 10)
2. Add positioning constants for all 10 components in left-to-right order
3. Update calculated dimensions for new component types
4. Ensure total width validation passes
5. Maintain touch accessibility requirements

### Prompt for Phase 2:
```
Phase 2: Update INIConfig.h Row4 Constants

Replace the current Row4 namespace in INIConfig.h with constants for all 10 components:
1. editIconWidthPercent, chevronWidthPercent, dropdownWidthPercent, favoriteIconWidthPercent, labelWidthPercent (×5)
2. Add positioning: editIconX/Y, leftChevronX/Y, dropdownX/Y, rightChevronX/Y, favoriteIconX/Y, togglesLabelX/Y, fillsLabelX/Y, swingLabelX/Y, energyLabelX/Y, volumeLabelX/Y
3. Ensure percentages total ≤100% and maintain MIN_TOUCH_TARGET_PX compliance
4. Keep the existing comment unchanged - it's correct

Use JUCE 8 patterns and follow INI-driven layout principles.
```

---

## Phase 3: Update MainContentComponent.h Declarations
**Status:** PENDING
**Objective:** Add missing Row4 component declarations and remove obsolete ones

### Files to Modify:
- `/Users/larryseyer/AudioDevelopment/OTTO/Source/MainContentComponent.h`

### Tasks:
1. Replace current Row 4 component declarations with complete set of 10
2. Add PhosphorIconButton declarations for edit, chevrons, favorite
3. Add juce::Label declarations for toggles, fills, swing, energy, volume
4. Remove patternGroupLabel, patternStatusLabel, patternAddButton, patternDeleteButton
5. Update method signatures if needed

### Prompt for Phase 3:
```
Phase 3: Update MainContentComponent.h Declarations

In MainContentComponent.h, replace the current Row 4 component declarations with:
- PhosphorIconButton patternGroupEditButton;      // Edit/Pencil icon
- PhosphorIconButton patternGroupLeftChevron;     // Left chevron
- HierarchicalComboBox patternGroupDropdown;      // Keep existing
- PhosphorIconButton patternGroupRightChevron;    // Right chevron  
- PhosphorIconButton patternGroupFavoriteButton;  // Favorite icon
- juce::Label togglesLabel;                       // Toggles label
- juce::Label fillsLabel;                         // Fills label
- juce::Label swingLabel;                         // Swing label
- juce::Label energyLabel;                        // Energy label
- juce::Label volumeLabel;                        // Volume label

Remove: patternGroupLabel, patternStatusLabel, patternAddButton, patternDeleteButton

Follow JUCE 8 coding standards and maintain existing code organization.
```

---

## Phase 4: Implement MainContentComponent.cpp Row4 Layout
**Status:** PENDING
**Objective:** Implement setupRow4Components() and updateRow4Layout() methods

### Files to Modify:
- `/Users/larryseyer/AudioDevelopment/OTTO/Source/MainContentComponent.cpp`

### Tasks:
1. Update setupRow4Components() to initialize all 10 components
2. Implement updateRow4Layout() using new INIConfig::LayoutConstants::Row4:: constants
3. Set up proper FontManager fonts for labels
4. Configure PhosphorIconButton icons and callbacks
5. Remove old Row 4 component setup code
6. Ensure proper addAndMakeVisible() calls

### Prompt for Phase 4:
```
Phase 4: Implement MainContentComponent.cpp Row4 Layout

Update MainContentComponent.cpp:
1. In setupRow4Components(): Initialize all 10 Row 4 components with proper FontManager fonts, PhosphorIconButton icons, and addAndMakeVisible() calls
2. In updateRow4Layout(): Position all components using INIConfig::LayoutConstants::Row4:: constants with layoutManager.scaled() calls
3. Remove old patternGroupLabel, patternStatusLabel, patternAddButton, patternDeleteButton setup/positioning code
4. Use proper JUCE 8 patterns and maintain existing error handling/validation

Follow the exact left-to-right order: edit icon, left chevron, dropdown, right chevron, favorite, toggles label, fills label, swing label, energy label, volume label.
```

---

## Phase 5: Update LeftSection & RightSection for Row5 Only
**Status:** PENDING
**Objective:** Remove components that moved to Row4, keep only Row5 interactive controls

### Files to Modify:
- `/Users/larryseyer/AudioDevelopment/OTTO/Source/MainContentComponentLeftSection.h`
- `/Users/larryseyer/AudioDevelopment/OTTO/Source/MainContentComponentLeftSection.cpp`
- `/Users/larryseyer/AudioDevelopment/OTTO/Source/MainContentComponentRightSection.h`
- `/Users/larryseyer/AudioDevelopment/OTTO/Source/MainContentComponentRightSection.cpp`

### Tasks:
1. **LeftSection:** Remove editButton, leftChevronButton, rightChevronButton, favoriteButton declarations and implementations
2. **LeftSection:** Keep only drumButtons[16] and related 4x4 matrix functionality
3. **RightSection:** Remove togglesLabel, fillsLabel, swingLabel, energyLabel, volumeLabel declarations and implementations
4. **RightSection:** Keep only toggleButtons[5], fillButtons[5], swingSlider, energySlider, volumeSlider
5. Update layout methods to position only remaining components
6. Update getters/setters to remove label-related methods

### Prompt for Phase 5:
```
Phase 5: Update LeftSection & RightSection for Row5 Only

Clean up LeftSection and RightSection to contain only Row 5 interactive controls:

LeftSection changes:
- Remove: editButton, leftChevronButton, rightChevronButton, favoriteButton
- Keep: drumButtons[16] for 4x4 midi pattern matrix
- Update resized() method to layout only drum buttons

RightSection changes:  
- Remove: togglesLabel, fillsLabel, swingLabel, energyLabel, volumeLabel
- Keep: toggleButtons[5], fillButtons[5], swingSlider, energySlider, volumeSlider
- Update resized() method to layout only interactive controls
- Remove label-related getter methods

Maintain all existing functionality for the remaining components.
```

---

## Phase 6: Testing & Validation
**Status:** PENDING
**Objective:** Build, test, and validate the complete reorganization

### Files to Test:
- All modified files from previous phases
- Build system validation
- Runtime component positioning verification

### Tasks:
1. Build the project and resolve any compilation errors
2. Test component positioning and responsive layout
3. Verify all 10 Row 4 components appear correctly
4. Verify Row 5 contains only interactive controls
5. Test component functionality (clicks, sliders, dropdowns)
6. Validate touch target accessibility requirements
7. Test on different screen sizes/scaling factors

### Prompt for Phase 6:
```
Phase 6: Testing & Validation

Build and test the complete Row 4 reorganization:
1. Build the project using ./scripts/build/build_macos.sh
2. Test that all 10 Row 4 components appear in correct positions
3. Verify Row 5 contains only interactive controls (no labels)
4. Test component functionality and responsive scaling
5. Validate touch accessibility (44px minimum targets)
6. Check for any runtime errors or layout issues
7. Document any remaining issues that need fixes

If build errors occur, fix them following JUCE 8 coding standards and INI-driven layout principles.
```

---

## Success Criteria

### Row 4 Success:
- [ ] All 10 components visible in Row 4
- [ ] Components positioned left-to-right as specified in INIConfig comment
- [ ] Proper FontManager fonts applied to labels
- [ ] PhosphorIconButton icons display correctly
- [ ] Responsive scaling works properly
- [ ] Touch targets meet 44px minimum requirement

### Row 5 Success:
- [ ] Only interactive controls remain (no labels)
- [ ] 4x4 drum button matrix functional
- [ ] 5 toggle buttons + 5 fill buttons functional
- [ ] 3 sliders (swing, energy, volume) functional
- [ ] Proper spacing and layout maintained

### Code Quality:
- [ ] Follows JUCE 8 coding standards
- [ ] Uses INI-driven layout constants exclusively
- [ ] Maintains existing error handling patterns
- [ ] No hardcoded values introduced
- [ ] Proper memory management with JUCE patterns

---

## Current Status: Phase 1 Complete ✅

**Next Step:** Proceed to Phase 2 - Update INIConfig.h Row4 Constants

**Notes:**
- INIConfig.h Row 4 comment is CORRECT and should not be changed
- All other code needs to be updated to match this specification
- Maintain backward compatibility where possible
- Follow existing code patterns and architecture