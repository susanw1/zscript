/*
 * Clock.hpp
 *
 *  Created on: 24 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_LOWLEVEL_CLOCKSLOWLEVEL_CLOCK_HPP_
#define SRC_TEST_CPP_LOWLEVEL_CLOCKSLOWLEVEL_CLOCK_HPP_

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

#endif /* SRC_TEST_CPP_LOWLEVEL_CLOCKSLOWLEVEL_CLOCK_HPP_ */
