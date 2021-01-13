/*
 * AtoDManager.hpp
 *
 *  Created on: 5 Jan 2021
 *      Author: robert
 */

#ifndef SRC_TEST_C___LOWLEVEL_ATODLOWLEVEL_ATODMANAGER_HPP_
#define SRC_TEST_C___LOWLEVEL_ATODLOWLEVEL_ATODMANAGER_HPP_
#include "../GeneralLLSetup.hpp"
#include "../ClocksLowLevel/ClockManager.hpp"
#include "../GpioLowLevel/GpioManager.hpp"
#include "AtoD.hpp"

class AtoDManager {
    static AtoD atoDs[GeneralHalSetup::atoDCount];
    AtoDManager() {

    }
public:
    static void init();
    static uint16_t performAtoD(GpioPinName pin);
    static bool canPerformAtoD(GpioPinName pin);

    static AtoD* getAtoD(uint8_t aToD) {
        return atoDs + aToD;
    }
};

#endif /* SRC_TEST_C___LOWLEVEL_ATODLOWLEVEL_ATODMANAGER_HPP_ */
