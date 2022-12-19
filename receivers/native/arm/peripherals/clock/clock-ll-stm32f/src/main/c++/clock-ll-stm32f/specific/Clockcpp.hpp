/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_STM32G4_SYSTEM_CLOCK_SPECIFIC_CLOCKCPP_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_STM32G4_SYSTEM_CLOCK_SPECIFIC_CLOCKCPP_HPP_

#include "Clock.hpp"
#include <clock-ll/ClockManager.hpp>

template<class LL>
void clocksInternal_DO_NOT_USE_activateHSI14() {
    const uint32_t enableHSI14 = 0x1;
    const uint32_t HSI14Ready = 0x2;

    RCC->CR2 |= enableHSI14;
    while (!(RCC->CR2 & HSI14Ready))
        ;
    return;
}

template<class LL>
void clocksInternal_DO_NOT_USE_activateHSI() {
    const uint32_t enableHSI8 = 0x1;
    const uint32_t HSI8Ready = 0x2;

    RCC->CR |= enableHSI8;
    while (!(RCC->CR & HSI8Ready))
        ;
    return;
}

template<class LL>
void clocksInternal_DO_NOT_USE_activateLSI() {
    const uint32_t enableLSI = 0x1;
    const uint32_t LSIReady = 0x2;

    RCC->CSR |= enableLSI;
    while (!(RCC->CSR & LSIReady))
        ;
    return;
}

template<class LL>
int Clock<LL>::set(uint32_t targetFreqKhz, SystemClock source) {
    if (clock == SysClock) {
        const uint32_t sysClkSwitchMask = 0x03;
        const uint32_t sysClkSwitchHSI16 = 0x00;
        const uint32_t sysClkSwitchHSE = 0x01;
        const uint32_t sysClkSwitchPLL = 0x02;

        uint32_t cfgr = RCC->CFGR;
        if (source == PLL && (ClockManager<LL>::getClock(source)->freq != 0 || ClockManager<LL>::getClock(source)->set(targetFreqKhz, NONE) == 0)) {
            freq = ClockManager<LL>::getClock(source)->freq;
            this->source = (uint8_t) source;
            cfgr &= ~sysClkSwitchMask;
            cfgr |= sysClkSwitchPLL;
            RCC->CFGR = cfgr;
            return 0;
        } else if (source == HSI && (ClockManager<LL>::getClock(source)->freq != 0 || ClockManager<LL>::getClock(source)->set(targetFreqKhz, NONE) == 0)) {
            freq = ClockManager<LL>::getClock(source)->freq;
            this->source = (uint8_t) source;
            cfgr &= ~sysClkSwitchMask;
            cfgr |= sysClkSwitchHSI16;
            RCC->CFGR = cfgr;
            return 0;
        } else if (source == HSE && (ClockManager<LL>::getClock(source)->freq != 0 || ClockManager<LL>::getClock(source)->set(targetFreqKhz, NONE) == 0)) {
            freq = ClockManager<LL>::getClock(source)->freq;
            this->source = (uint8_t) source;
            cfgr &= ~sysClkSwitchMask;
            cfgr |= sysClkSwitchHSE;
            RCC->CFGR = cfgr;
            return 0;
        } else {
            return -1;
        }
    } else if (clock == HSI) {
        clocksInternal_DO_NOT_USE_activateHSI<LL>();
        freq = 8000;
        return 0;
    } else if (clock == HSI14) {
        clocksInternal_DO_NOT_USE_activateHSI14<LL>();
        freq = 14000;
        return 0;
    } else if (clock == LSI) {
        clocksInternal_DO_NOT_USE_activateLSI<LL>();
        freq = 40;
        return 0;
    } else if (clock == HSE) {
        return -1;
    } else if (clock == LSE) {
        return -1;
    } else if (clock == PLL) {
        const uint32_t pllEnable = 0x01000000;
        const uint32_t pllReady = 0x02000000;

        const uint32_t pllDivMask = 0xF;
        const uint32_t pllMultMask = 0x003c0000;

        const uint32_t pllHSISource = 0x00010000;

        uint8_t mult = 2;
        uint8_t div = 1;
        if (targetFreqKhz >= 48000) {
            return -1;
        } else if (targetFreqKhz < 4000) {
            return -1;
        }
        uint32_t bestDiff = 0xFFFFFFFF;
        for (uint8_t i = 2; i <= 16; i++) {
            if (source == HSI) {
                uint32_t freq = ClockManager<LL>::getClock(HSI)->freq * i / 2;
                if (freq < 48000) {
                    if (freq >= targetFreqKhz && freq - targetFreqKhz < bestDiff) {
                        mult = i;
                        bestDiff = freq - targetFreqKhz;
                    } else if (freq < targetFreqKhz && targetFreqKhz - freq < bestDiff) {
                        mult = i;
                        bestDiff = targetFreqKhz - freq;
                    }
                }
            } else {
                for (uint8_t j = 1; j <= 16; j++) {
                    uint32_t freq = ClockManager<LL>::getClock(HSE)->freq * i / j;
                    if (freq < 48000) {
                        if (freq >= targetFreqKhz && freq - targetFreqKhz < bestDiff) {
                            mult = i;
                            div = j;
                            bestDiff = freq - targetFreqKhz;
                        } else if (freq < targetFreqKhz && targetFreqKhz - freq < bestDiff) {
                            mult = i;
                            div = j;
                            bestDiff = targetFreqKhz - freq;
                        }
                    }
                }
            }
        }

        RCC->CR &= ~pllEnable; // Disable PLL (needed to change things)

        uint32_t cfg = RCC->CFGR;
        cfg &= ~(pllMultMask | pllHSISource);
        cfg |= (mult - 2) << 18;
        if (source == HSE) {
            cfg |= pllHSISource;
        }
        RCC->CFGR = cfg;
        if (source == HSE) {
            uint32_t cfg2 = RCC->CFGR2;
            cfg2 &= ~pllDivMask;
            cfg2 |= div - 1;
            RCC->CFGR = cfg2;
            freq = ClockManager<LL>::getClock(HSE)->freq * mult / div;
        } else {
            freq = ClockManager<LL>::getClock(HSI)->freq * mult / 2;
        }
        RCC->CR |= pllEnable; // Enable PLL
        while (!(RCC->CR & pllReady))
            ;
        this->source = (uint8_t) source;
        return 0;
    } else if (clock == HCLK) {
        const uint32_t hclkPrescalerMask = 0xF0;

        uint32_t sourceFreq = ClockManager<LL>::getClock(SysClock)->freq;
        uint32_t bestDiff = 0xFFFFFFFF;
        uint8_t presc = 0;
        for (uint8_t i = 0; i <= 8; ++i) {
            uint8_t j = i;
            if (j > 4) {
                j++;
            }
            uint32_t resultFreq = sourceFreq / (1 << j);
            if (resultFreq > targetFreqKhz) {
                if (resultFreq - targetFreqKhz < bestDiff) {
                    bestDiff = resultFreq - targetFreqKhz;
                    presc = i;
                }
            } else {
                if (targetFreqKhz - resultFreq < bestDiff) {
                    bestDiff = targetFreqKhz - resultFreq;
                    presc = i;
                }
            }
        }
        uint8_t j = presc;
        if (j > 4) {
            j++;
        }
        freq = sourceFreq / (1 << j);
        if (presc != 0) {
            presc += 7;
        }
        uint32_t cfgr = RCC->CFGR;
        cfgr &= ~hclkPrescalerMask;
        cfgr |= presc << 4;
        RCC->CFGR = cfgr;
        return 0;
    } else if (clock == PCLK) {
        const uint32_t pclkPrescalerMask = 0x700;

        uint32_t sourceFreq = ClockManager<LL>::getClock(HCLK)->freq;
        uint32_t bestDiff = 0xFFFFFFFF;
        uint8_t presc = 0;
        for (uint8_t i = 0; i <= 4; ++i) {
            uint32_t resultFreq = sourceFreq / (1 << i);
            if (resultFreq > targetFreqKhz) {
                if (resultFreq - targetFreqKhz < bestDiff) {
                    bestDiff = resultFreq - targetFreqKhz;
                    presc = i;
                }
            } else {
                if (targetFreqKhz - resultFreq < bestDiff) {
                    bestDiff = targetFreqKhz - resultFreq;
                    presc = i;
                }
            }
        }
        freq = sourceFreq / (1 << presc);
        if (presc != 0) {
            presc += 3;
        }
        uint32_t cfgr = RCC->CFGR;
        cfgr &= ~pclkPrescalerMask;
        cfgr |= presc << 8;
        RCC->CFGR = cfgr;
        return 0;
    } else {
        return -1;
    }
}

#endif /* SRC_MAIN_CPP_ARM_NO_OS_STM32G4_SYSTEM_CLOCK_SPECIFIC_CLOCKCPP_HPP_ */
