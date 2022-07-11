/*
 * ZcodeMakeCodeCommand.hpp
 *
 *  Created on: 11 Jul 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___ZCODE_MODULES_CORE_ZCODEMAKECODECOMMAND_HPP_
#define SRC_MAIN_C___ZCODE_MODULES_CORE_ZCODEMAKECODECOMMAND_HPP_

#include "../../ZcodeIncludes.hpp"
#include "../ZcodeCommand.hpp"

template<class ZP>
class ZcodeOutStream;

template<class ZP>
class ZcodeMakeCodeCommand: public ZcodeCommand<ZP> {
private:
    static const uint8_t CODE = 0x0c;

public:
    static uint16_t number;

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        number = ZP::numberGenerator();
        out->writeField16('C', number);
        out->writeStatus(OK);
    }
};
template<class ZP>
uint16_t ZcodeMakeCodeCommand<ZP>::number = 0;

#endif /* SRC_MAIN_C___ZCODE_MODULES_CORE_ZCODEMAKECODECOMMAND_HPP_ */
