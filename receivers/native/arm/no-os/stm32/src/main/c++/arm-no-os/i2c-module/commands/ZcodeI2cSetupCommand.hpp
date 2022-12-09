/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_I2C_MODULE_COMMANDS_ZCODEI2CSETUPCOMMAND_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_I2C_MODULE_COMMANDS_ZCODEI2CSETUPCOMMAND_HPP_

#include <zcode/modules/ZcodeCommand.hpp>
#include <arm-no-os/i2c-module/lowlevel/I2cManager.hpp>

#define COMMAND_EXISTS_0051 EXISTENCE_MARKER_UTIL

template<class ZP>
class ZcodeI2cModule;

template<class ZP>
class ZcodeI2cSetupCommand: public ZcodeCommand<ZP> {
private:

public:
    static constexpr uint8_t CODE = 0x01;
    // if set, determines which address to set addressing on
    static constexpr char CMD_PARAM_I2C_ADDRESS_B = 'B';

    // if set, determines which port to control; else set all ports (if not present, addressing flag has no effect)
    static constexpr char CMD_PARAM_I2C_PORT_P = 'P';

    // chooses comms frequency, from 10, 100, 400 and 1000 kHz
    static constexpr char CMD_PARAM_FREQ_F = 'F';

    typedef typename ZP::LL LL;
    typedef typename LL::HW HW;

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {

        uint16_t freq;
        if (slot.getFields()->get(CMD_PARAM_FREQ_F, &freq)) {
            uint8_t freqValue;

            switch (freq) {
            case 0:
                freqValue = kHz10;
                break;
            case 1:
                freqValue = kHz100;
                break;
            case 2:
                freqValue = kHz400;
                break;
            case 3:
                freqValue = kHz1000;
                break;
            default:
                slot.fail(BAD_PARAM, "Frequency field must be 0-3");
                return;
            }

            uint16_t port;
            if (slot.getFields()->get(CMD_PARAM_I2C_PORT_P, &port)) {
                if (port >= HW::i2cCount) {
                    slot.fail(BAD_PARAM, "Invalid I2C port");
                    return;
                }
                I2cManager<LL>::getI2cById(port)->setFrequency((I2cFrequency) freqValue);
            } else {
                for (I2cIdentifier i = 0; i < HW::i2cCount; i++) {
                    I2cManager<LL>::getI2cById(i)->setFrequency((I2cFrequency) freqValue);
                }
            }
        }
        uint16_t port;
        if (slot.getFields()->get(CMD_PARAM_I2C_PORT_P, &port)) {
            uint16_t addr;
            if (slot.getFields()->get(CMD_PARAM_I2C_ADDRESS_B, &addr)) {
#ifdef I2C_ADDRESSING
                if (slot.getFields()->has('A')) {
                    ZcodeI2cModule<ZP>::addressI2c(port, addr);
                } else {
                    ZcodeI2cModule<ZP>::unaddressI2c(port, addr);
                }
#else
                if (slot.getFields()->has('A')) {
                    slot.fail(CMD_FAIL, "I2C addressing not supported");
                    return;
                }
#endif
            }
        }
        slot.getOut()->writeStatus(OK);
    }
};

#endif /* SRC_MAIN_CPP_ARM_NO_OS_I2C_MODULE_COMMANDS_ZCODEI2CSETUPCOMMAND_HPP_ */
