/*
 * ZcodeI2cReceiveCommand.hpp
 *
 *  Created on: 26 Jul 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___ARDUINO_I2C_MODULE_COMMANDS_ZCODEI2CRECEIVECOMMAND_HPP_
#define SRC_MAIN_C___ARDUINO_I2C_MODULE_COMMANDS_ZCODEI2CRECEIVECOMMAND_HPP_

#include <zcode/modules/ZcodeCommand.hpp>
#include "GeneralI2cAction.hpp"
#include <Wire.h>

#define COMMAND_EXISTS_0053 EXISTENCE_MARKER_UTIL

template<class ZP>
class ZcodeI2cReceiveCommand: public ZcodeCommand<ZP> {
public:
    static constexpr uint8_t CODE = 0x03;

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        GeneralI2cAction<ZP>::executeSendReceive(slot, GeneralI2cAction<ZP>::ActionType::RECEIVE);
    }
};

#endif /* SRC_MAIN_C___ARDUINO_I2C_MODULE_COMMANDS_ZCODEI2CRECEIVECOMMAND_HPP_ */
