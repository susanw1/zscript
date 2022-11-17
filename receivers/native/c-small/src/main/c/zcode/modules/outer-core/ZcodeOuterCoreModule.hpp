/*
 * ZcodeOuterCoreModule.hpp
 *
 *  Created on: 11 Jul 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___ZCODE_MODULES_OUTER_CORE_ZCODEOUTERCOREMODULE_HPP_
#define SRC_MAIN_C___ZCODE_MODULES_OUTER_CORE_ZCODEOUTERCOREMODULE_HPP_
#ifdef SRC_TEST_CPP_ZCODE_ZCODE_HPP_
#error Must be included before Zcode.hpp
#endif

#include "../../ZcodeIncludes.hpp"
#include "../ZcodeModule.hpp"
#include "ZcodeAddressCommand.hpp"
#include "ZcodeChannelSetupCommand.hpp"

#define MODULE_EXISTS_001 EXISTENCE_MARKER_UTIL

#include "ZcodeExtendedCapabilitiesCommand.hpp"

#define MODULE_SWITCH_001 MODULE_SWITCH_UTIL(ZcodeOuterCoreModule<ZP>::execute)

template<class ZP>
class ZcodeOuterCoreModule: public ZcodeModule<ZP> {
    typedef typename ZP::Strings::string_t string_t;

public:

    static void execute(ZcodeExecutionCommandSlot<ZP> slot, uint8_t bottomBits) {

        switch (bottomBits) {
        case 0x0:
            ZcodeExtendedCapabilitiesCommand<ZP>::execute(slot);
            break;
#ifdef ZCODE_SUPPORT_ADDRESSING
        case 0x1:
            ZcodeAddressCommand<ZP>::execute(slot);
            break;
        case 0x2:
            ZP::AddressRouter::addressingControlCommand(slot);
            break;
#endif

#ifdef ZCODE_CORE_WRITE_GUID
        case 0x4:
            ZCODE_CORE_WRITE_GUID(slot);
            break;
#endif
        case 0x8:
            ZcodeChannelSetupCommand<ZP>::execute(slot);
            break;
#ifdef ZCODE_OUTER_CORE_USER_COMMAND
        case 0xf:
            ZCODE_OUTER_CORE_USER_COMMAND(slot);
            break;
#endif
        default:
            slot.fail(UNKNOWN_CMD, (string_t) ZP::Strings::failParseUnknownCommand);
            break;
        }
    }

};
// want function of form: void execute(ZcodeRunningCommandSlot<ZP> *slot, uint16_t command)

#endif /* SRC_MAIN_C___ZCODE_MODULES_OUTER_CORE_ZCODEOUTERCOREMODULE_HPP_ */
