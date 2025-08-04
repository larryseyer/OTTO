#pragma once
#include <JuceHeader.h>
#include "RowComponentBase.h"
#include "ComponentState.h"
#include <memory>
#include <vector>

class RowComponentManager {
public:
    RowComponentManager();
    ~RowComponentManager() = default;
    
    void registerRowComponent(std::unique_ptr<RowComponentBase> rowComponent);
    RowComponentBase* getRowComponent(int rowNumber) const;
    std::vector<RowComponentBase*> getAllRowComponents() const;
    
    void saveAllStates(ComponentState& state);
    void loadAllStates(const ComponentState& state);
    void updateAllFromState(const ComponentState& state);
    
    void broadcastStateChange(const ComponentState& state);
    void notifyRowChanged(int rowNumber, const ComponentState& state);
    
    void initializeAllRows();
    void resizeAllRows(const juce::Rectangle<int>& bounds);
    void updateAllColors();
    
    bool validateRowIntegrity() const;
    void logRowStatus() const;
    
private:
    std::vector<std::unique_ptr<RowComponentBase>> rowComponents;
    
    void sortRowsByNumber();
    bool isValidRowNumber(int rowNumber) const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RowComponentManager)
};
