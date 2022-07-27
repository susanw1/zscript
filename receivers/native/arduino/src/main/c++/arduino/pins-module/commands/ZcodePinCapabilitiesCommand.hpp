/*
 * ZcodePinCapabilitiesCommand.hpp
 *
 *  Created on: 24 Jul 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___ARDUINO_PINS_MODULE_COMMANDS_ZCODEPINCAPABILITIESCOMMAND_HPP_
#define SRC_MAIN_C___ARDUINO_PINS_MODULE_COMMANDS_ZCODEPINCAPABILITIESCOMMAND_HPP_

#include <zcode/modules/ZcodeCommand.hpp>

#define COMMAND_EXISTS_0030 EXISTENCE_MARKER_UTIL

template<class ZP>
class ZcodePinCapabilitiesCommand: public ZcodeCommand<ZP> {
public:

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        out->writeStatus(OK);
        out->writeField8('C', MODULE_CAPABILITIES(003));
        out->writeField8('P', ZP::pinCount);
    }

};

#endif /* SRC_MAIN_C___ARDUINO_PINS_MODULE_COMMANDS_ZCODEPINCAPABILITIESCOMMAND_HPP_ */
