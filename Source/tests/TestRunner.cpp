#include <JuceHeader.h>
#include "AudioProcessingTests.h"
#include "MidiTimingTests.h"
#include "StatePersistenceTests.h"
#include "MemoryLeakTests.h"
#include "CrossPlatformTests.h"



#if JucePlugin_Build_VST || JucePlugin_Build_VST3 || JucePlugin_Build_AU || JucePlugin_Build_AUv3 || JucePlugin_Build_AAX

    namespace OTTOTests {
        void runAllTests() {
            juce::UnitTestRunner runner;
            runner.setAssertOnFailure(false);
            runner.setPassesAreLogged(true);
            runner.runAllTests();

            DBG("=== Test Results ===");
            DBG("Total test suites: " << runner.getNumResults());

            int totalPasses = 0;
            int totalFailures = 0;

            for (int i = 0; i < runner.getNumResults(); ++i) {
                auto* result = runner.getResult(i);
                if (result) {
                    totalPasses += result->passes;
                    totalFailures += result->failures;
                }
            }

            DBG("Passed: " << totalPasses);
            DBG("Failed: " << totalFailures);
        }

        void runTestCategory(const juce::String& category) {
            juce::UnitTestRunner runner;
            runner.setAssertOnFailure(false);
            runner.setPassesAreLogged(true);
            runner.runTestsInCategory(category);

            DBG("=== Test Results for " << category << " ===");

            int totalPasses = 0;
            int totalFailures = 0;

            for (int i = 0; i < runner.getNumResults(); ++i) {
                auto* result = runner.getResult(i);
                if (result) {
                    totalPasses += result->passes;
                    totalFailures += result->failures;
                }
            }

            DBG("Passed: " << totalPasses);
            DBG("Failed: " << totalFailures);
        }
    }

#else

    class OTTOTestApplication : public juce::JUCEApplication {
    public:
        const juce::String getApplicationName() override { return "OTTO Tests"; }
        const juce::String getApplicationVersion() override { return "1.0"; }

        void initialise(const juce::String& commandLine) override {
            auto args = juce::StringArray::fromTokens(commandLine, true);

            testRunner = std::make_unique<juce::UnitTestRunner>();
            testRunner->setAssertOnFailure(false);
            testRunner->setPassesAreLogged(true);

            if (args.contains("--test")) {
                int index = args.indexOf("--test");
                if (index < args.size() - 1) {
                    juce::String testName = args[index + 1];
                    std::cout << "Running test: " << testName << std::endl;
                    testRunner->runTestsInCategory(testName);
                }
            }
            else if (args.contains("--list")) {
                listAllTests();
                quit();
                return;
            }
            else {
                std::cout << "Running all tests..." << std::endl;
                testRunner->runAllTests();
            }

            printResults();

            int totalFailures = 0;
            for (int i = 0; i < testRunner->getNumResults(); ++i) {
                auto* result = testRunner->getResult(i);
                if (result) {
                    totalFailures += result->failures;
                }
            }

            setApplicationReturnValue(totalFailures > 0 ? 1 : 0);
            quit();
        }

        void shutdown() override {
            testRunner = nullptr;
        }

        void anotherInstanceStarted(const juce::String&) override {}

    private:
        std::unique_ptr<juce::UnitTestRunner> testRunner;

        void listAllTests() {
            std::cout << "Available test categories:" << std::endl;
            std::cout << "  - Audio Processing Tests" << std::endl;
            std::cout << "  - MIDI Timing Tests" << std::endl;
            std::cout << "  - State Persistence Tests" << std::endl;
            std::cout << "  - Memory Leak Detection Tests" << std::endl;
            std::cout << "  - Cross-Platform Compatibility Tests" << std::endl;
        }

        void printResults() {
            std::cout << "\n=== Test Results ===" << std::endl;
            std::cout << "Total test suites: " << testRunner->getNumResults() << std::endl;

            int totalPasses = 0;
            int totalFailures = 0;

            for (int i = 0; i < testRunner->getNumResults(); ++i) {
                auto* result = testRunner->getResult(i);
                if (result) {
                    totalPasses += result->passes;
                    totalFailures += result->failures;

                    if (result->failures > 0) {
                        std::cout << "\nFailed in: " << result->unitTestName << std::endl;
                        for (const auto& message : result->messages) {
                            std::cout << "  - " << message << std::endl;
                        }
                    }
                }
            }

            std::cout << "\nTotal Passed: " << totalPasses << std::endl;
            std::cout << "Total Failed: " << totalFailures << std::endl;

            if (totalFailures == 0) {
                std::cout << "\nAll tests passed! ✓" << std::endl;
            } else {
                std::cout << "\nSome tests failed. Check the output above." << std::endl;
            }
        }
    };

    START_JUCE_APPLICATION(OTTOTestApplication)
#endif
