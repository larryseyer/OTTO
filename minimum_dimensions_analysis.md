# OTTO Minimum Dimensions Analysis

## Touch Accessibility Requirements
- **Minimum Touch Target:** 44px (iOS/Android accessibility standard)
- **Current Resolution:** 1920x1080

## Critical Layout Constraints

### Height Requirements
Based on 6-row layout with percentages:
- Row 1: 7.0% = 75.6px
- Row 2: 6.0% = 64.8px  
- Row 3: 16.0% = 172.8px ← **Most Height Critical**
- Row 4: 14.0% = 151.2px
- Row 5: 45.0% = 486px ← **Pattern Matrix Requirement**
- Row 6: 12.0% = 129.6px

**Height Analysis:**
- Row 3 needs 44px buttons (75% of content height)
- Content height = Row height - 2×padding
- If buttons = 44px, content height = 44/0.75 = 58.67px
- Row height = 58.67 + 2×padding
- Pattern matrix needs minimum 300px absolute
- Row 5 = 45% of total height, so total height ≥ 300/0.45 = 666.67px

### Width Requirements  
Most constrained: Row 3 layout
[Player#] [Edit] [<] [DrumKit ▼] [>] [Mute] [Mixer]

**Component Breakdown (at minimum scale):**
- defaultMargin: 1.67% of width
- playerNumber: 6.67% of width
- buttonSpacing: 0.83% of width (×6 spaces)
- editButton: 44px minimum
- leftChevron: 44px minimum
- dropdown: 12.5% of width
- rightChevron: 44px minimum
- muteButton: 44px minimum
- mixerButton: 44px minimum

**Width Calculation:**
Fixed components: 44px × 5 = 220px
Percentage components: 1.67% + 6.67% + (0.83% × 6) + 12.5% = 25.82%
Remaining space for fixed: 100% - 25.82% = 74.18%

If 74.18% = 220px, then 100% = 220/0.7418 = 296.64px

## **CALCULATED MINIMUMS**

### Absolute Minimum (Touch-Safe)
- **Minimum Width:** 800px
- **Minimum Height:** 667px

### Recommended Minimum (Usable)  
- **Minimum Width:** 1024px
- **Minimum Height:** 768px

## Validation Notes
- 800×667 maintains 44px touch targets
- Pattern matrix gets 300px in Row 5
- All percentage-based layouts scale correctly
- Font sizes remain readable (minimum 20px)