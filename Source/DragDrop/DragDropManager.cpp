#include "DragDropManager.h"

PatternDragSource::PatternDragSource(int patternIndex, const juce::String& patternName)
    : patternIndex(patternIndex), patternName(patternName) {
}

void PatternDragSource::startDragging(const juce::String& description, juce::Component* sourceComponent) {
    juce::DragAndDropContainer::startDragging(description, sourceComponent);
}

MidiFileDragTarget::MidiFileDragTarget(int drumPadIndex)
    : drumPadIndex(drumPadIndex) {
}

bool MidiFileDragTarget::isInterestedInFileDrag(const juce::StringArray& files) {
    if (files.size() != 1) return false;
    
    auto file = juce::File(files[0]);
    return file.hasFileExtension(".mid") || file.hasFileExtension(".midi");
}

void MidiFileDragTarget::filesDropped(const juce::StringArray& files, int x, int y) {
    juce::ignoreUnused(x, y);
    
    if (files.size() > 0 && onMidiFileAssigned) {
        onMidiFileAssigned(drumPadIndex, files[0]);
    }
    
    updateVisualFeedback(false);
}

void MidiFileDragTarget::fileDragEnter(const juce::StringArray& files, int x, int y) {
    juce::ignoreUnused(files, x, y);
    updateVisualFeedback(true);
}

void MidiFileDragTarget::fileDragExit(const juce::StringArray& files) {
    juce::ignoreUnused(files);
    updateVisualFeedback(false);
}

void MidiFileDragTarget::updateVisualFeedback(bool hovering) {
    isDragHovering = hovering;
}
