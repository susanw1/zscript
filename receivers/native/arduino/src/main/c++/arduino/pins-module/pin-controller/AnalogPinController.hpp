/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARDUINO_PINS_MODULE_PIN_CONTROLLER_ANALOGPINCONTROLLER_HPP_
#define SRC_MAIN_CPP_ARDUINO_PINS_MODULE_PIN_CONTROLLER_ANALOGPINCONTROLLER_HPP_

#include <zscript/modules/ZscriptCommand.hpp>
#include "PinController.hpp"

namespace Zscript {
template<class ZP>
class AnalogPinController: public PinController<ZP> {

public:
    static PinControllerCapability getPinCaps(uint8_t pin) {
        if (pin == A0 || pin == A1 || pin == A2 || pin == A3 || pin == A4 || pin == A5 || pin == A6 || pin == A7) {
            return CanReadPin;
        } else {
            return None;
        }
    }
    static void outputPinCaps(uint8_t pin, CommandOutStream<ZP> out) {
        if (getPinCaps(pin) == CanReadPin) {
            out.writeField('B', 10);
        }
    }

    static bool setupPin(uint8_t pin, uint16_t modeV) {
        (void) modeV;
        if (getPinCaps(pin) != CanReadPin) {
            return false;
        }
        pinMode(pin, INPUT);
        return true;
    }
    static void writePin(uint8_t pin, uint16_t value) {
        (void) pin;
        (void) value;
    }
    static uint16_t readPin(uint8_t pin) {
        return analogRead(pin) << 6;
    }
};
}

#endif /* SRC_MAIN_CPP_ARDUINO_PINS_MODULE_PIN_CONTROLLER_ANALOGPINCONTROLLER_HPP_ */
