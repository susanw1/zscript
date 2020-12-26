/*
 * ClockManager.cpp
 *
 *  Created on: 26 Dec 2020
 *      Author: robert
 */

#include "../ClockManager.hpp"

Clock ClockManager::clocks[GeneralHalSetup::systemClockCount] = { Clock(16000, SysClock, HSI),
        Clock(16000, HSI, NONE), Clock(0, HSI48, NONE), Clock(0, LSI, NONE), Clock(0, HSE, NONE),
        Clock(0, LSE, NONE), Clock(0, VCO, NONE), Clock(0, PLL_P, VCO), Clock(0, PLL_Q, VCO),
        Clock(0, PLL_R, VCO), Clock(16000, HCLK, SysClock), Clock(16000, PCLK_1, HCLK),
        Clock(16000, PCLK_2, HCLK) };
