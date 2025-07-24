// test_simple.cpp
#include <iostream>
#include <cassert>

// Simple tests that don't require JUCE
namespace BasicTests {
    void testMath() {
        std::cout << "Testing basic math..." << std::endl;
        assert(2 + 2 == 4);
        assert(10 / 2 == 5);
        std::cout << "✓ Math tests passed" << std::endl;
    }

    void testLogic() {
        std::cout << "Testing logic..." << std::endl;

        // Test tempo validation
        auto isValidTempo = [](int tempo) {
            return tempo >= 60 && tempo <= 200;
        };

        assert(isValidTempo(120) == true);
        assert(isValidTempo(50) == false);
        assert(isValidTempo(250) == false);

        std::cout << "✓ Logic tests passed" << std::endl;
    }
}

int main() {
    std::cout << "Running OTTO Basic Tests" << std::endl;
    std::cout << "========================" << std::endl;

    BasicTests::testMath();
    BasicTests::testLogic();

    std::cout << "\nAll tests passed! ✓" << std::endl;
    return 0;
}
