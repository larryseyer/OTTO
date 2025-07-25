#include "CommunityBrowser.h"
#include "INIConfig.h"

CommunityBrowser::CommunityBrowser(ColorScheme& cs)
    : colorScheme(cs), previewPanel(cs) {
    setupUI();

    startTimerHz(1);
}

CommunityBrowser::~CommunityBrowser() {
    stopTimer();
}

void CommunityBrowser::setupUI() {
    searchBox.setTextToShowWhenEmpty("Search presets...",
        colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
    searchBox.onTextChange = [this] { searchTextChanged(); };
    addAndMakeVisible(searchBox);

    categoryFilter.addItem("All Categories", 1);
    categoryFilter.addItem("Drum Kits", 2);
    categoryFilter.addItem("Patterns", 3);
    categoryFilter.addItem("Effects", 4);
    categoryFilter.addItem("Mix Templates", 5);
    categoryFilter.setSelectedId(1);
    categoryFilter.onChange = [this] { categoryChanged(); };
    addAndMakeVisible(categoryFilter);

    sortOptions.addItem("Most Popular", 1);
    sortOptions.addItem("Most Recent", 2);
    sortOptions.addItem("Top Rated", 3);
    sortOptions.addItem("Most Downloaded", 4);
    sortOptions.setSelectedId(1);
    sortOptions.onChange = [this] { sortChanged(); };
    addAndMakeVisible(sortOptions);

    presetList.setModel(&listModel);
    presetList.setRowHeight(INIConfig::LayoutConstants::defaultButtonHeight);
    presetList.setColour(juce::ListBox::backgroundColourId,
        colorScheme.getColor(ColorScheme::ColorRole::WindowBackground));
    presetList.setOutlineThickness(1);
    addAndMakeVisible(presetList);

    previewPanel.setVisible(false);
    addAndMakeVisible(previewPanel);

    downloadButton.setButtonText("Download");
    downloadButton.onClick = [this] { downloadSelected(); };
    downloadButton.setEnabled(false);
    addAndMakeVisible(downloadButton);

    shareButton.setButtonText("Share");
    shareButton.onClick = [this] { sharePreset(); };
    addAndMakeVisible(shareButton);

    refreshButton.setButtonText("Refresh");
    refreshButton.onClick = [this] { refreshContent(); };
    addAndMakeVisible(refreshButton);

    createDummyContent();
}

void CommunityBrowser::paint(juce::Graphics& g) {
    g.fillAll(colorScheme.getColor(ColorScheme::ColorRole::WindowBackground));

    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::Separator));
    g.drawRect(getLocalBounds(), 1);

    auto bounds = getLocalBounds().reduced(INIConfig::LayoutConstants::defaultMargin);

    auto headerFont = juce::Font(juce::FontOptions().withHeight(static_cast<float>(INIConfig::LayoutConstants::effectSectionHeaderHeight)));
    headerFont.setBold(true);
    g.setFont(headerFont);
    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    g.drawText("Community Browser", bounds.removeFromTop(INIConfig::LayoutConstants::effectSectionHeaderHeight),
               juce::Justification::centred);
}

void CommunityBrowser::resized() {
    auto bounds = getLocalBounds().reduced(INIConfig::LayoutConstants::defaultMargin);

    bounds.removeFromTop(INIConfig::LayoutConstants::effectSectionHeaderHeight + INIConfig::LayoutConstants::defaultMargin);

    auto topRow = bounds.removeFromTop(INIConfig::LayoutConstants::effectButtonHeight);
    searchBox.setBounds(topRow.removeFromLeft(topRow.getWidth() / INIConfig::LayoutConstants::customLookFeelThumbDivisor));
    topRow.removeFromLeft(INIConfig::LayoutConstants::defaultMargin);
    categoryFilter.setBounds(topRow.removeFromLeft(INIConfig::LayoutConstants::effectButtonWidth * INIConfig::LayoutConstants::customLookFeelThumbDivisor));
    topRow.removeFromLeft(INIConfig::LayoutConstants::defaultMargin);
    sortOptions.setBounds(topRow);

    bounds.removeFromTop(INIConfig::LayoutConstants::defaultMargin);

    auto contentArea = bounds.removeFromTop(bounds.getHeight() - INIConfig::LayoutConstants::effectButtonHeight - INIConfig::LayoutConstants::defaultMargin);

    if (previewPanel.isVisible()) {
        auto listWidth = contentArea.getWidth() * INIConfig::LayoutConstants::customLookFeelThumbDivisor / INIConfig::LayoutConstants::drumKitEditorPadRows;
        presetList.setBounds(contentArea.removeFromLeft(listWidth));
        contentArea.removeFromLeft(INIConfig::LayoutConstants::defaultMargin);
        previewPanel.setBounds(contentArea);
    } else {
        presetList.setBounds(contentArea);
    }

    bounds.removeFromTop(INIConfig::LayoutConstants::defaultMargin);
    auto buttonRow = bounds;

    refreshButton.setBounds(buttonRow.removeFromLeft(INIConfig::LayoutConstants::effectButtonWidth));
    buttonRow.removeFromLeft(INIConfig::LayoutConstants::defaultMargin);
    shareButton.setBounds(buttonRow.removeFromLeft(INIConfig::LayoutConstants::effectButtonWidth));

    downloadButton.setBounds(buttonRow.removeFromRight(INIConfig::LayoutConstants::effectButtonWidth));
}

void CommunityBrowser::searchTextChanged() {
    auto searchText = searchBox.getText().toLowerCase();

    filteredItems.clear();

    if (searchText.isEmpty()) {
        filteredItems = communityItems;
    } else {
        for (const auto& item : communityItems) {
            if (item.name.toLowerCase().contains(searchText) ||
                item.description.toLowerCase().contains(searchText) ||
                item.author.toLowerCase().contains(searchText)) {
                filteredItems.add(item);
            }
        }
    }

    presetList.updateContent();
}

void CommunityBrowser::categoryChanged() {
    filterByCategory();
}

void CommunityBrowser::sortChanged() {
    sortItems();
}

void CommunityBrowser::filterByCategory() {
    int categoryId = categoryFilter.getSelectedId();

    filteredItems.clear();

    if (categoryId == 1) {
        filteredItems = communityItems;
    } else {
        CommunityItem::Type filterType = static_cast<CommunityItem::Type>(categoryId - 2);

        for (const auto& item : communityItems) {
            if (item.type == filterType) {
                filteredItems.add(item);
            }
        }
    }

    if (searchBox.getText().isNotEmpty()) {
        searchTextChanged();
    } else {
        presetList.updateContent();
    }
}

void CommunityBrowser::sortItems() {
    int sortId = sortOptions.getSelectedId();

    struct ItemComparator {
        int sortId;

        int compareElements(const CommunityItem& a, const CommunityItem& b) const {
            switch (sortId) {
                case 1:
                    return ((a.downloads + a.rating * 10) >
                           (b.downloads + b.rating * 10)) ? -1 : 1;

                case 2:
                    return (a.uploadDate > b.uploadDate) ? -1 : 1;

                case 3:
                    return (a.rating > b.rating) ? -1 : 1;

                case 4:
                    return (a.downloads > b.downloads) ? -1 : 1;

                default:
                    return 0;
            }
        }
    };

    ItemComparator comparator{sortId};
    filteredItems.sort(comparator);

    presetList.updateContent();
}

void CommunityBrowser::downloadSelected() {
    int selectedRow = presetList.getSelectedRow();
    if (selectedRow < 0 || selectedRow >= filteredItems.size()) return;

    auto& item = filteredItems.getReference(selectedRow);

    item.downloads++;
    item.isDownloaded = true;

    juce::AlertWindow::showMessageBoxAsync(
        juce::AlertWindow::InfoIcon,
        "Download Complete",
        "Successfully downloaded: " + item.name,
        "OK");

    presetList.repaintRow(selectedRow);
}

void CommunityBrowser::sharePreset() {
    auto dialog = std::make_unique<ShareDialog>(colorScheme);
    auto* dialogPtr = dialog.get();

    dialogPtr->onShare = [this](const juce::String& name, const juce::String& description,
                            CommunityItem::Type type) {
        CommunityItem newItem;
        newItem.id = juce::Uuid().toString();
        newItem.name = name;
        newItem.description = description;
        newItem.author = "Current User";
        newItem.type = type;
        newItem.uploadDate = juce::Time::getCurrentTime();
        newItem.downloads = 0;
        newItem.rating = 0.0f;
        newItem.isDownloaded = true;

        communityItems.add(newItem);
        filterByCategory();
    };

    dialog->setVisible(true);
    dialog->centreWithSize(400, 300);
    dialog->enterModalState(true, nullptr, true);
    dialog.release();
}

void CommunityBrowser::refreshContent() {
    filterByCategory();
}

void CommunityBrowser::showPreview(const CommunityItem& item) {
    currentPreviewItem = item;
    previewPanel.updatePreview(item);
    previewPanel.setVisible(true);
    resized();
}

void CommunityBrowser::hidePreview() {
    previewPanel.setVisible(false);
    resized();
}

void CommunityBrowser::timerCallback() {
    presetList.repaint();
}

void CommunityBrowser::createDummyContent() {
    const juce::String authors[] = { "BeatMaker123", "DrumGuru", "RhythmKing", "GrooveMaster", "808Expert" };
    const juce::String kitNames[] = { "Vintage 909", "Modern Trap", "Jazz Essentials", "Rock Power", "Lo-Fi Dreams" };
    const juce::String patternNames[] = { "Funky Break", "Latin Groove", "Hip Hop Classic", "Techno Drive", "Reggae Skank" };

    juce::Random random;

    for (int i = 0; i < 5; ++i) {
        CommunityItem item;
        item.id = juce::Uuid().toString();
        item.name = kitNames[i % (sizeof(kitNames) / sizeof(kitNames[0]))];
        item.description = "High quality drum kit with authentic samples";
        item.author = authors[random.nextInt(sizeof(authors) / sizeof(authors[0]))];
        item.type = CommunityItem::Type::DrumKit;
        item.uploadDate = juce::Time::getCurrentTime() - juce::RelativeTime::days(random.nextInt(30));
        item.downloads = random.nextInt(1000);
        item.rating = 3.0f + random.nextFloat() * 2.0f;
        item.fileSize = 1024 * 1024 * (10 + random.nextInt(50));

        communityItems.add(item);
    }

    for (int i = 0; i < 5; ++i) {
        CommunityItem item;
        item.id = juce::Uuid().toString();
        item.name = patternNames[i % (sizeof(patternNames) / sizeof(patternNames[0]))];
        item.description = "Professional drum pattern for your tracks";
        item.author = authors[random.nextInt(sizeof(authors) / sizeof(authors[0]))];
        item.type = CommunityItem::Type::Pattern;
        item.uploadDate = juce::Time::getCurrentTime() - juce::RelativeTime::days(random.nextInt(30));
        item.downloads = random.nextInt(500);
        item.rating = 3.5f + random.nextFloat() * 1.5f;
        item.fileSize = 1024 * (50 + random.nextInt(200));

        communityItems.add(item);
    }

    filteredItems = communityItems;
}

int CommunityBrowser::ItemListModel::getNumRows() {
    return parent.filteredItems.size();
}

void CommunityBrowser::ItemListModel::paintListBoxItem(int rowNumber, juce::Graphics& g,
                                                       int width, int height,
                                                       bool rowIsSelected) {
    if (rowNumber >= parent.filteredItems.size()) return;

    const auto& item = parent.filteredItems[rowNumber];

    if (rowIsSelected) {
        g.fillAll(parent.colorScheme.getColor(ColorScheme::ColorRole::Accent).withAlpha(0.3f));
    } else if (rowNumber % 2 == 0) {
        g.fillAll(parent.colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground).darker(0.1f));
    }

    auto bounds = juce::Rectangle<int>(0, 0, width, height).reduced(5);

    auto iconArea = bounds.removeFromLeft(height - 10);
    g.setColour(parent.colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));

    switch (item.type) {
        case CommunityItem::Type::DrumKit:
            g.fillRoundedRectangle(iconArea.reduced(5).toFloat(), 4.0f);
            break;
        case CommunityItem::Type::Pattern:
            g.fillEllipse(iconArea.reduced(5).toFloat());
            break;
        case CommunityItem::Type::Effect:
            g.drawRoundedRectangle(iconArea.reduced(5).toFloat(), 4.0f, 2.0f);
            break;
        case CommunityItem::Type::MixTemplate:
            for (int i = 0; i < 4; ++i) {
                auto barArea = iconArea.reduced(5);
                auto barHeight = barArea.getHeight() / 4;
                auto bar = barArea.removeFromBottom(barHeight * (i + 1) / 4);
                g.fillRect(bar);
            }
            break;
    }

    bounds.removeFromLeft(10);

    auto nameFont = juce::Font(juce::FontOptions().withHeight(14.0f));
    nameFont.setBold(true);
    g.setFont(nameFont);
    g.setColour(item.isDownloaded ?
        parent.colorScheme.getColor(ColorScheme::ColorRole::Success) :
        parent.colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    g.drawText(item.name, bounds.removeFromTop(height / 2),
               juce::Justification::centredLeft);

    g.setFont(juce::Font(juce::FontOptions().withHeight(12.0f)));
    g.setColour(parent.colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
    g.drawText("by " + item.author, bounds, juce::Justification::centredLeft);

    auto statsArea = juce::Rectangle<int>(0, 0, width, height).reduced(5);

    g.setFont(juce::Font(juce::FontOptions().withHeight(12.0f)));
    g.setColour(parent.colorScheme.getColor(ColorScheme::ColorRole::Warning));
    juce::String rating = juce::String::charToString(0x2605) + " " +
                         juce::String(item.rating, 1);

    juce::GlyphArrangement glyphs;
    glyphs.addFittedText(g.getCurrentFont(), rating, 0, 0, 100, 20, juce::Justification::left, 1);
    auto ratingWidth = glyphs.getBoundingBox(0, -1, false).getWidth();

    g.drawText(rating, statsArea.removeFromRight(static_cast<int>(ratingWidth) + 10),
               juce::Justification::centredRight);

    g.setColour(parent.colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
    juce::String downloads = juce::String(item.downloads) + " downloads";
    g.drawText(downloads, statsArea, juce::Justification::centredRight);
}

void CommunityBrowser::ItemListModel::listBoxItemClicked(int row, const juce::MouseEvent&) {
    if (row >= 0 && row < parent.filteredItems.size()) {
        parent.downloadButton.setEnabled(true);
        parent.showPreview(parent.filteredItems[row]);
    }
}

void CommunityBrowser::ItemListModel::listBoxItemDoubleClicked(int row, const juce::MouseEvent&) {
   if (row >= 0 && row < parent.filteredItems.size()) {
       parent.downloadSelected();
   }
}

CommunityBrowser::PreviewPanel::PreviewPanel(ColorScheme& cs) : colorScheme(cs) {
   auto titleFont = juce::Font(juce::FontOptions().withHeight(20.0f));
   titleFont.setBold(true);
   titleLabel.setFont(titleFont);
   titleLabel.setJustificationType(juce::Justification::centredLeft);
   addAndMakeVisible(titleLabel);

   descriptionLabel.setFont(juce::Font(14.0f));
   descriptionLabel.setJustificationType(juce::Justification::topLeft);
   addAndMakeVisible(descriptionLabel);

   authorLabel.setFont(juce::Font(14.0f));
   authorLabel.setColour(juce::Label::textColourId,
       colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
   addAndMakeVisible(authorLabel);

   statsLabel.setFont(juce::Font(12.0f));
   statsLabel.setJustificationType(juce::Justification::topLeft);
   addAndMakeVisible(statsLabel);

   tagsLabel.setFont(juce::Font(12.0f));
   tagsLabel.setColour(juce::Label::textColourId,
       colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
   addAndMakeVisible(tagsLabel);
}

void CommunityBrowser::PreviewPanel::paint(juce::Graphics& g) {
   g.fillAll(colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground));

   g.setColour(colorScheme.getColor(ColorScheme::ColorRole::Separator));
   g.drawRect(getLocalBounds(), 1);

   auto bounds = getLocalBounds().reduced(INIConfig::LayoutConstants::defaultMargin);
   bounds.removeFromTop(60);

   auto vizArea = bounds.removeFromTop(getHeight() / 3);
   g.setColour(colorScheme.getColor(ColorScheme::ColorRole::ControlBackground));
   g.fillRoundedRectangle(vizArea.toFloat(), 4.0f);

   drawPreviewContent(g, vizArea);
}

void CommunityBrowser::PreviewPanel::resized() {
   auto bounds = getLocalBounds().reduced(INIConfig::LayoutConstants::defaultMargin);

   titleLabel.setBounds(bounds.removeFromTop(30));
   bounds.removeFromTop(5);

   authorLabel.setBounds(bounds.removeFromTop(20));
   bounds.removeFromTop(5);

   bounds.removeFromTop(getHeight() / 3 + 10);

   descriptionLabel.setBounds(bounds.removeFromTop(40));
   bounds.removeFromTop(10);

   statsLabel.setBounds(bounds.removeFromTop(60));
   bounds.removeFromTop(10);

   tagsLabel.setBounds(bounds.removeFromTop(20));
}

void CommunityBrowser::PreviewPanel::updatePreview(const CommunityItem& item) {
   currentItem = item;

   titleLabel.setText(item.name, juce::dontSendNotification);
   authorLabel.setText("by " + item.author, juce::dontSendNotification);
   descriptionLabel.setText(item.description, juce::dontSendNotification);

   juce::String stats;
   stats << "Rating: " << juce::String(item.rating, 1) << "/5.0\n";
   stats << "Downloads: " << item.downloads << "\n";
   stats << "Size: " << formatFileSize(item.fileSize) << "\n";
   stats << "Uploaded: " << item.uploadDate.toString(true, false);
   statsLabel.setText(stats, juce::dontSendNotification);

   juce::String tags = "#";
   switch (item.type) {
       case CommunityItem::Type::DrumKit:
           tags << "drumkit #samples #" << item.name.toLowerCase().replaceCharacter(' ', '_');
           break;
       case CommunityItem::Type::Pattern:
           tags << "pattern #groove #midi";
           break;
       case CommunityItem::Type::Effect:
           tags << "effect #processing #audio";
           break;
       case CommunityItem::Type::MixTemplate:
           tags << "mix #template #preset";
           break;
   }
   tagsLabel.setText(tags, juce::dontSendNotification);

   repaint();
}

void CommunityBrowser::PreviewPanel::drawPreviewContent(juce::Graphics& g, juce::Rectangle<int> area) {
   if (!currentItem.has_value()) return;

   g.setColour(colorScheme.getColor(ColorScheme::ColorRole::Accent).withAlpha(0.5f));

   switch (currentItem->type) {
       case CommunityItem::Type::DrumKit:
           for (int i = 0; i < INIConfig::Defaults::MAX_PLAYERS; ++i) {
               auto channelArea = area.reduced(10)
                                     .withHeight(area.getHeight() / INIConfig::Defaults::MAX_PLAYERS)
                                     .withY(area.getY() + i * area.getHeight() / INIConfig::Defaults::MAX_PLAYERS);

               juce::Path waveform;
               waveform.startNewSubPath(static_cast<float>(channelArea.getX()),
                                      static_cast<float>(channelArea.getCentreY()));

               for (int x = 0; x < channelArea.getWidth(); x += 2) {
                   float y = channelArea.getCentreY() +
                            std::sin(x * 0.1f) * channelArea.getHeight() * 0.3f *
                            std::exp(-x / static_cast<float>(channelArea.getWidth()));
                   waveform.lineTo(static_cast<float>(channelArea.getX() + x), y);
               }

               g.strokePath(waveform, juce::PathStrokeType(1.5f));
           }
           break;

       case CommunityItem::Type::Pattern:
           {
               int steps = 16;
               int rows = INIConfig::Defaults::MAX_PLAYERS;

               float stepWidth = static_cast<float>(area.getWidth()) / steps;
               float rowHeight = static_cast<float>(area.getHeight()) / rows;

               g.setColour(colorScheme.getColor(ColorScheme::ColorRole::GridLine).withAlpha(0.3f));

               for (int i = 0; i <= steps; ++i) {
                   float x = area.getX() + i * stepWidth;
                   g.drawVerticalLine(static_cast<int>(x), static_cast<float>(area.getY()),
                                     static_cast<float>(area.getBottom()));
               }

               for (int i = 0; i <= rows; ++i) {
                   float y = area.getY() + i * rowHeight;
                   g.drawHorizontalLine(static_cast<int>(y), static_cast<float>(area.getX()),
                                       static_cast<float>(area.getRight()));
               }

               g.setColour(colorScheme.getColor(ColorScheme::ColorRole::PatternActive));
               juce::Random random(currentItem->id.hashCode());

               for (int row = 0; row < rows; ++row) {
                   for (int step = 0; step < steps; ++step) {
                       if (random.nextFloat() > 0.7f) {
                           auto cellArea = juce::Rectangle<float>(
                               area.getX() + step * stepWidth,
                               area.getY() + row * rowHeight,
                               stepWidth - 1,
                               rowHeight - 1
                           );
                           g.fillRoundedRectangle(cellArea, 2.0f);
                       }
                   }
               }
           }
           break;

       case CommunityItem::Type::Effect:
           {
               juce::Path response;
               float centerY = static_cast<float>(area.getCentreY());
               response.startNewSubPath(static_cast<float>(area.getX()), centerY);

               for (int x = 0; x < area.getWidth(); ++x) {
                   float freq = std::pow(10.0f, (x / static_cast<float>(area.getWidth())) * 3);
                   float gain = std::sin(freq * 0.001f) * std::exp(-freq / 20000.0f);
                   float y = centerY - gain * area.getHeight() * 0.4f;

                   response.lineTo(static_cast<float>(area.getX() + x), y);
               }

               g.strokePath(response, juce::PathStrokeType(2.0f));

               g.setColour(colorScheme.getColor(ColorScheme::ColorRole::GridLine).withAlpha(0.2f));
               g.drawHorizontalLine(area.getCentreY(), static_cast<float>(area.getX()),
                                  static_cast<float>(area.getRight()));
           }
           break;

       case CommunityItem::Type::MixTemplate:
           {
               float channelWidth = static_cast<float>(area.getWidth()) / INIConfig::Defaults::MAX_PLAYERS;

               for (int i = 0; i < INIConfig::Defaults::MAX_PLAYERS; ++i) {
                   auto channelArea = juce::Rectangle<float>(
                       area.getX() + i * channelWidth,
                       static_cast<float>(area.getY()),
                       channelWidth - 2,
                       static_cast<float>(area.getHeight())
                   );

                   juce::Random random(currentItem->id.hashCode() + i);
                   float level = random.nextFloat();

                   auto faderArea = channelArea.reduced(4);
                   float faderHeight = faderArea.getHeight() * level;

                   g.setColour(colorScheme.getColor(ColorScheme::ColorRole::ControlBackground));
                   g.fillRoundedRectangle(faderArea, 2.0f);

                   g.setColour(colorScheme.getColor(ColorScheme::ColorRole::Accent));
                   g.fillRoundedRectangle(
                       faderArea.removeFromBottom(faderHeight),
                       2.0f
                   );
               }
           }
           break;
   }
}

juce::String CommunityBrowser::PreviewPanel::formatFileSize(int64_t bytes) {
   if (bytes < 1024) {
       return juce::String(bytes) + " B";
   } else if (bytes < 1024 * 1024) {
        return juce::String(bytes / 1024.0f, 1) + " KB";
   } else {
       return juce::String(bytes / (1024.0f * 1024.0f), 1) + " MB";
   }
}

CommunityBrowser::ShareDialog::ShareDialog(ColorScheme& cs) : colorScheme(cs) {
   setSize(400, 300);

   titleLabel.setText("Share Preset", juce::dontSendNotification);
   auto titleFont = juce::Font(juce::FontOptions().withHeight(20.0f));
   titleFont.setBold(true);
   titleLabel.setFont(titleFont);
   titleLabel.setJustificationType(juce::Justification::centred);
   addAndMakeVisible(titleLabel);

   nameEditor.setTextToShowWhenEmpty("Preset name...",
       colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
   addAndMakeVisible(nameEditor);

   descriptionEditor.setMultiLine(true);
   descriptionEditor.setTextToShowWhenEmpty("Description...",
       colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
   descriptionEditor.setReturnKeyStartsNewLine(true);
   addAndMakeVisible(descriptionEditor);

   typeSelector.addItem("Drum Kit", 1);
   typeSelector.addItem("Pattern", 2);
   typeSelector.addItem("Effect", 3);
   typeSelector.addItem("Mix Template", 4);
   typeSelector.setSelectedId(1);
   addAndMakeVisible(typeSelector);

   shareButton.setButtonText("Share");
   shareButton.onClick = [this] { doShare(); };
   addAndMakeVisible(shareButton);

   cancelButton.setButtonText("Cancel");
   cancelButton.onClick = [this] { exitModalState(0); };
   addAndMakeVisible(cancelButton);
}

void CommunityBrowser::ShareDialog::paint(juce::Graphics& g) {
   g.fillAll(colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground));

   g.setColour(colorScheme.getColor(ColorScheme::ColorRole::Separator));
   g.drawRect(getLocalBounds(), 2);
}

void CommunityBrowser::ShareDialog::resized() {
   auto bounds = getLocalBounds().reduced(20);

   titleLabel.setBounds(bounds.removeFromTop(30));
   bounds.removeFromTop(10);

   nameEditor.setBounds(bounds.removeFromTop(30));
   bounds.removeFromTop(10);

   typeSelector.setBounds(bounds.removeFromTop(30));
   bounds.removeFromTop(10);

   descriptionEditor.setBounds(bounds.removeFromTop(80));

   bounds.removeFromBottom(10);
   auto buttonRow = bounds.removeFromBottom(30);

   cancelButton.setBounds(buttonRow.removeFromLeft(100));
   shareButton.setBounds(buttonRow.removeFromRight(100));
}

void CommunityBrowser::ShareDialog::doShare() {
   auto name = nameEditor.getText().trim();
   auto description = descriptionEditor.getText().trim();

   if (name.isEmpty()) {
       juce::AlertWindow::showMessageBoxAsync(
           juce::AlertWindow::WarningIcon,
           "Missing Information",
           "Please enter a name for your preset.",
           "OK");
       return;
   }

   if (onShare) {
       onShare(name, description,
               static_cast<CommunityItem::Type>(typeSelector.getSelectedId() - 1));
   }

   exitModalState(1);
}
