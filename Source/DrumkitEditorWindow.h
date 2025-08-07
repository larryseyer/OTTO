#pragma once

#include <JuceHeader.h>
#include "PopupWindows.h"
#include "DrumKitEditorContent.h"
#include "ColorScheme.h"
#include "FontManager.h"
#include "ResponsiveLayoutManager.h"
#include "INIDataManager.h"

class DrumkitEditorWindow : public PopupWindow {
public:
    DrumkitEditorWindow(INIDataManager& iniManager, 
                       ColorScheme& colorScheme, 
                       FontManager& fontManager);
    ~DrumkitEditorWindow() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void showEditor();
    void hideEditor();
    bool isEditorVisible() const;

private:
    std::unique_ptr<DrumKitEditorContent> editorContent;
    INIDataManager& iniDataManager;
    ColorScheme& colorScheme;
    FontManager& fontManager;
    
    void setupWindow();
    void updateLayout();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DrumkitEditorWindow)
};
