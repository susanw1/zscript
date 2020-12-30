/*
 * RCodeCommandFinder.hpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_PARSING_RCODECOMMANDFINDER_HPP_
#define SRC_TEST_CPP_RCODE_PARSING_RCODECOMMANDFINDER_HPP_
#include "../RCodeIncludes.hpp"
#include "RCodeParameters.hpp"

class RCode;
class RCodeCommand;
class RCodeCommandSlot;

struct RCodeSupportedCommandArray {
    uint8_t cmds[RCodeParameters::commandCapabilitiesReturnArrayMaxSize];
    commandNum_t cmdNum;
};

class RCodeCommandFinder {
private:
    RCode *rcode;
    RCodeCommand *commands[RCodeParameters::commandNum];
    commandNum_t commandNum = 0;

public:
    RCodeCommandFinder(RCode *rcode) :
            rcode(rcode) {
    }
    RCodeCommandFinder* registerCommand(RCodeCommand *cmd);

    RCodeCommand* findCommand(RCodeCommandSlot *slot);

    RCodeSupportedCommandArray getSupportedCommands();
};

#include "../commands/RCodeCommand.hpp"
#include "RCodeCommandSlot.hpp"
#include "../RCode.hpp"

#endif /* SRC_TEST_CPP_RCODE_PARSING_RCODECOMMANDFINDER_HPP_ */
