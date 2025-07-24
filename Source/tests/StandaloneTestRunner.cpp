#include <JuceHeader.h>
#include "../tests/TestRunnerPlugin.h"

int main (int argc, char* argv[])
{
    juce::ScopedJuceInitialiser_GUI scopedJuce;

    std::cout << "Running OTTO Tests..." << std::endl;
    TestRunnerPlugin::runAllTests();

    return 0;
}
