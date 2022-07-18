/*
 *
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#include "ZcodeTestingSystem.hpp"

int main(void) {
    std::cout << "Running basic tests\n";
    if (!ZcodeTestingSystem::tryTest("\n", "\n")) {
        std::cerr << "Failed on empty string\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest(",,, , , , \t\t\t  \r, , , \r\n", "\n")) {
        std::cerr << "Failed on only whitespace\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("*", "*\n")) {
        std::cerr << "Failed on broadcast only\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("%", "\n")) {
        std::cerr << "Failed on parallel only\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("%*", "*\n")) {
        std::cerr << "Failed on parallel then broadcast only\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("*%", "*\n")) {
        std::cerr << "Failed on broadcast then parallel only\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z1", "S\n")) {
        std::cerr << "Failed on Echo\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z0001", "S\n")) {
        std::cerr << "Failed on Echo with extra 0s\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z1AB6Cf45Deacb+1234", "SAB6Cf45Deacb+1234\n")) {
        std::cerr << "Failed on Echo with data\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z1\"Hi\"", "S+4869\n")) {
        std::cerr << "Failed on string\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z1\"=23a=ff\"", "S+2361ff\n")) {
        std::cerr << "Failed on string with escaping\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z 1, A\t8\rB  6,,,,+, , , 1\t\r\r\t,,,, 2  3 4", "SA8B6+1234\n")) {
        std::cerr << "Failed on varied whitespace\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z1&Z1", "S&S\n")) {
        std::cerr << "Failed on Consecutive Echo\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z1&Z1\nZ1&Z1", "S&S\nS&S\n")) {
        std::cerr << "Failed on Consecutive Echo With \\n\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z1&Z1&Z1|Z1\nZ1&Z1", "S&S&S\nS&S\n")) {
        std::cerr << "Failed on Consecutive Echo with |\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z1&Z1S10&Z1|Z1\nZ1&Z1", "S&S10|S\nS&S\n")) {
        std::cerr << "Failed on Consecutive Echo with error\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z1&Z1S10&Z1|Z1S10|Z1\nZ1&Z1S1&Z1|Z1S10|Z1", "S&S10|S10|S\nS&S1\n")) {
        std::cerr << "Failed on Consecutive Echo with errors\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z0", "SC3107M7N14\n")) {
        std::cerr << "Failed on Capabilities\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z0000", "SC3107M7N14\n")) {
        std::cerr << "Failed on Capabilities\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z", "SC3107M7N14\n")) {
        std::cerr << "Failed on Capabilities with no 0s\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z2", "AS\n")) {
        std::cerr << "Failed on Activation\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z2&Z2", "AS&A1S\n")) {
        std::cerr << "Failed on Activation\n";
        return 1;
    }
}
