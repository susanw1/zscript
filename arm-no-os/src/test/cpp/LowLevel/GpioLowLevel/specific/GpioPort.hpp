/*
 * GpioPort.hpp
 *
 *  Created on: 23 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_INCLUDE_LOWLEVEL_GPIOLOWLEVEL_SPECIFIC_GPIOPORT_HPP_
#define SRC_TEST_CPP_INCLUDE_LOWLEVEL_GPIOLOWLEVEL_SPECIFIC_GPIOPORT_HPP_
#include "../../GeneralHalSetup.hpp"
#include "GpioNames.hpp"

enum OutputMode {
    PushPull, OpenDrain
};
enum PullMode {
    NoPull, PullUp, PullDown
};
enum PinSpeed {
    LowSpeed, MediumSpeed, HighSpeed, VeryHighSpeed
};
enum PinMode {
    Input, Output, AlternateFunction, Analog
};

class GpioPort {
private:
    volatile uint32_t MODE;
    volatile uint32_t OTYPE;
    volatile uint32_t OSPEED;
    volatile uint32_t PUPD;
    volatile uint32_t ID;
    volatile uint32_t OD;
    volatile uint32_t BSR;
    volatile uint32_t LCK;
    volatile uint32_t AFRL;
    volatile uint32_t AFRH;
    volatile uint32_t BR;
    public:
    void setMode(GpioPinName pin, PinMode mode) {
        MODE &= ~(0x3 << (pin.pin * 2));
        MODE |= (mode) << (pin.pin * 2);
    }
    void setOutputType(GpioPinName pin, OutputMode mode) {
        OTYPE &= ~(0x1 << (pin.pin));
        OTYPE |= (mode) << (pin.pin);
    }
    void setOutputSpeed(GpioPinName pin, PinSpeed mode) {
        OSPEED &= ~(0x3 << (pin.pin * 2));
        OSPEED |= (mode) << (pin.pin * 2);
    }
    void setPullMode(GpioPinName pin, PullMode mode) {
        PUPD &= ~(0x3 << (pin.pin * 2));
        PUPD |= (mode) << (pin.pin * 2);
    }
    bool getPinValue(GpioPinName pin) {
        return ID & (1 << pin.pin);
    }
    void setPin(GpioPinName pin) {
        BSR = (1 << pin.pin);
    }
    void resetPin(GpioPinName pin) {
        BSR = (0x10000 << pin.pin);
    }
    void setAlternateFunction(GpioPinName pin, uint8_t function) {
        if (pin.pin < 8) {
            AFRL &= ~(0xF << (pin.pin * 4));
            AFRL |= (function << (pin.pin * 4));
        } else {
            AFRH &= ~(0xF << (pin.pin * 4 - 32));
            AFRH |= (function << (pin.pin * 4 - 32));
        }
    }
};

#endif /* SRC_TEST_CPP_INCLUDE_LOWLEVEL_GPIOLOWLEVEL_SPECIFIC_GPIOPORT_HPP_ */
