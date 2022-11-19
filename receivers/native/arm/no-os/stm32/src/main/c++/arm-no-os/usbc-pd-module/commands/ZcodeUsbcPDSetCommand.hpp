/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_USBC_PD_MODULE_COMMANDS_ZCODEUSBCPDSETCOMMAND_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_USBC_PD_MODULE_COMMANDS_ZCODEUSBCPDSETCOMMAND_HPP_

#include <zcode/modules/ZcodeCommand.hpp>
#include <arm-no-os/usbc-pd-module/lowlevel/Ucpd.hpp>

#define COMMAND_EXISTS_0112 EXISTENCE_MARKER_UTIL

template<class ZP>
class ZcodeUsbcPDSetCommand: public ZcodeCommand<ZP> {
public:
    static constexpr uint8_t CODE = 0x02;

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();

        uint16_t current = Ucpd<typename ZP::LL>::getMaxCurrent();
        slot.getFields()->get('I', &current);
        uint16_t minVoltage = Ucpd<typename ZP::LL>::getMinVoltage();
        uint16_t maxVoltage = Ucpd<typename ZP::LL>::getMaxVoltage();
        if (slot.getFields()->get('V', &minVoltage)) {
            minVoltage *= 20;
            maxVoltage = minVoltage;
        }
        if (slot.getFields()->get('A', &minVoltage)) {
            maxVoltage = minVoltage;
        }
        slot.getFields()->get('A', &maxVoltage);

        if (minVoltage == Ucpd<typename ZP::LL>::getMinVoltage() && maxVoltage == Ucpd<typename ZP::LL>::getMaxVoltage()) {
            if (current != Ucpd<typename ZP::LL>::getMaxCurrent()) {
                Ucpd<typename ZP::LL>::renegotiateCurrent(current);
            }
        } else if (current == Ucpd<typename ZP::LL>::getMaxCurrent()) {
            Ucpd<typename ZP::LL>::renegotiateVoltage(minVoltage, maxVoltage);
        } else {
            Ucpd<typename ZP::LL>::renegotiateAll(current, minVoltage, maxVoltage);
        }
        out->writeStatus(OK);
    }

};

#endif /* SRC_MAIN_CPP_ARM_NO_OS_USBC_PD_MODULE_COMMANDS_ZCODEUSBCPDSETCOMMAND_HPP_ */
