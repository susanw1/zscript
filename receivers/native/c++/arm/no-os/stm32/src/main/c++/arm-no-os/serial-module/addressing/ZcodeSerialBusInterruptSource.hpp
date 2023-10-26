/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_SERIAL_MODULE_ADDRESSING_ZCODESERIALBUSINTERRUPTSOURCE_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_SERIAL_MODULE_ADDRESSING_ZCODESERIALBUSINTERRUPTSOURCE_HPP_

#include <zcode/ZcodeIncludes.hpp>
#include <zcode/ZcodeBusInterruptSource.hpp>
#include <arm-no-os/serial-module/lowlevel/UartManager.hpp>
#include <arm-no-os/serial-module/lowlevel/Serial.hpp>

template<class ZP>
class ZcodeSerialBusInterruptSource: public ZcodeBusInterruptSource<ZP> {
    typedef typename ZP::LL LL;
    typedef typename LL::HW HW;
    static uint16_t serialIntBits;
    static uint16_t serialIntReceivedBits;

    static void callback(SerialIdentifier id) {
        if (!UartManager<LL>::isMasked(id)) {
            serialIntReceivedBits |= 1 << id;
        }
    }
public:

    ZcodeSerialBusInterruptSource() {
        for (uint16_t i = 0; i < HW::serialCount; ++i) {
            UartManager<LL>::getUartById(i)->setTargetValue(&ZcodeSerialBusInterruptSource::callback, EOL_SYMBOL);
        }
    }

    ZcodeNotificationInfo takeUncheckedNotification() {
        if (serialIntReceivedBits != 0) {
            for (uint8_t i = 0; i < HW::serialCount; ++i) {
                if ((serialIntReceivedBits & (1 << i)) != 0) {
                    serialIntReceivedBits &= ~(1 << i);
                    serialIntBits |= 1 << i;
                    ZcodeNotificationInfo info;
                    info.id = i;
                    info.module = 0x7;
                    info.port = i;
                    info.valid = true;
                    return info;
                }
            }
        }
        ZcodeNotificationInfo invalid;
        invalid.valid = false;
        return invalid;
    }

    void clearNotification(uint8_t id) {
        serialIntBits &= ~(1 << id);
    }

};
template<class ZP>
uint16_t ZcodeSerialBusInterruptSource<ZP>::serialIntBits = 0;

template<class ZP>
uint16_t ZcodeSerialBusInterruptSource<ZP>::serialIntReceivedBits = 0;

#endif /* SRC_MAIN_CPP_ARM_NO_OS_SERIAL_MODULE_ADDRESSING_ZCODESERIALBUSINTERRUPTSOURCE_HPP_ */
