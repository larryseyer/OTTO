#pragma once

#include "WindowsErrorHandling.h"
#include <JuceHeader.h>

#if JUCE_WINDOWS
    #include <mmdeviceapi.h>
    #include <audiopolicy.h>
    #include <functiondiscoverykeys_devpkey.h>
    #include <dsound.h>
    #pragma comment(lib, "ole32.lib")
    #pragma comment(lib, "dsound.lib")
#endif

/**
 * OTTO Phase 2.3.1 - Windows Audio Driver Validation
 * Validates and tests Windows audio drivers for optimal JUCE 8 performance
 * Ensures all Phase 2.2 safety features work with Windows audio systems
 */

#if JUCE_WINDOWS

class WindowsAudioValidator
{
public:
    struct AudioDeviceInfo {
        juce::String deviceName;
        juce::String deviceId;
        bool isDefault = false;
        bool isEnabled = false;
        int sampleRate = 0;
        int bufferSize = 0;
        int channels = 0;
        WindowsErrorHandler::AudioDriverType driverType = WindowsErrorHandler::AudioDriverType::Unknown;
    };
    
    struct ValidationResult {
        bool wasapiAvailable = false;
        bool directSoundAvailable = false;
        bool asioAvailable = false;
        bool winMMAvailable = false;
        
        std::vector<AudioDeviceInfo> inputDevices;
        std::vector<AudioDeviceInfo> outputDevices;
        
        juce::String recommendedDriver;
        juce::String warnings;
        juce::String errors;
        
        bool isValid() const {
            return wasapiAvailable || directSoundAvailable || winMMAvailable;
        }
    };
    
    //==============================================================================
    // Main Validation Functions
    //==============================================================================
    
    static ValidationResult validateAllAudioDrivers() {
        ValidationResult result;
        
        DBG("Starting Windows audio driver validation...");
        
        // Test WASAPI (preferred for Windows 10+)
        result.wasapiAvailable = validateWASAPI(result);
        
        // Test DirectSound (legacy but widely supported)
        result.directSoundAvailable = validateDirectSound(result);
        
        // Test WinMM (fallback)
        result.winMMAvailable = validateWinMM(result);
        
        // Test ASIO (professional audio)
        result.asioAvailable = validateASIO(result);
        
        // Determine recommended driver
        determineRecommendedDriver(result);
        
        // Log results
        logValidationResults(result);
        
        return result;
    }
    
    //==============================================================================
    // WASAPI Validation
    //==============================================================================
    
    static bool validateWASAPI(ValidationResult& result) {
        bool success = false;
        
        OTTO_SAFE_WINDOWS_CALL("WASAPI Validation", {
            // Initialize COM
            HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
            if (SUCCEEDED(hr) || hr == RPC_E_CHANGED_MODE) {
                // Create device enumerator
                IMMDeviceEnumerator* deviceEnumerator = nullptr;
                hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                    __uuidof(IMMDeviceEnumerator), (void**)&deviceEnumerator);
                
                if (SUCCEEDED(hr)) {
                    success = true;
                    
                    // Enumerate output devices
                    enumerateWASAPIDevices(deviceEnumerator, eRender, result.outputDevices);
                    
                    // Enumerate input devices  
                    enumerateWASAPIDevices(deviceEnumerator, eCapture, result.inputDevices);
                    
                    deviceEnumerator->Release();
                } else {
                    OTTO_SAFE_COM_CALL(hr, "Create WASAPI Device Enumerator", "WASAPI");
                }
                
                if (hr != RPC_E_CHANGED_MODE) {
                    CoUninitialize();
                }
            }
        });
        
        return success;
    }
    
    //==============================================================================
    // DirectSound Validation
    //==============================================================================
    
    static bool validateDirectSound(ValidationResult& result) {
        bool success = false;
        
        OTTO_SAFE_WINDOWS_CALL("DirectSound Validation", {
            // Test DirectSound creation
            LPDIRECTSOUND8 directSound = nullptr;
            HRESULT hr = DirectSoundCreate8(nullptr, &directSound, nullptr);
            
            if (SUCCEEDED(hr)) {
                success = true;
                
                // Set cooperative level
                hr = directSound->SetCooperativeLevel(GetDesktopWindow(), DSSCL_PRIORITY);
                if (FAILED(hr)) {
                    OTTO_SAFE_COM_CALL(hr, "Set DirectSound Cooperative Level", "DirectSound");
                }
                
                // Test primary buffer creation
                DSBUFFERDESC bufferDesc = {};
                bufferDesc.dwSize = sizeof(DSBUFFERDESC);
                bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
                
                LPDIRECTSOUNDBUFFER primaryBuffer = nullptr;
                hr = directSound->CreateSoundBuffer(&bufferDesc, &primaryBuffer, nullptr);
                
                if (SUCCEEDED(hr)) {
                    // Test buffer format setting
                    WAVEFORMATEX waveFormat = {};
                    waveFormat.wFormatTag = WAVE_FORMAT_PCM;
                    waveFormat.nChannels = 2;
                    waveFormat.nSamplesPerSec = 44100;
                    waveFormat.wBitsPerSample = 16;
                    waveFormat.nBlockAlign = waveFormat.nChannels * waveFormat.wBitsPerSample / 8;
                    waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
                    
                    hr = primaryBuffer->SetFormat(&waveFormat);
                    if (FAILED(hr)) {
                        OTTO_SAFE_COM_CALL(hr, "Set DirectSound Buffer Format", "DirectSound");
                    }
                    
                    primaryBuffer->Release();
                } else {
                    OTTO_SAFE_COM_CALL(hr, "Create DirectSound Primary Buffer", "DirectSound");
                }
                
                directSound->Release();
            } else {
                OTTO_SAFE_COM_CALL(hr, "Create DirectSound Device", "DirectSound");
            }
        });
        
        return success;
    }
    
    //==============================================================================
    // WinMM Validation
    //==============================================================================
    
    static bool validateWinMM(ValidationResult& result) {
        bool success = false;
        
        OTTO_SAFE_WINDOWS_CALL("WinMM Validation", {
            // Check output devices
            UINT numOutputDevices = waveOutGetNumDevs();
            if (numOutputDevices > 0) {
                success = true;
                
                for (UINT i = 0; i < numOutputDevices; ++i) {
                    WAVEOUTCAPS caps;
                    if (waveOutGetDevCaps(i, &caps, sizeof(caps)) == MMSYSERR_NOERROR) {
                        AudioDeviceInfo info;
                        info.deviceName = juce::String(caps.szPname);
                        info.deviceId = "WinMM_Out_" + juce::String(i);
                        info.isEnabled = true;
                        info.channels = caps.wChannels;
                        info.driverType = WindowsErrorHandler::AudioDriverType::WinMM;
                        result.outputDevices.push_back(info);
                    }
                }
            }
            
            // Check input devices
            UINT numInputDevices = waveInGetNumDevs();
            if (numInputDevices > 0) {
                success = true;
                
                for (UINT i = 0; i < numInputDevices; ++i) {
                    WAVEINCAPS caps;
                    if (waveInGetDevCaps(i, &caps, sizeof(caps)) == MMSYSERR_NOERROR) {
                        AudioDeviceInfo info;
                        info.deviceName = juce::String(caps.szPname);
                        info.deviceId = "WinMM_In_" + juce::String(i);
                        info.isEnabled = true;
                        info.channels = caps.wChannels;
                        info.driverType = WindowsErrorHandler::AudioDriverType::WinMM;
                        result.inputDevices.push_back(info);
                    }
                }
            }
        });
        
        return success;
    }
    
    //==============================================================================
    // ASIO Validation
    //==============================================================================
    
    static bool validateASIO(ValidationResult& result) {
        // ASIO validation requires ASIO SDK
        // For now, just check if ASIO4ALL is installed
        bool success = false;
        
        OTTO_SAFE_WINDOWS_CALL("ASIO Validation", {
            // Check registry for ASIO drivers
            HKEY hKey;
            LONG regResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
                                        L"SOFTWARE\\ASIO", 0, KEY_READ, &hKey);
            
            if (regResult == ERROR_SUCCESS) {
                success = true;
                RegCloseKey(hKey);
                
                // Add a generic ASIO entry
                AudioDeviceInfo info;
                info.deviceName = "ASIO Driver (Available)";
                info.deviceId = "ASIO_Generic";
                info.isEnabled = true;
                info.driverType = WindowsErrorHandler::AudioDriverType::ASIO;
                result.outputDevices.push_back(info);
            } else {
                WindowsErrorHandler::reportRegistryError("SOFTWARE\\ASIO", "Open", regResult);
            }
        });
        
        return success;
    }

private:
    //==============================================================================
    // WASAPI Device Enumeration
    //==============================================================================
    
    static void enumerateWASAPIDevices(IMMDeviceEnumerator* enumerator, EDataFlow dataFlow, 
                                     std::vector<AudioDeviceInfo>& devices) {
        IMMDeviceCollection* deviceCollection = nullptr;
        HRESULT hr = enumerator->EnumAudioEndpoints(dataFlow, DEVICE_STATE_ACTIVE, &deviceCollection);
        
        if (SUCCEEDED(hr)) {
            UINT deviceCount = 0;
            hr = deviceCollection->GetCount(&deviceCount);
            
            if (SUCCEEDED(hr)) {
                for (UINT i = 0; i < deviceCount; ++i) {
                    IMMDevice* device = nullptr;
                    hr = deviceCollection->Item(i, &device);
                    
                    if (SUCCEEDED(hr)) {
                        AudioDeviceInfo info = getWASAPIDeviceInfo(device, enumerator, dataFlow);
                        if (info.deviceName.isNotEmpty()) {
                            devices.push_back(info);
                        }
                        device->Release();
                    }
                }
            }
            deviceCollection->Release();
        } else {
            OTTO_SAFE_COM_CALL(hr, "Enumerate WASAPI Devices", "WASAPI");
        }
    }
    
    static AudioDeviceInfo getWASAPIDeviceInfo(IMMDevice* device, IMMDeviceEnumerator* enumerator, EDataFlow dataFlow) {
        AudioDeviceInfo info;
        info.driverType = WindowsErrorHandler::AudioDriverType::WASAPI;
        
        // Get device ID
        LPWSTR deviceId = nullptr;
        HRESULT hr = device->GetId(&deviceId);
        if (SUCCEEDED(hr)) {
            info.deviceId = juce::String(deviceId);
            CoTaskMemFree(deviceId);
        }
        
        // Get device properties
        IPropertyStore* propertyStore = nullptr;
        hr = device->OpenPropertyStore(STGM_READ, &propertyStore);
        if (SUCCEEDED(hr)) {
            PROPVARIANT variant;
            PropVariantInit(&variant);
            
            // Get device name
            hr = propertyStore->GetValue(PKEY_Device_FriendlyName, &variant);
            if (SUCCEEDED(hr) && variant.vt == VT_LPWSTR) {
                info.deviceName = juce::String(variant.pwszVal);
            }
            PropVariantClear(&variant);
            
            propertyStore->Release();
        }
        
        // Check if it's the default device
        IMMDevice* defaultDevice = nullptr;
        hr = enumerator->GetDefaultAudioEndpoint(dataFlow, eConsole, &defaultDevice);
        if (SUCCEEDED(hr)) {
            LPWSTR defaultDeviceId = nullptr;
            hr = defaultDevice->GetId(&defaultDeviceId);
            if (SUCCEEDED(hr)) {
                info.isDefault = (info.deviceId == juce::String(defaultDeviceId));
                CoTaskMemFree(defaultDeviceId);
            }
            defaultDevice->Release();
        }
        
        info.isEnabled = true;
        return info;
    }
    
    //==============================================================================
    // Driver Recommendation Logic
    //==============================================================================
    
    static void determineRecommendedDriver(ValidationResult& result) {
        if (result.wasapiAvailable && !result.outputDevices.empty()) {
            result.recommendedDriver = "WASAPI";
        } else if (result.directSoundAvailable) {
            result.recommendedDriver = "DirectSound";
            result.warnings += "WASAPI not available, using DirectSound. ";
        } else if (result.winMMAvailable) {
            result.recommendedDriver = "WinMM";
            result.warnings += "Using legacy WinMM driver. Consider updating Windows. ";
        } else {
            result.recommendedDriver = "None";
            result.errors += "No working audio drivers found. ";
        }
        
        if (result.asioAvailable) {
            result.warnings += "ASIO drivers detected - professional audio features available. ";
        }
    }
    
    static void logValidationResults(const ValidationResult& result) {
        DBG("=== Windows Audio Validation Results ===");
        DBG("WASAPI: " + juce::String(result.wasapiAvailable ? "Available" : "Not Available"));
        DBG("DirectSound: " + juce::String(result.directSoundAvailable ? "Available" : "Not Available"));
        DBG("WinMM: " + juce::String(result.winMMAvailable ? "Available" : "Not Available"));
        DBG("ASIO: " + juce::String(result.asioAvailable ? "Available" : "Not Available"));
        DBG("Recommended Driver: " + result.recommendedDriver);
        DBG("Output Devices: " + juce::String(result.outputDevices.size()));
        DBG("Input Devices: " + juce::String(result.inputDevices.size()));
        
        if (result.warnings.isNotEmpty()) {
            DBG("Warnings: " + result.warnings);
        }
        
        if (result.errors.isNotEmpty()) {
            DBG("Errors: " + result.errors);
        }
    }
};

#else

// Stub implementation for non-Windows platforms
class WindowsAudioValidator
{
public:
    struct ValidationResult {
        bool isValid() const { return false; }
    };
    
    static ValidationResult validateAllAudioDrivers() {
        return ValidationResult{};
    }
};

#endif // JUCE_WINDOWS