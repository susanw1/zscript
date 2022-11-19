/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_STM32F0_PINS_MODULE_PIN_CONTROLLER_PINCONTROLLER_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_STM32F0_PINS_MODULE_PIN_CONTROLLER_PINCONTROLLER_HPP_

#include <zcode/modules/ZcodeCommand.hpp>
#include <arm-no-os/pins-module/lowlevel/Gpio.hpp>

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

template<class ZP>
class PinController {
private:
    typedef typename ZP::LL LL;
    typedef typename LL::HW HW;
    static uint8_t controller[HW::pinCount];

    static bool canWrite(PinControllerCapability cap) {
        return cap == CanWritePin || cap == CanReadWritePin;
    }
    static bool canRead(PinControllerCapability cap) {
        return cap == CanReadPin || cap == CanReadWritePin;
    }

public:
    static bool setupPin(uint8_t pinIndex, PinControllerMode mode, uint16_t internalMode, ZcodeExecutionCommandSlot<ZP> slot);
    static bool pinInfo(uint8_t pinIndex, ZcodeExecutionCommandSlot<ZP> slot);
    static bool readPin(uint8_t pinIndex, ZcodeExecutionCommandSlot<ZP> slot);
    static bool writePin(uint8_t pinIndex, uint16_t value, ZcodeExecutionCommandSlot<ZP> slot);
};
template<class ZP>
uint8_t PinController<ZP>::controller[HW::pinCount];

#include <arm-no-os/pins-module/pin-controller/DigitalPinController.hpp>

template<class ZP>
bool PinController<ZP>::setupPin(uint8_t pinIndex, PinControllerMode mode, uint16_t internalMode, ZcodeExecutionCommandSlot<ZP> slot) {
    GpioPin<LL> pin = GpioManager<LL>::getPin(pinIndex);
    pin.init();
    bool worked;
    switch (mode) {
    case DigitalPin:
        controller[pinIndex] = mode;
        worked = DigitalPinController<ZP>::setupPin(pin, internalMode);
        break;
    default:
        slot.fail(BAD_PARAM, "Unknown pin mode");
        return false;
    }
    if (!worked) {
        slot.fail(CMD_FAIL, "Pin mode not supported");
        return false;
    }
    return true;
}
template<class ZP>
bool PinController<ZP>::pinInfo(uint8_t pinIndex, ZcodeExecutionCommandSlot<ZP> slot) {
    GpioPin<LL> pin = GpioManager<LL>::getPin(pinIndex);
    switch (controller[pinIndex]) {
    case DigitalPin:
        DigitalPinController<ZP>::outputPinCaps(pin, slot.getOut());
        break;
    default:
        slot.fail(BAD_PARAM, "Unknown pin mode");
        return false;
    }
    return true;
}
template<class ZP>
bool PinController<ZP>::readPin(uint8_t pinIndex, ZcodeExecutionCommandSlot<ZP> slot) {
    GpioPin<LL> pin = GpioManager<LL>::getPin(pinIndex);
    uint16_t value;
    bool worked = true;
    switch (controller[pinIndex]) {
    case DigitalPin:
        if (!canRead(DigitalPinController<ZP>::getPinCaps(pin))) {
            worked = false;
            break;
        }
        value = DigitalPinController<ZP>::readPin(pin);
        break;
    default:
        slot.fail(BAD_PARAM, "Unknown pin mode");
        return false;
    }
    if (!worked) {
        slot.fail(CMD_FAIL, "Pin read not supported");
        return false;
    }
    slot.getOut()->writeField16('V', value);
    return true;
}
template<class ZP>
bool PinController<ZP>::writePin(uint8_t pinIndex, uint16_t value, ZcodeExecutionCommandSlot<ZP> slot) {
    GpioPin<LL> pin = GpioManager<LL>::getPin(pinIndex);
    bool worked = true;
    switch (controller[pinIndex]) {
    case DigitalPin:
        if (!canWrite(DigitalPinController<ZP>::getPinCaps(pin))) {
            worked = false;
            break;
        }
        DigitalPinController<ZP>::writePin(pin, value);
        break;
    default:
        slot.fail(BAD_PARAM, "Unknown pin mode");
        return false;
    }
    if (!worked) {
        slot.fail(CMD_FAIL, "Pin write not supported");
        return false;
    }
    return true;
}

#endif /* SRC_MAIN_CPP_ARM_NO_OS_STM32F0_PINS_MODULE_PIN_CONTROLLER_PINCONTROLLER_HPP_ */
