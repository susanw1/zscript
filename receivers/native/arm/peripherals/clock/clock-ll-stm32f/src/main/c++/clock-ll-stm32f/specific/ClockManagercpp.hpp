/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_STM32G4_SYSTEM_CLOCK_SPECIFIC_CLOCKMANAGERCPP_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_STM32G4_SYSTEM_CLOCK_SPECIFIC_CLOCKMANAGERCPP_HPP_

#include <clock-ll/ClockManager.hpp>
#include "Clock.hpp"
#include "SystemClocks.hpp"

template<class LL>
Clock<LL> ClockManager<LL>::clocks[NONE] = {
        Clock<LL>(8000, SysClock, HSI),
        Clock<LL>(8000, HSI, NONE),
        Clock<LL>(0, HSI14, NONE),
        Clock<LL>(0, LSI, NONE),
        Clock<LL>(0, HSE, NONE),
        Clock<LL>(0, LSE, NONE),
        Clock<LL>(0, PLL, HSI),
        Clock<LL>(8000, HCLK, SysClock),
        Clock<LL>(8000, PCLK, HCLK),
};

template<class LL>
void ClockManager<LL>::fastSetup() {
    ClockManager<LL>::getClock(HSI)->set(8000, NONE);
    ClockManager<LL>::getClock(PLL)->set(40000, HSI);
    ClockManager<LL>::getClock(SysClock)->set(40000, PLL);
    ClockManager<LL>::getClock(HCLK)->set(40000, SysClock);
    ClockManager<LL>::getClock(PCLK)->set(20000, HCLK);
}
template<class LL>
void ClockManager<LL>::basicSetup() {
    ClockManager<LL>::getClock(HSI)->set(8000, NONE);
    ClockManager<LL>::getClock(SysClock)->set(8000, HSI);
    ClockManager<LL>::getClock(HCLK)->set(8000, SysClock);
    ClockManager<LL>::getClock(PCLK)->set(8000, HCLK);
}
template<class LL>
void ClockManager<LL>::slowSetup() {
    ClockManager<LL>::getClock(LSI)->set(32, NONE);
    ClockManager<LL>::getClock(SysClock)->set(32, LSI);
    ClockManager<LL>::getClock(HCLK)->set(32, SysClock);
    ClockManager<LL>::getClock(PCLK)->set(32, HCLK);
}
#endif /* SRC_MAIN_CPP_ARM_NO_OS_STM32G4_SYSTEM_CLOCK_SPECIFIC_CLOCKMANAGERCPP_HPP_ */
