/*
 * ZcodeSerialAddressingSystem.hpp
 *
 *  Created on: 24 Jul 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___ARM_NO_OS_SERIAL_MODULE_ADDRESSING_ZCODESERIALADDRESSINGSYSTEM_HPP_
#define SRC_MAIN_C___ARM_NO_OS_SERIAL_MODULE_ADDRESSING_ZCODESERIALADDRESSINGSYSTEM_HPP_

#include <zcode/ZcodeIncludes.hpp>
#include <zcode/modules/ZcodeCommand.hpp>
#include <zcode/addressing/ZcodeModuleAddressingSystem.hpp>
#include <arm-no-os/serial-module/lowlevel/UartManager.hpp>
#include <arm-no-os/serial-module/lowlevel/Serial.hpp>

#define ADDRESSING_SWITCH007 ADDRESSING_SWITCH_UTIL(ZcodeSerialAddressingSystem<ZP>::routeAddress)
#define ADDRESSING_RESP_SWITCH007 ADDRESSING_RESP_SWITCH_UTIL(ZcodeSerialAddressingSystem<ZP>::routeResponse)
#define ADDRESSING_LEVEL007 1

template<class ZP>
class ZcodeOutStream;

template<class ZP>
class Zcode;

template<class ZP>
class ZcodeSerialAddressingSystem: public ZcodeModuleAddressingSystem<ZP> {
    typedef typename ZP::Strings::string_t string_t;

    typedef typename ZP::LL LL;

public:

    static void routeAddress(ZcodeExecutionCommandSlot<ZP> slot, ZcodeAddressingInfo<ZP> *addressingInfo) {

        uint8_t port = addressingInfo->port;
        if (port >= LL::serialCount) {
            slot.fail(BAD_PARAM, "Invalid Serial port");
            return;
        }
        Serial *serial = UartManager<LL>::getUartById(port);
        serial->write(slot.getBigField()->getData() + addressingInfo->start, slot.getBigField()->getLength() - addressingInfo->start);
        serial->write(EOL_SYMBOL);
        serial->transmitWriteBuffer();
    }

    static void routeResponse(ZcodeBusInterrupt<ZP> interrupt, ZcodeOutStream<ZP> *out) {
        uint8_t port = interrupt.getNotificationPort();
        if (port >= LL::serialCount) {
            interrupt.clear();
            return;
        }
        Serial *serial = UartManager<LL>::getUartById(port);
        while (true) {
            int16_t v = serial->read();
            if (v == -1) {
                out->writeCommandSequenceSeparator();
                return;
            }
            if (v == EOL_SYMBOL) {
                return;
            }
            out->writeByte((uint8_t) v);
        }
    }
};

#endif /* SRC_MAIN_C___ARM_NO_OS_SERIAL_MODULE_ADDRESSING_ZCODESERIALADDRESSINGSYSTEM_HPP_ */
