/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_PINS_MODULE_ATODLOWLEVEL_ATODMANAGER_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_PINS_MODULE_ATODLOWLEVEL_ATODMANAGER_HPP_

#include <arm-no-os/llIncludes.hpp>
#include "AtoD.hpp"
#include <arm-no-os/pins-module/lowlevel/GpioManager.hpp>

template<class LL>
class AtoDManager {
    typedef typename LL::HW HW;
    static AtoD<LL> atoDs[HW::atoDCount];

    AtoDManager() {
    }

public:
    static void init();

    static uint16_t performAtoD(GpioPinName pin);

    static bool canPerformAtoD(GpioPinName pin);

    static AtoD<LL>* getAtoD(uint8_t aToD) {
        return atoDs + aToD;
    }
};
#include "specific/AtoDManagercpp.hpp"

#endif /* SRC_MAIN_CPP_ARM_NO_OS_PINS_MODULE_ATODLOWLEVEL_ATODMANAGER_HPP_ */
