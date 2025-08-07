#include <JuceHeader.h>
#include "../Source/JUCE8_CODING_STANDARDS.h"
#include "../Source/INIConfig.h"

// Include all E2E test suites
#include "UIWorkflowTests.h"
#include "ComponentIntegrationTests.h"
#include "UserInteractionTests.h"
#include "StateManagementE2ETests.h"
#include "E2ETestRunner.h"

//==============================================================================
// E2E Test Main - Standalone E2E Test Application
// Provides comprehensive end-to-end testing capabilities
//==============================================================================

class E2ETestApplication : public juce::JUCEApplication {
public:
    E2ETestApplication() = default;

    const juce::String getApplicationName() override {
        return "OTTO E2E Tests";
    }

    const juce::String getApplicationVersion() override {
        return "1.0.0";
    }

    bool moreThanOneInstanceAllowed() override {
        return true;
    }

    void initialise(const juce::String& commandLine) override {
        DBG("OTTO E2E Test Suite Starting...");
        DBG("Command line: " + commandLine);
        
        // Verify JUCE 8 compliance
        if (!JUCE8Standards::verifyJUCE8Compliance()) {
            DBG("ERROR: JUCE 8 compliance verification failed!");
            quit();
            return;
        }
        
        DBG("✓ JUCE 8 compliance verified");
        
        // Parse command line arguments
        parseCommandLineArguments(commandLine);
        
        // Initialize test environment
        initializeTestEnvironment();
        
        // Run tests based on command line options
        if (shouldRunTests) {
            runE2ETests();
        } else {
            // Create GUI test runner window
            createTestRunnerWindow();
        }
    }

    void shutdown() override {
        DBG("OTTO E2E Test Suite Shutting Down...");
        
        // Clean up test runner window
        testRunnerWindow.reset();
        
        // Clean up any test resources
        cleanupTestEnvironment();
        
        DBG("E2E Test Suite shutdown complete");
    }

    void systemRequestedQuit() override {
        quit();
    }

    void anotherInstanceStarted(const juce::String& commandLine) override {
        DBG("Another E2E test instance started with: " + commandLine);
    }

private:
    //==============================================================================
    // Test Configuration
    //==============================================================================
    
    struct TestConfiguration {
        bool runAllTests = false;
        bool runUIWorkflowTests = false;
        bool runComponentIntegrationTests = false;
        bool runUserInteractionTests = false;
        bool runStateManagementTests = false;
        bool runPerformanceTests = false;
        bool verboseOutput = false;
        bool generateReport = false;
        juce::String reportPath;
        juce::String testFilter;
    };
    
    TestConfiguration config;
    bool shouldRunTests = false;
    std::unique_ptr<juce::DocumentWindow> testRunnerWindow;

    //==============================================================================
    // Command Line Parsing
    //==============================================================================
    
    void parseCommandLineArguments(const juce::String& commandLine) {
        juce::StringArray args = juce::StringArray::fromTokens(commandLine, true);
        
        for (int i = 0; i < args.size(); ++i) {
            const auto& arg = args[i];
            
            if (arg == "--help" || arg == "-h") {
                printUsage();
                quit();
                return;
            }
            else if (arg == "--all") {
                config.runAllTests = true;
                shouldRunTests = true;
            }
            else if (arg == "--ui-workflow") {
                config.runUIWorkflowTests = true;
                shouldRunTests = true;
            }
            else if (arg == "--component-integration") {
                config.runComponentIntegrationTests = true;
                shouldRunTests = true;
            }
            else if (arg == "--user-interaction") {
                config.runUserInteractionTests = true;
                shouldRunTests = true;
            }
            else if (arg == "--state-management") {
                config.runStateManagementTests = true;
                shouldRunTests = true;
            }
            else if (arg == "--performance") {
                config.runPerformanceTests = true;
                shouldRunTests = true;
            }
            else if (arg == "--verbose" || arg == "-v") {
                config.verboseOutput = true;
            }
            else if (arg == "--report") {
                config.generateReport = true;
                if (i + 1 < args.size()) {
                    config.reportPath = args[++i];
                }
            }
            else if (arg == "--filter") {
                if (i + 1 < args.size()) {
                    config.testFilter = args[++i];
                }
            }
            else if (arg == "--gui") {
                shouldRunTests = false; // Force GUI mode
            }
        }
        
        // If no specific tests selected but shouldRunTests is true, run all
        if (shouldRunTests && !config.runAllTests && 
            !config.runUIWorkflowTests && !config.runComponentIntegrationTests &&
            !config.runUserInteractionTests && !config.runStateManagementTests &&
            !config.runPerformanceTests) {
            config.runAllTests = true;
        }
    }
    
    void printUsage() {
        std::cout << "\nOTTO E2E Test Suite Usage:\n\n";
        std::cout << "Command Line Options:\n";
        std::cout << "  --help, -h                 Show this help message\n";
        std::cout << "  --all                      Run all E2E tests\n";
        std::cout << "  --ui-workflow              Run UI workflow tests only\n";
        std::cout << "  --component-integration    Run component integration tests only\n";
        std::cout << "  --user-interaction         Run user interaction tests only\n";
        std::cout << "  --state-management         Run state management tests only\n";
        std::cout << "  --performance              Run performance tests only\n";
        std::cout << "  --verbose, -v              Enable verbose output\n";
        std::cout << "  --report [path]            Generate test report (optional path)\n";
        std::cout << "  --filter [pattern]         Filter tests by pattern\n";
        std::cout << "  --gui                      Launch GUI test runner\n\n";
        std::cout << "Examples:\n";
        std::cout << "  ./E2ETests --all --verbose\n";
        std::cout << "  ./E2ETests --ui-workflow --report results.xml\n";
        std::cout << "  ./E2ETests --gui\n\n";
    }

    //==============================================================================
    // Test Environment
    //==============================================================================
    
    void initializeTestEnvironment() {
        DBG("Initializing E2E test environment...");
        
        // Initialize INI configuration system
        try {
            // Verify INI config constants are available
            auto defaultWidth = INIConfig::LayoutConstants::defaultInterfaceWidth;
            auto defaultHeight = INIConfig::LayoutConstants::defaultInterfaceHeight;
            
            DBG("✓ INI Config system initialized");
            DBG("  Default interface size: " + juce::String(defaultWidth) + "x" + juce::String(defaultHeight));
            
        } catch (const std::exception& e) {
            DBG("ERROR: Failed to initialize INI config system: " + juce::String(e.what()));
        }
        
        // Initialize JUCE systems
        juce::MessageManager::getInstance();
        
        DBG("✓ E2E test environment initialized successfully");
    }
    
    void cleanupTestEnvironment() {
        DBG("Cleaning up E2E test environment...");
        
        // Cleanup would go here if needed
        
        DBG("✓ E2E test environment cleanup complete");
    }

    //==============================================================================
    // Test Execution
    //==============================================================================
    
    void runE2ETests() {
        DBG("Starting E2E test execution...");
        
        juce::UnitTestRunner runner;
        runner.setAssertOnFailure(false);
        runner.setPassesAreLogged(config.verboseOutput);
        
        // Configure test runner based on options
        if (config.runAllTests) {
            DBG("Running all E2E test suites...");
            runAllTestSuites(runner);
        } else {
            DBG("Running selected E2E test suites...");
            runSelectedTestSuites(runner);
        }
        
        // Generate report if requested
        if (config.generateReport) {
            generateTestReport(runner);
        }
        
        // Print results
        printTestResults(runner);
        
        // Exit with appropriate code
        bool allTestsPassed = (runner.getNumResults() > 0);
        for (int i = 0; i < runner.getNumResults(); ++i) {
            auto* result = runner.getResult(i);
            if (result && result->failures > 0) {
                allTestsPassed = false;
                break;
            }
        }
        
        DBG("E2E test execution complete. Success: " + juce::String(allTestsPassed ? "true" : "false"));
        
        // Delay before quit to allow log output
        juce::Timer::callAfterDelay(1000, [this]() {
            quit();
        });
    }
    
    void runAllTestSuites(juce::UnitTestRunner& runner) {
        runner.runTestsInCategory("UI Workflow Tests");
        runner.runTestsInCategory("Component Integration Tests");
        runner.runTestsInCategory("User Interaction Tests");
        runner.runTestsInCategory("State Management E2E Tests");
        runner.runTestsInCategory("E2E Test Runner");
    }
    
    void runSelectedTestSuites(juce::UnitTestRunner& runner) {
        if (config.runUIWorkflowTests) {
            runner.runTestsInCategory("UI Workflow Tests");
        }
        
        if (config.runComponentIntegrationTests) {
            runner.runTestsInCategory("Component Integration Tests");
        }
        
        if (config.runUserInteractionTests) {
            runner.runTestsInCategory("User Interaction Tests");
        }
        
        if (config.runStateManagementTests) {
            runner.runTestsInCategory("State Management E2E Tests");
        }
        
        if (config.runPerformanceTests) {
            runner.runTestsInCategory("E2E Test Runner");
        }
    }
    
    void generateTestReport(const juce::UnitTestRunner& runner) {
        DBG("Generating test report...");
        
        juce::String reportPath = config.reportPath;
        if (reportPath.isEmpty()) {
            reportPath = "e2e_test_results.xml";
        }
        
        juce::File reportFile(reportPath);
        
        // Generate XML report
        juce::XmlElement report("E2ETestResults");
        report.setAttribute("timestamp", juce::Time::getCurrentTime().toISO8601(true));
        report.setAttribute("totalSuites", runner.getNumResults());
        
        int totalPasses = 0;
        int totalFailures = 0;
        
        for (int i = 0; i < runner.getNumResults(); ++i) {
            auto* result = runner.getResult(i);
            if (result) {
                auto* suiteElement = report.createNewChildElement("TestSuite");
                suiteElement->setAttribute("name", result->unitTestName);
                suiteElement->setAttribute("passes", result->passes);
                suiteElement->setAttribute("failures", result->failures);
                
                totalPasses += result->passes;
                totalFailures += result->failures;
                
                if (result->failures > 0) {
                    auto* failuresElement = suiteElement->createNewChildElement("Failures");
                    for (const auto& message : result->messages) {
                        auto* messageElement = failuresElement->createNewChildElement("Message");
                        messageElement->addTextElement(message);
                    }
                }
            }
        }
        
        report.setAttribute("totalPasses", totalPasses);
        report.setAttribute("totalFailures", totalFailures);
        
        // Write report to file
        if (reportFile.replaceWithText(report.toString())) {
            DBG("✓ Test report generated: " + reportFile.getFullPathName());
        } else {
            DBG("ERROR: Failed to write test report to: " + reportFile.getFullPathName());
        }
    }
    
    void printTestResults(const juce::UnitTestRunner& runner) {
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "OTTO E2E Test Results Summary" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        
        int totalPasses = 0;
        int totalFailures = 0;
        
        for (int i = 0; i < runner.getNumResults(); ++i) {
            auto* result = runner.getResult(i);
            if (result) {
                totalPasses += result->passes;
                totalFailures += result->failures;
                
                std::cout << "\n📋 " << result->unitTestName << std::endl;
                std::cout << "   ✅ Passes: " << result->passes << std::endl;
                std::cout << "   ❌ Failures: " << result->failures << std::endl;
                
                if (result->failures > 0 && config.verboseOutput) {
                    std::cout << "   📝 Failure Details:" << std::endl;
                    for (const auto& message : result->messages) {
                        std::cout << "      • " << message << std::endl;
                    }
                }
            }
        }
        
        std::cout << "\n" << std::string(60, '-') << std::endl;
        std::cout << "📊 Overall Results:" << std::endl;
        std::cout << "   Total Test Suites: " << runner.getNumResults() << std::endl;
        std::cout << "   Total Passes: " << totalPasses << std::endl;
        std::cout << "   Total Failures: " << totalFailures << std::endl;
        
        if (totalFailures == 0) {
            std::cout << "\n🎉 ALL E2E TESTS PASSED! 🎉" << std::endl;
            std::cout << "OTTO is ready for production deployment." << std::endl;
        } else {
            float successRate = totalFailures == 0 ? 100.0f : 
                               (static_cast<float>(totalPasses) / static_cast<float>(totalPasses + totalFailures)) * 100.0f;
            std::cout << "   Success Rate: " << std::fixed << std::setprecision(1) << successRate << "%" << std::endl;
            
            if (successRate >= 90.0f) {
                std::cout << "\n✅ E2E tests mostly successful with minor issues." << std::endl;
            } else if (successRate >= 75.0f) {
                std::cout << "\n⚠️  E2E tests show significant issues requiring attention." << std::endl;
            } else {
                std::cout << "\n❌ E2E tests indicate major issues requiring immediate attention." << std::endl;
            }
        }
        
        std::cout << std::string(60, '=') << std::endl;
    }

    //==============================================================================
    // GUI Test Runner
    //==============================================================================
    
    void createTestRunnerWindow() {
        DBG("Creating GUI test runner window...");
        
        testRunnerWindow = std::make_unique<E2ETestRunnerWindow>();
        testRunnerWindow->setVisible(true);
        testRunnerWindow->toFront(true);
    }
    
    //==============================================================================
    // GUI Test Runner Window
    //==============================================================================
    
    class E2ETestRunnerWindow : public juce::DocumentWindow {
    public:
        E2ETestRunnerWindow() 
            : juce::DocumentWindow("OTTO E2E Test Runner", 
                                 juce::Colours::darkgrey, 
                                 juce::DocumentWindow::allButtons) {
            
            setUsingNativeTitleBar(true);
            setContentOwned(new E2ETestRunnerComponent(), true);
            
            // Set window size based on INI config
            auto width = INIConfig::LayoutConstants::defaultInterfaceWidth;
            auto height = INIConfig::LayoutConstants::defaultInterfaceHeight;
            setSize(width, height);
            
            centreWithSize(width, height);
            setResizable(true, true);
        }
        
        void closeButtonPressed() override {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }
        
    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(E2ETestRunnerWindow)
    };
    
    //==============================================================================
    // GUI Test Runner Component
    //==============================================================================
    
    class E2ETestRunnerComponent : public juce::Component {
    public:
        E2ETestRunnerComponent() {
            setupUI();
        }
        
        void paint(juce::Graphics& g) override {
            g.fillAll(juce::Colours::white);
            
            g.setColour(juce::Colours::black);
            g.setFont(JUCE8_FONT(INIConfig::LayoutConstants::fontSizeTitle));
            g.drawText("OTTO E2E Test Runner", getLocalBounds().removeFromTop(50), 
                      juce::Justification::centred);
        }
        
        void resized() override {
            auto area = getLocalBounds();
            area.removeFromTop(60); // Title space
            area.reduce(20, 20);
            
            // Layout test suite buttons
            auto buttonHeight = 40;
            auto spacing = 10;
            
            runAllButton.setBounds(area.removeFromTop(buttonHeight));
            area.removeFromTop(spacing);
            
            runUIWorkflowButton.setBounds(area.removeFromTop(buttonHeight));
            area.removeFromTop(spacing);
            
            runComponentIntegrationButton.setBounds(area.removeFromTop(buttonHeight));
            area.removeFromTop(spacing);
            
            runUserInteractionButton.setBounds(area.removeFromTop(buttonHeight));
            area.removeFromTop(spacing);
            
            runStateManagementButton.setBounds(area.removeFromTop(buttonHeight));
            area.removeFromTop(spacing * 2);
            
            // Results area
            if (area.getHeight() > 100) {
                resultsTextEditor.setBounds(area);
            }
        }
        
    private:
        juce::TextButton runAllButton{"Run All E2E Tests"};
        juce::TextButton runUIWorkflowButton{"Run UI Workflow Tests"};
        juce::TextButton runComponentIntegrationButton{"Run Component Integration Tests"};
        juce::TextButton runUserInteractionButton{"Run User Interaction Tests"};
        juce::TextButton runStateManagementButton{"Run State Management Tests"};
        juce::TextEditor resultsTextEditor;
        
        void setupUI() {
            // Setup buttons
            addAndMakeVisible(runAllButton);
            addAndMakeVisible(runUIWorkflowButton);
            addAndMakeVisible(runComponentIntegrationButton);
            addAndMakeVisible(runUserInteractionButton);
            addAndMakeVisible(runStateManagementButton);
            
            // Setup results display
            addAndMakeVisible(resultsTextEditor);
            resultsTextEditor.setMultiLine(true);
            resultsTextEditor.setReadOnly(true);
            resultsTextEditor.setFont(JUCE8_FONT(12.0f));
            resultsTextEditor.setText("Click a button above to run E2E tests...");
            
            // Setup button callbacks
            runAllButton.onClick = [this]() { runTestSuite("all"); };
            runUIWorkflowButton.onClick = [this]() { runTestSuite("ui-workflow"); };
            runComponentIntegrationButton.onClick = [this]() { runTestSuite("component-integration"); };
            runUserInteractionButton.onClick = [this]() { runTestSuite("user-interaction"); };
            runStateManagementButton.onClick = [this]() { runTestSuite("state-management"); };
        }
        
        void runTestSuite(const juce::String& suiteType) {
            resultsTextEditor.setText("Running " + suiteType + " tests...\n\n");
            
            juce::UnitTestRunner runner;
            runner.setAssertOnFailure(false);
            runner.setPassesAreLogged(true);
            
            if (suiteType == "all") {
                runner.runTestsInCategory("UI Workflow Tests");
                runner.runTestsInCategory("Component Integration Tests");
                runner.runTestsInCategory("User Interaction Tests");
                runner.runTestsInCategory("State Management E2E Tests");
                runner.runTestsInCategory("E2E Test Runner");
            } else if (suiteType == "ui-workflow") {
                runner.runTestsInCategory("UI Workflow Tests");
            } else if (suiteType == "component-integration") {
                runner.runTestsInCategory("Component Integration Tests");
            } else if (suiteType == "user-interaction") {
                runner.runTestsInCategory("User Interaction Tests");
            } else if (suiteType == "state-management") {
                runner.runTestsInCategory("State Management E2E Tests");
            }
            
            // Display results
            displayTestResults(runner);
        }
        
        void displayTestResults(const juce::UnitTestRunner& runner) {
            juce::String results = "Test Results:\n";
            results += juce::String::repeatedString("=", 50) + "\n\n";
            
            int totalPasses = 0;
            int totalFailures = 0;
            
            for (int i = 0; i < runner.getNumResults(); ++i) {
                auto* result = runner.getResult(i);
                if (result) {
                    totalPasses += result->passes;
                    totalFailures += result->failures;
                    
                    results += result->unitTestName + "\n";
                    results += "  Passes: " + juce::String(result->passes) + "\n";
                    results += "  Failures: " + juce::String(result->failures) + "\n";
                    
                    if (result->failures > 0) {
                        results += "  Failed Tests:\n";
                        for (const auto& message : result->messages) {
                            results += "    - " + message + "\n";
                        }
                    }
                    results += "\n";
                }
            }
            
            results += juce::String::repeatedString("-", 50) + "\n";
            results += "Total Passes: " + juce::String(totalPasses) + "\n";
            results += "Total Failures: " + juce::String(totalFailures) + "\n";
            
            if (totalFailures == 0) {
                results += "\n🎉 ALL TESTS PASSED! 🎉\n";
            } else {
                float successRate = static_cast<float>(totalPasses) / static_cast<float>(totalPasses + totalFailures) * 100.0f;
                results += "Success Rate: " + juce::String(successRate, 1) + "%\n";
            }
            
            resultsTextEditor.setText(results);
        }
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(E2ETestRunnerComponent)
    };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(E2ETestApplication)
};

//==============================================================================
// Application Entry Point
//==============================================================================

START_JUCE_APPLICATION(E2ETestApplication)