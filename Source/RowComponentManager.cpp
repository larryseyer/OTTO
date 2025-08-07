#include "RowComponentManager.h"
#include "ErrorHandling.h"
#include <algorithm>

RowComponentManager::RowComponentManager() {
    rowComponents.reserve(6);
}

void RowComponentManager::registerRowComponent(std::unique_ptr<RowComponentBase> rowComponent) {
    if (!rowComponent) {
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "Attempted to register null row component", "RowComponentManager");
        return;
    }
    
    int rowNumber = rowComponent->getRowNumber();
    if (!isValidRowNumber(rowNumber)) {
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
            "Invalid row number: " + juce::String(rowNumber), "RowComponentManager");
        return;
    }
    
    for (const auto& existing : rowComponents) {
        if (existing->getRowNumber() == rowNumber) {
            ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Warning,
                "Row " + juce::String(rowNumber) + " already registered, replacing", "RowComponentManager");
            break;
        }
    }
    
    rowComponents.push_back(std::move(rowComponent));
    sortRowsByNumber();
}

RowComponentBase* RowComponentManager::getRowComponent(int rowNumber) const {
    for (const auto& component : rowComponents) {
        if (component->getRowNumber() == rowNumber) {
            return component.get();
        }
    }
    return nullptr;
}

std::vector<RowComponentBase*> RowComponentManager::getAllRowComponents() const {
    std::vector<RowComponentBase*> result;
    result.reserve(rowComponents.size());
    
    for (const auto& component : rowComponents) {
        result.push_back(component.get());
    }
    
    return result;
}

void RowComponentManager::saveAllStates(ComponentState& state) {
    for (const auto& component : rowComponents) {
        try {
            component->saveStates(state);
        } catch (const std::exception& e) {
            ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
                "Failed to save state for " + component->getRowName() + ": " + juce::String(e.what()),
                "RowComponentManager");
        }
    }
}

void RowComponentManager::loadAllStates(const ComponentState& state) {
    for (const auto& component : rowComponents) {
        try {
            component->loadStates(state);
        } catch (const std::exception& e) {
            ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
                "Failed to load state for " + component->getRowName() + ": " + juce::String(e.what()),
                "RowComponentManager");
        }
    }
}

void RowComponentManager::updateAllFromState(const ComponentState& state) {
    for (const auto& component : rowComponents) {
        try {
            component->updateFromState(state);
        } catch (const std::exception& e) {
            ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
                "Failed to update from state for " + component->getRowName() + ": " + juce::String(e.what()),
                "RowComponentManager");
        }
    }
}

void RowComponentManager::broadcastStateChange(const ComponentState& state) {
    updateAllFromState(state);
}

void RowComponentManager::notifyRowChanged(int rowNumber, const ComponentState& state) {
    auto* component = getRowComponent(rowNumber);
    if (component) {
        try {
            component->updateFromState(state);
        } catch (const std::exception& e) {
            ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error,
                "Failed to notify row " + juce::String(rowNumber) + ": " + juce::String(e.what()),
                "RowComponentManager");
        }
    }
}

void RowComponentManager::initializeAllRows() {
    for (const auto& component : rowComponents) {
        component->setupCommonComponents();
    }
}

void RowComponentManager::resizeAllRows(const juce::Rectangle<int>& bounds) {
    for (const auto& component : rowComponents) {
        auto rowBounds = component->getRowBounds();
        component->setBounds(rowBounds);
    }
}

void RowComponentManager::updateAllColors() {
    for (const auto& component : rowComponents) {
        component->updateCommonColors();
    }
}

bool RowComponentManager::validateRowIntegrity() const {
    if (rowComponents.size() != 6) {
        return false;
    }
    
    for (int i = 1; i <= 6; ++i) {
        if (!getRowComponent(i)) {
            return false;
        }
    }
    
    return true;
}

void RowComponentManager::logRowStatus() const {
    DBG("RowComponentManager Status:");
    DBG("  Registered components: " << rowComponents.size());
    for (const auto& component : rowComponents) {
        DBG("  Row " << component->getRowNumber() << ": " << component->getRowName());
    }
}

bool RowComponentManager::isValidRowNumber(int rowNumber) const {
    return rowNumber >= 1 && rowNumber <= 6;
}

void RowComponentManager::sortRowsByNumber() {
    std::sort(rowComponents.begin(), rowComponents.end(),
        [](const std::unique_ptr<RowComponentBase>& a, const std::unique_ptr<RowComponentBase>& b) {
            return a->getRowNumber() < b->getRowNumber();
        });
}
