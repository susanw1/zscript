/*
 * ZcodeCapabilitiesCommand.hpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEI2CSETUPCOMMAND_HPP_
#define SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEI2CSETUPCOMMAND_HPP_

#include <modules/ZcodeCommand.hpp>
#include "../../LowLevel/I2cLowLevel/I2cManager.hpp"

template<class ZP>
class ZcodeI2cSetupCommand: public ZcodeCommand<ZP> {
private:

public:
    static constexpr uint8_t CODE = 0x01;

    // if set, determines which port to control; else set all ports
    static constexpr char CMD_PARAM_I2C_PORT_P = 'P';

    // chooses comms frequency, from 10, 100, 400 and 1000 kHz
    static constexpr char CMD_PARAM_FREQ_F = 'F';

    typedef typename ZP::LL LL;

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
                if (port >= LL::i2cCount) {
                    slot.fail(BAD_PARAM, "Invalid I2C port");
                    return;
                }
                I2cManager<LL>::getI2cById(port)->setFrequency((I2cFrequency) freqValue);
            } else {
                for (I2cIdentifier i = 0; i < LL::i2cCount; i++) {
                    I2cManager<LL>::getI2cById(i)->setFrequency((I2cFrequency) freqValue);
                }
            }
        }
        slot.getOut()->writeStatus(OK);
    }
};

#endif /* SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEI2CSETUPCOMMAND_HPP_ */
