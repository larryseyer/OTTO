#include "MixerWindow.h"
#include "INIConfig.h"

MixerWindow::MixerWindow(Mixer& mixer,
                        INIDataManager& iniManager, 
                        ColorScheme& colorScheme, 
                        FontManager& fontManager)
    : PopupWindow("Mixer", juce::Colours::transparentBlack, true)
    , mixer(mixer)
    , iniDataManager(iniManager)
    , colorScheme(colorScheme)
    , fontManager(fontManager) {
    
    setupWindow();
    setupMixerControls();
    
    setSize(600, 400);
    centreWithSize(600, 400);
}

MixerWindow::~MixerWindow() {
    channelSliders.clear();
    channelLabels.clear();
    masterSlider.reset();
    masterLabel.reset();
}

void MixerWindow::paint(juce::Graphics& g) {
    g.fillAll(colorScheme.getColor(ColorScheme::ColorRole::WindowBackground));
    
    auto bounds = getLocalBounds();
    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::Separator));
    g.drawRect(bounds, 2);
    
    auto titleArea = bounds.removeFromTop(40);
    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground));
    g.fillRect(titleArea);
    
    g.setColour(colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    g.setFont(fontManager.getFont(FontManager::FontRole::Header, 16.0f));
    g.drawText("Mixer", titleArea, juce::Justification::centred);
}

void MixerWindow::resized() {
    auto bounds = getLocalBounds();
    bounds.removeFromTop(40);
    bounds = bounds.reduced(10);
    
    int sliderWidth = 60;
    int spacing = 10;
    int totalWidth = (NUM_CHANNELS + 1) * sliderWidth + NUM_CHANNELS * spacing;
    int startX = (bounds.getWidth() - totalWidth) / 2;
    
    for (int i = 0; i < NUM_CHANNELS; ++i) {
        int x = startX + i * (sliderWidth + spacing);
        
        if (channelLabels[i]) {
            channelLabels[i]->setBounds(x, bounds.getY(), sliderWidth, 20);
        }
        
        if (channelSliders[i]) {
            channelSliders[i]->setBounds(x, bounds.getY() + 25, sliderWidth, bounds.getHeight() - 50);
        }
    }
    
    int masterX = startX + NUM_CHANNELS * (sliderWidth + spacing);
    if (masterLabel) {
        masterLabel->setBounds(masterX, bounds.getY(), sliderWidth, 20);
    }
    
    if (masterSlider) {
        masterSlider->setBounds(masterX, bounds.getY() + 25, sliderWidth, bounds.getHeight() - 50);
    }
}

void MixerWindow::showMixer() {
    setVisible(true);
    toFront(true);
    grabKeyboardFocus();
}

void MixerWindow::hideMixer() {
    setVisible(false);
}

bool MixerWindow::isMixerVisible() const {
    return isVisible();
}

void MixerWindow::setupWindow() {
    setUsingNativeTitleBar(false);
    setDropShadowEnabled(true);
    setResizable(true, true);
    
    setColour(juce::ResizableWindow::backgroundColourId, 
              colorScheme.getColor(ColorScheme::ColorRole::WindowBackground));
}

void MixerWindow::setupMixerControls() {
    for (int i = 0; i < NUM_CHANNELS; ++i) {
        auto slider = std::make_unique<juce::Slider>(juce::Slider::LinearVertical, juce::Slider::TextBoxBelow);
        slider->setRange(0.0, 1.0, 0.01);
        slider->setValue(0.8);
        slider->setColour(juce::Slider::backgroundColourId, colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground));
        slider->setColour(juce::Slider::trackColourId, colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
        slider->setColour(juce::Slider::thumbColourId, colorScheme.getColor(ColorScheme::ColorRole::Accent));
        addAndMakeVisible(*slider);
        channelSliders.add(std::move(slider));
        
        auto label = std::make_unique<juce::Label>();
        label->setText("CH " + juce::String(i + 1), juce::dontSendNotification);
        label->setJustificationType(juce::Justification::centred);
        label->setColour(juce::Label::textColourId, colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
        label->setFont(fontManager.getFont(FontManager::FontRole::Body, 12.0f));
        addAndMakeVisible(*label);
        channelLabels.add(std::move(label));
    }
    
    masterSlider = std::make_unique<juce::Slider>(juce::Slider::LinearVertical, juce::Slider::TextBoxBelow);
    masterSlider->setRange(0.0, 1.0, 0.01);
    masterSlider->setValue(0.8);
    masterSlider->setColour(juce::Slider::backgroundColourId, colorScheme.getColor(ColorScheme::ColorRole::ComponentBackground));
    masterSlider->setColour(juce::Slider::trackColourId, colorScheme.getColor(ColorScheme::ColorRole::SecondaryText));
    masterSlider->setColour(juce::Slider::thumbColourId, colorScheme.getColor(ColorScheme::ColorRole::Accent));
    addAndMakeVisible(*masterSlider);
    
    masterLabel = std::make_unique<juce::Label>();
    masterLabel->setText("MASTER", juce::dontSendNotification);
    masterLabel->setJustificationType(juce::Justification::centred);
    masterLabel->setColour(juce::Label::textColourId, colorScheme.getColor(ColorScheme::ColorRole::PrimaryText));
    masterLabel->setFont(fontManager.getFont(FontManager::FontRole::Header, 12.0f));
    addAndMakeVisible(*masterLabel);
}

void MixerWindow::updateLayout() {
    resized();
}
