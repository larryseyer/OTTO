#pragma once
#include <JuceHeader.h>
#include "../ColorScheme.h"
#include "../FontManager.h"

class PatternDragSource : public juce::DragAndDropContainer {
public:
    PatternDragSource(int patternIndex, const juce::String& patternName);
    
    void startDragging(const juce::String& description, juce::Component* sourceComponent);
    
private:
    int patternIndex;
    juce::String patternName;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatternDragSource)
};

class MidiFileDragTarget : public juce::FileDragAndDropTarget {
public:
    MidiFileDragTarget(int drumPadIndex);
    
    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;
    void fileDragEnter(const juce::StringArray& files, int x, int y) override;
    void fileDragExit(const juce::StringArray& files) override;
    
    std::function<void(int, const juce::String&)> onMidiFileAssigned;
    
private:
    int drumPadIndex;
    bool isDragHovering = false;
    
    void updateVisualFeedback(bool hovering);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiFileDragTarget)
};
