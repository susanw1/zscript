/*
 * ZcodeSerialSetupCommand.hpp
 *
 *  Created on: 20 Jul 2022
 *      Author: robert
 */

#ifndef NO_OS_SRC_MAIN_C___MODULES_SERIAL_ZCODESERIALSETUPCOMMAND_HPP_
#define NO_OS_SRC_MAIN_C___MODULES_SERIAL_ZCODESERIALSETUPCOMMAND_HPP_

#include <zcode/modules/ZcodeCommand.hpp>
#include <arm-no-os/serial-module/lowlevel/UartManager.hpp>
#include <arm-no-os/serial-module/lowlevel/Serial.hpp>

#define COMMAND_EXISTS_0071 EXISTENCE_MARKER_UTIL

template<class ZP>
class ZcodeSerialSetupCommand: public ZcodeCommand<ZP> {
    typedef typename ZP::LL LL;
    typedef typename LL::HW HW;

public:
    static constexpr uint8_t CODE = 0x01;

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        uint16_t port = 0;
        uint32_t baud = 0;
        bool singleNdoubleStop = false;

        if (!slot.getFields()->get('P', &port)) {
            slot.fail(BAD_PARAM, "Port number missing");
            return;
        }
        if (port >= HW::serialCount) {
            slot.fail(BAD_PARAM, "Port number invalid");
            return;
        }
        if (UartManager<LL>::isMasked(port)) {
            slot.fail(BAD_PARAM, "Port not available");
            return;
        }
        Serial *serial = UartManager<LL>::getUartById(port);

        for (uint8_t i = 0; i < slot.getBigField()->getLength(); ++i) {
            baud <<= 8;
            baud |= slot.getBigField()->getData()[i];
        }
        if (slot.getBigField()->getLength() == 0) {
            baud = 9600;
        } else if (slot.getBigField()->getLength() > 4 || baud <= serial->getMinBaud() || baud >= serial->getMaxBaud()) {
            slot.fail(BAD_PARAM, "Invalid baud rate");
            return;
        }
        if (slot.getFields()->has('C')) { //check parity
            slot.fail(CMD_FAIL, "Parity not supported");
            return;
        }
        if (slot.getFields()->has('O')) {
            slot.fail(CMD_FAIL, "Noise detection not supported");
            return;
        }
        if (slot.getFields()->has('S')) {
            singleNdoubleStop = true;
        }
        if (slot.getFields()->has('A')) {
            slot.fail(CMD_FAIL, "Auto baud rate detection not supported");
            return;
        }

        serial->init(NULL, baud, singleNdoubleStop);
        out->writeStatus(OK);
    }

};

#endif /* NO_OS_SRC_MAIN_C___MODULES_SERIAL_ZCODESERIALSETUPCOMMAND_HPP_ */
