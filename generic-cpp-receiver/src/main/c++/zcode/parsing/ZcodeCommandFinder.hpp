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

template<class ZP>
class Zcode;

template<class ZP>
class ZcodeCommand;

template<class ZP>
class ZcodeCommandSlot;

template<class ZP>
struct ZcodeSupportedCommandArray {
        typedef typename ZP::commandNum_t commandNum_t;

        uint8_t cmds[ZP::commandCapabilitiesReturnArrayMaxSize];
        commandNum_t cmdNum;
};

template<class ZP>
class ZcodeCommandFinder {
    private:
        typedef typename ZP::commandNum_t commandNum_t;

        Zcode<ZP> *zcode;
        ZcodeCommand<ZP> *commands[ZP::commandNum];
        commandNum_t commandNum = 0;

    public:
        ZcodeCommandFinder(Zcode<ZP> *zcode) :
                zcode(zcode) {
        }
        ZcodeCommandFinder<ZP>* registerCommand(ZcodeCommand<ZP> *cmd);

        ZcodeCommand<ZP>* findCommand(ZcodeCommandSlot<ZP> *slot);

        ZcodeSupportedCommandArray<ZP> getSupportedCommands();
};

template<class ZP>
ZcodeCommandFinder<ZP>* ZcodeCommandFinder<ZP>::registerCommand(ZcodeCommand<ZP> *cmd) {
    if (commandNum < ZP::commandNum) {
        commands[commandNum++] = cmd;
    } else {
        zcode->configFail("Too many commands registered.");
    }
    return this;
}

template<class ZP>
ZcodeCommand<ZP>* ZcodeCommandFinder<ZP>::findCommand(ZcodeCommandSlot<ZP> *slot) {
    int fieldSectionNum = slot->getFields()->countFieldSections(Zchars::CMD_PARAM);
    if (fieldSectionNum == 0) {
        zcode->getDebug() << "No Z field in command\n";
        return NULL;
    } else if (fieldSectionNum == 1) {
        uint8_t rVal = slot->getFields()->get(Zchars::CMD_PARAM, 0xFF);
        for (int i = 0; i < commandNum; i++) {
            if (commands[i]->getCode() == rVal && commands[i]->getCodeLength() == 1) {
                return commands[i];
            }
        }
        zcode->getDebug() << "Unknown command: Z" << ZcodeDebugOutputMode::hex << rVal << "\n";
        return NULL;
    } else if (ZP::hasMultiByteCommands) {
        ZcodeFieldMap<ZP> *map = slot->getFields();
        uint8_t code[fieldSectionNum];
        for (int i = 0; i < fieldSectionNum; i++) {
            code[i] = map->get(Zchars::CMD_PARAM, i, 0xFF);
        }
        for (int i = 0; i < commandNum; i++) {
            if (commands[i]->getCodeLength() == fieldSectionNum && commands[i]->matchesCode(code, fieldSectionNum)) {
                return commands[i];
            }
        }
        zcode->getDebug() << "Unknown command: Z" << ZcodeDebugOutputMode::hex;
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

template<class ZP>
ZcodeSupportedCommandArray<ZP> ZcodeCommandFinder<ZP>::getSupportedCommands() {
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
    ZcodeSupportedCommandArray<ZP> result;
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
