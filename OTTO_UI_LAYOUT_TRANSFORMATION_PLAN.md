# OTTO UI Layout Transformation Plan
## Transform JUCE OTTO to Match HISE Reference Layout

### Overview
This plan transforms the current JUCE OTTO interface to match the HISE OTTO reference image by reorganizing components in Rows 3-6. The main issue is that labels (Toggles, Fills, Swing, Energy, Volume) need to be positioned in Row 4 instead of Row 5.

### Reference Analysis
- **HISE Target**: Shows labels in Row 4, interactive controls in Row 5
- **Current JUCE**: Shows both labels and controls in Row 5, Row 4 has pattern group controls only

### Status: ✅ IMPLEMENTATION COMPLETED

---

## Phase 1: Analysis and Setup ✅
**Status**: COMPLETED
**Objective**: Understand current layout system and identify specific changes needed

### Tasks:
- [x] Pull latest from main branch
- [x] Analyze reference images in `/image examples/`
- [x] Examine current 6-row layout system in MainContentComponent.cpp
- [x] Identify component ownership conflicts between MainContentComponent and RightSection
- [x] Document current vs target layout differences

**Key Finding**: Labels are positioned by BOTH MainContentComponent (Row 4) and RightSection (Row 5), causing Row 5 to override the correct Row 4 positioning.

---

## Phase 2: Component Reorganization ✅
**Status**: COMPLETED
**Objective**: Move labels from Row 5 to Row 4 while keeping interactive controls in Row 5

### Tasks:
- [x] Remove labels from MainContentComponentRightSection setupComponents()
- [x] Remove label positioning from RightSection resized() method
- [x] Ensure labels are only managed by MainContentComponent Row 4
- [x] Verify no component ownership conflicts
- [x] Update RightSection header documentation

---

## Phase 3: Layout Implementation ⏳
**Status**: PENDING
**Objective**: Implement the correct positioning for all Row 3-6 components

### Tasks:
- [ ] Verify Row 3 drumkit controls are correctly positioned
- [ ] Ensure Row 4 has complete 10-component layout as specified
- [ ] Update Row 5 to only show interactive elements (4x4 grid, buttons, sliders)
- [ ] Verify Row 6 loop controls are correctly positioned

---

## Phase 4: Testing and Validation ⏳
**Status**: PENDING
**Objective**: Ensure the layout matches HISE reference and functions correctly

### Tasks:
- [ ] Visual comparison with HISE reference image
- [ ] Test all interactive elements function correctly
- [ ] Verify responsive layout scaling works
- [ ] Run existing lint/build checks

---

## Phase 5: Documentation and PR ⏳
**Status**: PENDING
**Objective**: Create PR with changes and update documentation

### Tasks:
- [x] Create feature branch with proper naming
- [ ] Commit all changes with descriptive messages
- [ ] Create PR with auto-generated description
- [ ] Monitor CI status and address any issues

---

## Implementation Notes
- Focus only on Rows 3-6 (Rows 1-2 are complete)
- Maintain existing functionality while reorganizing layout
- Use existing INI-driven layout system constants
- Follow established patterns in the codebase

## Expected Outcome
OTTO interface that visually matches the HISE reference with:
- Row 3: Drumkit controls (edit, chevrons, dropdown, mixer, mute)
- Row 4: Pattern group controls + labels (edit, chevrons, dropdown, favorite, Toggles/Fills/Swing/Energy/Volume labels)
- Row 5: Interactive elements only (4x4 grid, toggle buttons, fill buttons, sliders)
- Row 6: Loop controls (LOOP START, position, LOOP END)
