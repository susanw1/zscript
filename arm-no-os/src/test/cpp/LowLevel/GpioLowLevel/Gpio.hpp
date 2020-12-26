/*
 * Gpio.hpp
 *
 *  Created on: 23 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_INCLUDE_LOWLEVEL_GPIOLOWLEVEL_GPIO_HPP_
#define SRC_TEST_CPP_INCLUDE_LOWLEVEL_GPIOLOWLEVEL_GPIO_HPP_

#include "../GeneralHalSetup.hpp"
#include "specific/GpioPort.hpp"

class GpioPin {
private:
    GpioPort *port;
    GpioPinName pin;
    bool lockBool;
    public:
    GpioPin(GpioPort *port, GpioPinName pin) :
            port(port), pin(pin), lockBool(false) {
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
};
#include "GpioManager.hpp"

#endif /* SRC_TEST_CPP_INCLUDE_LOWLEVEL_GPIOLOWLEVEL_GPIO_HPP_ */
