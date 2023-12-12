/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARDUINO_I2C_MODULE_COMMANDS_ZSCRIPTI2CSETUPCOMMAND_HPP_
#define SRC_MAIN_CPP_ARDUINO_I2C_MODULE_COMMANDS_ZSCRIPTI2CSETUPCOMMAND_HPP_

#include <zscript/modules/ZscriptCommand.hpp>
#include <net/zscript/model/modules/base/I2cModule.hpp>
#include <Wire.h>

#define COMMAND_EXISTS_0051 EXISTENCE_MARKER_UTIL

namespace Zscript {

namespace i2c_module {

template<class ZP>
class ZscriptI2cSetupCommand: public I2cSetup_CommandDefs {
public:
    static void execute(ZscriptCommandContext<ZP> ctx
#ifdef ZSCRIPT_I2C_SUPPORT_NOTIFICATIONS
            , bool *addressing, bool *notifications
#endif
    ) {

        uint16_t freq;
        if (ctx.getField(ReqFrequency__F, &freq)) {
            uint32_t freqValue;

            switch (freq) {
                case 0:
                    freqValue = 10000;
                    break;
                case 1:
                    freqValue = 100000;
                    break;
                case 2:
                    freqValue = 400000;
                    break;
                case 3:
                    freqValue = 1000000;
                    break;
                default:
                    ctx.status(ResponseStatus::VALUE_OUT_OF_RANGE);
                    return;
            }

            uint16_t interface;
            if (!ctx.getReqdFieldCheckLimit(ReqInterface__I, 1, &interface)) {
                return;
            }
            Wire.setClock(freqValue);
            ctx.getOutStream().writeField(RespFrequencykHz__K, freqValue / 1000);
        }
#ifdef ZSCRIPT_I2C_SUPPORT_NOTIFICATIONS
        uint16_t notif;
        if (ctx.getField(ReqNotifications__N, &notif)) {
            *notifications = (notif != 0);
        }
    #ifdef ZSCRIPT_SUPPORT_ADDRESSING
        uint16_t addr;
        if (ctx.getField(ReqAddressing__A, &addr)) {
            *addressing = (addr != 0);
        }
    #endif
#endif
    }
};

}

}

#endif /* SRC_MAIN_CPP_ARDUINO_I2C_MODULE_COMMANDS_ZSCRIPTI2CSETUPCOMMAND_HPP_ */
