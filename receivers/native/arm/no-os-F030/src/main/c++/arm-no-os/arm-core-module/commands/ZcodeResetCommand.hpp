/*
 * ZcodeResetCommand.hpp
 *
 *  Created on: 18 Jul 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___MODULES_CORE_ZCODERESETCOMMAND_HPP_
#define SRC_MAIN_C___MODULES_CORE_ZCODERESETCOMMAND_HPP_

#include <zcode/modules/ZcodeCommand.hpp>

#define COMMAND_EXISTS_0003 EXISTENCE_MARKER_UTIL

extern "C" {
void Reset_Handler();
}

template<class ZP>
class ZcodeResetCommand: public ZcodeCommand<ZP> {
    typedef typename ZP::LL LL;

public:
    static constexpr uint8_t CODE = 0x03;

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        uint16_t target = 0;
        if (slot.getFields()->get('C', &target)) {
            if (target >= Zcode<ZP>::zcode.getChannelCount()) {
                slot.fail(BAD_PARAM, "Channel index too large");
                return;
            }
            if (!Zcode<ZP>::zcode.getChannel(target)->reset()) {
                slot.fail(CMD_FAIL, "Channel reset failed");
                return;
            }
        } else {
            NVIC_SystemReset();
        }
        out->writeStatus(OK);
    }

};

#define ZCODE_CORE_RESET_COMMAND ZcodeResetCommand<ZP>::execute

#endif /* SRC_MAIN_C___MODULES_CORE_ZCODERESETCOMMAND_HPP_ */
