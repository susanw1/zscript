/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_STM32F0_SYSTEM_CLOCK_CLOCK_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_STM32F0_SYSTEM_CLOCK_CLOCK_HPP_

#include <arm-no-os/llIncludes.hpp>
#include "specific/SystemClocks.hpp"

template<class LL>
class Clock {
private:
    uint32_t freq;
    uint8_t clock;
    uint8_t source;

public:
    Clock(uint32_t freq,
            SystemClock clock,
            SystemClock source) :
            freq(freq), clock((uint8_t) clock), source((uint8_t) source) {
    }

    uint32_t getDivider(uint32_t targetFreqKhz) {
        return freq / targetFreqKhz;
    }

    int set(uint32_t targetFreqKhz, SystemClock source);

    uint32_t getFreqKhz() {
        return freq;
    }

    SystemClock getSource() {
        return source;
    }
};
#ifndef CLOCKLOWLEVEL_NO_CPP
#include "specific/Clockcpp.hpp"
#endif

#include "ClockManager.hpp"

#endif /* SRC_MAIN_CPP_ARM_NO_OS_STM32F0_SYSTEM_CLOCK_CLOCK_HPP_ */
