/*
 * Gpio.hpp
 *
 *  Created on: 23 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_INCLUDE_LOWLEVEL_GPIOLOWLEVEL_GPIO_HPP_
#define SRC_TEST_CPP_INCLUDE_LOWLEVEL_GPIOLOWLEVEL_GPIO_HPP_

#include <arm-no-os/llIncludes.hpp>
#include "specific/GpioPort.hpp"

template<class LL>
class GpioPin {
private:
    GpioPinName pin;

public:
    GpioPin(GpioPinName pin) :
            pin(pin) {
    }
    void init();

    void write(bool value);

    void set();

    void reset();

    bool read();

    void setMode(PinMode mode);

    void setOutputMode(OutputMode mode);

    void setOutputSpeed(PinSpeed speed);

    void setPullMode(PullMode mode);

    void setAlternateFunction(uint8_t function);

    GpioPinName getPinName() {
        return pin;
    }
};

#include "GpioManager.hpp"

#ifndef GPIOLOWLEVEL_NO_CPP
#include "specific/Gpiocpp.hpp"
#endif

#endif /* SRC_TEST_CPP_INCLUDE_LOWLEVEL_GPIOLOWLEVEL_GPIO_HPP_ */
