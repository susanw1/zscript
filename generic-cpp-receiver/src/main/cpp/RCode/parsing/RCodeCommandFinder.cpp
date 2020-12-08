/*
 * RCodeCommandFinder.cpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#include "RCodeCommandFinder.hpp"

RCodeCommandFinder* RCodeCommandFinder::registerCommand(RCodeCommand *cmd) {
    if (commandNum < RCodeParameters::commandNum) {
        commands[commandNum++] = cmd;
    } else {
        rcode->configFail("Too many commands registered.");
    }
    return this;
}
RCodeCommand* RCodeCommandFinder::findCommand(RCodeCommandSlot *slot) {
    int fieldSectionNum = slot->getFields()->countFieldSections('R');
    if (fieldSectionNum == 0) {
        rcode->getDebug() << "No R field in command\n";
        return NULL;
    } else if (fieldSectionNum == 1) {
        uint8_t rVal = slot->getFields()->get('R', 0xFF);
        for (int i = 0; i < commandNum; i++) {
            if (commands[i]->getCode() == rVal
                    && commands[i]->getCodeLength() == 1) {
                return commands[i];
            }
        }
        rcode->getDebug() << "Unknown command: R" << hex << rVal << "\n";
        return NULL;
    } else if (RCodeParameters::hasMultiByteCommands) {
        RCodeFieldMap *map = slot->getFields();
        uint8_t code[fieldSectionNum];
        for (int i = 0; i < fieldSectionNum; i++) {
            code[i] = map->get('R', i, 0xFF);
        }
        for (int i = 0; i < commandNum; i++) {
            if (commands[i]->getCodeLength() == fieldSectionNum
                    && commands[i]->matchesCode(code, fieldSectionNum)) {
                return commands[i];
            }
        }
        rcode->getDebug() << "Unknown command: R" << hex;
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

RCodeSupportedCommandArray RCodeCommandFinder::getSupportedCommands() {
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
    int pos = 0;
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
    RCodeSupportedCommandArray result;
    result.cmdNum = pos;
    for (int i = 0; i < pos; ++i) {
        result.cmds[i] = cmds[i];
    }
    return result;
}
