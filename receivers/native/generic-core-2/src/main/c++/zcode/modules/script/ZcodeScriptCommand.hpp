/*
 * ZcodeSendDebugCommand.hpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_COMMANDS_ZCODESCRIPTCOMMAND_HPP_
#define SRC_TEST_CPP_ZCODE_COMMANDS_ZCODESCRIPTCOMMAND_HPP_

#include "../../ZcodeIncludes.hpp"
#include "../ZcodeCommand.hpp"

template<class ZP>
class ZcodeOutStream;
template<class ZP>
class ZcodeScriptSpace;

template<class ZP>
class Zcode;

template<class ZP>
class ZcodeScriptCommand: public ZcodeCommand<ZP> {
private:
    static const uint8_t CODE = 0x01;

public:

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        ZcodeScriptSpace<ZP> *space = slot.getZcode()->getSpace();
        bool worked = true;
        uint16_t target = 0;
        uint16_t address = 0;
        uint16_t delay = 0;
        if (slot.getFields()->get('T', &target)) {
            if (target >= space->getChannelCount()) {
                worked = false;
                slot.fail(BAD_PARAM, ZP::Strings::failScriptUnknownChannel);
            }
        }

        if (worked && slot.getFields()->get('A', &address)) {
            if (address >= space->getLength()) {
                worked = false;
                slot.fail(BAD_PARAM, ZP::Strings::failScriptBadAddress);
            } else {
                space->getChannel((uint8_t) target)->move(address);
            }
        }
        if (worked && slot.getFields()->get('D', &delay)) {
            if (delay > 0xFF) {
                worked = false;
                slot.fail(BAD_PARAM, ZP::Strings::failScriptBadDelay);
            } else {
                space->setDelay((uint8_t) delay);
            }
        }
        if (worked && slot.getFields()->has('G')) {
            space->setRunning(true);
        } else {
            space->setRunning(false);
        }
        space->setFailed(false);
        if (worked) {
            out->writeStatus(OK);
        }
    }
};

#endif /* SRC_TEST_CPP_ZCODE_COMMANDS_ZCODESCRIPTCOMMAND_HPP_ */
