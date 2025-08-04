#include "RowComponentBase.h"
#include "ErrorHandling.h"

RowComponentBase::RowComponentBase(int rowNumber,
                                 ResponsiveLayoutManager& layoutManager,
                                 FontManager& fontManager,
                                 ColorScheme& colorScheme)
    : rowNumber(rowNumber)
    , layoutManager(layoutManager)
    , fontManager(fontManager)
    , colorScheme(colorScheme) {
    
    if (rowNumber < 1 || rowNumber > 6) {
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "Invalid row number: " + juce::String(rowNumber), "RowComponentBase");
    }
    
    setupCommonComponents();
}

void RowComponentBase::lookAndFeelChanged() {
    updateCommonColors();
}

void RowComponentBase::setupCommonComponents() {
    setOpaque(false);
}

void RowComponentBase::updateCommonColors() {
    repaint();
}

juce::Rectangle<int> RowComponentBase::getScaledBounds(int x, int y, int width, int height) const {
    return juce::Rectangle<int>(
        layoutManager.scaled(x),
        layoutManager.scaled(y),
        layoutManager.scaled(width),
        layoutManager.scaled(height)
    );
}
