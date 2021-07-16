/*
 * RCodePinSystem.hpp
 *
 *  Created on: 8 Jan 2021
 *      Author: robert
 */

#ifndef SRC_TEST_C___COMMANDS_PINS_RCODEPINSYSTEM_HPP_
#define SRC_TEST_C___COMMANDS_PINS_RCODEPINSYSTEM_HPP_
#include <RCodeParameters.hpp>
#include <GpioLowLevel/Gpio.hpp>
#include <GpioLowLevel/GpioManager.hpp>

class RCodePinSystem {
    static GpioPinName names[RCodePeripheralParameters::pinNumber];

public:
    static GpioPinName getRcodePinName(uint8_t number) {
        return names[number];
    }
    static uint8_t getPinNumber(GpioPinName name) {
        for (uint8_t i = 0; i < RCodePeripheralParameters::pinNumber; ++i) {
            if (names[i].port == name.port && names[i].pin == name.pin) {
                return i;
            }
        }
        return RCodePeripheralParameters::pinNumber;
    }
};

#endif /* SRC_TEST_C___COMMANDS_PINS_RCODEPINSYSTEM_HPP_ */
