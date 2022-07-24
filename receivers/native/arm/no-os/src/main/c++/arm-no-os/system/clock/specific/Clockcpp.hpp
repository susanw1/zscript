/*
 * Clockcpp.hpp
 *
 *  Created on: 6 Jul 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___LOWLEVEL_CLOCKSLOWLEVEL_SPECIFIC_CLOCKCPP_HPP_
#define SRC_MAIN_C___LOWLEVEL_CLOCKSLOWLEVEL_SPECIFIC_CLOCKCPP_HPP_

#include "../Clock.hpp"
#include "../ClockManager.hpp"

template<class LL>
void clocksInternal_DO_NOT_USE_activateHSI48() {
    const uint32_t enableHSI48 = 0x1;
    const uint32_t HSI48Ready = 0x2;

    RCC->CRRCR |= enableHSI48;
    while (!(RCC->CRRCR & HSI48Ready))
        ;
    return;
}

template<class LL>
void clocksInternal_DO_NOT_USE_activateHSI() {
    const uint32_t enableHSI16 = 0x100;
    const uint32_t HSI16Ready = 0x400;

    RCC->CR |= enableHSI16;
    while (!(RCC->CR & HSI16Ready))
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
        const uint32_t sysClkSwitchHSI16 = 0x01;
        const uint32_t sysClkSwitchHSE = 0x02;
        const uint32_t sysClkSwitchPLL = 0x03;

        uint32_t cfgr = RCC->CFGR;
        if (source == PLL_R && (ClockManager<LL>::getClock(source)->freq != 0 || ClockManager<LL>::getClock(source)->set(targetFreqKhz, NONE) == 0)) {
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
        freq = 16000;
        return 0;
    } else if (clock == HSI48) {
        clocksInternal_DO_NOT_USE_activateHSI48<LL>();
        freq = 48000;
        return 0;
    } else if (clock == LSI) {
        clocksInternal_DO_NOT_USE_activateLSI<LL>();
        freq = 32;
        return 0;
    } else if (clock == HSE) {
        return -1;
    } else if (clock == LSE) {
        return -1;
    } else if (clock == VCO) {
        const uint32_t pllEnable = 0x01000000;
        const uint32_t pllReady = 0x02000000;

        const uint32_t pllQEnable = 0x00100000;
        const uint32_t pllPEnable = 0x00010000;
        const uint32_t pllREnable = 0x01000000;

        const uint32_t pllSrcMask = 0x3;
        const uint32_t pllSrcHSI16 = 0x2;
        const uint32_t pllSrcHSE = 0x3;

        const uint32_t pllDividerMask = 0x7ff3;

        if (targetFreqKhz > 344000) {
            return -1;
        }
        if (source == HSI && (ClockManager<LL>::getClock(source)->freq != 0 || ClockManager<LL>::getClock(source)->set(targetFreqKhz, NONE) == 0)) {
            this->source = (uint8_t) source;
        } else if (source == HSE && (ClockManager<LL>::getClock(source)->freq != 0 || ClockManager<LL>::getClock(source)->set(targetFreqKhz, NONE) == 0)) {
            this->source = (uint8_t) source;
        } else {
            return -1;
        }
        RCC->CR &= ~pllEnable; // Disable PLL (needed to change things)
        bool has_p = RCC->PLLCFGR & pllQEnable;
        bool has_q = RCC->PLLCFGR & pllPEnable;
        bool has_r = RCC->PLLCFGR & pllREnable;
        RCC->PLLCFGR &= ~(pllQEnable | pllPEnable | pllREnable); // Disable all the PLLs
        RCC->PLLCFGR &= ~pllSrcMask;
        uint32_t sourceFreq = ClockManager<LL>::getClock(source)->freq;

        // Pick the best scaler/divider combo we can
        uint8_t n = 1;
        uint8_t m = 1;
        int32_t prevDiff = 0x7FFFFFFF; //something really big, so anything is better
        for (uint8_t i = 1; i <= 16; i++) {
            uint32_t currentN = (targetFreqKhz * i) / sourceFreq;
            if (currentN > 127) {
                currentN = 127;
            } else if (currentN <= 8) {
                currentN = 8;
            }
            int32_t currentDiff = targetFreqKhz - sourceFreq * currentN / i;
            if (currentDiff < 0) {
                currentDiff = -currentDiff;
            }
            if (currentDiff < prevDiff) {
                prevDiff = currentDiff;
                n = currentN;
                m = i;
            }
        }
        freq = sourceFreq * n / m;
        uint32_t cfg = RCC->PLLCFGR;
        cfg &= ~pllDividerMask;
        cfg |= n << 8;
        cfg |= (m - 1) << 4;
        if (source == HSI) {
            cfg |= pllSrcHSI16;
        } else if (source == HSE) {
            cfg |= pllSrcHSE;
        }
        RCC->PLLCFGR = cfg;
        RCC->CR |= pllEnable; // Enable PLL
        while (!(RCC->CR & pllReady))
            ;
        if (has_p) {
            ClockManager<LL>::getClock(PLL_P)->set(ClockManager<LL>::getClock(PLL_P)->freq, VCO);
        }
        if (has_q) {
            ClockManager<LL>::getClock(PLL_Q)->set(ClockManager<LL>::getClock(PLL_Q)->freq, VCO);
        }
        if (has_r) {
            ClockManager<LL>::getClock(PLL_R)->set(ClockManager<LL>::getClock(PLL_R)->freq, VCO);
        }
        return 0;
    } else if (clock == PLL_P) {
        const uint32_t pllEnable = 0x01000000;
        const uint32_t pllReady = 0x02000000;

        const uint32_t pllPEnable = 0x00010000;
        const uint32_t pllPDivMask = 0xF8000000;

        if (targetFreqKhz > 170000) {
            return -1;
        } else if (targetFreqKhz < 2065) {
            return -1;
        }
        if (ClockManager<LL>::getClock(VCO)->freq < targetFreqKhz * 2) {
            return -1;
        }

        RCC->CR &= ~pllEnable; // Disable PLL (needed to change things)
        uint8_t div = ClockManager<LL>::getClock(VCO)->freq / targetFreqKhz;
        if (div > 31) {
            div = 31;
        } else if (div <= 1) {
            div = 2;
        }
        uint32_t cfg = RCC->PLLCFGR;
        cfg &= ~pllPDivMask;
        cfg |= pllPEnable;
        cfg |= (div - 1) << 27;
        RCC->PLLCFGR = cfg;
        freq = ClockManager<LL>::getClock(VCO)->freq / div;
        RCC->CR |= pllEnable; // Enable PLL
        while (!(RCC->CR & pllReady))
            ;
        return 0;
    } else if (clock == PLL_Q) {
        const uint32_t pllEnable = 0x01000000;
        const uint32_t pllReady = 0x02000000;

        const uint32_t pllQEnable = 0x00100000;
        const uint32_t pllQDivMask = 0x00600000;

        if (targetFreqKhz > 170000) {
            return -1;
        } else if (targetFreqKhz < 8000) {
            return -1;
        }
        if (ClockManager<LL>::getClock(VCO)->freq < targetFreqKhz * 2) {
            return -1;
        }

        RCC->CR &= ~pllEnable; // Disable PLL (needed to change things)
        uint8_t div = ClockManager<LL>::getClock(VCO)->freq / 2 / targetFreqKhz;
        if (div > 4) {
            div = 4;
        } else if (div <= 1) {
            div = 1;
        }
        uint32_t cfg = RCC->PLLCFGR;
        cfg &= ~pllQDivMask;
        cfg |= pllQEnable;
        cfg |= (div - 1) << 21;
        RCC->PLLCFGR = cfg;
        freq = ClockManager<LL>::getClock(VCO)->freq / div / 2;
        RCC->CR |= pllEnable; // Enable PLL
        while (!(RCC->CR & pllReady))
            ;
        return 0;
    } else if (clock == PLL_R) {
        const uint32_t pllEnable = 0x01000000;
        const uint32_t pllReady = 0x02000000;

        const uint32_t pllREnable = 0x01000000;
        const uint32_t pllRDivMask = 0x06000000;

        if (targetFreqKhz > 170000) {
            return -1;
        } else if (targetFreqKhz < 8000) {
            return -1;
        }
        if (ClockManager<LL>::getClock(VCO)->freq < targetFreqKhz * 2) {
            return -1;
        }

        RCC->CR &= ~pllEnable; // Disable PLL (needed to change things)
        uint8_t div = ClockManager<LL>::getClock(VCO)->freq / 2 / targetFreqKhz;
        if (div > 4) {
            div = 4;
        } else if (div <= 1) {
            div = 1;
        }
        uint32_t cfg = RCC->PLLCFGR;
        cfg &= ~pllRDivMask;
        cfg |= pllREnable;
        cfg |= (div - 1) << 25;
        RCC->PLLCFGR = cfg;
        freq = ClockManager<LL>::getClock(VCO)->freq / div / 2;
        RCC->CR |= pllEnable; // Enable PLL
        while (!(RCC->CR & pllReady))
            ;
        return 0;
    } else if (clock == HCLK) {
        const uint32_t hclkPrescalerMask = 0xF0;

        uint32_t sourceFreq = ClockManager<LL>::getClock(SysClock)->freq;
        uint32_t bestDiff = 0xFFFFFFFF;
        uint8_t presc = 0;
        for (uint8_t i = 0; i <= 8; ++i) {
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
            presc += 7;
        }
        uint32_t cfgr = RCC->CFGR;
        cfgr &= ~hclkPrescalerMask;
        cfgr |= presc << 4;
        RCC->CFGR = cfgr;
        return 0;
    } else if (clock == PCLK_1) {
        const uint32_t pclk1PrescalerMask = 0x700;

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
        cfgr &= ~pclk1PrescalerMask;
        cfgr |= presc << 8;
        RCC->CFGR = cfgr;
        return 0;
    } else if (clock == PCLK_2) {
        const uint32_t pclk2PrescalerMask = 0x3800;

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
        cfgr &= ~pclk2PrescalerMask;
        cfgr |= presc << 11;
        RCC->CFGR = cfgr;
        return 0;
    } else {
        return -1;
    }
}
#endif /* SRC_MAIN_C___LOWLEVEL_CLOCKSLOWLEVEL_SPECIFIC_CLOCKCPP_HPP_ */
