/*
 * ZcodeNotificationHostCommand.hpp
 *
 *  Created on: 30 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_COMMANDS_ZCODESCRIPTSTORECOMMAND_HPP_
#define SRC_TEST_CPP_ZCODE_COMMANDS_ZCODESCRIPTSTORECOMMAND_HPP_

#include "../../ZcodeIncludes.hpp"
#include "../ZcodeCommand.hpp"

template<class ZP>
class ZcodeOutStream;
template<class ZP>
class ZcodeScriptSpace;

template<class ZP>
class Zcode;

template<class ZP>
class ZcodeScriptStoreCommand: public ZcodeCommand<ZP> {
private:
    static const uint8_t CODE = 0x02;

public:

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        ZcodeScriptSpace<ZP> *space = slot.getZcode()->getSpace();

        uint16_t address = 0;
        bool valid = true;
        if (valid && slot.getFields()->get('A', &address)) {
            if (address >= space->getLength()) {
                valid = false;
                slot.fail(BAD_PARAM, "Target Address beyond end of Execution Space");
            }
        }
        if (valid && address + slot.getBigField()->getLength() < ZP::scriptLength) {
            space->write(slot.getBigField()->getData(), slot.getBigField()->getLength(), address, slot.getFields()->has('L'));
            out->writeStatus(OK);
        } else {
            slot.fail(BAD_PARAM, "Write goes off end of execution space");
        }
    }
};

#endif /* SRC_TEST_CPP_ZCODE_COMMANDS_ZCODESCRIPTSTORECOMMAND_HPP_ */
