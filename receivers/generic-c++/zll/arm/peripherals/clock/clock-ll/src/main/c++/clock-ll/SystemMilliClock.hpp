/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_STM32F0_SYSTEM_CLOCK_SYSTEMMILLICLOCK_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_STM32F0_SYSTEM_CLOCK_SYSTEMMILLICLOCK_HPP_

#include <llIncludes.hpp>
#include <interrupt-ll/InterruptManager.hpp>
#include "ClockManager.hpp"

template<class LL>
class SystemMilliClock {
private:
    static volatile uint32_t timeBroad;

    friend class InterruptManager;

    static void resetHappened(uint8_t i) {
        (void) i;
        timeBroad++;
    }

public:
    static void init();

    static void blockDelayMillis(uint32_t delay);

    static uint32_t getTimeMillis();

};

template<class LL>
volatile uint32_t SystemMilliClock<LL>::timeBroad = 0;

#include "specific/SystemMilliClockcpp.hpp"

#endif /* SRC_MAIN_CPP_ARM_NO_OS_STM32F0_SYSTEM_CLOCK_SYSTEMMILLICLOCK_HPP_ */
