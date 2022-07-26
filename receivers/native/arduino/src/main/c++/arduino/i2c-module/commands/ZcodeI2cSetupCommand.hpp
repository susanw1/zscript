/*
 * ZcodeI2cSetupCommand.hpp
 *
 *  Created on: 26 Jul 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___ARDUINO_I2C_MODULE_COMMANDS_ZCODEI2CSETUPCOMMAND_HPP_
#define SRC_MAIN_C___ARDUINO_I2C_MODULE_COMMANDS_ZCODEI2CSETUPCOMMAND_HPP_

#include <zcode/modules/ZcodeCommand.hpp>
#include "../I2cStrings.hpp"
#include <Wire.h>

#define COMMAND_EXISTS_0051 EXISTENCE_MARKER_UTIL

template<class ZP>
class ZcodeI2cSetupCommand: public ZcodeCommand<ZP> {
    typedef typename ZP::Strings::string_t string_t;

public:
    static constexpr uint8_t CODE = 0x01;

    // if set, determines which port to control; else set all ports
    static constexpr char CMD_PARAM_I2C_PORT_P = 'P';

    // chooses comms frequency, from 10, 100, 400 and 1000 kHz
    static constexpr char CMD_PARAM_FREQ_F = 'F';

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {

        uint16_t freq;
        if (slot.getFields()->get(CMD_PARAM_FREQ_F, &freq)) {
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
                slot.fail(BAD_PARAM, (string_t) I2cStrings<ZP>::badFreq);
                return;
            }

            uint16_t port;
            if (slot.getFields()->get(CMD_PARAM_I2C_PORT_P, &port)) {
                if (port != 0) {
                    slot.fail(BAD_PARAM, (string_t) I2cStrings<ZP>::badPort);
                    return;
                }
            }
            Wire.setClock(freqValue);
        }
        slot.getOut()->writeStatus(OK);
    }
};

#endif /* SRC_MAIN_C___ARDUINO_I2C_MODULE_COMMANDS_ZCODEI2CSETUPCOMMAND_HPP_ */
