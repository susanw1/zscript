/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_SERIAL_MODULE_COMMANDS_ZCODESERIALREADCOMMAND_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_SERIAL_MODULE_COMMANDS_ZCODESERIALREADCOMMAND_HPP_

#include <zcode/modules/ZcodeCommand.hpp>
#include <serial-ll/lowlevel/SerialManager.hpp>
#include <serial-ll/lowlevel/Serial.hpp>

#define COMMAND_EXISTS_0073 EXISTENCE_MARKER_UTIL

template<class ZP>
class ZcodeSerialReadCommand: public ZcodeCommand<ZP> {
    typedef typename ZP::LL LL;
    typedef typename LL::HW HW;

public:
    static constexpr uint8_t CODE = 0x03;

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        uint16_t port = 0;
        uint16_t length = 0;

        if (!slot.getFields()->get('P', &port)) {
            slot.fail(BAD_PARAM, "Port number missing");
            return;
        }
        if (port >= SerialManager<LL>::serialCount) {
            slot.fail(BAD_PARAM, "Port number invalid");
            return;
        }
        if (SerialManager<LL>::isMasked(port)) {
            slot.fail(BAD_PARAM, "Port not available");
            return;
        }

        if (!slot.getFields()->get('L', &length)) {
            slot.fail(BAD_PARAM, "Port number missing");
            return;
        }

        SerialError error = SerialManager<LL>::getSerialById(port)->getError(length);
        bool failOnDataOut = slot.getFields()->has('F');
        bool useBigString = slot.getFields()->has('T');

        uint8_t data[length];
        uint16_t lengthRead = SerialManager<LL>::getSerialById(port)->read(data, length);
        if (error != SerialNoError) {
            out->writeField8('F', (uint8_t) error);
        }
        if (useBigString) {
            out->writeBigStringField(data, lengthRead);
        } else {
            out->writeBigHexField(data, lengthRead);
        }
        if (lengthRead != length && failOnDataOut) {
            slot.fail(CMD_FAIL, NULL);
        } else if (error != SerialNoError) {
            slot.fail(CMD_FAIL, NULL);
        } else {
            out->writeStatus(OK);
        }
    }

};

#endif /* SRC_MAIN_CPP_ARM_NO_OS_SERIAL_MODULE_COMMANDS_ZCODESERIALREADCOMMAND_HPP_ */
