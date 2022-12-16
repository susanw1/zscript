/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_PINS_MODULE_PIN_CONTROLLER_ANALOGPINCONTROLLER_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_PINS_MODULE_PIN_CONTROLLER_ANALOGPINCONTROLLER_HPP_

#include <zcode/modules/ZcodeCommand.hpp>
#include <arm-no-os/pins-module/lowlevel/Gpio.hpp>
#include <arm-no-os/pins-module/AToDLowLevel/AtoDManager.hpp>
#include "PinController.hpp"

template<class ZP>
class AnalogPinController: public PinController<ZP> {
    typedef typename ZP::LL LL;

public:
    static PinControllerCapability getPinCaps(GpioPin<LL> pin) {
        if (AtoDManager<LL>::canPerformAtoD(pin.getPinName())) {
            return CanReadPin;
        } else {
            return None;
        }
    }
    static void outputPinCaps(GpioPin<LL> pin, ZcodeOutStream<ZP> *out) {
        if (AtoDManager<LL>::canPerformAtoD(pin.getPinName())) {
            out->writeField8('B', 12);
        }
    }

    static bool setupPin(GpioPin<LL> pin, uint16_t modeV) {
        if (!AtoDManager<LL>::canPerformAtoD(pin.getPinName())) {
            return false;
        }
        PinMode pinMode = Analog;
        PinSpeed speed = HighSpeed;
        PullMode pull = NoPull;
        pin.setMode(pinMode);
        pin.setOutputSpeed(speed);
        pin.setPullMode(pull);
        pin.setMode(pinMode);
        return true;
    }
    static void writePin(GpioPin<LL> pin, uint16_t value) {
    }
    static uint16_t readPin(GpioPin<LL> pin) {
        return AtoDManager<LL>::performAtoD(pin.getPinName()) << 4;
    }
};

#endif /* SRC_MAIN_CPP_ARM_NO_OS_PINS_MODULE_PIN_CONTROLLER_ANALOGPINCONTROLLER_HPP_ */
