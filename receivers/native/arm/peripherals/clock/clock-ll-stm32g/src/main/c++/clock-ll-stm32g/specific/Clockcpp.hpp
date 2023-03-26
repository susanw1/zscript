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
int8_t setConstantClock(uint32_t *dest, SystemClock clock, uint32_t targetFreqKhz, ClockStrategy strategy, uint32_t clockFreq) {
    if (strategy == EXACT_FREQUENCY && targetFreqKhz != clockFreq) {
        return -1;
    }
    if (!ClockManager<LL>::regs->enable(clock)) {
        return -1;
    }
    *dest = clockFreq;
    return 0;
}

template<class LL>
int8_t findPowerScale(uint32_t targetFreqKhz, uint32_t sourceFreq, ClockStrategy strategy, uint8_t maxPower, uint8_t skipPower) {
    uint32_t bestDiff = 0xFFFFFFFF;
    uint8_t presc = -1;
    for (uint8_t i = 0; i <= maxPower; ++i) {
        if (i == skipPower) {
            continue;
        }

        uint32_t resultFreq = sourceFreq / (1 << i);
        if (resultFreq > targetFreqKhz) {
            if (strategy != CLOSEST_FREQUENCY_BELOW && resultFreq - targetFreqKhz < bestDiff) {
                bestDiff = resultFreq - targetFreqKhz;
                presc = i;
            }
        } else {
            if (strategy != CLOSEST_FREQUENCY_ABOVE && targetFreqKhz - resultFreq < bestDiff) {
                bestDiff = targetFreqKhz - resultFreq;
                presc = i;
            }
        }
    }
    return presc;
}
template<class LL>
int8_t findPowerScale(uint32_t targetFreqKhz, uint32_t sourceFreq, ClockStrategy strategy, uint8_t maxPower) {
    return findPowerScale<LL>(targetFreqKhz, sourceFreq, strategy, maxPower, (uint8_t) 0xFF);
}
template<class LL>
int8_t findDivScale(uint32_t targetFreqKhz, uint32_t sourceFreq, ClockStrategy strategy, uint8_t maxDiv) {
    uint32_t bestDiff = 0xFFFFFFFF;
    uint8_t presc = -1;
    for (uint8_t i = 1; i <= maxDiv; ++i) {

        uint32_t resultFreq = sourceFreq / i;
        if (resultFreq > targetFreqKhz) {
            if (strategy != CLOSEST_FREQUENCY_BELOW && resultFreq - targetFreqKhz < bestDiff) {
                bestDiff = resultFreq - targetFreqKhz;
                presc = i;
            }
        } else {
            if (strategy != CLOSEST_FREQUENCY_ABOVE && targetFreqKhz - resultFreq < bestDiff) {
                bestDiff = targetFreqKhz - resultFreq;
                presc = i;
            }
        }
    }
    return presc;
}

template<class LL>
void vcoClockDie() {
    Clock<LL> *vcoClk = ClockManager<LL>::getClock(VCO);
    Clock<LL> *pllPclk = ClockManager<LL>::getClock(PLL_P);
    Clock<LL> *pllQclk = ClockManager<LL>::getClock(PLL_Q);
    Clock<LL> *pllRclk = ClockManager<LL>::getClock(PLL_R);
    vcoClk->freq = 0;
    vcoClk->source = (uint8_t) NONE;

    pllPclk->freq = 0;
    pllPclk->source = (uint8_t) NONE;

    pllQclk->freq = 0;
    pllQclk->source = (uint8_t) NONE;

    pllRclk->freq = 0;
    pllRclk->source = (uint8_t) NONE;
}

template<class LL>
int32_t setVcoClock(uint32_t targetFreqKhz, SystemClock source, ClockStrategy strategy) {
    uint32_t sourceFreq = ClockManager<LL>::getClock(source)->freq;

    // Pick the best scaler/divider combo we can
    uint8_t n = 1;
    uint8_t m = 1;
    int32_t prevDiff = 0x7FFFFFFF; //something really big, so anything is better
    for (uint8_t i = 8; i <= 127; i++) {
        uint32_t inFreq = sourceFreq * i;
        int8_t currentM = findDivScale<LL>(targetFreqKhz, inFreq, strategy, 16);
        if (currentM == -1) {
            continue;
        }
        int32_t currentDiff = targetFreqKhz - inFreq / currentM;
        if (currentDiff < 0) {
            currentDiff = -currentDiff;
        }
        if (currentDiff < prevDiff) {
            prevDiff = currentDiff;
            n = i;
            m = currentM;
        }
    }
    if (strategy == EXACT_FREQUENCY && sourceFreq * n / m != targetFreqKhz) {
        return -1;
    }
    ClockManager<LL>::regs->disable(VCO); // Disable PLL (needed to change things)
    bool has_p = ClockManager<LL>::regs->isEnabled(PLL_P);
    bool has_q = ClockManager<LL>::regs->isEnabled(PLL_Q);
    bool has_r = ClockManager<LL>::regs->isEnabled(PLL_R);
    ClockManager<LL>::regs->disable(PLL_P);
    ClockManager<LL>::regs->disable(PLL_Q);
    ClockManager<LL>::regs->disable(PLL_R);
    ClockManager<LL>::regs->setPllRatio(n, m - 1, source);
    if (!ClockManager<LL>::regs->enable(VCO)) {
        return -1;
    }
    if (has_p) {
        Clock<LL> *pllPclk = ClockManager<LL>::getClock(PLL_P);
        pllPclk->set(pllPclk->freq, VCO, pllPclk->strategy);
    }
    if (has_q) {
        Clock<LL> *pllQclk = ClockManager<LL>::getClock(PLL_Q);
        pllQclk->set(pllQclk->freq, VCO, pllQclk->strategy);
    }
    if (has_r) {
        Clock<LL> *pllRclk = ClockManager<LL>::getClock(PLL_R);
        pllRclk->set(pllRclk->freq, VCO, pllRclk->strategy);
    }
    return sourceFreq * n / m;
}

template<class LL>
int8_t Clock<LL>::set(uint32_t targetFreqKhz, SystemClock source, ClockStrategy strategy) {
    int8_t div;
    uint32_t sourceFreq;
    int8_t presc;
    this->strategy = (uint8_t) strategy;
    switch (clock) {
    case SysClock:
        if (source != PLL_R && source != HSI && source != HSE) {
            return -1;
        }
        if (ClockManager<LL>::getClock(source)->freq == 0 && ClockManager<LL>::getClock(source)->set(targetFreqKhz, NONE, strategy) != 0) {
            return -1;
        }
        if (strategy == EXACT_FREQUENCY && targetFreqKhz != ClockManager<LL>::getClock(source)->freq) {
            return -1;
        }
        freq = ClockManager<LL>::getClock(source)->freq;
        this->source = (uint8_t) source;
        ClockManager<LL>::regs->selectSysClock(source);
        return 0;
    case HSI:
        return setConstantClock<LL>(&freq, clock, targetFreqKhz, strategy, 16000);
    case HSI48:
        return setConstantClock<LL>(&freq, clock, targetFreqKhz, strategy, 48000);
    case LSI:
        return setConstantClock<LL>(&freq, clock, targetFreqKhz, strategy, 32);
    case HSE:
        if (!ClockManager<LL>::regs->enable(HSE)) {
            return -1;
        }
        freq = targetFreqKhz;
        return 0;
    case LSE:
        if (!ClockManager<LL>::regs->enable(LSE)) {
            return -1;
        }
        freq = targetFreqKhz;
        return 0;
    case VCO:
        if (targetFreqKhz > 344000) {
            return -1;
        }
        if (source != HSI && source != HSE) {
            return -1;
        }
        if ((ClockManager<LL>::getClock(source)->freq == 0 && ClockManager<LL>::getClock(source)->set(targetFreqKhz, NONE, strategy) != 0)) {
            return -1;
        }
        this->source = (uint8_t) source;
        int32_t result = setVcoClock<LL>(targetFreqKhz, source, strategy);
        if (result == -1) {
            vcoClockDie<LL>();
            return -1;
        }
        freq = result;
        return 0;
    case PLL_P:
        if (targetFreqKhz > 170000) {
            return -1;
        } else if (targetFreqKhz < 2065) {
            return -1;
        }
        if (ClockManager<LL>::getClock(VCO)->freq < targetFreqKhz * 2) {
            return -1;
        }

        div = findDivScale<LL>(targetFreqKhz, ClockManager<LL>::getClock(VCO)->freq, strategy, 31);
        if (div == -1) {
            return -1;
        }
        if (div <= 1) {
            div = 2;
        }
        if (strategy == EXACT_FREQUENCY && targetFreqKhz != ClockManager<LL>::getClock(VCO)->freq / div) {
            return -1;
        }
        ClockManager<LL>::regs->disable(VCO); // Disable PLL (needed to change things)
        ClockManager<LL>::regs->setPllPDiv(div);
        freq = ClockManager<LL>::getClock(VCO)->freq / div;
        if (!ClockManager<LL>::regs->enable(VCO)) {
            vcoClockDie<LL>();
            return -1;
        }
        return 0;
    case PLL_Q:
        if (targetFreqKhz > 170000) {
            return -1;
        } else if (targetFreqKhz < 8000) {
            return -1;
        }
        if (ClockManager<LL>::getClock(VCO)->freq < targetFreqKhz * 2) {
            return -1;
        }

        div = findDivScale<LL>(targetFreqKhz, ClockManager<LL>::getClock(VCO)->freq / 2, strategy, 4);
        if (div == -1) {
            return -1;
        }
        if (strategy == EXACT_FREQUENCY && targetFreqKhz != ClockManager<LL>::getClock(VCO)->freq / 2 / div) {
            return -1;
        }
        ClockManager<LL>::regs->disable(VCO); // Disable PLL (needed to change things)
        ClockManager<LL>::regs->setPllQDiv(div);
        freq = ClockManager<LL>::getClock(VCO)->freq / div / 2;
        if (!ClockManager<LL>::regs->enable(VCO)) {
            vcoClockDie<LL>();
            return -1;
        }
        return 0;
    case PLL_R:
        if (targetFreqKhz > 170000) {
            return -1;
        } else if (targetFreqKhz < 8000) {
            return -1;
        }
        if (ClockManager<LL>::getClock(VCO)->freq < targetFreqKhz * 2) {
            return -1;
        }

        div = findDivScale<LL>(targetFreqKhz, ClockManager<LL>::getClock(VCO)->freq / 2, strategy, 4);
        if (div == -1) {
            return -1;
        }
        if (strategy == EXACT_FREQUENCY && targetFreqKhz != ClockManager<LL>::getClock(VCO)->freq / 2 / div) {
            return -1;
        }
        ClockManager<LL>::regs->disable(VCO); // Disable PLL (needed to change things)
        ClockManager<LL>::regs->setPllRDiv(div);
        freq = ClockManager<LL>::getClock(VCO)->freq / div / 2;
        if (!ClockManager<LL>::regs->enable(VCO)) {
            vcoClockDie<LL>();
            return -1;
        }
        return 0;
    case HCLK:
        sourceFreq = ClockManager<LL>::getClock(SysClock)->freq;
        presc = findPowerScale<LL>(targetFreqKhz, sourceFreq, strategy, 9, 4);
        if (presc == -1) {
            return -1;
        }
        if (strategy == EXACT_FREQUENCY && targetFreqKhz != sourceFreq / (1 << presc)) {
            return -1;
        }
        freq = sourceFreq / (1 << presc);
        if (presc > 4) {
            presc--;
        }
        if (presc != 0) {
            presc += 7;
        }
        ClockManager<LL>::regs->setHclkPresc(presc);
        return 0;
    case PCLK_1:
        sourceFreq = ClockManager<LL>::getClock(HCLK)->freq;
        presc = findPowerScale<LL>(targetFreqKhz, sourceFreq, strategy, 4);
        if (presc == -1) {
            return -1;
        }
        if (strategy == EXACT_FREQUENCY && targetFreqKhz != sourceFreq / (1 << presc)) {
            return -1;
        }
        freq = sourceFreq / (1 << presc);
        if (presc != 0) {
            presc += 3;
        }
        ClockManager<LL>::regs->setPclk1Presc(presc);
        return 0;
    case PCLK_2:
        sourceFreq = ClockManager<LL>::getClock(HCLK)->freq;
        presc = findPowerScale<LL>(targetFreqKhz, sourceFreq, strategy, 4);
        if (presc == -1) {
            return -1;
        }
        if (strategy == EXACT_FREQUENCY && targetFreqKhz != sourceFreq / (1 << presc)) {
            return -1;
        }
        freq = sourceFreq / (1 << presc);
        if (presc != 0) {
            presc += 3;
        }
        ClockManager<LL>::regs->setPclk2Presc(presc);
        return 0;
    default:
        return -1;
    }
}

#endif /* SRC_MAIN_CPP_ARM_NO_OS_STM32G4_SYSTEM_CLOCK_SPECIFIC_CLOCKCPP_HPP_ */
