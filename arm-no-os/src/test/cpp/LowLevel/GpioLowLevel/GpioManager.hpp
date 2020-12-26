/*
 * GpioManager.hpp
 *
 *  Created on: 23 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_INCLUDE_LOWLEVEL_GPIOLOWLEVEL_SPECIFIC_GPIOMANAGER_HPP_
#define SRC_TEST_CPP_INCLUDE_LOWLEVEL_GPIOLOWLEVEL_SPECIFIC_GPIOMANAGER_HPP_

#include "../GeneralHalSetup.hpp"
#include "specific/GpioNames.hpp"
#include "Gpio.hpp"

class GpioManager {
    static GpioPin pins[GeneralHalSetup::pinCount];
    public:
    static void init();

    static void activateClock(GpioPinName name);

    static GpioPin* getPin(GpioPinName name);
};

#include "stm32g4xx.h"
#include "stm32g484xx.h"
#endif /* SRC_TEST_CPP_INCLUDE_LOWLEVEL_GPIOLOWLEVEL_SPECIFIC_GPIOMANAGER_HPP_ */
