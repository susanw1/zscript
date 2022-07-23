/*
 * ZcodeSerialReadCommand.hpp
 *
 *  Created on: 20 Jul 2022
 *      Author: robert
 */

#ifndef NO_OS_SRC_MAIN_C___MODULES_SERIAL_ZCODESERIALREADCOMMAND_HPP_
#define NO_OS_SRC_MAIN_C___MODULES_SERIAL_ZCODESERIALREADCOMMAND_HPP_

#include <modules/ZcodeCommand.hpp>
#include <LowLevel/UartLowLevel/UartManager.hpp>
#include <LowLevel/UartLowLevel/Serial.hpp>

#define COMMAND_EXISTS_0073 EXISTENCE_MARKER_UTIL

template<class ZP>
class ZcodeSerialReadCommand: public ZcodeCommand<ZP> {
    typedef typename ZP::LL LL;

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
        if (port >= LL::serialCount) {
            slot.fail(BAD_PARAM, "Port number invalid");
            return;
        }

        if (!slot.getFields()->get('L', &length)) {
            slot.fail(BAD_PARAM, "Port number missing");
            return;
        }

        SerialError error = UartManager<LL>::getUartById(port)->getError(length);
        bool failOnDataOut = slot.getFields()->has('F');
        bool useBigString = slot.getFields()->has('T');

        uint8_t data[length];
        uint16_t lengthRead = UartManager<LL>::getUartById(port)->read(data, length);
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

#endif /* NO_OS_SRC_MAIN_C___MODULES_SERIAL_ZCODESERIALREADCOMMAND_HPP_ */
