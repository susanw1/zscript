/*
 * AtoDManager.hpp
 *
 *  Created on: 5 Jan 2021
 *      Author: robert
 */

#ifndef SRC_TEST_C___LOWLEVEL_ATODLOWLEVEL_ATODMANAGER_HPP_
#define SRC_TEST_C___LOWLEVEL_ATODLOWLEVEL_ATODMANAGER_HPP_

#include <arm-no-os/llIncludes.hpp>
#include "AtoD.hpp"
#include <arm-no-os/system/clock/ClockManager.hpp>
#include <arm-no-os/pins-module/lowlevel/GpioManager.hpp>

template<class LL>
class AtoDManager {
    typedef typename LL::HW HW;
    static AtoD<LL> atoDs[HW::atoDCount];

    AtoDManager() {
    }

public:
    static void init();

    static uint16_t performAtoD(GpioPinName pin);

    static bool canPerformAtoD(GpioPinName pin);

    static AtoD<LL>* getAtoD(uint8_t aToD) {
        return atoDs + aToD;
    }
};
#include "specific/AtoDManagercpp.hpp"

#endif /* SRC_TEST_C___LOWLEVEL_ATODLOWLEVEL_ATODMANAGER_HPP_ */
