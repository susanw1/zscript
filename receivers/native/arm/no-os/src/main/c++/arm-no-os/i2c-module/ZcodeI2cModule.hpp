#ifndef SRC_MAIN_CPP_ZCODE_COMMANDS_ZCODEI2CMODULE_HPP_
#define SRC_MAIN_CPP_ZCODE_COMMANDS_ZCODEI2CMODULE_HPP_

#ifdef SRC_TEST_CPP_ZCODE_ZCODE_HPP_
#error Must be included before Zcode.hpp
#endif

#include <zcode/modules/ZcodeModule.hpp>
#ifdef I2C_ADDRESSING
#include <arm-no-os/i2c-module/addressing/ZcodeI2cAddressingSystem.hpp>
#include <arm-no-os/i2c-module/addressing/ZcodeI2cBusInterruptSource.hpp>
#endif

#include "commands/ZcodeI2cSetupCommand.hpp"
#include "commands/ZcodeI2cSendCommand.hpp"
#include "commands/ZcodeI2cReceiveCommand.hpp"
#include "commands/ZcodeI2cSendReceiveCommand.hpp"
#include "commands/ZcodeI2cCapabilitiesCommand.hpp"

#define MODULE_EXISTS_005 EXISTENCE_MARKER_UTIL
#define MODULE_SWITCH_005 MODULE_SWITCH_UTIL(ZcodeI2cModule<ZP>::execute)

template<class ZP>
class ZcodeI2cModule: public ZcodeModule<ZP> {
    typedef typename ZP::Strings::string_t string_t;
    typedef typename ZP::LL LL;

public:

#ifdef I2C_ADDRESSING
    typedef ZcodeI2cBusInterruptSource<ZP> busInterruptSource;
#endif

    static void init() {
        I2cManager<LL>::init();
    }

    static I2c<LL>* getI2c(I2cIdentifier id) {
        return I2cManager<LL>::getI2cById(id);
    }

    static void execute(ZcodeExecutionCommandSlot<ZP> slot, uint8_t bottomBits) {
        switch (bottomBits) {
        case ZcodeI2cCapabilitiesCommand<ZP>::CODE:
            ZcodeI2cCapabilitiesCommand<ZP>::execute(slot);
            break;
        case ZcodeI2cSetupCommand<ZP>::CODE:
            ZcodeI2cSetupCommand<ZP>::execute(slot);
            break;
        case ZcodeI2cSendCommand<ZP>::CODE:
            ZcodeI2cSendCommand<ZP>::execute(slot);
            break;
        case ZcodeI2cReceiveCommand<ZP>::CODE:
            ZcodeI2cReceiveCommand<ZP>::execute(slot);
            break;
        case ZcodeI2cSendReceiveCommand<ZP>::CODE:
            ZcodeI2cSendReceiveCommand<ZP>::execute(slot);
            break;
        default:
            slot.fail(UNKNOWN_CMD, (string_t) ZP::Strings::failParseUnknownCommand);
            break;
        }
    }
};

#endif /* SRC_MAIN_CPP_ZCODE_COMMANDS_ZCODEI2CMODULE_HPP_ */
