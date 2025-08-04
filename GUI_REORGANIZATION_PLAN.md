# OTTO GUI Row Reorganization Plan

## Overview
This document outlines the multi-phase plan to reorganize OTTO's GUI elements to match the HISE reference layout.

## Current Issues (JUCE Layout)
- Labels (TOGGLES, FILLS, SWING, ENERGY, VOLUME) are misplaced in Row 5
- Red row markers are not positioned correctly on far left
- Row 5 contains both interactive controls and labels (should be interactive only)

## Target Layout (HISE Reference)
- Row 1: TopBar (gear, presets, transport, tempo)
- Row 2: Player tabs and current player display  
- Row 3: DrumKit controls (edit, chevrons, dropdown, mixer, mute)
- Row 4: Pattern controls + labels (edit, chevrons, dropdown, favorite, LABELS)
- Row 5: Interactive controls only (4x4 grid, toggle/fill buttons, sliders)
- Row 6: Loop controls (start label, position slider, end label)

## Implementation Phases

### Phase 1: Layout Constants Update ✅
- Added Row1-Row6 namespace constants to INIConfig.h
- Defined label positioning values in Row4 namespace
- Ensured proper spacing calculations

### Phase 2: Row4Component Enhancement ✅
- Updated updatePatternGroupLayout() method
- Positioned labels using new Row4 constants
- Maintained pattern group control positioning

### Phase 3: Row5Component Cleanup ✅
- Verified Row5Component focuses on interactive controls only
- Confirmed no label positioning code in updateInteractiveLayout()
- Optimized layout for available space

### Phase 4: Row Marker Positioning ✅
- Positioned red markers on far left
- Aligned markers above black horizontal lines
- Updated MainContentComponent resized() method

### Phase 5: Duplicate Removal ✅
- Verified labels only exist in Row4Component
- Confirmed no duplicate label components
- Verified single source of truth for each element

## Verification Strategy
- Run lint.sh for code quality
- Test with run_local.sh for visual verification
- Compare with HISE reference image
- Ensure no duplicate elements remain

## Files Modified
- Source/INIConfig.h (added Row1-Row6 layout constants)
- Source/Row4Component.cpp (updated label positioning)
- Source/MainContentComponent.cpp (row markers and component positioning)

## Key Changes Made

### INIConfig.h
- Added comprehensive Row1-Row6 namespace constants
- Defined Row4 label positioning (togglesLabelX, fillsLabelX, etc.)
- Ensured proper percentage-based responsive scaling

### Row4Component.cpp
- Updated label positioning to use new Row4 constants
- Maintained existing pattern group control layout
- Labels now properly positioned in Row 4

### MainContentComponent.cpp
- Added proper row component positioning using new constants
- Positioned red row markers on far left above black lines
- Added row separator positioning for visual clarity

## Expected Results
- Labels (TOGGLES, FILLS, SWING, ENERGY, VOLUME) now appear in Row 4
- Row 5 contains only interactive controls (4x4 grid, toggles, fills, sliders)
- Red row markers positioned correctly on far left
- Layout matches HISE reference image structure
- Maintains 8-player support (ignoring HISE's 4-player layout)

## Testing
1. Run `./lint.sh` to verify code quality
2. Run `./run_local.sh` to test visual layout
3. Compare with HISE reference image
4. Verify all elements are in correct rows
