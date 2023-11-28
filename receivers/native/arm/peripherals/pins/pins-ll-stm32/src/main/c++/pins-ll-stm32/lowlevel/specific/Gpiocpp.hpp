/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_PINS_MODULE_LOWLEVEL_SPECIFIC_GPIOCPP_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_PINS_MODULE_LOWLEVEL_SPECIFIC_GPIOCPP_HPP_

#include <pins-ll/lowlevel/Gpio.hpp>

#include <pins-ll/lowlevel/GpioManager.hpp>

template<class LL>
void GpioPin<LL>::init() {
    GpioManager<LL>::activateClock(pin);
}

template<class LL>
void GpioPin<LL>::write(bool value) {
    if (value) {
        GpioManager<LL>::getPort(pin)->setPin(pin);
    } else {
        GpioManager<LL>::getPort(pin)->resetPin(pin);
    }
}

template<class LL>
void GpioPin<LL>::set() {
    GpioManager<LL>::getPort(pin)->setPin(pin);
}

template<class LL>
void GpioPin<LL>::reset() {
    GpioManager<LL>::getPort(pin)->resetPin(pin);
}

template<class LL>
bool GpioPin<LL>::read() {
    return GpioManager<LL>::getPort(pin)->getPinValue(pin);
}

template<class LL>
void GpioPin<LL>::setOutputMode(OutputMode mode) {
    GpioManager<LL>::getPort(pin)->setOutputType(pin, mode);
}

template<class LL>
void GpioPin<LL>::setPullMode(PullMode mode) {
    GpioManager<LL>::getPort(pin)->setPullMode(pin, mode);
}

template<class LL>
void GpioPin<LL>::setMode(PinMode mode) {
    GpioManager<LL>::getPort(pin)->setMode(pin, mode);
}

template<class LL>
void GpioPin<LL>::setOutputSpeed(PinSpeed speed) {
    GpioManager<LL>::getPort(pin)->setOutputSpeed(pin, speed);
}

template<class LL>
void GpioPin<LL>::setAlternateFunction(uint8_t function) {
    GpioManager<LL>::getPort(pin)->setAlternateFunction(pin, function);
}

#endif /* SRC_MAIN_CPP_ARM_NO_OS_PINS_MODULE_LOWLEVEL_SPECIFIC_GPIOCPP_HPP_ */
