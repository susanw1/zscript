/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARDUINO_PINS_MODULE_PIN_CONTROLLER_PINCONTROLLER_HPP_
#define SRC_MAIN_CPP_ARDUINO_PINS_MODULE_PIN_CONTROLLER_PINCONTROLLER_HPP_

#include <zscript/modules/ZscriptCommand.hpp>

enum PinControllerCapability {
    None,
    CanReadPin,
    CanWritePin,
    CanReadWritePin
};

enum PinControllerMode {
    DigitalPin,
    AnalogPin
};

namespace Zscript {
template<class ZP>
class PinController {
private:
    static uint8_t controller[ZP::pinCount];

    static bool canWrite(PinControllerCapability cap) {
        return cap == CanWritePin || cap == CanReadWritePin;
    }
    static bool canRead(PinControllerCapability cap) {
        return cap == CanReadPin || cap == CanReadWritePin;
    }

public:
    static bool setupPin(uint8_t pinIndex, PinControllerMode mode, uint16_t internalMode, ZscriptCommandContext<ZP> ctx);
    static bool pinInfo(uint8_t pinIndex, ZscriptCommandContext<ZP> ctx);
    static bool readPin(uint8_t pinIndex, ZscriptCommandContext<ZP> ctx);
    static bool writePin(uint8_t pinIndex, uint16_t value, ZscriptCommandContext<ZP> ctx);
};
template<class ZP>
uint8_t PinController<ZP>::controller[ZP::pinCount];
}
#include "DigitalPinController.hpp"
#include "AnalogPinController.hpp"

namespace Zscript {
template<class ZP>
bool PinController<ZP>::setupPin(uint8_t pinIndex, PinControllerMode mode, uint16_t internalMode, ZscriptCommandContext<ZP> ctx) {
    bool worked;
    switch (mode) {
    case DigitalPin:
        controller[pinIndex] = mode;
        worked = DigitalPinController<ZP>::setupPin(pinIndex, internalMode);
        break;
    case AnalogPin:
        controller[pinIndex] = mode;
        worked = AnalogPinController<ZP>::setupPin(pinIndex, internalMode);
        break;
    default:
        ctx.status(ResponseStatus::VALUE_OUT_OF_RANGE);
        return false;
    }
    if (!worked) {
        ctx.status(ResponseStatus::EXECUTION_ERROR);
        return false;
    }
    return true;
}
template<class ZP>
bool PinController<ZP>::pinInfo(uint8_t pinIndex, ZscriptCommandContext<ZP> ctx) {
    switch (controller[pinIndex]) {
    case DigitalPin:
        DigitalPinController<ZP>::outputPinCaps(pinIndex, ctx.getOutStream());
        break;
    case AnalogPin:
        AnalogPinController<ZP>::outputPinCaps(pinIndex, ctx.getOutStream());
        break;
    default:
        ctx.status(ResponseStatus::VALUE_OUT_OF_RANGE);
        return false;
    }
    return true;
}
template<class ZP>
bool PinController<ZP>::readPin(uint8_t pinIndex, ZscriptCommandContext<ZP> ctx) {
    uint16_t value;
    bool worked = true;
    switch (controller[pinIndex]) {
    case DigitalPin:
        if (!canRead(DigitalPinController<ZP>::getPinCaps(pinIndex))) {
            worked = false;
            break;
        }
        value = DigitalPinController<ZP>::readPin(pinIndex);
        break;
    case AnalogPin:
        if (!canRead(AnalogPinController<ZP>::getPinCaps(pinIndex))) {
            worked = false;
            break;
        }
        value = AnalogPinController<ZP>::readPin(pinIndex);
        break;
    default:
        ctx.status(ResponseStatus::VALUE_OUT_OF_RANGE);
        return false;
    }
    if (!worked) {
        ctx.status(ResponseStatus::EXECUTION_ERROR);
        return false;
    }
    ctx.getOutStream().writeField('V', value);
    return true;
}
template<class ZP>
bool PinController<ZP>::writePin(uint8_t pinIndex, uint16_t value, ZscriptCommandContext<ZP> ctx) {
    bool worked = true;
    switch (controller[pinIndex]) {
    case DigitalPin:
        if (!canWrite(DigitalPinController<ZP>::getPinCaps(pinIndex))) {
            worked = false;
            break;
        }
        DigitalPinController<ZP>::writePin(pinIndex, value);
        break;
    case AnalogPin:
        if (!canWrite(AnalogPinController<ZP>::getPinCaps(pinIndex))) {
            worked = false;
            break;
        }
        AnalogPinController<ZP>::writePin(pinIndex, value);
        break;
    default:
        ctx.status(ResponseStatus::VALUE_OUT_OF_RANGE);
        return false;
    }
    if (!worked) {
        ctx.status(ResponseStatus::EXECUTION_ERROR);
        return false;
    }
    return true;
}
}
#endif /* SRC_MAIN_CPP_ARDUINO_PINS_MODULE_PIN_CONTROLLER_PINCONTROLLER_HPP_ */
