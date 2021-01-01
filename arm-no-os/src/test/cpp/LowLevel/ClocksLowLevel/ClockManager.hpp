/*
 * ClockManager.hpp
 *
 *  Created on: 24 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_LOWLEVEL_CLOCKSLOWLEVEL_CLOCKMANAGER_HPP_
#define SRC_TEST_CPP_LOWLEVEL_CLOCKSLOWLEVEL_CLOCKMANAGER_HPP_
#include "../GeneralLLSetup.hpp"
#include "Clock.hpp"

class ClockManager {
    static Clock clocks[GeneralHalSetup::systemClockCount];
    public:
    static Clock* getClock(SystemClock clock) {
        return clocks + (int) clock;
    }
};
#endif /* SRC_TEST_CPP_LOWLEVEL_CLOCKSLOWLEVEL_CLOCKMANAGER_HPP_ */
