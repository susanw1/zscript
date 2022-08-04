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

#include <zcode/modules/ZcodeModule.hpp>

#ifdef SERIAL_ADDRESSING
#include <arm-no-os/serial-module/addressing/ZcodeSerialAddressingSystem.hpp>
#include <arm-no-os/serial-module/addressing/ZcodeSerialBusInterruptSource.hpp>
#endif

#include <arm-no-os/serial-module/channels/SerialChannel.hpp>

#include "commands/ZcodeSerialSetupCommand.hpp"
#include "commands/ZcodeSerialSendCommand.hpp"
#include "commands/ZcodeSerialReadCommand.hpp"
#include "commands/ZcodeSerialSkipCommand.hpp"

#include "commands/ZcodeSerialCapabilitiesCommand.hpp"

#define MODULE_EXISTS_007 EXISTENCE_MARKER_UTIL
#define MODULE_SWITCH_007 MODULE_SWITCH_UTIL(ZcodeSerialModule<ZP>::execute)

template<class ZP>
class ZcodeSerialModule: public ZcodeModule<ZP> {
    typedef typename ZP::Strings::string_t string_t;
    typedef typename ZP::LL LL;

public:

#ifdef SERIAL_ADDRESSING
    typedef ZcodeSerialBusInterruptSource<ZP> busInterruptSource;
#endif

    typedef SerialChannel<ZP> channel;

    static void init() {
        UartManager<LL>::init();
    }

    static Serial* getUart(SerialIdentifier id) {
        return UartManager<LL>::getUartById(id);
    }
    static Serial* getUsb() {
        return UartManager<LL>::getUartById(LL::HW::uartCount);
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
