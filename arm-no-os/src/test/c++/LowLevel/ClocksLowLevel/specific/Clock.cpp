/*
 * Clock.cpp
 *
 *  Created on: 26 Dec 2020
 *      Author: robert
 */

#include "../Clock.hpp"

void activateHSI48() {
    RCC->CRRCR |= 0x1;
    while (!(RCC->CRRCR & 0x2))
        ;
    return;
}
void activateHSI() {
    RCC->CR |= 0x100;
    while (!(RCC->CR & 0x400))
        ;
    return;
}
void activateLSI() {
    RCC->CSR |= 1;
    while (!(RCC->CSR & 2))
        ;
    return;
}

int Clock::set(uint32_t targetFreqKhz, SystemClock source) {
    if (clock == SysClock) {
        uint32_t cfgr = RCC->CFGR;
        if (source == PLL_R && (ClockManager::getClock(source)->freq != 0 || ClockManager::getClock(source)->set(targetFreqKhz, NONE) == 0)) {
            freq = ClockManager::getClock(source)->freq;
            this->source = source;
            cfgr |= 0x03;
            RCC->CFGR = cfgr;
            return 0;
        } else if (source == HSI && (ClockManager::getClock(source)->freq != 0 || ClockManager::getClock(source)->set(targetFreqKhz, NONE) == 0)) {
            freq = ClockManager::getClock(source)->freq;
            this->source = source;
            cfgr &= ~0x3;
            cfgr |= 0x01;
            RCC->CFGR = cfgr;
            return 0;
        } else if (source == HSE && (ClockManager::getClock(source)->freq != 0 || ClockManager::getClock(source)->set(targetFreqKhz, NONE) == 0)) {
            freq = ClockManager::getClock(source)->freq;
            this->source = source;
            cfgr &= ~0x3;
            cfgr |= 0x02;
            RCC->CFGR = cfgr;
            return 0;
        } else {
            return -1;
        }
    } else if (clock == HSI) {
        activateHSI();
        freq = 16000;
        return 0;
    } else if (clock == HSI48) {
        activateHSI48();
        freq = 48000;
        return 0;
    } else if (clock == LSI) {
        activateLSI();
        freq = 32;
        return 0;
    } else if (clock == HSE) {
        return -1;
    } else if (clock == LSE) {
        return -1;
    } else if (clock == VCO) {
        if (targetFreqKhz > 344000) {
            return -1;
        }
        if (source == HSI && (ClockManager::getClock(source)->freq != 0 || ClockManager::getClock(source)->set(targetFreqKhz, NONE) == 0)) {
            this->source = source;
        } else if (source == HSE && (ClockManager::getClock(source)->freq != 0 || ClockManager::getClock(source)->set(targetFreqKhz, NONE) == 0)) {
            this->source = source;
        } else {
            return -1;
        }
        RCC->CR &= ~0x01000000; // Disable PLL (needed to change things)
        bool has_p = RCC->PLLCFGR & 0x00010000;
        bool has_q = RCC->PLLCFGR & 0x00100000;
        bool has_r = RCC->PLLCFGR & 0x01000000;
        RCC->PLLCFGR &= ~0x01110000; // Disable all the PLLs
        RCC->PLLCFGR &= ~0x00000003;
        uint32_t sourceFreq = ClockManager::getClock(source)->freq;

        // Pick the best scaler/divider combo we can
        uint8_t n = 1;
        uint8_t m = 1;
        int32_t prevDiff = 0x7FFFFFFF;
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
        cfg &= ~0x7ff3;
        cfg |= n << 8;
        cfg |= m << 4;
        if (source == HSI) {
            cfg |= 2;
        } else if (source == HSE) {
            cfg |= 3;
        }
        RCC->PLLCFGR = cfg;
        RCC->CR |= 0x01000000; // Enable PLL
        while (!(RCC->CR & 0x02000000))
            ;
        if (has_p) {
            ClockManager::getClock(PLL_P)->set(ClockManager::getClock(PLL_P)->freq, VCO);
        }
        if (has_q) {
            ClockManager::getClock(PLL_Q)->set(ClockManager::getClock(PLL_Q)->freq, VCO);
        }
        if (has_r) {
            ClockManager::getClock(PLL_R)->set(ClockManager::getClock(PLL_R)->freq, VCO);
        }
        return 0;
    } else if (clock == PLL_P) {
        if (targetFreqKhz > 170000) {
            return -1;
        } else if (targetFreqKhz < 2065) {
            return -1;
        }
        if (ClockManager::getClock(VCO)->freq < targetFreqKhz * 2) {
            return -1;
        }
        uint8_t div = ClockManager::getClock(VCO)->freq / targetFreqKhz;
        if (div > 31) {
            div = 31;
        } else if (div <= 1) {
            div = 2;
        }
        uint32_t cfg = RCC->PLLCFGR;
        cfg &= ~0xF8000000;
        cfg |= 0x10000;
        cfg |= div << 27;
        RCC->PLLCFGR = cfg;
        freq = ClockManager::getClock(VCO)->freq / div;
        return 0;
    } else if (clock == PLL_Q) {
        if (targetFreqKhz > 170000) {
            return -1;
        } else if (targetFreqKhz < 8000) {
            return -1;
        }
        if (ClockManager::getClock(VCO)->freq < targetFreqKhz * 2) {
            return -1;
        }

        uint8_t div = ClockManager::getClock(VCO)->freq / 2 / targetFreqKhz;
        if (div > 4) {
            div = 4;
        } else if (div <= 1) {
            div = 1;
        }
        uint32_t cfg = RCC->PLLCFGR;
        cfg &= ~0x00600000;
        cfg |= 0x100000;
        cfg |= div << 21;
        RCC->PLLCFGR = cfg;
        freq = ClockManager::getClock(VCO)->freq / div / 2;
        return 0;
    } else if (clock == PLL_R) {
        if (targetFreqKhz > 170000) {
            return -1;
        } else if (targetFreqKhz < 8000) {
            return -1;
        }
        if (ClockManager::getClock(VCO)->freq < targetFreqKhz * 2) {
            return -1;
        }

        uint8_t div = ClockManager::getClock(VCO)->freq / 2 / targetFreqKhz;
        if (div > 4) {
            div = 4;
        } else if (div <= 1) {
            div = 1;
        }
        uint32_t cfg = RCC->PLLCFGR;
        cfg &= ~0x06000000;
        cfg |= 0x01000000;
        cfg |= div << 25;
        RCC->PLLCFGR = cfg;
        freq = ClockManager::getClock(VCO)->freq / div / 2;
        return 0;
    } else if (clock == HCLK) {
        uint32_t sourceFreq = ClockManager::getClock(SysClock)->freq;
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
        cfgr &= ~0xF0;
        cfgr |= presc << 4;
        RCC->CFGR = cfgr;
        return 0;
    } else if (clock == PCLK_1) {
        uint32_t sourceFreq = ClockManager::getClock(HCLK)->freq;
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
        cfgr &= ~0x700;
        cfgr |= presc << 8;
        RCC->CFGR = cfgr;
        return 0;
    } else if (clock == PCLK_2) {
        uint32_t sourceFreq = ClockManager::getClock(HCLK)->freq;
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
        cfgr &= ~0x3100;
        cfgr |= presc << 11;
        RCC->CFGR = cfgr;
        return 0;
    } else {
        return -1;
    }
}
