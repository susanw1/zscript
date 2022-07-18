#ifndef SRC_MAIN_CPP_ZCODE_COMMANDS_ZCODEI2CSENDRECEIVECOMMAND_HPP_
#define SRC_MAIN_CPP_ZCODE_COMMANDS_ZCODEI2CSENDRECEIVECOMMAND_HPP_

#include <modules/ZcodeCommand.hpp>
#include "../../LowLevel/I2cLowLevel/I2cManager.hpp"
#include "GeneralI2cAction.hpp"

#define COMMAND_VALUE_0055 MODULE_CAPABILITIES_UTIL
template<class ZP>
class ZcodeI2cSendReceiveCommand: public ZcodeCommand<ZP> {
public:
    static constexpr uint8_t CODE = 0x04;

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        GeneralI2cAction<ZP>::executeSendReceive(slot, GeneralI2cAction<ZP>::ActionType::SEND_RECEIVE);
    }
};

#endif /* SRC_MAIN_CPP_ZCODE_COMMANDS_ZCODEI2CSENDRECEIVECOMMAND_HPP_ */
