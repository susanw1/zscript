/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_PINS_MODULE_LOWLEVEL_GPIOMANAGER_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_PINS_MODULE_LOWLEVEL_GPIOMANAGER_HPP_

#define GPIOLOWLEVEL_NO_CPP
#include <arm-no-os/llIncludes.hpp>
#include "Gpio.hpp"
#include "specific/GpioNames.hpp"

template<class LL>
class GpioManager {
private:
    typedef typename LL::HW HW;
    static GpioPin<LL> pins[HW::pinCount];

public:
    static void init();

    static void activateClock(GpioPinName name);

    static GpioPin<LL> getPin(GpioPinName name);

    static GpioPin<LL> getPin(uint8_t id) {
        if (id >= HW::pinCount) {
            return pins[0];
        }
        return pins[id];
    }

    static GpioPort* getPort(GpioPinName name);
};

#include "specific/GpioManagercpp.hpp"
#undef GPIOLOWLEVEL_NO_CPP
#include "specific/Gpiocpp.hpp"

#endif /* SRC_MAIN_CPP_ARM_NO_OS_PINS_MODULE_LOWLEVEL_GPIOMANAGER_HPP_ */
