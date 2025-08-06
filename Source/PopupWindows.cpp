#include "PopupWindows.h"

//==============================================================================
// PatternGroupEditorWindow Implementation
//==============================================================================

class PatternGroupEditorWindow::PatternGroupEditorContent : public juce::Component {
public:
    PatternGroupEditorContent(int patternGroupIndex,
                            INIDataManager* iniDataManager,
                            ColorScheme& colorScheme,
                            FontManager& fontManager)
        : patternGroupIndex(patternGroupIndex)
        , iniDataManager(iniDataManager)
        , colorScheme(colorScheme)
        , fontManager(fontManager)
        , nameEditor("Pattern Group Name")
        , saveButton("Save")
        , cancelButton("Cancel") {
        
        setupComponents();
    }
    
    void paint(juce::Graphics& g) override {
        g.fillAll(colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground));
        
        g.setColour(colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
        g.setFont(fontManager.getFont(FontManager::FontRole::Header, 18.0f));
        g.drawText("Pattern Group Editor", getLocalBounds().removeFromTop(40), juce::Justification::centred);
    }
    
    void resized() override {
        auto bounds = getLocalBounds();
        bounds.removeFromTop(50); // Title space
        
        auto buttonArea = bounds.removeFromBottom(50);
        buttonArea.removeFromBottom(10);
        
        cancelButton.setBounds(buttonArea.removeFromRight(100));
        buttonArea.removeFromRight(10);
        saveButton.setBounds(buttonArea.removeFromRight(100));
        
        bounds.removeFromTop(20);
        nameEditor.setBounds(bounds.removeFromTop(30));
    }
    
    std::function<void(int)> onPatternGroupChanged;
    
private:
    int patternGroupIndex;
    INIDataManager* iniDataManager;
    ColorScheme& colorScheme;
    FontManager& fontManager;
    
    juce::TextEditor nameEditor;
    juce::TextButton saveButton;
    juce::TextButton cancelButton;
    
    void setupComponents() {
        addAndMakeVisible(nameEditor);
        addAndMakeVisible(saveButton);
        addAndMakeVisible(cancelButton);
        
        nameEditor.setText("Group " + juce::String(patternGroupIndex + 1));
        nameEditor.setFont(fontManager.getFont(FontManager::FontRole::Body, 14.0f));
        
        saveButton.onClick = [this]() {
            if (onPatternGroupChanged) {
                onPatternGroupChanged(patternGroupIndex);
            }
            if (auto* parent = findParentComponentOfClass<PatternGroupEditorWindow>()) {
                parent->closeButtonPressed();
            }
        };
        
        cancelButton.onClick = [this]() {
            if (auto* parent = findParentComponentOfClass<PatternGroupEditorWindow>()) {
                parent->closeButtonPressed();
            }
        };
    }
};

PatternGroupEditorWindow::PatternGroupEditorWindow(int patternGroupIndex,
                                                 INIDataManager* iniDataManager,
                                                 ColorScheme& colorScheme,
                                                 FontManager& fontManager)
    : juce::DocumentWindow("Pattern Group Editor", 
                          colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground), 
                          juce::DocumentWindow::closeButton)
    , patternGroupIndex(patternGroupIndex)
    , iniDataManager(iniDataManager)
    , colorScheme(colorScheme)
    , fontManager(fontManager) {
    
    content = std::make_unique<PatternGroupEditorContent>(patternGroupIndex, iniDataManager, colorScheme, fontManager);
    content->onPatternGroupChanged = [this](int newIndex) {
        if (onPatternGroupChanged) {
            onPatternGroupChanged(newIndex);
        }
    };
    
    setContentOwned(content.get(), true);
    setResizable(true, true);
    setUsingNativeTitleBar(true);
}

PatternGroupEditorWindow::~PatternGroupEditorWindow() = default;

void PatternGroupEditorWindow::closeButtonPressed() {
    setVisible(false);
    delete this;
}

void PatternGroupEditorWindow::saveStates(ComponentState& state) {
    // Save window state if needed
}

void PatternGroupEditorWindow::loadStates(const ComponentState& state) {
    // Load window state if needed
}
