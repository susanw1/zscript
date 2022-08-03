/*
 * GpioManager.hpp
 *
 *  Created on: 23 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_INCLUDE_LOWLEVEL_GPIOLOWLEVEL_SPECIFIC_GPIOMANAGER_HPP_
#define SRC_TEST_CPP_INCLUDE_LOWLEVEL_GPIOLOWLEVEL_SPECIFIC_GPIOMANAGER_HPP_
#define GPIOLOWLEVEL_NO_CPP
#include <arm-no-os/llIncludes.hpp>
#include "Gpio.hpp"
#include "specific/GpioNames.hpp"

template<class LL>
class GpioManager {
private:
    typedef typename LL::HW HW;
    static GpioPin<LL> pins[HW::pinCount];

public:
    static void init();

    static void activateClock(GpioPinName name);

    static GpioPin<LL> getPin(GpioPinName name);

    static GpioPin<LL> getPin(uint8_t id) {
        if (id >= HW::pinCount) {
            return pins[0];
        }
        return pins[id];
    }

    static GpioPort* getPort(GpioPinName name);
};

#include "specific/GpioManagercpp.hpp"
#undef GPIOLOWLEVEL_NO_CPP
#include "specific/Gpiocpp.hpp"
#endif /* SRC_TEST_CPP_INCLUDE_LOWLEVEL_GPIOLOWLEVEL_SPECIFIC_GPIOMANAGER_HPP_ */
