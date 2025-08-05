#pragma once

#include "JUCE8_CODING_STANDARDS.h"
#include <JuceHeader.h>
#include "ThemeManager.h"
#include "ComponentState.h"
#include "INIConfig.h"

/**
 * @class ThemeEditor
 * @brief Real-time theme customization interface for OTTO Phase 9D
 * 
 * Provides a comprehensive interface for creating and editing themes:
 * - Real-time color picker with live preview
 * - Gradient editor for advanced theming
 * - Accessibility validation and preview
 * - Theme import/export functionality
 * - Preset management and sharing
 * 
 * Integrates seamlessly with ThemeManager for immediate theme application.
 */
class ThemeEditor : public juce::Component,
                   public juce::ChangeListener,
                   public ThemeManager::Listener {
public:
    /**
     * @enum EditMode
     * @brief Different editing modes for the theme editor
     */
    enum class EditMode {
        Colors,        // Basic color editing
        Gradients,     // Gradient and advanced effects
        Accessibility, // Accessibility testing and validation
        Preview,       // Full theme preview
        Export         // Theme sharing and export
    };

    /**
     * @struct ColorGroup
     * @brief Organized color categories for editing
     */
    struct ColorGroup {
        juce::String name;
        juce::String description;
        std::vector<std::pair<juce::String, juce::Colour*>> colors;
        bool expanded = true;
        
        ColorGroup(const juce::String& groupName, const juce::String& desc = {})
            : name(groupName), description(desc) {}
    };

    ThemeEditor(ThemeManager& themeManager);
    ~ThemeEditor() override;

    // Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override;

    // Theme editing
    void startEditingTheme(const juce::String& themeName);
    void createNewTheme(const juce::String& basedOn = "Dark");
    void saveCurrentTheme();
    void cancelEditing();
    void resetToOriginal();
    
    // Edit mode management
    void setEditMode(EditMode mode);
    EditMode getCurrentEditMode() const { return currentEditMode; }
    
    // Color editing
    void setSelectedColorGroup(int groupIndex);
    int getSelectedColorGroup() const { return selectedColorGroup; }
    void setSelectedColor(const juce::String& colorName);
    juce::String getSelectedColor() const { return selectedColorName; }
    
    // Preview management
    void enableLivePreview(bool enabled);
    bool isLivePreviewEnabled() const { return livePreviewEnabled; }
    void refreshPreview();
    
    // Accessibility testing
    void runAccessibilityCheck();
    juce::StringArray getAccessibilityIssues() const { return accessibilityIssues; }
    void previewAccessibilityMode(ThemeManager::AccessibilityMode mode);
    
    // Import/Export
    void exportTheme();
    void importTheme();
    void shareTheme();
    
    // State management
    void saveState(ComponentState& state) const;
    void loadState(const ComponentState& state);

    // ChangeListener override
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    
    // ThemeManager::Listener overrides
    void themeChanged(const juce::String& newThemeName) override;
    void themePreviewStarted(const juce::String& previewThemeName) override;
    void themePreviewStopped() override;

private:
    ThemeManager& themeManager;
    
    // Current editing state
    EditMode currentEditMode = EditMode::Colors;
    juce::String editingThemeName;
    ThemeManager::ThemeColors editingColors;
    ThemeManager::ThemeColors originalColors;
    bool isEditing = false;
    bool livePreviewEnabled = true;
    
    // Color organization
    std::vector<ColorGroup> colorGroups;
    int selectedColorGroup = 0;
    juce::String selectedColorName;
    
    // Accessibility validation
    juce::StringArray accessibilityIssues;
    ThemeManager::AccessibilityMode previewAccessibilityMode = ThemeManager::AccessibilityMode::None;
    
    // UI Components
    std::unique_ptr<juce::TabbedComponent> modeTabsComponent;
    
    // Colors tab components
    std::unique_ptr<juce::ListBox> colorGroupsList;
    std::unique_ptr<juce::ColourSelector> colorSelector;
    std::unique_ptr<juce::Label> colorNameLabel;
    std::unique_ptr<juce::Label> colorValueLabel;
    std::unique_ptr<juce::TextButton> resetColorButton;
    
    // Gradients tab components
    std::unique_ptr<juce::ToggleButton> useGradientsToggle;
    std::unique_ptr<juce::Component> gradientEditor;
    std::unique_ptr<juce::Slider> gradientOpacitySlider;
    
    // Accessibility tab components
    std::unique_ptr<juce::ComboBox> accessibilityModeCombo;
    std::unique_ptr<juce::TextButton> runAccessibilityCheckButton;
    std::unique_ptr<juce::TextEditor> accessibilityResultsEditor;
    std::unique_ptr<juce::Component> contrastPreview;
    
    // Preview tab components
    std::unique_ptr<juce::Component> themePreviewComponent;
    std::unique_ptr<juce::ToggleButton> livePreviewToggle;
    std::unique_ptr<juce::TextButton> fullScreenPreviewButton;
    
    // Export tab components
    std::unique_ptr<juce::TextEditor> themeNameEditor;
    std::unique_ptr<juce::TextEditor> themeDescriptionEditor;
    std::unique_ptr<juce::TextEditor> themeAuthorEditor;
    std::unique_ptr<juce::TextButton> exportButton;
    std::unique_ptr<juce::TextButton> importButton;
    std::unique_ptr<juce::TextButton> shareButton;
    
    // Control buttons
    std::unique_ptr<juce::TextButton> saveButton;
    std::unique_ptr<juce::TextButton> cancelButton;
    std::unique_ptr<juce::TextButton> resetButton;
    std::unique_ptr<juce::TextButton> newThemeButton;
    
    // Layout helpers
    void setupColorGroupsList();
    void setupColorSelector();
    void setupGradientEditor();
    void setupAccessibilityTab();
    void setupPreviewTab();
    void setupExportTab();
    void setupControlButtons();
    
    void layoutColorsTab(juce::Rectangle<int> area);
    void layoutGradientsTab(juce::Rectangle<int> area);
    void layoutAccessibilityTab(juce::Rectangle<int> area);
    void layoutPreviewTab(juce::Rectangle<int> area);
    void layoutExportTab(juce::Rectangle<int> area);
    
    // Color group management
    void initializeColorGroups();
    void updateColorGroupsList();
    void selectColorGroup(int groupIndex);
    void selectColor(const juce::String& colorName);
    
    // Color editing helpers
    void onColorChanged(const juce::Colour& newColor);
    void updateColorSelector();
    void updateColorLabels();
    juce::Colour* getSelectedColorPointer();
    
    // Gradient editing helpers
    void onGradientToggled();
    void updateGradientEditor();
    void onGradientChanged();
    
    // Accessibility helpers
    void updateAccessibilityModeCombo();
    void onAccessibilityModeChanged();
    void performAccessibilityCheck();
    void updateContrastPreview();
    float calculateContrastRatio(const juce::Colour& fg, const juce::Colour& bg);
    
    // Preview helpers
    void updateThemePreview();
    void onLivePreviewToggled();
    void showFullScreenPreview();
    
    // Export/Import helpers
    void onExportButtonClicked();
    void onImportButtonClicked();
    void onShareButtonClicked();
    void updateExportFields();
    
    // Control button handlers
    void onSaveButtonClicked();
    void onCancelButtonClicked();
    void onResetButtonClicked();
    void onNewThemeButtonClicked();
    
    // Validation helpers
    bool validateThemeName(const juce::String& name);
    bool hasUnsavedChanges() const;
    void showUnsavedChangesDialog();
    
    // Color group list box model
    class ColorGroupListBoxModel : public juce::ListBoxModel {
    public:
        ColorGroupListBoxModel(ThemeEditor& editor) : themeEditor(editor) {}
        
        int getNumRows() override;
        void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
        void selectedRowsChanged(int lastRowSelected) override;
        
    private:
        ThemeEditor& themeEditor;
    };
    
    std::unique_ptr<ColorGroupListBoxModel> colorGroupsListModel;
    
    // Theme preview component
    class ThemePreviewComponent : public juce::Component {
    public:
        ThemePreviewComponent(ThemeEditor& editor);
        void paint(juce::Graphics& g) override;
        void resized() override;
        void updatePreview();
        
    private:
        ThemeEditor& themeEditor;
        std::unique_ptr<juce::TextButton> sampleButton;
        std::unique_ptr<juce::Slider> sampleSlider;
        std::unique_ptr<juce::Label> sampleLabel;
        std::unique_ptr<juce::ComboBox> sampleCombo;
        std::unique_ptr<juce::ToggleButton> sampleToggle;
    };
    
    // Gradient editor component
    class GradientEditorComponent : public juce::Component {
    public:
        GradientEditorComponent(ThemeEditor& editor);
        void paint(juce::Graphics& g) override;
        void resized() override;
        void mouseDown(const juce::MouseEvent& e) override;
        void mouseDrag(const juce::MouseEvent& e) override;
        
        void setGradient(const juce::ColourGradient& gradient);
        juce::ColourGradient getGradient() const { return currentGradient; }
        
        std::function<void(const juce::ColourGradient&)> onGradientChanged;
        
    private:
        ThemeEditor& themeEditor;
        juce::ColourGradient currentGradient;
        int selectedStop = -1;
        
        void addColorStop(float position, const juce::Colour& color);
        void removeColorStop(int index);
        void updateGradient();
    };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ThemeEditor)
};