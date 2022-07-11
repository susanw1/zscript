/*
 *
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#include "ZcodeTestingSystem.hpp"

int main(void) {
    std::cout << "Running debug tests\n";
    if (!ZcodeTestingSystem::tryTest("Z2&Z18DN", "AS&S\n")) {
        std::cerr << "Failed on Set Debug target\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z2&Z11\"0.0Hello\"", "AS&S9\"Addressing only allowed from notification channel\"\n")) {
        std::cerr << "Failed on debug with no notification\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z2&Z18N&Z11\"0.0Hello\"", "AS&S&S\n")) {
        std::cerr << "Failed on debug with no target\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z2&Z18DN&Z11\"0.0Hello\"", "AS&S&S\n#@Hello\n")) {
        std::cerr << "Failed on debug with target set\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z2&Z18DN&Z11\"0.0Hello1\"&Z11\"0.0Hello2\"&Z11\"0.0Hello3\"", "AS&S&S&S&S\n#@Hello1\n#@Hello2\n#@Hello3\n")) {
        std::cerr << "Failed on debug with target set\n";
        return 1;
    }
}
