/*
 * RCodeCommandFinder.hpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_PARSING_RCODECOMMANDFINDER_HPP_
#define SRC_TEST_CPP_RCODE_PARSING_RCODECOMMANDFINDER_HPP_

#include "../RCodeIncludes.hpp"
#include "../RCodeDebugOutput.hpp"

template <class RP>
class RCode;

template <class RP>
class RCodeCommand;

template <class RP>
class RCodeCommandSlot;

template<class RP>
struct RCodeSupportedCommandArray {
    typedef typename RP::commandNum_t commandNum_t;

    uint8_t cmds[RP::commandCapabilitiesReturnArrayMaxSize];
    commandNum_t cmdNum;
};

template<class RP>
class RCodeCommandFinder {
    typedef typename RP::commandNum_t commandNum_t;
private:
    RCode<RP> *rcode;
    RCodeCommand<RP> *commands[RP::commandNum];
    commandNum_t commandNum = 0;

public:
    RCodeCommandFinder(RCode<RP> *rcode) :
            rcode(rcode) {
    }
    RCodeCommandFinder<RP>* registerCommand(RCodeCommand<RP> *cmd);

    RCodeCommand<RP>* findCommand(RCodeCommandSlot<RP> *slot);

    RCodeSupportedCommandArray<RP> getSupportedCommands();
};

template<class RP>
RCodeCommandFinder<RP>* RCodeCommandFinder<RP>::registerCommand(RCodeCommand<RP> *cmd) {
    if (commandNum < RP::commandNum) {
        commands[commandNum++] = cmd;
    } else {
        rcode->configFail("Too many commands registered.");
    }
    return this;
}

template<class RP>
RCodeCommand<RP>* RCodeCommandFinder<RP>::findCommand(RCodeCommandSlot<RP> *slot) {
    int fieldSectionNum = slot->getFields()->countFieldSections('R');
    if (fieldSectionNum == 0) {
        rcode->getDebug() << "No R field in command\n";
        return NULL;
    } else if (fieldSectionNum == 1) {
        uint8_t rVal = slot->getFields()->get('R', 0xFF);
        for (int i = 0; i < commandNum; i++) {
            if (commands[i]->getCode() == rVal && commands[i]->getCodeLength() == 1) {
                return commands[i];
            }
        }
        rcode->getDebug() << "Unknown command: R" << RCodeDebugOutputMode::hex << rVal << "\n";
        return NULL;
    } else if (RP::hasMultiByteCommands) {
        RCodeFieldMap<RP> *map = slot->getFields();
        uint8_t code[fieldSectionNum];
        for (int i = 0; i < fieldSectionNum; i++) {
            code[i] = map->get('R', i, 0xFF);
        }
        for (int i = 0; i < commandNum; i++) {
            if (commands[i]->getCodeLength() == fieldSectionNum && commands[i]->matchesCode(code, fieldSectionNum)) {
                return commands[i];
            }
        }
        rcode->getDebug() << "Unknown command: R" << RCodeDebugOutputMode::hex;
        for (int i = 0; i < fieldSectionNum; i++) {
            rcode->getDebug() << code[i];
        }
        rcode->getDebug() << "\n";
        return NULL;
    } else {
        rcode->getDebug() << "Multi-byte commands disabled\n";
        return NULL;
    }
}

template<class RP>
RCodeSupportedCommandArray<RP> RCodeCommandFinder<RP>::getSupportedCommands() {
    int highestCodeLength = 0;
    int totalLength = 0;
    for (int i = 0; i < commandNum; i++) {
        if (commands[i]->getCodeLength() > highestCodeLength) {
            highestCodeLength = commands[i]->getCodeLength();
        }
        totalLength += commands[i]->getCodeLength();
    }
    for (int l = 1; l <= highestCodeLength; l++) {
        for (int i = 0; i < commandNum; i++) {
            if (commands[i]->getCodeLength() == l) {
                totalLength += l;
                break;
            }
        }
    }
    uint8_t cmds[totalLength];
    commandNum_t pos = 0;
    for (int l = 1; l <= highestCodeLength; l++) {
        bool hasInitialisedLength = l == 1;
        for (int i = 0; i < commandNum; i++) {
            if (commands[i]->getCodeLength() == l) {
                if (!hasInitialisedLength) {
                    for (int j = 0; j < l; j++) {
                        cmds[pos++] = 0;
                    }
                    hasInitialisedLength = true;
                }
                uint8_t const *fullCode = commands[i]->getFullCode();
                for (int j = 0; j < l; j++) {
                    cmds[pos++] = fullCode[j];
                }
            }
        }
    }
    RCodeSupportedCommandArray<RP> result;
    result.cmdNum = pos;
    for (int i = 0; i < pos; ++i) {
        result.cmds[i] = cmds[i];
    }
    return result;
}


#include "../commands/RCodeCommand.hpp"
#include "RCodeCommandSlot.hpp"
#include "../RCode.hpp"

#endif /* SRC_TEST_CPP_RCODE_PARSING_RCODECOMMANDFINDER_HPP_ */
