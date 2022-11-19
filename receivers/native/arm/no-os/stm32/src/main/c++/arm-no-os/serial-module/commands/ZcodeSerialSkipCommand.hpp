/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_SERIAL_MODULE_COMMANDS_ZCODESERIALSKIPCOMMAND_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_SERIAL_MODULE_COMMANDS_ZCODESERIALSKIPCOMMAND_HPP_

#include <zcode/modules/ZcodeCommand.hpp>
#include <arm-no-os/serial-module/lowlevel/UartManager.hpp>
#include <arm-no-os/serial-module/lowlevel/Serial.hpp>

#define COMMAND_EXISTS_0074 EXISTENCE_MARKER_UTIL

template<class ZP>
class ZcodeSerialSkipCommand: public ZcodeCommand<ZP> {
    typedef typename ZP::LL LL;
    typedef typename LL::HW HW;

public:
    static constexpr uint8_t CODE = 0x04;

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        uint16_t port = 0;
        uint16_t length = 0;

        if (!slot.getFields()->get('P', &port)) {
            slot.fail(BAD_PARAM, "Port number missing");
            return;
        }
        if (UartManager<LL>::isMasked(port)) {
            slot.fail(BAD_PARAM, "Port not available");
            return;
        }

        if (port >= HW::serialCount) {
            slot.fail(BAD_PARAM, "Port number invalid");
            return;
        }

        if (!slot.getFields()->get('L', &length)) {
            slot.fail(BAD_PARAM, "Port number missing");
            return;
        }

        SerialError error = UartManager<LL>::getUartById(port)->getError(length);
        bool failOnDataOut = slot.getFields()->has('F');

        uint16_t lengthSkipped = UartManager<LL>::getUartById(port)->skip(length);
        out->writeField16('L', lengthSkipped);
        if (error != SerialNoError) {
            out->writeField8('F', (uint8_t) error);
        }
        if (lengthSkipped != length && failOnDataOut) {
            slot.fail(CMD_FAIL, "Not enough data available");
        } else if (error != SerialNoError) {
            slot.fail(CMD_FAIL, NULL);
        } else {
            out->writeStatus(OK);
        }
    }

};

#endif /* SRC_MAIN_CPP_ARM_NO_OS_SERIAL_MODULE_COMMANDS_ZCODESERIALSKIPCOMMAND_HPP_ */
