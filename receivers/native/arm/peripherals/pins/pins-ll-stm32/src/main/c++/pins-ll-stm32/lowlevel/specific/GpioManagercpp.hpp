/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_PINS_MODULE_LOWLEVEL_SPECIFIC_GPIOMANAGERCPP_HPP_
#define SRC_MAIN_CPP_PINS_MODULE_LOWLEVEL_SPECIFIC_GPIOMANAGERCPP_HPP_
#include <pins-ll/lowlevel/GpioManager.hpp>

template<class LL>
void GpioManager<LL>::init() {
}

template<class LL>
void GpioManager<LL>::activateClock(GpioPinName name) {
    RCC->AHB2ENR |= 1 << name.port;
}

template<class LL>
GpioPort* GpioManager<LL>::getPort(GpioPinName name) {
    switch (name.port) {
    case PortA:
        return PortAAddr;
    case PortB:
        return PortBAddr;
    case PortC:
        return PortCAddr;
    case PortD:
        return PortDAddr;
    case PortE:
        return PortEAddr;
    case PortF:
        return PortFAddr;
    case PortG:
        return PortGAddr;
    default:
        return NULL;
    }
}

#endif /* SRC_MAIN_CPP_PINS_MODULE_LOWLEVEL_SPECIFIC_GPIOMANAGERCPP_HPP_ */
