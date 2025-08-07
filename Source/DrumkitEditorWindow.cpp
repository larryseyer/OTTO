#include "DrumkitEditorWindow.h"
#include "INIConfig.h"

DrumkitEditorWindow::DrumkitEditorWindow(INIDataManager& iniManager, 
                                       ColorScheme& colorScheme, 
                                       FontManager& fontManager)
    : PopupWindow("DrumKit Editor", juce::Colours::transparentBlack, true)
    , iniDataManager(iniManager)
    , colorScheme(colorScheme)
    , fontManager(fontManager) {
    
    setupWindow();
    
    editorContent = std::make_unique<DrumKitEditorContent>(iniDataManager, colorScheme, fontManager);
    addAndMakeVisible(*editorContent);
    
    setSize(800, 600);
    centreWithSize(800, 600);
}

DrumkitEditorWindow::~DrumkitEditorWindow() {
    editorContent.reset();
}

void DrumkitEditorWindow::paint(juce::Graphics& g) {
    g.fillAll(colorScheme.getColor(ColorScheme::ColorRole::WindowBackground));
    
    auto bounds = getLocalBounds();
    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::Separator));
    g.drawRect(bounds, 2);
    
    auto titleArea = bounds.removeFromTop(40);
    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground));
    g.fillRect(titleArea);
    
    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    g.setFont(fontManager.getFont(FontManager::FontRole::Header, 16.0f));
    g.drawText("DrumKit Editor", titleArea, juce::Justification::centred);
}

void DrumkitEditorWindow::resized() {
    auto bounds = getLocalBounds();
    bounds.removeFromTop(40);
    
    if (editorContent) {
        editorContent->setBounds(bounds.reduced(10));
    }
}

void DrumkitEditorWindow::showEditor() {
    setVisible(true);
    toFront(true);
    grabKeyboardFocus();
}

void DrumkitEditorWindow::hideEditor() {
    setVisible(false);
}

bool DrumkitEditorWindow::isEditorVisible() const {
    return isVisible();
}

void DrumkitEditorWindow::setupWindow() {
    setUsingNativeTitleBar(false);
    setDropShadowEnabled(true);
    setResizable(true, true);
    
    setColour(juce::ResizableWindow::backgroundColourId, 
              colorScheme.getColor(ColorScheme::ColorRole::WindowBackground));
}

void DrumkitEditorWindow::updateLayout() {
    resized();
}
