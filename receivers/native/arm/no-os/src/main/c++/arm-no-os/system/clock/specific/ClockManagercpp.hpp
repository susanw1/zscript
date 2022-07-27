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
        Clock<LL>(16000, SysClock, HSI),
        Clock<LL>(16000, HSI, NONE),
        Clock<LL>(0, HSI48, NONE),
        Clock<LL>(0, LSI, NONE),
        Clock<LL>(0, HSE, NONE),
        Clock<LL>(0, LSE, NONE),
        Clock<LL>(0, VCO, NONE),
        Clock<LL>(0, PLL_P, VCO),
        Clock<LL>(0, PLL_Q, VCO),
        Clock<LL>(0, PLL_R, VCO),
        Clock<LL>(16000, HCLK, SysClock),
        Clock<LL>(16000, PCLK_1, HCLK),
        Clock<LL>(16000, PCLK_2, HCLK)
};

#endif /* SRC_MAIN_C___LOWLEVEL_CLOCKSLOWLEVEL_SPECIFIC_CLOCKMANAGERCPP_HPP_ */
