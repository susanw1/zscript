/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_PINS_MODULE_COMMANDS_ZCODEPINSETUPCOMMAND_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_PINS_MODULE_COMMANDS_ZCODEPINSETUPCOMMAND_HPP_

#include <zcode/modules/ZcodeCommand.hpp>
#include <arm-no-os/pins-module/lowlevel/GpioManager.hpp>
#include <arm-no-os/pins-module/pin-controller/PinController.hpp>

#define COMMAND_EXISTS_0031 EXISTENCE_MARKER_UTIL

template<class ZP>
class ZcodePinSetupCommand: public ZcodeCommand<ZP> {
    typedef typename ZP::LL LL;
    typedef typename LL::HW HW;

public:

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        uint16_t pin;
        if (!slot.getFields()->get('P', &pin)) {
            slot.fail(BAD_PARAM, "No pin specified");
            return;
        }
        if (pin >= HW::pinCount) {
            slot.fail(BAD_PARAM, "Invalid pin");
            return;
        }

        uint16_t mode;
        bool hasMode = slot.getFields()->get('C', &mode);
        uint16_t internalMode = 0;
        slot.getFields()->get('M', &internalMode);
        if (PinController<ZP>::pinInfo(pin, slot) && (!hasMode || PinController<ZP>::setupPin(pin, (PinControllerMode) mode, internalMode, slot))) {
            out->writeStatus(OK);
        }
    }

};

#endif /* SRC_MAIN_CPP_ARM_NO_OS_PINS_MODULE_COMMANDS_ZCODEPINSETUPCOMMAND_HPP_ */
