/*
 * ZcodeSerialModule.hpp
 *
 *  Created on: 20 Jul 2022
 *      Author: robert
 */

#ifndef NO_OS_SRC_MAIN_C___MODULES_SERIAL_ZCODESERIALMODULE_HPP_
#define NO_OS_SRC_MAIN_C___MODULES_SERIAL_ZCODESERIALMODULE_HPP_

#ifdef SRC_TEST_CPP_ZCODE_ZCODE_HPP_
#error Must be included before Zcode.hpp
#endif

#include <modules/ZcodeModule.hpp>

#include "ZcodeSerialSetupCommand.hpp"
#include "ZcodeSerialSendCommand.hpp"
#include "ZcodeSerialReadCommand.hpp"
#include "ZcodeSerialSkipCommand.hpp"

#include "ZcodeSerialCapabilitiesCommand.hpp"

#define ZCODE_SERIAL_MODULE_ADDRESS 0x07

#define MODULE_ADDRESS007 MODULE_ADDRESS_UTIL

#define COMMAND_SWITCH007 COMMAND_SWITCH_UTIL(ZCODE_SERIAL_MODULE_ADDRESS, ZcodeSerialModule<ZP>::execute)

template<class ZP>
class ZcodeSerialModule: public ZcodeModule<ZP> {
    typedef typename ZP::Strings::string_t string_t;

public:
    ZcodeSerialModule() :
            ZcodeModule<ZP>(ZCODE_SERIAL_MODULE_ADDRESS) {
    }

    static void execute(ZcodeExecutionCommandSlot<ZP> slot, uint8_t bottomBits) {
        switch (bottomBits) {
        case ZcodeSerialCapabilitiesCommand<ZP>::CODE:
            ZcodeSerialCapabilitiesCommand<ZP>::execute(slot);
            break;
        case ZcodeSerialSetupCommand<ZP>::CODE:
            ZcodeSerialSetupCommand<ZP>::execute(slot);
            break;
        case ZcodeSerialSendCommand<ZP>::CODE:
            ZcodeSerialSendCommand<ZP>::execute(slot);
            break;
        case ZcodeSerialReadCommand<ZP>::CODE:
            ZcodeSerialReadCommand<ZP>::execute(slot);
            break;
        case ZcodeSerialSkipCommand<ZP>::CODE:
            ZcodeSerialSkipCommand<ZP>::execute(slot);
            break;
        default:
            slot.fail(UNKNOWN_CMD, (string_t) ZP::Strings::failParseUnknownCommand);
            break;
        }
    }
};

#endif /* NO_OS_SRC_MAIN_C___MODULES_SERIAL_ZCODESERIALMODULE_HPP_ */
