/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_PINS_MODULE_PIN_CONTROLLER_DIGITALPINCONTROLLER_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_PINS_MODULE_PIN_CONTROLLER_DIGITALPINCONTROLLER_HPP_

#include <zcode/modules/ZcodeCommand.hpp>
#include <arm-no-os/pins-module/lowlevel/Gpio.hpp>
#include "PinController.hpp"

enum DigitalPinMode {
    DigitalInput = 0,
    DigitalInputPullUp = 1,
    DigitalInputPullDown = 2,
    DigitalOutput = 4,
    DigitalOpenDrain = 8,
    DigitalOpenDrainPullUp = 9,
    DigitalOpenDrainPullDown = 10,
};

template<class ZP>
class DigitalPinController: public PinController<ZP> {
    typedef typename ZP::LL LL;

public:
    static PinControllerCapability getPinCaps(GpioPin<LL> pin) {
        return CanReadWritePin;
    }
    static void outputPinCaps(GpioPin<LL> pin, ZcodeOutStream<ZP> *out) {
        out->writeField16('M', 0x0717);
    }

    static bool setupPin(GpioPin<LL> pin, uint16_t modeV) {
        DigitalPinMode mode = (DigitalPinMode) modeV;
        PinMode pinMode;
        OutputMode outputMode = OutputMode::OpenDrain;
        PinSpeed speed = HighSpeed;
        PullMode pull = NoPull;
        switch (mode) {
        case DigitalInput:
            pinMode = PinMode::Input;
            break;
        case DigitalInputPullUp:
            pinMode = PinMode::Input;
            pull = PullUp;
            break;
        case DigitalInputPullDown:
            pinMode = PinMode::Input;
            pull = PullDown;
            break;
        case DigitalOutput:
            pinMode = PinMode::Output;
            outputMode = OutputMode::PushPull;
            break;
        case DigitalOpenDrain:
            pinMode = PinMode::Output;
            break;
        case DigitalOpenDrainPullUp:
            pinMode = PinMode::Output;
            pull = PullUp;
            break;
        case DigitalOpenDrainPullDown:
            pinMode = PinMode::Output;
            pull = PullDown;
            break;
        default:
            return false;
            break;
        }
        pin.setOutputMode(outputMode);
        pin.setOutputSpeed(speed);
        pin.setPullMode(pull);
        pin.setMode(pinMode);
        return true;
    }
    static void writePin(GpioPin<LL> pin, uint16_t value) {
        if (value != 0) {
            pin.set();
        } else {
            pin.reset();
        }
    }
    static uint16_t readPin(GpioPin<LL> pin) {
        return pin.read() ? 1 : 0;
    }
};

#endif /* SRC_MAIN_CPP_ARM_NO_OS_PINS_MODULE_PIN_CONTROLLER_DIGITALPINCONTROLLER_HPP_ */
