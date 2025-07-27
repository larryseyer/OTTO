#pragma once

#include "ErrorHandling.h"
#include <JuceHeader.h>

#if JUCE_WINDOWS
    #include <windows.h>
    #include <comdef.h>
    #include <DbgHelp.h>
    #pragma comment(lib, "dbghelp.lib")
#endif

/**
 * OTTO Phase 2.3.1 - Windows-Specific Error Handling Extensions
 * Enhances the Phase 2.2 ErrorHandler with Windows-specific features
 * Maintains all existing safety features while adding Windows optimizations
 */

#if JUCE_WINDOWS

class WindowsErrorHandler : public juce::DeletedAtShutdown
{
public:
    static WindowsErrorHandler& getInstance() {
        static WindowsErrorHandler instance;
        return instance;
    }

    WindowsErrorHandler() {
        // Initialize Windows-specific error handling
        initializeWindowsErrorHandling();
    }

    ~WindowsErrorHandler() override {
        cleanup();
    }

    //==============================================================================
    // Windows COM Error Handling
    //==============================================================================
    
    static juce::String getComErrorString(HRESULT hr) {
        _com_error err(hr);
        juce::String errorMsg = juce::String(err.ErrorMessage());
        
        if (errorMsg.isEmpty()) {
            errorMsg = "COM Error 0x" + juce::String::toHexString((int)hr);
        }
        
        return errorMsg;
    }
    
    static void reportComError(HRESULT hr, const juce::String& operation, const juce::String& component = {}) {
        if (FAILED(hr)) {
            juce::String message = "COM operation failed: " + operation + " - " + getComErrorString(hr);
            ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error, message, component);
        }
    }
    
    //==============================================================================
    // Windows Audio Device Error Handling
    //==============================================================================
    
    enum class AudioDriverType {
        DirectSound,
        WASAPI,
        ASIO,
        WinMM,
        Unknown
    };
    
    static void reportAudioDriverError(AudioDriverType driverType, const juce::String& error, const juce::String& deviceName = {}) {
        juce::String driverName;
        ErrorHandler::ErrorLevel level = ErrorHandler::ErrorLevel::Error;
        
        switch (driverType) {
            case AudioDriverType::DirectSound:
                driverName = "DirectSound";
                break;
            case AudioDriverType::WASAPI:
                driverName = "WASAPI";
                level = ErrorHandler::ErrorLevel::Critical; // WASAPI failures are more serious
                break;
            case AudioDriverType::ASIO:
                driverName = "ASIO";
                break;
            case AudioDriverType::WinMM:
                driverName = "WinMM";
                level = ErrorHandler::ErrorLevel::Warning; // WinMM is legacy
                break;
            default:
                driverName = "Unknown Driver";
                break;
        }
        
        juce::String message = driverName + " error";
        if (deviceName.isNotEmpty()) {
            message += " (device: " + deviceName + ")";
        }
        message += ": " + error;
        
        ErrorHandler::getInstance().reportError(level, message, "Windows Audio");
    }
    
    //==============================================================================
    // Windows Memory Error Handling
    //==============================================================================
    
    static void reportMemoryError(const juce::String& operation, size_t requestedSize = 0) {
        juce::String message = "Memory allocation failed: " + operation;
        if (requestedSize > 0) {
            message += " (requested: " + juce::String(requestedSize) + " bytes)";
        }
        
        // Get available memory info
        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(MEMORYSTATUSEX);
        if (GlobalMemoryStatusEx(&memInfo)) {
            message += " - Available: " + juce::String(memInfo.ullAvailPhys / (1024 * 1024)) + "MB";
        }
        
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Critical, message, "Windows Memory");
    }
    
    //==============================================================================
    // Windows Registry Error Handling
    //==============================================================================
    
    static void reportRegistryError(const juce::String& keyPath, const juce::String& operation, LONG result) {
        if (result != ERROR_SUCCESS) {
            juce::String message = "Registry " + operation + " failed for key: " + keyPath + 
                                 " (Error: " + juce::String(result) + ")";
            ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Warning, message, "Windows Registry");
        }
    }
    
    //==============================================================================
    // Windows Threading Error Handling
    //==============================================================================
    
    static void reportThreadingError(const juce::String& operation, DWORD lastError = 0) {
        if (lastError == 0) {
            lastError = GetLastError();
        }
        
        juce::String message = "Threading operation failed: " + operation + 
                             " (Windows Error: " + juce::String(lastError) + ")";
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error, message, "Windows Threading");
    }
    
    //==============================================================================
    // Windows File System Error Handling
    //==============================================================================
    
    static void reportFileSystemError(const juce::String& operation, const juce::String& filePath, DWORD lastError = 0) {
        if (lastError == 0) {
            lastError = GetLastError();
        }
        
        juce::String message = "File system operation failed: " + operation + 
                             " on file: " + filePath + 
                             " (Windows Error: " + juce::String(lastError) + ")";
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Error, message, "Windows FileSystem");
    }
    
    //==============================================================================
    // Exception Handling and Stack Traces
    //==============================================================================
    
    static juce::String getStackTrace() {
        juce::String stackTrace;
        
        #if JUCE_DEBUG
        // Capture stack trace in debug builds
        void* stack[64];
        WORD numberOfFrames = CaptureStackBackTrace(0, 64, stack, nullptr);
        
        HANDLE process = GetCurrentProcess();
        SymInitialize(process, nullptr, TRUE);
        
        for (WORD i = 0; i < numberOfFrames; ++i) {
            DWORD64 address = (DWORD64)(stack[i]);
            
            char symbolBuffer[sizeof(SYMBOL_INFO) + 256];
            SYMBOL_INFO* symbol = (SYMBOL_INFO*)symbolBuffer;
            symbol->MaxNameLen = 255;
            symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
            
            if (SymFromAddr(process, address, nullptr, symbol)) {
                stackTrace += juce::String::formatted("%d: %s (0x%016llx)\n", i, symbol->Name, address);
            } else {
                stackTrace += juce::String::formatted("%d: Unknown (0x%016llx)\n", i, address);
            }
        }
        
        SymCleanup(process);
        #else
        stackTrace = "Stack trace not available in release builds";
        #endif
        
        return stackTrace;
    }
    
    //==============================================================================
    // Windows-Specific Safe Execution Templates
    //==============================================================================
    
    template<typename Func>
    static bool safeWindowsCall(Func&& func, const juce::String& operation) {
        __try {
            return ErrorHandler::safeExecute(std::forward<Func>(func), operation);
        }
        __except(EXCEPTION_EXECUTE_HANDLER) {
            juce::String message = "Windows exception in operation: " + operation + 
                                 " (Exception Code: 0x" + juce::String::toHexString(GetExceptionCode()) + ")";
            ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Critical, message, "Windows Exception");
            return false;
        }
    }
    
    template<typename T>
    static std::unique_ptr<T> safeWindowsCreate(std::function<std::unique_ptr<T>()> creator, const juce::String& component) {
        __try {
            return ErrorHandler::safeCreate<T>(creator, component);
        }
        __except(EXCEPTION_EXECUTE_HANDLER) {
            juce::String message = "Windows exception during creation of: " + component + 
                                 " (Exception Code: 0x" + juce::String::toHexString(GetExceptionCode()) + ")";
            ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Critical, message, "Windows Exception");
            return nullptr;
        }
    }

private:
    void initializeWindowsErrorHandling() {
        // Set up structured exception handling
        SetUnhandledExceptionFilter(unhandledExceptionFilter);
        
        // Initialize symbol handling for stack traces
        #if JUCE_DEBUG
        HANDLE process = GetCurrentProcess();
        SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
        SymInitialize(process, nullptr, TRUE);
        #endif
        
        DBG("Windows error handling initialized");
    }
    
    void cleanup() {
        #if JUCE_DEBUG
        HANDLE process = GetCurrentProcess();
        SymCleanup(process);
        #endif
    }
    
    static LONG WINAPI unhandledExceptionFilter(EXCEPTION_POINTERS* exceptionInfo) {
        juce::String message = "Unhandled Windows exception: Code 0x" + 
                             juce::String::toHexString(exceptionInfo->ExceptionRecord->ExceptionCode);
        
        #if JUCE_DEBUG
        message += "\nStack trace:\n" + getStackTrace();
        #endif
        
        ErrorHandler::getInstance().reportError(ErrorHandler::ErrorLevel::Critical, message, "Windows Exception Handler");
        
        return EXCEPTION_EXECUTE_HANDLER;
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WindowsErrorHandler)
};

#endif // JUCE_WINDOWS

//==============================================================================
// Convenience Macros for Windows Error Handling
//==============================================================================

#if JUCE_WINDOWS

#define OTTO_SAFE_WINDOWS_CALL(operation, code) \
    WindowsErrorHandler::safeWindowsCall([&]() { code; return true; }, operation)

#define OTTO_SAFE_COM_CALL(hr, operation, component) \
    WindowsErrorHandler::reportComError(hr, operation, component)

#define OTTO_SAFE_AUDIO_CALL(driverType, error, deviceName) \
    WindowsErrorHandler::reportAudioDriverError(driverType, error, deviceName)

#define OTTO_REPORT_WINDOWS_ERROR(operation, lastError) \
    WindowsErrorHandler::reportThreadingError(operation, lastError)

#else

// Fallback macros for non-Windows platforms
#define OTTO_SAFE_WINDOWS_CALL(operation, code) \
    ErrorHandler::safeExecute([&]() { code; }, operation)

#define OTTO_SAFE_COM_CALL(hr, operation, component) \
    /* COM not available on non-Windows platforms */

#define OTTO_SAFE_AUDIO_CALL(driverType, error, deviceName) \
    /* Windows-specific audio drivers not available */

#define OTTO_REPORT_WINDOWS_ERROR(operation, lastError) \
    /* Windows-specific error reporting not available */

#endif