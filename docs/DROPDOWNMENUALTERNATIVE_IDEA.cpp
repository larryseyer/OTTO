class DrumMachineLookAndFeel : public LookAndFeel_V4
{
public:
    void drawPopupMenuItem(Graphics& g, const Rectangle<int>& area,
                          bool isSeparator, bool isActive, bool isHighlighted,
                          bool isTicked, bool hasSubMenu, const String& text,
                          const String& shortcutKeyText, const Drawable* icon,
                          const Colour* textColour) override
    {
        if (text.startsWith("❌"))
        {
            // Draw X mark items with special styling
            String cleanText = text.substring(2); // Remove the ❌
            g.setColour(Colours::red.withAlpha(0.7f));
            g.drawText("❌", area.removeFromLeft(20), Justification::centred);
            g.setColour(findColour(PopupMenu::textColourId));
            g.drawText(cleanText, area, Justification::centredLeft);
        }
        else if (text.contains("UNAVAILABLE")) // or however you mark unavailable fonts
        {
            // Draw with strikethrough
            g.setColour(findColour(PopupMenu::textColourId).withAlpha(0.5f));
            Font font = g.getCurrentFont();
            g.setFont(font);
            g.drawText(text, area, Justification::centredLeft);

            // Draw strikethrough line
            auto textBounds = font.getStringWidth(text);
            int y = area.getCentreY();
            g.drawLine(area.getX(), y, area.getX() + textBounds, y, 1.0f);
        }
        else
        {
            // Use default drawing for normal items, but handle checkmarks
            if (isTicked)
            {
                g.setColour(Colours::green);
                g.drawText("✅", area.removeFromLeft(20), Justification::centred);
                g.setColour(findColour(PopupMenu::textColourId));
                g.drawText(text, area, Justification::centredLeft);
            }
            else
            {
                LookAndFeel_V4::drawPopupMenuItem(g, area, isSeparator, isActive,
                                                 isHighlighted, isTicked, hasSubMenu,
                                                 text, shortcutKeyText, icon, textColour);
            }
        }
    }
};
