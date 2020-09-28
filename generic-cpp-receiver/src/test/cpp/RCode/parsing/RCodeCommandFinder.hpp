/*
 * RCodeCommandFinder.hpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_PARSING_RCODECOMMANDFINDER_HPP_
#define SRC_TEST_CPP_RCODE_PARSING_RCODECOMMANDFINDER_HPP_
#include "../RCodeIncludes.hpp"
#include "../RCodeParameters.hpp"

class RCodeCommand;
class RCodeCommandSlot;

struct RCodeSupportedCommandArray {
    uint8_t cmds[RCodeParameters::commandCapabilitiesReturnArrayMaxSize];
    uint16_t cmdNum;
};

class RCodeCommandFinder {
private:
    RCodeCommand *commands[RCodeParameters::commandNum];
    int commandNum = 0;

public:

    bool registerCommand(RCodeCommand *cmd) {
        if (commandNum < RCodeParameters::commandNum) {
            commands[commandNum++] = cmd;
            return true;
        }
        return false;
    }

    RCodeCommand* findCommand(RCodeCommandSlot *slot);

    RCodeSupportedCommandArray getSupportedCommands();
};

#include "../commands/RCodeCommand.hpp"
#include "RCodeCommandSlot.hpp"

#endif /* SRC_TEST_CPP_RCODE_PARSING_RCODECOMMANDFINDER_HPP_ */
