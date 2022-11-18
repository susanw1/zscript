/*
 *
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#include "ZcodeTestingSystem.hpp"

int main(void) {
    std::cout << "=========================\nRunning basic tests\n=========================" << std::endl;

    /*
     * Basic line structure and whitespace
     */
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

    /*
     * Simple Echo, and tests on param/big-field structure
     */
    if (!ZcodeTestingSystem::tryTest("Z1", "S\n")) {
        std::cerr << "Failed on simple Echo\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z0001", "S\n")) {
        std::cerr << "Failed on Echo with extra 0s\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest(" Z1", "S\n")) {
        std::cerr << "Failed on Echo with extra space at start\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("  Z 1  ", "S\n")) {
        std::cerr << "Failed on Echo with extra spaces all over\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z1 A", "SA\n")) {
        std::cerr << "Failed on Echo with data\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z1 A B6 Cf45 Deacb +1234", "SAB6Cf45Deacb+1234\n")) {
        std::cerr << "Failed on Echo with data\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z1\"Hi\"", "S+4869\n")) {
        std::cerr << "Failed on string\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z1\"A & B\"", "S+4120262042\n")) {
        std::cerr << "Failed on string containing '&' char\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z1\"A | B\"", "S+41207c2042\n")) {
        std::cerr << "Failed on string containing '|' char\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z1 \"A\"\"B\"", "S+4142\n")) {
        std::cerr << "Failed on 2 abutting strings\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z1 \"A\" X6 \"B\"", "SX6+4142\n")) {
        std::cerr << "Failed on 2 separated strings\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z1 \"A\" X6 +43", "SX6+4143\n")) {
        std::cerr << "Failed on string and binary big field\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z1 \"A\" X6++\"\" +43\"E\"", "SX6+414345\n")) {
        std::cerr << "Failed on multiple string and binary big fields\n";
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
    if (!ZcodeTestingSystem::tryTest("Z1E3", "E3S\n")) {
        std::cerr << "Failed on 'E' echo param in Echo command\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z1A2E3B3", "E3SA2B3\n")) {
        std::cerr << "Failed on 'E' echo param in Echo command\n";
        return 1;
    }

    /*
     * Conditional & and | tests
     */
    if (!ZcodeTestingSystem::tryTest("Z1&Z1", "S&S\n")) {
        std::cerr << "Failed on Consecutive Echo\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z1&Z1\nZ1&Z1", "S&S\nS&S\n")) {
        std::cerr << "Failed on Consecutive Echo With \\n\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z1 & Z1 &Z1 | Z1 \n Z1 & Z1", "S&S&S\nS&S\n")) {
        std::cerr << "Failed on Consecutive Echo with |\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z1A & Z1B & Z1C | Z1D \n Z1G & Z1H", "SA&SB&SC\nSG&SH\n")) {
        std::cerr << "Failed on Consecutive Echo with | and params\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z1A & Z1BS10 & Z1C | Z1D \n Z1F & Z1G", "SA&S10B|SD\nSF&SG\n")) {
        std::cerr << "Failed on Consecutive Echo with error\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z1 & Z1S10 & Z1 | Z1S10 |Z1\nZ1 & Z1S1 & Z1 | Z1S10 | Z1", "S&S10|S10|S\nS&S1\n")) {
        std::cerr << "Failed on Consecutive Echo with errors\n";
        return 1;
    }

    /*
     * Z0 (capabilities) and Z2 (activation) commands
     */

#ifdef ZCODE_SUPPORT_SCRIPT_SPACE
    const char *capResp = "C3107M7\"My Test System!\"V1S\n";
#else
    const char *capResp = "C3107M3\"My Test System!\"V1S\n";
#endif
    if (!ZcodeTestingSystem::tryTest("Z0", capResp)) {
        std::cerr << "Failed on Capabilities\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z0000", capResp)) {
        std::cerr << "Failed on Capabilities with all zeroes\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z", capResp)) {
        std::cerr << "Failed on Capabilities with no 0s\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z2", "AS\n")) {
        std::cerr << "Failed on Activation\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("Z2 & Z2", "AS&A1S\n")) {
        std::cerr << "Failed on Activation twice\n";
        return 1;
    }

    if (!ZcodeTestingSystem::tryTest("Z2 E3", "E3AS\n")) {
        std::cerr << "Failed on 'E' echo param in Activation command\n";
        return 1;
    }

    /*
     * More 'E' param tests, with ORs and ANDs
     */
    if (!ZcodeTestingSystem::tryTest("EZ1", "ES\n")) {
        std::cerr << "Failed on simple echoed 'E' field\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("E1Z1 & E2Z1", "E1S&E2S\n")) {
        std::cerr << "Failed on &-sequenced echoed 'E' params\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("E1Z1 & E2Z1", "E1S&E2S\n")) {
        std::cerr << "Failed on &-sequenced echoed 'E' params\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("E1Z1 & E2Z1S10 | E3Z1 & E4Z1", "E1S&E2S10|E3S&E4S\n")) {
        std::cerr << "Failed on &- and |-sequenced echoed 'E' params\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("E1Z1 & E2Z1S10 & E3Z1S4 | E5Z1 & E6Z1", "E1S&E2S10|E5S&E6S\n")) {
        std::cerr << "Failed on &- and |-sequenced echoed 'E' params, with (non-system-error) skipped command\n";
        return 1;
    }
    if (!ZcodeTestingSystem::tryTest("E1Z1 & E2Z1S4 & E3Z1S4 | E5Z1 & E6Z1", "E1S&E2S4\n")) {
        std::cerr << "Failed on &- and |-sequenced echoed 'E' params, with (system-error) skipped command\n";
        return 1;
    }
}
