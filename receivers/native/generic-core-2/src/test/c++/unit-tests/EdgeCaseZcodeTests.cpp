/*
 *
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#include "ZcodeTestingSystem.hpp"

int main(void) {
    std::cout << "Running edge case tests\n";
    if (!ZcodeTestingSystem::tryTest("Ze", "S4\"Unknown command\"\n")) {
        std::cerr << "Test failed for Unknown command\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Zaf2a", "S7\"System not activated\"\n")) {
        std::cerr << "Test failed for not activated\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z2&Zaf2a", "AS&S4\"Unknown command\"\n")) {
        std::cerr << "Test failed for Unknown command with activation\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("A3", "S4\"No Z field\"\n")) {
        std::cerr << "Test failed for no Z code\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z24567", "S8\"Field too long\"\n")) {
        std::cerr << "Test failed for field too long\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z1+2", "S3\"Big field odd length\"\n")) {
        std::cerr << "Test failed for odd big field\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z1\"=A\"", "S3\"String escaping error\"\n")) {
        std::cerr << "Test failed for invalid string escaping (wrong char)\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z1\"=\"", "S3\"Misplaced string field end\"\n")) {
        std::cerr << "Test failed for invalid string escaping (end after =)\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z1\"=2\"", "S3\"Misplaced string field end\"\n")) {
        std::cerr << "Test failed for invalid string escaping (end after =2)\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z1A1\"", "S3\"String field not ended\"\n")) {
        std::cerr << "Test failed for string not finished\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z1;", "S3\"Unknown character\"\n")) {
        std::cerr << "Test failed for invalid character\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("%100000000000000000000000000000000000000000000000000000000000Z1", "S8\"Too many locks\"\n")) {
        std::cerr << "Test failed for too many locks\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("**", "*S3\"Broadcast set twice\"\n")) {
        std::cerr << "Test failed for double broadcast\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("%%", "S3\"Locks set twice\"\n")) {
        std::cerr << "Test failed for double parallel\n";
        return 1;
    }
}
