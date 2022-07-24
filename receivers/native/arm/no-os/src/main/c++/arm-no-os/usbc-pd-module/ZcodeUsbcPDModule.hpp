/*
 * ZcodeUsbcPDModule.hpp
 *
 *  Created on: 15 Jul 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___MODULES_USBCPD_ZCODEUSBCPDMODULE_HPP_
#define SRC_MAIN_C___MODULES_USBCPD_ZCODEUSBCPDMODULE_HPP_

#include <zcode/ZcodeIncludes.hpp>
#include <zcode/modules/ZcodeModule.hpp>
#include "commands/ZcodeUsbcPDStatusCommand.hpp"
#include "commands/ZcodeUsbcPDSetCommand.hpp"
#include "commands/ZcodeUsbcPDSourceCommand.hpp"
#include "commands/ZcodeUsbcPDSourceStatusCommand.hpp"
#include "commands/ZcodeUsbcPDSourceExtendedCommand.hpp"
#include "commands/ZcodeUsbcPDCapabilitiesCommand.hpp"

#define MODULE_EXISTS_011 EXISTENCE_MARKER_UTIL
#define MODULE_SWITCH_011 MODULE_SWITCH_UTIL(ZcodeUsbcPDModule<ZP>::execute)

template<class ZP>
class ZcodeUsbcPDModule: public ZcodeModule<ZP> {
    typedef typename ZP::Strings::string_t string_t;

public:

    static void execute(ZcodeExecutionCommandSlot<ZP> slot, uint8_t bottomBits) {

        switch (bottomBits) {
        case 0x0:
            ZcodeUsbcPDCapabilitiesCommand<ZP>::execute(slot);
            break;
        case 0x1:
            ZcodeUsbcPDStatusCommand<ZP>::execute(slot);
            break;
        case 0x2:
            ZcodeUsbcPDSetCommand<ZP>::execute(slot);
            break;
        case 0x3:
            ZcodeUsbcPDSourceCommand<ZP>::execute(slot);
            break;
        case 0x4:
            ZcodeUsbcPDSourceStatusCommand<ZP>::execute(slot);
            break;
        case 0x5:
            ZcodeUsbcPDSourceExtendedCommand<ZP>::execute(slot);
            break;
        default:
            slot.fail(UNKNOWN_CMD, (string_t) ZP::Strings::failParseUnknownCommand);
            break;
        }
    }

};
// want function of form: void execute(ZcodeRunningCommandSlot<ZP> *slot, uint16_t command)

#endif /* SRC_MAIN_C___MODULES_USBCPD_ZCODEUSBCPDMODULE_HPP_ */
