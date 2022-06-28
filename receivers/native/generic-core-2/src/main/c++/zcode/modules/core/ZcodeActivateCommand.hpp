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

template<class ZP>
class ZcodeOutStream;

template<class ZP>
class ZcodeActivateCommand: public ZcodeCommand<ZP> {
private:
    static bool activated;
    static const uint8_t CODE = 0x03;

public:
    static const uint16_t MAX_SYSTEM_CODE = 15;

    static bool isActivated() {
        return activated;
    }

    static void reset() {
        activated = false;
    }

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        if (!activated) {
            out->writeField8('A', (uint8_t) 0);
            out->writeStatus(OK);
        } else {
            out->writeField8('A', (uint8_t) 1);
            out->writeStatus(OK);
        }
        activated = true;
    }

};

template<class ZP>
bool ZcodeActivateCommand<ZP>::activated = false;

#endif /* SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEACTIVATECOMMAND_HPP_ */
