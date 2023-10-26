/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_SERIAL_MODULE_COMMANDS_ZCODESERIALSENDCOMMAND_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_SERIAL_MODULE_COMMANDS_ZCODESERIALSENDCOMMAND_HPP_

#include <zcode/modules/ZcodeCommand.hpp>
#include <arm-no-os/serial-module/lowlevel/UartManager.hpp>
#include <arm-no-os/serial-module/lowlevel/Serial.hpp>

#define COMMAND_EXISTS_0072 EXISTENCE_MARKER_UTIL

template<class ZP>
class ZcodeSerialSendCommand: public ZcodeCommand<ZP> {
    typedef typename ZP::LL LL;
    typedef typename LL::HW HW;

public:
    static constexpr uint8_t CODE = 0x02;

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        uint16_t port = 0;

        if (!slot.getFields()->get('P', &port)) {
            slot.fail(BAD_PARAM, "Port number missing");
            return;
        }
        if (port >= HW::serialCount) {
            slot.fail(BAD_PARAM, "Port number invalid");
            return;
        }
        if (UartManager<LL>::getUartById(port)->write(slot.getBigField()->getData(), slot.getBigField()->getLength())) {
            UartManager<LL>::getUartById(port)->transmitWriteBuffer();
            out->writeStatus(OK);
        } else {
            slot.fail(CMD_FAIL, "UART busy or message too long");
        }
    }

};

#endif /* SRC_MAIN_CPP_ARM_NO_OS_SERIAL_MODULE_COMMANDS_ZCODESERIALSENDCOMMAND_HPP_ */
