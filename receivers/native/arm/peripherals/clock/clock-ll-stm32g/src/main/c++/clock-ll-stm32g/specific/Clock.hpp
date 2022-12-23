/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_STM32G4_SYSTEM_CLOCK_CLOCK_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_STM32G4_SYSTEM_CLOCK_CLOCK_HPP_

#include <llIncludes.hpp>
#include "SystemClocks.hpp"

enum ClockStrategy {
    EXACT_FREQUENCY,
    CLOSEST_FREQUENCY,
    CLOSEST_FREQUENCY_BELOW,
    CLOSEST_FREQUENCY_ABOVE
};

template<class LL>
class Clock {
private:
    uint32_t freq;
    uint8_t clock;
    uint8_t source;
    uint8_t strategy;

public:
    Clock(uint32_t freq,
            SystemClock clock,
            SystemClock source) :
            freq(freq), clock((uint8_t) clock), source((uint8_t) source), strategy(EXACT_FREQUENCY) {
    }

    uint32_t getDivider(uint32_t targetFreqKhz) {
        return freq / targetFreqKhz;
    }

    int set(uint32_t targetFreqKhz, SystemClock source, ClockStrategy strategy);

    uint32_t getFreqKhz() {
        return freq;
    }

    SystemClock getSource() {
        return source;
    }
};
#include "Clockcpp.hpp"

#include <clock-ll/ClockManager.hpp>
#endif /* SRC_MAIN_CPP_ARM_NO_OS_STM32G4_SYSTEM_CLOCK_CLOCK_HPP_ */
