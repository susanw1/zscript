/*
 * ZcodeActivateCommand.hpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEACTIVATECOMMAND_HPP_
#define SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEACTIVATECOMMAND_HPP_

#include "../../ZcodeIncludes.hpp"
#include "../ZcodeCommand.hpp"

#define COMMAND_VALUE_0002 MODULE_CAPABILITIES_UTIL

template<class ZP>
class ZcodeOutStream;

template<class ZP>
class ZcodeActivateCommand: public ZcodeCommand<ZP> {
private:
    static const uint8_t CODE = 0x02;

public:

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        if (!slot.getZcode()->isActivated()) {
            out->writeField8('A', (uint8_t) 0);
            out->writeStatus(OK);
        } else {
            out->writeField8('A', (uint8_t) 1);
            out->writeStatus(OK);
        }
        slot.getZcode()->setActivated();
    }

};

#endif /* SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEACTIVATECOMMAND_HPP_ */
