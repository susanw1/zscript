/*
 * ZcodeCapabilitiesCommand.hpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEINTERRUPTSETUPCOMMAND_HPP_
#define SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEINTERRUPTSETUPCOMMAND_HPP_

#include "../../ZcodeIncludes.hpp"
#include "../ZcodeCommand.hpp"

template<class ZP>
class ZcodeOutStream;
template<class ZP>
class ZcodeScriptSpace;

template<class ZP>
class Zcode;

template<class ZP>
class ZcodeInterruptSetupCommand: public ZcodeCommand<ZP> {
private:
    static const uint8_t CODE = 0x00;

public:

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
//        ZcodeScriptSpace<ZP> *space = slot.getZcode()->getSpace();

        out->writeField8('I', ZP::interruptVectorNum);
        out->writeStatus(OK);

    }

};

#endif /* SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEINTERRUPTSETUPCOMMAND_HPP_ */
