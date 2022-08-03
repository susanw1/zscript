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
    static Clock<LL> clocks[NONE];

public:
    static Clock<LL>* getClock(SystemClock clock) {
        return clocks + (uint8_t) clock;
    }
    static void basicSetup() {
        ClockManager<GeneralHalSetup>::getClock(HSI)->set(8000, NONE);
        ClockManager<GeneralHalSetup>::getClock(PLL)->set(40000, HSI);
        ClockManager<GeneralHalSetup>::getClock(SysClock)->set(40000, PLL);
        ClockManager<GeneralHalSetup>::getClock(HCLK)->set(40000, SysClock);
        ClockManager<GeneralHalSetup>::getClock(PCLK)->set(20000, HCLK);
    }
};
#include "specific/ClockManagercpp.hpp"

#undef CLOCKLOWLEVEL_NO_CPP

#include "specific/Clockcpp.hpp"

#endif /* SRC_TEST_CPP_LOWLEVEL_CLOCKSLOWLEVEL_CLOCKMANAGER_HPP_ */
