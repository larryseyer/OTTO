#pragma once
#include <JuceHeader.h>
#include "AudioProcessingTests.h"
#include "MidiTimingTests.h"
#include "StatePersistenceTests.h"
#include "MemoryLeakTests.h"
#include "CrossPlatformTests.h"

class TestRunnerPlugin {
public:
    static void runAllTests() {
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

                if (result->failures > 0) {
                    DBG("Failed in: " << result->unitTestName);
                    for (const auto& message : result->messages) {
                        DBG("  - " << message);
                    }
                }
            }
        }

        DBG("Passed: " << totalPasses);
        DBG("Failed: " << totalFailures);
    }
};
