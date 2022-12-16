/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_USBC_PD_MODULE_COMMANDS_ZCODEUSBCPDSTATUSCOMMAND_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_USBC_PD_MODULE_COMMANDS_ZCODEUSBCPDSTATUSCOMMAND_HPP_

#include <zcode/modules/ZcodeCommand.hpp>
#include <usbc-pd-ll/lowlevel/Ucpd.hpp>

#define COMMAND_EXISTS_0111 EXISTENCE_MARKER_UTIL

template<class ZP>
class ZcodeUsbcPDStatusCommand: public ZcodeCommand<ZP> {
public:
    static constexpr uint8_t CODE = 0x01;

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        out->writeField16('I', Ucpd<typename ZP::LL>::getMaxCurrent());
        out->writeField8('V', Ucpd<typename ZP::LL>::getMinVoltage() / 20);

        out->writeField16('A', Ucpd<typename ZP::LL>::getMinVoltage());
        out->writeField16('M', Ucpd<typename ZP::LL>::getMaxVoltage());

        out->writeField16('P', Ucpd<typename ZP::LL>::getPowerStatus());
        out->writeStatus(OK);
    }

};

#endif /* SRC_MAIN_CPP_ARM_NO_OS_USBC_PD_MODULE_COMMANDS_ZCODEUSBCPDSTATUSCOMMAND_HPP_ */
