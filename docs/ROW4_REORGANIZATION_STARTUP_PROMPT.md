# Row 4 Reorganization - New Chat Startup Prompt

## Copy and paste this exact prompt to start a new chat for this project:

---

**OTTO Row 4 Component Reorganization - Phase 2 Start**

I need to implement a 6-phase reorganization of OTTO's UI components to match the correct Row 4 layout. 

**Context:**
- Repository: `/Users/larryseyer/AudioDevelopment/OTTO`
- INIConfig.h Row 4 comment is CORRECT and specifies 10 components
- Current implementation only has 5 components in Row 4
- Missing components are incorrectly placed in LeftSection/RightSection (Row 5)

**Read these files first:**
1. `/Users/larryseyer/AudioDevelopment/OTTO/docs/ROW4_REORGANIZATION_PLAN.md` - Complete 6-phase plan
2. `/Users/larryseyer/AudioDevelopment/OTTO/docs/JUCE8_CODING_STANDARDS.h` - Coding standards
3. `/Users/larryseyer/AudioDevelopment/OTTO/docs/ZENCODER_RULES.md` - Development rules
4. `/Users/larryseyer/AudioDevelopment/OTTO/docs/DESIGNRULES.md` - Design principles
5. `/Users/larryseyer/AudioDevelopment/OTTO/Source/INIConfig.h` - Current Row4 namespace (lines 1542-1609)

**Current Status:** Phase 1 Complete ✅ - Analysis & Planning done

**Execute Phase 2:** Update INIConfig.h Row4 Constants

Replace the current Row4 namespace in `/Users/larryseyer/AudioDevelopment/OTTO/Source/INIConfig.h` with constants for all 10 components specified in the comment:

**Required Components (left-to-right):**
1. Edit/Pencil icon for Pattern groups → `editIconWidthPercent` + positioning
2. Left chevron → `chevronWidthPercent` + positioning  
3. Pattern groups dropdown menu/label combo → `dropdownWidthPercent` + positioning
4. Right chevron → `chevronWidthPercent` + positioning
5. Favorites group icon → `favoriteIconWidthPercent` + positioning
6. Toggles buttons label → `labelWidthPercent` + positioning
7. Fills buttons label → `labelWidthPercent` + positioning
8. Swing slider label → `labelWidthPercent` + positioning
9. Energy slider label → `labelWidthPercent` + positioning
10. Volume slider label → `labelWidthPercent` + positioning

**Requirements:**
- Keep the existing Row4 comment unchanged (it's correct)
- Ensure percentages total ≤100% of interface width
- Maintain MIN_TOUCH_TARGET_PX compliance (44px minimum)
- Use proper constexpr calculations with Defaults::DEFAULT_INTERFACE_WIDTH
- Follow existing INI-driven layout patterns
- Add positioning constants for all 10 components (X/Y coordinates)

**Success Criteria:**
- All 10 components have width/height percentages defined
- All 10 components have X/Y positioning constants
- Total width validation passes (static_assert)
- Touch accessibility validation passes (static_assert)
- Follows JUCE 8 coding standards and existing code patterns

Start by reading the plan document, then examine the current INIConfig.h Row4 namespace, and implement the complete replacement.

---