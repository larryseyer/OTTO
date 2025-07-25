#pragma once
#include <JuceHeader.h>
#include "../FontManager.h"
#include "../INIConfig.h"

class CrossPlatformTests : public juce::UnitTest {
public:
    CrossPlatformTests() : juce::UnitTest("Cross-Platform Compatibility Tests") {}

    void runTest() override {
        beginTest("File Path Handling");
        testFilePathHandling();

        beginTest("Font Loading");
        testFontLoading();

        beginTest("Audio Device Enumeration");
        testAudioDeviceEnumeration();

        beginTest("MIDI Device Enumeration");
        testMidiDeviceEnumeration();

        beginTest("Window Sizing");
        testWindowSizing();

        beginTest("DPI Scaling");
        testDPIScaling();

        beginTest("File System Permissions");
        testFileSystemPermissions();

        beginTest("Unicode Support");
        testUnicodeSupport();

        beginTest("Performance Consistency");
        testPerformanceConsistency();
    }

private:
    void testFilePathHandling() {
        juce::File testFile;

        #if JUCE_WINDOWS
            testFile = juce::File("C:\\Test\\Path\\file.txt");
            expect(testFile.getFullPathName().contains("\\"), "Windows paths should use backslashes");
        #elif JUCE_MAC
            testFile = juce::File("/Users/Test/Path/file.txt");
            expect(testFile.getFullPathName().contains("/"), "Mac paths should use forward slashes");
        #elif JUCE_LINUX
            testFile = juce::File("/home/test/path/file.txt");
            expect(testFile.getFullPathName().contains("/"), "Linux paths should use forward slashes");
        #endif

        // Test special locations
        auto docsDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
        expect(docsDir.exists(), "Documents directory should exist");

        auto tempDir = juce::File::getSpecialLocation(juce::File::tempDirectory);
        expect(tempDir.exists(), "Temp directory should exist");

        // Test OTTO data directory
        auto ottoDir = INIConfig::getOTTODataDirectory();
        logMessage("OTTO data directory: " + ottoDir.getFullPathName());

        // Test relative paths
        auto parent = ottoDir.getParentDirectory();
        auto child = parent.getChildFile(ottoDir.getFileName());
        expect(child.getFullPathName() == ottoDir.getFullPathName(),
            "Relative path resolution should work");
    }

    void testFontLoading() {
        FontManager fontManager;

        // Test system font availability
        auto systemFonts = juce::Font::findAllTypefaceNames();
        expect(systemFonts.size() > INIConfig::Defaults::ZERO_VALUE, "System fonts should be available");

        // Test custom font loading
        auto assetsPath = FontManager::getAssetsPath();
        if (assetsPath.exists()) {
            fontManager.loadCustomFonts(assetsPath);
            expect(fontManager.hasCustomFonts(), "Custom fonts should load if assets exist");
        }

        // Test font rendering at different sizes
        const float sizes[] = { static_cast<float>(INIConfig::Defaults::MAX_PLAYERS),
                               static_cast<float>(INIConfig::UI::MAX_GROUP_NAME_LENGTH),
                               INIConfig::Fonts::BODY_DEFAULT_SIZE,
                               INIConfig::Fonts::BRAND_DEFAULT_SIZE,
                               INIConfig::Fonts::BRAND_DEFAULT_SIZE * INIConfig::Audio::NUM_SEND_TYPES,
                               INIConfig::Fonts::MAX_FONT_SIZE };

        for (auto size : sizes) {
            auto font = fontManager.getFont(FontManager::FontRole::Body, size);
            expect(font.getHeight() > INIConfig::Validation::MIN_VOLUME, "Font should have valid height at size " + juce::String(size));
        }

        // Test Phosphor icon fonts
        for (int weight = INIConfig::Validation::MIN_PHOSPHOR_WEIGHT; weight <= INIConfig::Validation::MAX_PHOSPHOR_WEIGHT; ++weight) {
            auto iconFont = fontManager.getPhosphorFont(
                static_cast<FontManager::PhosphorWeight>(weight), INIConfig::Fonts::PHOSPHOR_ICON_SIZE_MEDIUM);

            // Icons might not be available on all platforms
            if (iconFont.getTypefaceName().contains("Phosphor")) {
                expect(iconFont.getHeight() > INIConfig::Validation::MIN_VOLUME, "Phosphor font should have valid height");
            }
        }
    }

    void testAudioDeviceEnumeration() {
        juce::AudioDeviceManager deviceManager;

        auto* currentDevice = deviceManager.getCurrentAudioDevice();
        if (currentDevice == nullptr) {
            // Initialize with default devices
            auto result = deviceManager.initialiseWithDefaultDevices(INIConfig::Defaults::DEFAULT_INPUT_CHANNELS, INIConfig::Defaults::DEFAULT_OUTPUT_CHANNELS);
            expect(result.isEmpty(), "Audio device initialization should succeed or provide error");
        }

        // Get available device types
        auto& types = deviceManager.getAvailableDeviceTypes();
        expect(types.size() > INIConfig::Defaults::ZERO_VALUE, "Should have at least one audio device type");

        for (auto* type : types) {
            logMessage("Audio device type: " + type->getTypeName());

            auto deviceNames = type->getDeviceNames();
            logMessage("  Input devices: " + juce::String(deviceNames.size()));

            // Note: In JUCE 8, getDeviceNames() returns a StringArray directly
            // Not a struct with inputDeviceNames/outputDeviceNames
            for (const auto& name : deviceNames) {
                logMessage("  Device: " + name);
            }
        }

        // Test sample rates
        if (currentDevice != nullptr) {
            auto availableSampleRates = currentDevice->getAvailableSampleRates();
            expect(availableSampleRates.size() > INIConfig::Defaults::ZERO_VALUE, "Should have available sample rates");

            for (auto sr : availableSampleRates) {
                logMessage("  Sample rate: " + juce::String(sr));
            }

            // Test buffer sizes
            auto availableBufferSizes = currentDevice->getAvailableBufferSizes();
            expect(availableBufferSizes.size() > INIConfig::Defaults::ZERO_VALUE, "Should have available buffer sizes");
        }
    }

    void testMidiDeviceEnumeration() {
        // Test MIDI inputs
        auto midiInputs = juce::MidiInput::getAvailableDevices();
        logMessage("MIDI Input devices: " + juce::String(midiInputs.size()));

        for (const auto& device : midiInputs) {
            logMessage("  Input: " + device.name + " (" + device.identifier + ")");
        }

        // Test MIDI outputs
        auto midiOutputs = juce::MidiOutput::getAvailableDevices();
        logMessage("MIDI Output devices: " + juce::String(midiOutputs.size()));

        for (const auto& device : midiOutputs) {
            logMessage("  Output: " + device.name + " (" + device.identifier + ")");
        }

        // Platform-specific expectations
        #if JUCE_MAC
            // Mac should typically have at least IAC Driver
            expect(midiInputs.size() > INIConfig::Defaults::ZERO_VALUE || midiOutputs.size() > INIConfig::Defaults::ZERO_VALUE,
                "Mac should have MIDI devices available");
        #endif
    }

    void testWindowSizing() {
        // Test display information
        auto& desktop = juce::Desktop::getInstance();
        auto displays = desktop.getDisplays();

        expect(displays.displays.size() > INIConfig::Defaults::ZERO_VALUE, "Should have at least one display");

        for (int i = 0; i < displays.displays.size(); ++i) {
            const auto& display = displays.displays.getReference(i);

            logMessage("Display " + juce::String(i) + ":");
            logMessage("  Total area: " + display.totalArea.toString());
            logMessage("  User area: " + display.userArea.toString());
            logMessage("  DPI: " + juce::String(display.dpi));
            logMessage("  Scale: " + juce::String(display.scale));

            expect(display.totalArea.getWidth() > INIConfig::Defaults::ZERO_VALUE && display.totalArea.getHeight() > INIConfig::Defaults::ZERO_VALUE,
                "Display should have valid dimensions");
        }

        // Test window constraints
        ResponsiveLayoutManager::LayoutConfig config;
        expect(config.baseWidth > INIConfig::Defaults::ZERO_VALUE && config.baseHeight > INIConfig::Defaults::ZERO_VALUE,
            "Default window size should be valid");

        // Test minimum window size
        expect(LayoutConstants::minWindowWidth >= INIConfig::Defaults::DEFAULT_INTERFACE_X * static_cast<int>(INIConfig::Defaults::BEATS_PER_BAR) + INIConfig::Defaults::DEFAULT_INTERFACE_X - INIConfig::UI::CONTENT_MARGIN, "Minimum width should be reasonable");
        expect(LayoutConstants::minWindowHeight >= INIConfig::Defaults::DEFAULT_INTERFACE_Y * INIConfig::Audio::NUM_SEND_TYPES + INIConfig::Defaults::DEFAULT_INTERFACE_Y + INIConfig::UI::EDITOR_HEIGHT, "Minimum height should be reasonable");
    }

    void testDPIScaling() {
        ResponsiveLayoutManager::LayoutConfig config;
        ResponsiveLayoutManager layoutManager(config);

        // Test system DPI scale
        float systemScale = layoutManager.getSystemDPIScale();
        expect(systemScale > INIConfig::Validation::MIN_VOLUME, "System DPI scale should be positive");
        logMessage("System DPI scale: " + juce::String(systemScale));

        // Test scaling calculations
        const int testDimensions[] = { INIConfig::UI::MAX_TOGGLE_STATES * INIConfig::Audio::NUM_SEND_TYPES,
                                      static_cast<int>(INIConfig::Defaults::SWING),
                                      INIConfig::Defaults::FIXED_VELOCITY,
                                      INIConfig::Defaults::DEFAULT_INTERFACE_Y * INIConfig::Audio::NUM_SEND_TYPES,
                                      INIConfig::Defaults::DEFAULT_INTERFACE_Y * INIConfig::UI::MAX_TOGGLE_STATES,
                                      INIConfig::Defaults::DEFAULT_AUTO_SAVE_INTERVAL * INIConfig::Audio::NUM_EQ_BANDS + INIConfig::Defaults::DEFAULT_AUTO_SAVE_INTERVAL / INIConfig::Audio::NUM_EQ_BANDS };

        for (auto dim : testDimensions) {
            int scaled = layoutManager.scaled(dim);
            expect(scaled > INIConfig::Defaults::ZERO_VALUE, "Scaled dimension should be positive");

            // Verify scaling is applied correctly
            float expectedScaled = dim * layoutManager.getCurrentScale();
            expectWithinAbsoluteError(static_cast<float>(scaled), expectedScaled, INIConfig::Validation::MAX_VOLUME);
        }

        // Test high DPI scenarios
        layoutManager.setScale(INIConfig::Validation::MAX_INTERFACE_SCALE);
        expect(layoutManager.getCurrentScale() == INIConfig::Validation::MAX_INTERFACE_SCALE, "Should handle 2x scaling");

        layoutManager.setScale((INIConfig::Validation::MIN_INTERFACE_SCALE + INIConfig::Validation::MAX_INTERFACE_SCALE) / INIConfig::Audio::NUM_SEND_TYPES);
        expect(layoutManager.getCurrentScale() == (INIConfig::Validation::MIN_INTERFACE_SCALE + INIConfig::Validation::MAX_INTERFACE_SCALE) / INIConfig::Audio::NUM_SEND_TYPES, "Should handle 1.5x scaling");
    }

    void testFileSystemPermissions() {
        // Test write permissions in user directories
        auto testDir = juce::File::getSpecialLocation(juce::File::tempDirectory)
                        .getChildFile("OTTOPermissionTest");

        // Create directory
        auto result = testDir.createDirectory();
        expect(result.wasOk(), "Should be able to create directory in temp: " + result.getErrorMessage());

        if (testDir.exists()) {
            // Test file creation
            auto testFile = testDir.getChildFile("test.txt");
            testFile.replaceWithText("Permission test");
            expect(testFile.existsAsFile(), "Should be able to create file");

            // Test file reading
            auto content = testFile.loadFileAsString();
            expectEquals(content, juce::String("Permission test"), "Should be able to read file");

            // Cleanup
            testDir.deleteRecursively();
        }

        // Test OTTO data directory permissions
        auto ottoDir = INIConfig::getOTTODataDirectory();
        if (!ottoDir.exists()) {
            auto createResult = ottoDir.createDirectory();
            expect(createResult.wasOk(), "Should be able to create OTTO directory: " +
                createResult.getErrorMessage());
        }
    }

    void testUnicodeSupport() {
        // Test Unicode in file names
        auto tempDir = juce::File::getSpecialLocation(juce::File::tempDirectory);

        const juce::String unicodeNames[] = {
            "Test_英文_File.txt",
            "Тест_Файл.txt",
            "テスト_ファイル.txt",
            "Test_Émojis_🎵🎹🥁.txt"
        };

        for (const auto& name : unicodeNames) {
            auto file = tempDir.getChildFile(name);

            // Try to create file
            file.replaceWithText("Unicode test");

            if (file.existsAsFile()) {
                expect(file.loadFileAsString() == "Unicode test",
                    "Should handle Unicode filename: " + name);
                file.deleteFile();
            } else {
                logMessage("Platform doesn't support Unicode filename: " + name);
            }
        }

        // Test Unicode in INI files
        INIDataManager iniManager;
        GlobalSettings settings;
        settings.setDefaults();
        settings.settingsName = "Test 測試 тест";

        expect(iniManager.saveGlobalSettings(settings), "Should save Unicode settings");

        GlobalSettings loaded;
        if (iniManager.loadGlobalSettings(loaded)) {
            expectEquals(loaded.settingsName, settings.settingsName,
                "Should preserve Unicode in settings");
        }
    }

    void testPerformanceConsistency() {
        auto processor = std::make_unique<OTTOAudioProcessor>();
        
        #if JUCE_MAC || JUCE_IOS
            double optimalSampleRate = 48000.0;
            int optimalBufferSize = 256;
            const int warmupIterations = 100;
        #elif JUCE_WINDOWS
            double optimalSampleRate = 44100.0;
            int optimalBufferSize = 512;
            const int warmupIterations = 50;
        #elif JUCE_LINUX
            double optimalSampleRate = 48000.0;
            int optimalBufferSize = 1024;
            const int warmupIterations = 50;
        #elif JUCE_ANDROID
            double optimalSampleRate = 44100.0;
            int optimalBufferSize = 1024;
            const int warmupIterations = 50;
        #else
            double optimalSampleRate = static_cast<double>(INIConfig::Defaults::DEFAULT_SAMPLE_RATE);
            int optimalBufferSize = INIConfig::Defaults::DEFAULT_BUFFER_SIZE;
            const int warmupIterations = 50;
        #endif
        
        processor->prepareToPlay(optimalSampleRate, optimalBufferSize);

        juce::AudioBuffer<float> buffer(2, optimalBufferSize);
        juce::MidiBuffer midiBuffer;

        for (int i = 0; i < warmupIterations; ++i) {
            processor->processBlock(buffer, midiBuffer);
        }

        const int numBlocks = 1000;
        auto startTime = juce::Time::getHighResolutionTicks();

        for (int i = 0; i < numBlocks; ++i) {
            processor->processBlock(buffer, midiBuffer);
        }

        auto endTime = juce::Time::getHighResolutionTicks();
        double processingTimeMs = juce::Time::highResolutionTicksToSeconds(endTime - startTime) * 1000.0;
        double averageBlockTime = processingTimeMs / numBlocks;

        logMessage("Platform: " + juce::SystemStats::getOperatingSystemName());
        logMessage("Optimal sample rate: " + juce::String(optimalSampleRate));
        logMessage("Optimal buffer size: " + juce::String(optimalBufferSize));
        logMessage("Average block processing time: " + juce::String(averageBlockTime, 3) + "ms");

        double maxExpectedTime = (optimalBufferSize / optimalSampleRate) * 1000.0 * 0.5;
        expect(averageBlockTime < maxExpectedTime, 
               "Processing should be faster than 50% real-time on " + juce::SystemStats::getOperatingSystemName());
    }

    // Helper method
    void expectWithinAbsoluteError(float actual, float expected, float tolerance) {
        expect(std::abs(actual - expected) <= tolerance,
               "Expected " + juce::String(expected) + " but got " + juce::String(actual));
    }
};

static CrossPlatformTests crossPlatformTests;
