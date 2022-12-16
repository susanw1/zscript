/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_SERIAL_MODULE_LOWLEVEL_UARTMANAGER_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_SERIAL_MODULE_LOWLEVEL_UARTMANAGER_HPP_

#include <uart-ll/UartLLInterfaceInclude.hpp>

#include <arm-no-os/llIncludes.hpp>
#include "Uart.hpp"

class Serial;

template<class LL>
class UartManager {
    typedef typename LL::HW HW;
    private:
    static Uart<LL> uarts[HW::uartCount];

    static void interrupt(uint8_t id);

    UartManager() {
    }

public:
    static void init();

    static Uart<LL>* getUartById(SerialIdentifier id) {
        return uarts + id;
    }
};

#include "specific/UartManagercpp.hpp"

#endif /* SRC_MAIN_CPP_ARM_NO_OS_SERIAL_MODULE_LOWLEVEL_UARTMANAGER_HPP_ */
