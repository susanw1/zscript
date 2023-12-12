/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_SERIAL_MODULE_LOWLEVEL_UARTMANAGER_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_SERIAL_MODULE_LOWLEVEL_UARTMANAGER_HPP_

#include <arm-no-os/llIncludes.hpp>
#include "Uart.hpp"
#ifdef USE_USB_SERIAL
#include <arm-no-os/usb-module/lowlevel/Usb.hpp>
#endif

class Serial;

template<class LL>
class UartManager {
    typedef typename LL::HW HW;
    private:
    static Uart<LL> uarts[HW::uartCount];

    static uint16_t mask;

    static void interrupt(uint8_t id);

    UartManager() {
    }

public:
    static void maskSerial(SerialIdentifier id) {
        mask |= (1 << id);
    }
    static void unmaskSerial(SerialIdentifier id) {
        mask &= ~(1 << id);
    }
    static bool isMasked(SerialIdentifier id) {
        return (mask & (1 << id)) != 0;
    }

    static void init();

    static Serial* getUartById(SerialIdentifier id) {

#ifdef USE_USB_SERIAL
        if (id == HW::uartCount) {
            return &Usb<LL>::usb;
        }
#endif
        return uarts + id;
    }
};
template<class ZP>
uint16_t UartManager<ZP>::mask = 0;

#include "specific/UartManagercpp.hpp"

#endif /* SRC_MAIN_CPP_ARM_NO_OS_SERIAL_MODULE_LOWLEVEL_UARTMANAGER_HPP_ */
