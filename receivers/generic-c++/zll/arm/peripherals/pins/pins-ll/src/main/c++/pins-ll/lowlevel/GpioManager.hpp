/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_PINS_MODULE_LOWLEVEL_GPIOMANAGER_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_PINS_MODULE_LOWLEVEL_GPIOMANAGER_HPP_

#define GPIOLOWLEVEL_NO_CPP
#include <llIncludes.hpp>
#include "Gpio.hpp"

template<class LL>
class GpioManager {
private:
    typedef typename LL::HW HW;

public:
    static void init();

    static void activateClock(GpioPinName name);

    static GpioPin<LL> getPin(GpioPinName name) {
        return GpioPin(name);
    }

    static GpioPort* getPort(GpioPinName name);
};

#include "specific/GpioManagercpp.hpp"
#undef GPIOLOWLEVEL_NO_CPP
#include "specific/Gpiocpp.hpp"

#endif /* SRC_MAIN_CPP_ARM_NO_OS_PINS_MODULE_LOWLEVEL_GPIOMANAGER_HPP_ */
