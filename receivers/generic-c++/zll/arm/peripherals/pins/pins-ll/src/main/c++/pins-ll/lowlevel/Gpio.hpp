/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_PINS_MODULE_LOWLEVEL_GPIO_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_PINS_MODULE_LOWLEVEL_GPIO_HPP_

#include <llIncludes.hpp>
#include "specific/GpioPort.hpp"

template<class LL>
class GpioPin {
private:
    GpioPinName pin;

public:
    GpioPin(GpioPinName pin) :
            pin(pin) {
    }
    void init();

    void write(bool value);

    void set();

    void reset();

    bool read();

    void setMode(PinMode mode);

    void setOutputMode(OutputMode mode);

    void setOutputSpeed(PinSpeed speed);

    void setPullMode(PullMode mode);

    void setAlternateFunction(uint8_t function);

    GpioPinName getPinName() {
        return pin;
    }
};

#include "GpioManager.hpp"

#ifndef GPIOLOWLEVEL_NO_CPP
#include "specific/Gpiocpp.hpp"
#endif

#endif /* SRC_MAIN_CPP_ARM_NO_OS_PINS_MODULE_LOWLEVEL_GPIO_HPP_ */
