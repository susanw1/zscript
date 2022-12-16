/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_STM32F0_SYSTEM_CLOCK_SYSTEMMILLICLOCK_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_STM32F0_SYSTEM_CLOCK_SYSTEMMILLICLOCK_HPP_

#include <arm-no-os/llIncludes.hpp>
#include <arm-no-os/system/interrupt/InterruptManager.hpp>
#include "../clock-ll/ClockManager.hpp"

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
    static void init() {
        const uint32_t enableSysTickEnable = 0x1;
        const uint32_t enableSysTickEnableInt = 0x2;
        const uint32_t enableSysTickSetAsClock = 0x4;
        SysTick->LOAD = ClockManager<LL>::getClock(SysClock)->getFreqKhz();
        SysTick->CTRL = enableSysTickEnable | enableSysTickEnableInt | enableSysTickSetAsClock;

        InterruptManager::setInterrupt(&SystemMilliClock::resetHappened, SysTickInt);
        InterruptManager::enableInterrupt(SysTickInt, 0, 5);
        timeBroad = 0;
    }

    static void blockDelayMillis(uint32_t delay) {
        uint32_t end = getTimeMillis() + delay;
        while (end > getTimeMillis())
            ;
        return;
    }

    static uint32_t getTimeMillis() {
        return timeBroad;
    }

};

template<class LL>
volatile uint32_t SystemMilliClock<LL>::timeBroad = 0;

#endif /* SRC_MAIN_CPP_ARM_NO_OS_STM32F0_SYSTEM_CLOCK_SYSTEMMILLICLOCK_HPP_ */
