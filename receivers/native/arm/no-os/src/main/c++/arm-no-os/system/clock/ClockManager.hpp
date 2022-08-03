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
        ClockManager<LL>::getClock(VCO)->set(240000, HSI);
        ClockManager<LL>::getClock(PLL_R)->set(120000, VCO);
        ClockManager<LL>::getClock(SysClock)->set(120000, PLL_R);
        ClockManager<LL>::getClock(HCLK)->set(120000, SysClock);
        ClockManager<LL>::getClock(PCLK_1)->set(60000, HCLK);
        ClockManager<LL>::getClock(PCLK_2)->set(60000, HCLK);
    }
};
#include "specific/ClockManagercpp.hpp"

#undef CLOCKLOWLEVEL_NO_CPP

#include "specific/Clockcpp.hpp"

#endif /* SRC_TEST_CPP_LOWLEVEL_CLOCKSLOWLEVEL_CLOCKMANAGER_HPP_ */
