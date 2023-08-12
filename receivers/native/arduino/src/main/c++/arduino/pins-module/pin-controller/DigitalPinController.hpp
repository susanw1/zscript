/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARDUINO_PINS_MODULE_PIN_CONTROLLER_DIGITALPINCONTROLLER_HPP_
#define SRC_MAIN_CPP_ARDUINO_PINS_MODULE_PIN_CONTROLLER_DIGITALPINCONTROLLER_HPP_

#include <zscript/modules/ZscriptCommand.hpp>
#include "PinController.hpp"

namespace Zscript {
enum DigitalPinMode {
    DigitalInput = 0,
    DigitalInputPullUp = 1,
    DigitalOutput = 4,
    DigitalOpenDrain = 8,
    DigitalOpenDrainPullUp = 9
};

template<class ZP>
class DigitalPinController: public PinController<ZP> {
    static uint8_t openDrain[(ZP::pinCount + 7) / 8];
    static uint8_t pullUp[(ZP::pinCount + 7) / 8];

    static void setOpenDrain(uint8_t pin) {
        openDrain[pin / 8] |= (1 << (pin % 8));
    }
    static void clearOpenDrain(uint8_t pin) {
        openDrain[pin / 8] &= ~(1 << (pin % 8));
    }
    static bool isOpenDrain(uint8_t pin) {
        return (openDrain[pin / 8] & (1 << (pin % 8))) != 0;
    }

    static void setPullUp(uint8_t pin) {
        pullUp[pin / 8] |= (1 << (pin % 8));
    }
    static void clearPullUp(uint8_t pin) {
        pullUp[pin / 8] &= ~(1 << (pin % 8));
    }
    static bool isPullUp(uint8_t pin) {
        return (pullUp[pin / 8] & (1 << (pin % 8))) != 0;
    }

public:
    static PinControllerCapability getPinCaps(uint8_t pin) {
        (void) pin;
        return CanReadWritePin;
    }
    static void outputPinCaps(uint8_t pin, CommandOutStream<ZP> out) {
        (void) pin;
        out.writeField('M', 0x0313);
    }

    static bool setupPin(uint8_t pin, uint16_t modeV) {
        DigitalPinMode mode = (DigitalPinMode) modeV;
        switch (mode) {
        case DigitalInput:
            pinMode(pin, INPUT);
            break;
        case DigitalInputPullUp:
            pinMode(pin, INPUT_PULLUP);
            break;
        case DigitalOutput:
            pinMode(pin, OUTPUT);
            break;
        case DigitalOpenDrain:
            pinMode(pin, INPUT);
            digitalWrite(pin, LOW);
            break;
        case DigitalOpenDrainPullUp:
            pinMode(pin, INPUT_PULLUP);
            digitalWrite(pin, LOW);
            break;
        default:
            return false;
            break;
        }
        return true;
    }
    static void writePin(uint8_t pin, uint16_t value) {
        if (isOpenDrain(pin)) {
            if (value != 0) {
                if (isPullUp(pin)) {
                    pinMode(pin, INPUT_PULLUP);
                } else {
                    pinMode(pin, INPUT);
                }
            } else {
                pinMode(pin, OUTPUT);
            }
        } else {
            if (value != 0) {
                digitalWrite(pin, HIGH);
            } else {
                digitalWrite(pin, LOW);
            }
        }
    }
    static uint16_t readPin(uint8_t pin) {
        return digitalRead(pin) == HIGH ? 1 : 0;
    }
};
template<class ZP>
uint8_t DigitalPinController<ZP>::openDrain[(ZP::pinCount + 7) / 8];
template<class ZP>
uint8_t DigitalPinController<ZP>::pullUp[(ZP::pinCount + 7) / 8];
}

#endif /* SRC_MAIN_CPP_ARDUINO_PINS_MODULE_PIN_CONTROLLER_DIGITALPINCONTROLLER_HPP_ */
