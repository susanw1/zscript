/*
 *
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#include "ZcodeTestingSystem.hpp"

int main(void) {
    std::cout << "Running debug tests\n";
    if (!ZcodeTestingSystem::tryTest("Z9", "S\n")) {
        std::cerr << "Failed on Set Debug target\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Zf\"0.0Hello\"", "S\n")) {
        std::cerr << "Failed on debug with no target\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z9&Zf\"0.0Hello\"", "S&S\n#@Hello\n")) {
        std::cerr << "Failed on debug with target set\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z9&Zf\"0.0Hello1\"&Zf\"0.0Hello2\"&Zf\"0.0Hello3\"", "S&S&S&S\n#@Hello1\n#@Hello2\n#@Hello3\n")) {
        std::cerr << "Failed on debug with target set\n";
        return 1;
    }
}
