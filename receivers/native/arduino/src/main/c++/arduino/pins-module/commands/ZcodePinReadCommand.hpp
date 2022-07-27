/*
 * ZcodePinReadCommand.hpp
 *
 *  Created on: 25 Jul 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___ARDUINO_PINS_MODULE_COMMANDS_ZCODEPINREADCOMMAND_HPP_
#define SRC_MAIN_C___ARDUINO_PINS_MODULE_COMMANDS_ZCODEPINREADCOMMAND_HPP_

#include <zcode/modules/ZcodeCommand.hpp>
#include <arduino/pins-module/pin-controller/PinController.hpp>

#define COMMAND_EXISTS_0033 EXISTENCE_MARKER_UTIL

template<class ZP>
class ZcodePinReadCommand: public ZcodeCommand<ZP> {

public:

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        uint16_t pin;
        if (!slot.getFields()->get('P', &pin)) {
            slot.fail(BAD_PARAM, "No pin specified");
            return;
        }
        if (pin >= ZP::pinCount) {
            slot.fail(BAD_PARAM, "Invalid pin");
            return;
        }

        if (PinController<ZP>::readPin(pin, slot)) {
            out->writeStatus(OK);
        }
    }

};

#endif /* SRC_MAIN_C___ARDUINO_PINS_MODULE_COMMANDS_ZCODEPINREADCOMMAND_HPP_ */
