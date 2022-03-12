/*
 * ZcodeCommandFinder.hpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_PARSING_ZCODECOMMANDFINDER_HPP_
#define SRC_TEST_CPP_ZCODE_PARSING_ZCODECOMMANDFINDER_HPP_

#include "../ZcodeIncludes.hpp"
#include "../ZcodeDebugOutput.hpp"

template <class RP>
class Zcode;

template <class RP>
class ZcodeCommand;

template <class RP>
class ZcodeCommandSlot;

template<class RP>
struct ZcodeSupportedCommandArray {
    typedef typename RP::commandNum_t commandNum_t;

    uint8_t cmds[RP::commandCapabilitiesReturnArrayMaxSize];
    commandNum_t cmdNum;
};

template<class RP>
class ZcodeCommandFinder {
    typedef typename RP::commandNum_t commandNum_t;
private:
    Zcode<RP> *zcode;
    ZcodeCommand<RP> *commands[RP::commandNum];
    commandNum_t commandNum = 0;

public:
    ZcodeCommandFinder(Zcode<RP> *zcode) :
            zcode(zcode) {
    }
    ZcodeCommandFinder<RP>* registerCommand(ZcodeCommand<RP> *cmd);

    ZcodeCommand<RP>* findCommand(ZcodeCommandSlot<RP> *slot);

    ZcodeSupportedCommandArray<RP> getSupportedCommands();
};

template<class RP>
ZcodeCommandFinder<RP>* ZcodeCommandFinder<RP>::registerCommand(ZcodeCommand<RP> *cmd) {
    if (commandNum < RP::commandNum) {
        commands[commandNum++] = cmd;
    } else {
        zcode->configFail("Too many commands registered.");
    }
    return this;
}

template<class RP>
ZcodeCommand<RP>* ZcodeCommandFinder<RP>::findCommand(ZcodeCommandSlot<RP> *slot) {
    int fieldSectionNum = slot->getFields()->countFieldSections('R');
    if (fieldSectionNum == 0) {
        zcode->getDebug() << "No R field in command\n";
        return NULL;
    } else if (fieldSectionNum == 1) {
        uint8_t rVal = slot->getFields()->get('R', 0xFF);
        for (int i = 0; i < commandNum; i++) {
            if (commands[i]->getCode() == rVal && commands[i]->getCodeLength() == 1) {
                return commands[i];
            }
        }
        zcode->getDebug() << "Unknown command: R" << ZcodeDebugOutputMode::hex << rVal << "\n";
        return NULL;
    } else if (RP::hasMultiByteCommands) {
        ZcodeFieldMap<RP> *map = slot->getFields();
        uint8_t code[fieldSectionNum];
        for (int i = 0; i < fieldSectionNum; i++) {
            code[i] = map->get('R', i, 0xFF);
        }
        for (int i = 0; i < commandNum; i++) {
            if (commands[i]->getCodeLength() == fieldSectionNum && commands[i]->matchesCode(code, fieldSectionNum)) {
                return commands[i];
            }
        }
        zcode->getDebug() << "Unknown command: R" << ZcodeDebugOutputMode::hex;
        for (int i = 0; i < fieldSectionNum; i++) {
            zcode->getDebug() << code[i];
        }
        zcode->getDebug() << "\n";
        return NULL;
    } else {
        zcode->getDebug() << "Multi-byte commands disabled\n";
        return NULL;
    }
}

template<class RP>
ZcodeSupportedCommandArray<RP> ZcodeCommandFinder<RP>::getSupportedCommands() {
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
    ZcodeSupportedCommandArray<RP> result;
    result.cmdNum = pos;
    for (int i = 0; i < pos; ++i) {
        result.cmds[i] = cmds[i];
    }
    return result;
}


#include "../commands/ZcodeCommand.hpp"
#include "ZcodeCommandSlot.hpp"
#include "../Zcode.hpp"

#endif /* SRC_TEST_CPP_ZCODE_PARSING_ZCODECOMMANDFINDER_HPP_ */
