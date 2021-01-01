/*
 * Clock.hpp
 *
 *  Created on: 24 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_LOWLEVEL_CLOCKSLOWLEVEL_CLOCK_HPP_
#define SRC_TEST_CPP_LOWLEVEL_CLOCKSLOWLEVEL_CLOCK_HPP_
#include "../GeneralLLSetup.hpp"
#include "specific/SystemClocks.hpp"

class Clock {
    uint32_t freq;
    SystemClock clock;
    SystemClock source;

public:
    Clock(uint32_t freq,
            SystemClock clock,
            SystemClock source) :
            freq(freq), clock(clock), source(source) {
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

#include "ClockManager.hpp"

#endif /* SRC_TEST_CPP_LOWLEVEL_CLOCKSLOWLEVEL_CLOCK_HPP_ */
