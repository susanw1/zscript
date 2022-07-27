/*
 * ClockManager.hpp
 *
 *  Created on: 24 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_LOWLEVEL_CLOCKSLOWLEVEL_CLOCKMANAGER_HPP_
#define SRC_TEST_CPP_LOWLEVEL_CLOCKSLOWLEVEL_CLOCKMANAGER_HPP_

#define CLOCKLOWLEVEL_NO_CPP
#include <arm-no-os/llIncludes.hpp>
#include "Clock.hpp"

template<class LL>
class ClockManager {
private:
    static Clock<LL> clocks[LL::systemClockCount];

public:
    static Clock<LL>* getClock(SystemClock clock) {
        return clocks + (uint8_t) clock;
    }
};
#include "specific/ClockManagercpp.hpp"

#undef CLOCKLOWLEVEL_NO_CPP

#include "specific/Clockcpp.hpp"

#endif /* SRC_TEST_CPP_LOWLEVEL_CLOCKSLOWLEVEL_CLOCKMANAGER_HPP_ */