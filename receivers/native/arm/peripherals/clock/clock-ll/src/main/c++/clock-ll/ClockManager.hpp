/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_STM32F0_SYSTEM_CLOCK_CLOCKMANAGER_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_STM32F0_SYSTEM_CLOCK_CLOCKMANAGER_HPP_

#include <llIncludes.hpp>
#include "specific/Clock.hpp"

template<class LL>
class ClockManager {
private:
    static Clock<LL> clocks[NONE];

public:
    static Clock<LL>* getClock(SystemClock clock) {
        return clocks + (uint8_t) clock;
    }
    static void fastSetup();
    static void basicSetup();
    static void slowSetup();
};
#include "specific/ClockManagercpp.hpp"

#endif /* SRC_MAIN_CPP_ARM_NO_OS_STM32F0_SYSTEM_CLOCK_CLOCKMANAGER_HPP_ */
