/*
 * ZcodeSerialSendCommand.hpp
 *
 *  Created on: 20 Jul 2022
 *      Author: robert
 */

#ifndef NO_OS_SRC_MAIN_C___MODULES_SERIAL_ZCODESERIALSENDCOMMAND_HPP_
#define NO_OS_SRC_MAIN_C___MODULES_SERIAL_ZCODESERIALSENDCOMMAND_HPP_

#include <modules/ZcodeCommand.hpp>
#include <LowLevel/UartLowLevel/UartManager.hpp>
#include <LowLevel/UartLowLevel/Serial.hpp>

#define COMMAND_EXISTS_0072 EXISTENCE_MARKER_UTIL

template<class ZP>
class ZcodeSerialSendCommand: public ZcodeCommand<ZP> {
    typedef typename ZP::LL LL;

public:
    static constexpr uint8_t CODE = 0x02;

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        uint16_t port = 0;

        if (!slot.getFields()->get('P', &port)) {
            slot.fail(BAD_PARAM, "Port number missing");
            return;
        }
        if (port >= LL::serialCount) {
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

#endif /* NO_OS_SRC_MAIN_C___MODULES_SERIAL_ZCODESERIALSENDCOMMAND_HPP_ */
