/*
 * ZcodeCoreModule.hpp
 *
 *  Created on: 8 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_COMMANDS_ZCODECOREMODULE_HPP_
#define SRC_TEST_CPP_ZCODE_COMMANDS_ZCODECOREMODULE_HPP_

#include "../../ZcodeIncludes.hpp"
#include "../ZcodeModule.hpp"
#include "ZcodeActivateCommand.hpp"
#include "ZcodeCapabilitiesCommand.hpp"
#include "ZcodeEchoCommand.hpp"
#include "ZcodeSendDebugCommand.hpp"
#include "ZcodeSetDebugChannelCommand.hpp"
#include "ZcodeNotificationHostCommand.hpp"

#define ZCODE_CORE_MODULE_ADDRESS 0x00

#define COMMAND_SWITCH000 COMMAND_SWITCH_UTIL(ZCODE_CORE_MODULE_ADDRESS, ZcodeCoreModule<ZP>::execute)

template<class ZP>
class ZcodeCoreModule: public ZcodeModule<ZP> {

public:
    ZcodeCoreModule() :
            ZcodeModule<ZP>(ZCODE_CORE_MODULE_ADDRESS) {
    }

    static void execute(ZcodeExecutionCommandSlot<ZP> slot, uint16_t command) {
        uint8_t bottomBits = command & 0xF;

        switch (bottomBits) {
        case 0x0:
            ZcodeCapabilitiesCommand<ZP>::execute(slot);
            break;
        case 0x1:
            ZcodeEchoCommand<ZP>::execute(slot);
            break;
        case 0x3:
            ZcodeActivateCommand<ZP>::execute(slot);
            break;
        case 0x8:
            ZcodeNotificationHostCommand<ZP>::execute(slot);
            break;
        case 0x9:
            ZcodeSetDebugChannelCommand<ZP>::execute(slot);
            break;
        case 0xa:
            ZcodeSendDebugCommand<ZP>::execute(slot);
            break;
        case 0xb:
            ZcodeSendDebugCommand<ZP>::execute(slot);
            break;
        case 0xf:
            ZcodeSendDebugCommand<ZP>::execute(slot);
            break;
        default:
            slot.fail(UNKNOWN_CMD, "Unknown command");
            break;
        }
    }

};
// want function of form: void execute(ZcodeRunningCommandSlot<ZP> *slot, uint16_t command)

#endif /* SRC_TEST_CPP_ZCODE_COMMANDS_ZCODECOREMODULE_HPP_ */
