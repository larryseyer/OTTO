#pragma once
#include <JuceHeader.h>
#include <functional>
#include <memory>

class ErrorHandler {
public:
    enum class ErrorLevel {
        Warning,
        Error,
        Critical
    };

    struct ErrorInfo {
        ErrorLevel level;
        juce::String message;
        juce::String component;
        juce::Time timestamp;
    };

    using ErrorCallback = std::function<void(const ErrorInfo&)>;

    static ErrorHandler& getInstance() {
        static ErrorHandler instance;
        return instance;
    }

    void setErrorCallback(ErrorCallback callback) {
        errorCallback = std::move(callback);
    }

    void reportError(ErrorLevel level, const juce::String& message, const juce::String& component = {}) {
        ErrorInfo info{level, message, component, juce::Time::getCurrentTime()};
        
        if (errorCallback) {
            errorCallback(info);
        }
        
        DBG("Error [" + component + "]: " + message);
    }

    template<typename T>
    static std::unique_ptr<T> safeCreate(std::function<std::unique_ptr<T>()> creator, const juce::String& component = {}) {
        try {
            return creator();
        } catch (const std::exception& e) {
            getInstance().reportError(ErrorLevel::Error, 
                "Failed to create component: " + juce::String(e.what()), component);
            return nullptr;
        }
    }

    template<typename Func>
    static bool safeExecute(Func&& func, const juce::String& operation = {}) {
        try {
            func();
            return true;
        } catch (const std::exception& e) {
            getInstance().reportError(ErrorLevel::Error,
                "Operation failed: " + juce::String(e.what()), operation);
            return false;
        }
    }

private:
    ErrorCallback errorCallback;
    ErrorHandler() = default;
};
