/*
 * ZcodeUsbcPDSetCommand.hpp
 *
 *  Created on: 16 Jul 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___MODULES_USBCPD_ZCODEUSBCPDSETCOMMAND_HPP_
#define SRC_MAIN_C___MODULES_USBCPD_ZCODEUSBCPDSETCOMMAND_HPP_

#include <modules/ZcodeCommand.hpp>
#include <LowLevel/UsbcPD/Ucpd.hpp>

#define COMMAND_VALUE_0112 MODULE_CAPABILITIES_UTIL

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

#endif /* SRC_MAIN_C___MODULES_USBCPD_ZCODEUSBCPDSETCOMMAND_HPP_ */
