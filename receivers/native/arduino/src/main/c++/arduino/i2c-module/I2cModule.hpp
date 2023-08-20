/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARDUINO_I2C_MODULE_ZSCRIPTI2CMODULE_HPP_
#define SRC_MAIN_CPP_ARDUINO_I2C_MODULE_ZSCRIPTI2CMODULE_HPP_

#ifdef ZSCRIPT_HPP_INCLUDED
#error Must be included before Zscript.hpp
#endif

#include <zscript/modules/ZscriptModule.hpp>
#include <zscript/execution/ZscriptCommandContext.hpp>

#include "commands/I2cSetupCommand.hpp"
#include "commands/GeneralI2cAction.hpp"
#include "commands/I2cCapabilitiesCommand.hpp"

#define MODULE_EXISTS_005 EXISTENCE_MARKER_UTIL
#define MODULE_SWITCH_005 MODULE_SWITCH_UTIL(I2cModule<ZP>::execute)

namespace Zscript {
template<class ZP>
class I2cModule: public ZscriptModule<ZP> {
public:

    static void execute(ZscriptCommandContext<ZP> ctx, uint8_t bottomBits) {
        switch (bottomBits) {
        case ZscriptI2cCapabilitiesCommand<ZP>::CODE:
            ZscriptI2cCapabilitiesCommand<ZP>::execute(ctx);
            break;
        case ZscriptI2cSetupCommand<ZP>::CODE:
            ZscriptI2cSetupCommand<ZP>::execute(ctx);
            break;
        case GeneralI2cAction<ZP>::SEND_CODE:
            GeneralI2cAction<ZP>::executeSendReceive(ctx, GeneralI2cAction<ZP>::ActionType::SEND);
            break;
        case GeneralI2cAction<ZP>::RECEIVE_CODE:
            GeneralI2cAction<ZP>::executeSendReceive(ctx, GeneralI2cAction<ZP>::ActionType::RECEIVE);
            break;
        case GeneralI2cAction<ZP>::SEND_RECEIVE_CODE:
            GeneralI2cAction<ZP>::executeSendReceive(ctx, GeneralI2cAction<ZP>::ActionType::SEND_RECEIVE);
            break;
        default:
            ctx.status(ResponseStatus::COMMAND_NOT_FOUND);
            break;
        }
    }
};
}

#endif /* SRC_MAIN_CPP_ARDUINO_I2C_MODULE_ZSCRIPTI2CMODULE_HPP_ */
