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
        Clock<LL>(16000, SysClock, HSI),
        Clock<LL>(16000, HSI, NONE),
        Clock<LL>(0, HSI48, NONE),
        Clock<LL>(0, LSI, NONE),
        Clock<LL>(0, HSE, NONE),
        Clock<LL>(0, LSE, NONE),
        Clock<LL>(0, VCO, NONE),
        Clock<LL>(0, PLL_P, VCO),
        Clock<LL>(0, PLL_Q, VCO),
        Clock<LL>(0, PLL_R, VCO),
        Clock<LL>(16000, HCLK, SysClock),
        Clock<LL>(16000, PCLK_1, HCLK),
        Clock<LL>(16000, PCLK_2, HCLK)
};

template<class LL>
void ClockManager<LL>::fastSetup() {
    ClockManager<LL>::getClock(HSI)->set(16000, NONE);
    ClockManager<LL>::getClock(VCO)->set(240000, HSI);
    ClockManager<LL>::getClock(PLL_R)->set(120000, VCO);
    ClockManager<LL>::getClock(SysClock)->set(120000, PLL_R);
    ClockManager<LL>::getClock(HCLK)->set(120000, SysClock);
    ClockManager<LL>::getClock(PCLK_1)->set(60000, HCLK);
    ClockManager<LL>::getClock(PCLK_2)->set(60000, HCLK);
}
template<class LL>
void ClockManager<LL>::basicSetup() {
    ClockManager<LL>::getClock(HSI)->set(16000, NONE);
    ClockManager<LL>::getClock(SysClock)->set(16000, HSI);
    ClockManager<LL>::getClock(HCLK)->set(16000, SysClock);
    ClockManager<LL>::getClock(PCLK_1)->set(16000, HCLK);
    ClockManager<LL>::getClock(PCLK_2)->set(16000, HCLK);
}
template<class LL>
void ClockManager<LL>::slowSetup() {
    ClockManager<LL>::getClock(LSI)->set(32, NONE);
    ClockManager<LL>::getClock(SysClock)->set(32, LSI);
    ClockManager<LL>::getClock(HCLK)->set(32, SysClock);
    ClockManager<LL>::getClock(PCLK_1)->set(32, HCLK);
    ClockManager<LL>::getClock(PCLK_2)->set(32, HCLK);
}

#endif /* SRC_MAIN_CPP_ARM_NO_OS_STM32G4_SYSTEM_CLOCK_SPECIFIC_CLOCKMANAGERCPP_HPP_ */
