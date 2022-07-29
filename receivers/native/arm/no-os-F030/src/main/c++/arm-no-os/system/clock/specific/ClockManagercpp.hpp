/*
 * ClockManagercpp.hpp
 *
 *  Created on: 6 Jul 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___LOWLEVEL_CLOCKSLOWLEVEL_SPECIFIC_CLOCKMANAGERCPP_HPP_
#define SRC_MAIN_C___LOWLEVEL_CLOCKSLOWLEVEL_SPECIFIC_CLOCKMANAGERCPP_HPP_

#include "../ClockManager.hpp"

template<class LL>
Clock<LL> ClockManager<LL>::clocks[NONE] = {
        Clock<LL>(8000, SysClock, HSI),
        Clock<LL>(8000, HSI, NONE),
        Clock<LL>(0, HSI14, NONE),
        Clock<LL>(0, LSI, NONE),
        Clock<LL>(0, HSE, NONE),
        Clock<LL>(0, LSE, NONE),
        Clock<LL>(0, PLL, HSI),
        Clock<LL>(8000, HCLK, SysClock),
        Clock<LL>(8000, PCLK, HCLK),
};

#endif /* SRC_MAIN_C___LOWLEVEL_CLOCKSLOWLEVEL_SPECIFIC_CLOCKMANAGERCPP_HPP_ */
