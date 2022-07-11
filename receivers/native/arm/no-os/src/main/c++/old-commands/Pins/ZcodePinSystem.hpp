/*
 * ZcodePinSystem.hpp
 *
 *  Created on: 8 Jan 2021
 *      Author: robert
 */

#ifndef SRC_TEST_C___COMMANDS_PINS_ZCODEPINSYSTEM_HPP_
#define SRC_TEST_C___COMMANDS_PINS_ZCODEPINSYSTEM_HPP_

#include <ZcodeParameters.hpp>
#include "../../LowLevel/GpioLowLevel/Gpio.hpp"
#include "../../LowLevel/GpioLowLevel/GpioManager.hpp"

class ZcodePinSystem {
    private:
        static GpioPinName names[ZcodePeripheralParameters::pinNumber];

    public:
        static GpioPinName getZcodePinName(uint8_t number) {
            return names[number];
        }

        static uint8_t getPinNumber(GpioPinName name) {
            for (uint8_t i = 0; i < ZcodePeripheralParameters::pinNumber; ++i) {
                if (names[i].port == name.port && names[i].pin == name.pin) {
                    return i;
                }
            }
            return ZcodePeripheralParameters::pinNumber;
        }
};

#endif /* SRC_TEST_C___COMMANDS_PINS_ZCODEPINSYSTEM_HPP_ */
