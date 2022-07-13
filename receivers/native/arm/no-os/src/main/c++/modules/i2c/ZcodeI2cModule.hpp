#ifndef SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEI2CMODULE_HPP_
#define SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEI2CMODULE_HPP_

#ifdef SRC_TEST_CPP_ZCODE_ZCODE_HPP_
#error Must be included before Zcode.hpp
#endif

#include <modules/ZcodeModule.hpp>

#include "ZcodeI2cCapabilitiesCommand.hpp"
#include "ZcodeI2cSetupCommand.hpp"
#include "ZcodeI2cSendCommand.hpp"

#define ZCODE_I2C_MODULE_ADDRESS 0x05

#define COMMAND_SWITCH005 COMMAND_SWITCH_UTIL(ZCODE_I2C_MODULE_ADDRESS, ZcodeI2cModule<ZP>::execute)

template<class ZP>
class ZcodeI2cModule: public ZcodeModule<ZP> {
    typedef typename ZP::Strings::string_t string_t;

public:
    ZcodeI2cModule() :
            ZcodeModule<ZP>(ZCODE_I2C_MODULE_ADDRESS) {
    }

    static void execute(ZcodeExecutionCommandSlot<ZP> slot, uint16_t command) {
        uint8_t bottomBits = command & 0xF;
        switch (bottomBits) {
        case ZcodeI2cCapabilitiesCommand<ZP>::CODE:
            ZcodeI2cCapabilitiesCommand<ZP>::execute(slot);
            break;
        case ZcodeI2cSetupCommand<ZP>::CODE:
            ZcodeI2cSetupCommand<ZP>::execute(slot);
            break;
        default:
            slot.fail(UNKNOWN_CMD, (string_t) ZP::Strings::failParseUnknownCommand);
            break;
        }
    }
};

#endif /* SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEI2CMODULE_HPP_ */
