/*
 * ZcodePinModule.hpp
 *
 *  Created on: 25 Jul 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___ARM_NO_OS_PINS_MODULE_ZCODEPINMODULE_HPP_
#define SRC_MAIN_C___ARM_NO_OS_PINS_MODULE_ZCODEPINMODULE_HPP_

#ifdef SRC_TEST_CPP_ZCODE_ZCODE_HPP_
#error Must be included before Zcode.hpp
#endif

#include <zcode/modules/ZcodeModule.hpp>

#include "commands/ZcodePinSetupCommand.hpp"
#include "commands/ZcodePinWriteCommand.hpp"
#include "commands/ZcodePinReadCommand.hpp"
#include "commands/ZcodePinCapabilitiesCommand.hpp"
#include "pin-controller/PinController.hpp"

#define MODULE_EXISTS_003 EXISTENCE_MARKER_UTIL
#define MODULE_SWITCH_003 MODULE_SWITCH_UTIL(ZcodePinModule<ZP>::execute)

template<class ZP>
class ZcodePinModule: public ZcodeModule<ZP> {
    typedef typename ZP::Strings::string_t string_t;
    typedef typename ZP::LL LL;

public:

    static void init() {
        GpioManager<LL>::init();
#ifdef HAS_ATOD_SYSTEM
        AtoDManager<LL>::init();
#endif
    }

    static GpioPin<LL> getPin(GpioPinName name) {
        return GpioManager<LL>::getPin(name);
    }

    static void execute(ZcodeExecutionCommandSlot<ZP> slot, uint8_t bottomBits) {
        switch (bottomBits) {
        case 0x0:
            ZcodePinCapabilitiesCommand<ZP>::execute(slot);
            break;
        case 0x1:
            ZcodePinSetupCommand<ZP>::execute(slot);
            break;
        case 0x2:
            ZcodePinWriteCommand<ZP>::execute(slot);
            break;
        case 0x3:
            ZcodePinReadCommand<ZP>::execute(slot);
            break;
        default:
            slot.fail(UNKNOWN_CMD, (string_t) ZP::Strings::failParseUnknownCommand);
            break;
        }
    }
};

#endif /* SRC_MAIN_C___ARM_NO_OS_PINS_MODULE_ZCODEPINMODULE_HPP_ */
