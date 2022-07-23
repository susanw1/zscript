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
    if (!ZcodeTestingSystem::tryTest("Z2&Z18N&Z11\"Hello\"", "AS&S&\n")) {
        std::cerr << "Failed on debug with no target\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z2&Z18DN&Z11\"Hello\"", "AS&S&\n#Hello\n")) {
        std::cerr << "Failed on debug with target set\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z2&Z18DN&Z11\"Hello1\"&Z11\"Hello2\"&Z11\"Hello3\"", "AS&S&&&\n#Hello1\n#Hello2\n#Hello3\n")) {
        std::cerr << "Failed on debug with target set\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z2&Z18DN\n@Hello\n@Goodbye\n", "AS&S\n#Hello\n#Goodbye\n")) {
        std::cerr << "Failed on addressing\n";
        return 1;
    }
}
