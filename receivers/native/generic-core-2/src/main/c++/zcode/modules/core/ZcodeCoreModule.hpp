/*
 * ZcodeCoreModule.hpp
 *
 *  Created on: 8 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_COMMANDS_ZCODECOREMODULE_HPP_
#define SRC_TEST_CPP_ZCODE_COMMANDS_ZCODECOREMODULE_HPP_
#ifdef SRC_TEST_CPP_ZCODE_ZCODE_HPP_
#error Must be included before Zcode.hpp
#endif

#include "../../ZcodeIncludes.hpp"
#include "../ZcodeModule.hpp"
#include "ZcodeActivateCommand.hpp"
#include "ZcodeCapabilitiesCommand.hpp"
#include "ZcodeEchoCommand.hpp"
#include "ZcodeSetDebugChannelCommand.hpp"
#ifdef ZCODE_GENERATE_NOTIFICATIONS
#include "ZcodeNotificationHostCommand.hpp"
#endif
#ifdef ZCODE_SUPPORT_ADDRESSING
#ifdef ZCODE_SUPPORT_DEBUG
#include "ZcodeDebugAddressRouter.hpp"
#endif
#include "ZcodeAddressCommand.hpp"
#endif

#define ZCODE_CORE_MODULE_ADDRESS 0x00

#define COMMAND_SWITCH000 COMMAND_SWITCH_UTIL(ZCODE_CORE_MODULE_ADDRESS, ZcodeCoreModule<ZP>::execute)

template<class ZP>
class ZcodeCoreModule: public ZcodeModule<ZP> {
#if defined(ZCODE_SUPPORT_ADDRESSING) && defined(ZCODE_SUPPORT_DEBUG)
    ZcodeDebugAddressRouter<ZP> addrRouter;
#endif

public:
    ZcodeCoreModule() :
            ZcodeModule<ZP>(ZCODE_CORE_MODULE_ADDRESS

#if defined(ZCODE_SUPPORT_ADDRESSING) && defined(ZCODE_SUPPORT_DEBUG)
                    , &addrRouter
#endif

                    ) {
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
#ifdef ZCODE_GENERATE_NOTIFICATIONS
        case 0x8:
            ZcodeNotificationHostCommand<ZP>::execute(slot);
            break;
#endif
#ifdef ZCODE_SUPPORT_DEBUG
        case 0x9:
            ZcodeSetDebugChannelCommand<ZP>::execute(slot);
            break;
#endif
#ifdef ZCODE_SUPPORT_ADDRESSING
        case 0xf:
            ZcodeAddressCommand<ZP>::execute(slot);
            break;
#endif
        default:
            slot.fail(UNKNOWN_CMD, ZCODE_STRING_SURROUND("Unknown command"));
            break;
        }
    }

};
// want function of form: void execute(ZcodeRunningCommandSlot<ZP> *slot, uint16_t command)

#endif /* SRC_TEST_CPP_ZCODE_COMMANDS_ZCODECOREMODULE_HPP_ */
