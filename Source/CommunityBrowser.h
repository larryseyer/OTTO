#pragma once
#include <JuceHeader.h>
#include "INIConfig.h"
#include "ColorScheme.h"

class CommunityBrowser : public juce::Component,
                        private juce::Timer {
public:
    CommunityBrowser(ColorScheme& colorScheme);
    ~CommunityBrowser() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    struct CommunityItem {
        enum class Type {
            DrumKit, Pattern, Effect, MixTemplate
        };

        juce::String id;
        juce::String name;
        juce::String description;
        juce::String author;
        Type type;
        juce::Time uploadDate;
        int downloads = INIConfig::Defaults::ZERO_VALUE;
        float rating = INIConfig::UI::COMMUNITY_BROWSER_MIN_RATING;
        int64_t fileSize = INIConfig::Defaults::ZERO_VALUE;
        bool isDownloaded = false;
    };

private:
    void setupUI();
    void timerCallback() override;

    void searchTextChanged();
    void categoryChanged();
    void sortChanged();
    void filterByCategory();
    void sortItems();
    void downloadSelected();
    void sharePreset();
    void refreshContent();
    void showPreview(const CommunityItem& item);
    void hidePreview();
    void createDummyContent();

    ColorScheme& colorScheme;

    juce::TextEditor searchBox;
    juce::ComboBox categoryFilter;
    juce::ComboBox sortOptions;
    juce::ListBox presetList;
    juce::TextButton downloadButton;
    juce::TextButton shareButton;
    juce::TextButton refreshButton;

    juce::Array<CommunityItem> communityItems;
    juce::Array<CommunityItem> filteredItems;
    std::optional<CommunityItem> currentPreviewItem;

    class ItemListModel : public juce::ListBoxModel {
    public:
        ItemListModel(CommunityBrowser& p) : parent(p) {}

        int getNumRows() override;
        void paintListBoxItem(int rowNumber, juce::Graphics& g,
                            int width, int height, bool rowIsSelected) override;
        void listBoxItemClicked(int row, const juce::MouseEvent&) override;
        void listBoxItemDoubleClicked(int row, const juce::MouseEvent&) override;

    private:
        CommunityBrowser& parent;
    };

    ItemListModel listModel{*this};

    class PreviewPanel : public juce::Component {
    public:
        PreviewPanel(ColorScheme& cs);
        void paint(juce::Graphics& g) override;
        void resized() override;
        void updatePreview(const CommunityItem& item);

    private:
        void drawPreviewContent(juce::Graphics& g, juce::Rectangle<int> area);
        static juce::String formatFileSize(int64_t bytes);

        ColorScheme& colorScheme;
        juce::Label titleLabel;
        juce::Label descriptionLabel;
        juce::Label authorLabel;
        juce::Label statsLabel;
        juce::Label tagsLabel;
        std::optional<CommunityItem> currentItem;
    };

    PreviewPanel previewPanel;

    class ShareDialog : public juce::Component {
    public:
        ShareDialog(ColorScheme& cs);
        void paint(juce::Graphics& g) override;
        void resized() override;

        std::function<void(const juce::String&, const juce::String&, CommunityItem::Type)> onShare;

    private:
        void doShare();

        ColorScheme& colorScheme;
        juce::Label titleLabel;
        juce::TextEditor nameEditor;
        juce::TextEditor descriptionEditor;
        juce::ComboBox typeSelector;
        juce::TextButton shareButton;
        juce::TextButton cancelButton;
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CommunityBrowser)
};
