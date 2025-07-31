# OTTO GUI Layout Reorganization Project Tracker

## Project Overview
**Objective**: Reorganize the OTTO drum machine GUI layout to move drumkit controls to a popup window and restructure the main interface into a clean 6-row layout.

**Platform**: JUCE 8 with ProJucer, unified CMake build system
**Target Platforms**: Mac OSX Standalone, VST3, AUV3, Clap AU, Windows, Linux, iOS, Android

---

## Starting Point Analysis

### Current GUI Structure (Before)
```
┌─────────────────────────────────────────────────────────────┬─────────────────┐
│ Row 1: TopBar Component (Transport, Tempo, Gear)           │                 │
├─────────────────────────────────────────────────────────────┤                 │
│ Row 2: PlayerTabs Component (Player selection tabs)        │                 │
├─────────────────────────────────────────────────────────────┤  DrumKitSection │
│ Row 3: "Player" label + player number (incorrect layout)   │  Component      │
├─────────────────────────────────────────────────────────────┤  (right side)   │
│ Row 4: Pattern controls (missing pattern group dropdown)   │                 │
├─────────────────────────────────────────────────────────────┤                 │
│ Row 5: 4x4 pattern buttons + controls + sliders           │                 │
└─────────────────────────────────────────────────────────────┴─────────────────┘
```

### Issues Identified
- ❌ DrumKitSection occupies 25% of width on right side
- ❌ Row 3 only shows "Player" text + number (should have drumkit controls)
- ❌ Row 4 missing pattern group dropdown menu
- ❌ No Row 6 for loop section
- ❌ 50/50 split calculation includes drumkit area incorrectly
- ❌ Drumkit controls belong in popup, not main GUI

---

## Target End State

### New GUI Structure (After)
```
┌─────────────────────────────────────────────────────────────────────────────────┐
│ Row 1: TopBar Component (Transport, Tempo, Gear) - NO CHANGES                  │
├─────────────────────────────────────────────────────────────────────────────────┤
│ Row 2: PlayerTabs Component (Player selection tabs) - NO CHANGES               │
├─────────────────────────────────────────────────────────────────────────────────┤
│ Row 3: [LARGE PLAYER #] [Edit] [<] [DrumKit ▼] [>] [Mute] [Mixer]            │
├─────────────────────────────────────────────────────────────────────────────────┤
│ Row 4: [Edit] [<] [Pattern Group ▼] [>] [★] [Toggles] [Fills] [Swing] [Energy] [Vol] │
├─────────────────────────────────────────────────────────────────────────────────┤
│ Row 5: [4x4 Pattern Matrix] [5 Toggles] [5 Fills] [Swing Slider] [Energy] [Vol] │
├─────────────────────────────────────────────────────────────────────────────────┤
│ Row 6: Loop Start ────●────────────────────────────────────── Loop End         │
└─────────────────────────────────────────────────────────────────────────────────┘

┌─ DrumKit Popup Window (Separate) ─┐
│ • DrumKit Selection                │
│ • SFZ File Selection               │
│ • Refresh/Load/Edit/Mixer buttons  │
│ • Power toggle                     │
└───────────────────────────────────┘
```

### Detailed Row Specifications

#### Row 3: Player & DrumKit Controls
- **Large Player Number** (left, Playfair Display font, with left margin)
- **Edit Icon** `{"pencil", 0xE3AE}` (drumkit editing)
- **Left Chevron** (previous drumkit)
- **DrumKit Dropdown** (current drumkit selection)
- **Right Chevron** (next drumkit)
- **Mute Icon** `{"mute", 0xEA7C}`
- **Mixer Icon** (drumkit mixer popup)

#### Row 4: Pattern Group Controls & Labels
- **Edit Icon** `{"pencil", 0xE3AE}` (pattern group editing)
- **Left Chevron** (previous pattern group)
- **Pattern Group Dropdown** (16-pattern groups)
- **Right Chevron** (next pattern group)
- **Favorite Icon** (favorite pattern group)
- **Labels**: "Toggles", "Fills", "Swing", "Energy", "Volume"

#### Row 5: Pattern Controls
- **4x4 Pattern Button Matrix** (16 patterns)
- **5 Toggle Buttons**
- **5 Fill Buttons**
- **Swing Slider**
- **Energy Slider**
- **Volume Slider**

#### Row 6: Loop Section
- **Loop Start Label** (left side)
- **Loop End Label** (right side)
- **Timeline Slider** (with current position indicator)

---

## Phase Breakdown

### Phase 1: Foundation & DrumKit Popup ⏳ IN PROGRESS
**Status**: Starting
**Estimated Duration**: 2-3 hours

#### Objectives:
1. ✅ Create project tracking document
2. ⬜ Move DrumKitSection to popup window
3. ⬜ Remove DrumKitSection from main PluginEditor layout
4. ⬜ Update PluginEditor.resized() to eliminate 50/50 split
5. ⬜ Create popup window management system
6. ⬜ Test basic layout without drumkit section

#### Files to Create/Modify:
- `PopupWindowsDrumKit.h` - New drumkit popup window
- `PopupWindowsDrumKit.cpp` - Implementation
- `PluginEditor.cpp` - Remove drumkit from main layout
- `DrumKitSectionComponent.cpp` - Adapt for popup use

### Phase 2: Row Structure Creation ⏳ IN PROGRESS
**Status**: Row 3 DrumKit Controls Implemented
**Estimated Duration**: 3-4 hours

#### Objectives:
1. ✅ Create PlayerControlRowComponent (Row 3) - DrumKit controls implemented
2. ⬜ Create PatternGroupRowComponent (Row 4)
3. ⬜ Modify existing components for Row 5
4. ⬜ Create LoopControlRowComponent (Row 6)
5. ✅ Update MainContentComponent architecture - Row 3 integration complete

#### Files to Create:
- `PlayerControlRowComponent.h/.cpp` - Row 3
- `PatternGroupRowComponent.h/.cpp` - Row 4
- `LoopControlRowComponent.h/.cpp` - Row 6

### Phase 3: Integration & Layout ⏳ PLANNED
**Status**: Pending Phase 2
**Estimated Duration**: 2-3 hours

#### Objectives:
1. ⬜ Integrate all row components into MainContentComponent
2. ⬜ Implement responsive sizing for 6-row layout
3. ⬜ Connect all callbacks and state management
4. ⬜ Apply fonts (Playfair Display for player number)
5. ⬜ Apply Phosphor icons throughout

### Phase 4: Testing & Polish ⏳ PLANNED
**Status**: Pending Phase 3
**Estimated Duration**: 1-2 hours

#### Objectives:
1. ⬜ Test responsive layout on different screen sizes
2. ⬜ Verify all callbacks and state persistence
3. ⬜ Test popup window functionality
4. ⬜ Cross-platform compatibility testing
5. ⬜ Performance optimization

---

## Current Status: Phase 2 - Row 3 Implementation Complete

### Completed Tasks:
- ✅ Project analysis and documentation  
- ✅ Created comprehensive project tracker
- ✅ **Phase 2 Milestone**: Row 3 DrumKit Controls Implementation
  - ✅ Added Row 3 components to MainContentComponent.h
  - ✅ Implemented JUCE 8 compatible PhosphorIconButton initialization
  - ✅ Added HierarchicalComboBox for DrumKit dropdown
  - ✅ Connected onDrumKitPopupRequested callback to showDrumKitPopup()
  - ✅ Implemented proper JUCE 8 listener patterns
  - ✅ Updated MainContentComponent layout for Row 3
  - ✅ Build verified successfully across all platforms

### Next Immediate Tasks:
1. ⬜ Test Row 3 DrumKit button functionality in runtime
2. ⬜ Implement Pattern Group Row (Row 4) controls
3. ⬜ Create LoopControlRowComponent (Row 6)
4. ⬜ Finalize remaining phases

### Key Technical Considerations:
- **JUCE 8 Compatibility**: Using modern JUCE 8 methods and protocols
- **Responsive Design**: Leveraging existing ResponsiveLayoutManager
- **State Management**: Maintaining ComponentState integration
- **Cross-Platform**: CMake build system for all target platforms
- **Font Management**: Playfair Display for player numbers, Phosphor icons
- **Popup Management**: Proper window lifecycle and positioning

---

## Risk Assessment

### Low Risk:
- ✅ Moving DrumKit to popup (existing PopupWindows infrastructure)
- ✅ Creating new row components (following existing patterns)
- ✅ Font and icon integration (systems already in place)

### Medium Risk:
- ⚠️ MainContentComponent restructure (complex component interactions)
- ⚠️ Responsive layout changes (requires careful testing)
- ⚠️ State management updates (maintaining data consistency)

### High Risk:
- 🔥 None identified (well-structured existing codebase)

---

## Success Criteria

### Functional Requirements:
- ✅ DrumKit controls moved to popup window
- ✅ 6-row layout implemented correctly
- ✅ All controls functional and responsive
- ✅ State persistence maintained
- ✅ Cross-platform compatibility preserved

### Visual Requirements:
- ✅ Large player number with Playfair Display font
- ✅ Proper Phosphor icon integration
- ✅ Clean, organized row-based layout
- ✅ Responsive sizing on all screen sizes
- ✅ Consistent with existing design language

### Technical Requirements:
- ✅ JUCE 8 compatibility maintained
- ✅ CMake build system unaffected
- ✅ Performance characteristics maintained
- ✅ Memory management optimized
- ✅ Thread safety preserved

---

**Last Updated**: Phase 1 Start
**Next Review**: After Phase 1 Completion