/*
 * ZcodeI2cSetupCommand.hpp
 *
 *  Created on: 13 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_COMMANDS_I2C_RCODEI2CSETUPCOMMAND_HPP_
#define SRC_TEST_CPP_COMMANDS_I2C_RCODEI2CSETUPCOMMAND_HPP_
#include "ZcodeIncludes.hpp"
#include "ZcodeParameters.hpp"
#include "commands/ZcodeCommand.hpp"
#include "ZcodeI2cSubsystem.hpp"

class ZcodeI2cSetupCommand: public ZcodeCommand<ZcodeParameters> {
private:
    const uint8_t code = 0x50;

public:

    void execute(ZcodeCommandSlot<ZcodeParameters> *slot, ZcodeCommandSequence<ZcodeParameters> *sequence, ZcodeOutStream<ZcodeParameters> *out) {
        slot->setComplete(true);
        uint8_t freqCount = slot->getFields()->countFieldSections('F');
        uint8_t freq = 1;
        if (freqCount > 1) {
            slot->fail("", BAD_PARAM);
            out->writeStatus(BAD_PARAM);
            out->writeBigStringField("Frequency field must be 0-3");
            return;
        } else if (freqCount == 1) {
            freq = slot->getFields()->get('F', 0);
            if (freq > 3) {
                slot->fail("", BAD_PARAM);
                out->writeStatus(BAD_PARAM);
                out->writeBigStringField("Frequency field must be 0-3");
                return;
            }
        }
        ZcodeI2cSubsystem::setCreateNotifications(slot->getFields()->has('N'));
        if (freqCount != 0) {
            if (freq == 0) {
                ZcodeI2cSubsystem::setFrequency(kHz10);
            } else if (freq == 1) {
                ZcodeI2cSubsystem::setFrequency(kHz100);
            } else if (freq == 2) {
                ZcodeI2cSubsystem::setFrequency(kHz400);
            } else if (freq == 3) {
                ZcodeI2cSubsystem::setFrequency(kHz1000);
            }
        }
        out->writeStatus(OK);
        out->writeField('N', (uint8_t) 0);
        if (ZcodeParameters::findInterruptSourceAddress) {
            out->writeField('A', (uint8_t) 0);
        }
        if (ZcodeParameters::isUsingInterruptVector) {
            out->writeField('I', (uint8_t) 0);
        }
        out->writeField('C', (uint8_t) 0x2F);
        out->writeField('B', (uint8_t)(ZcodePeripheralParameters::i2cBussesPerPhyBus * 4));
        out->writeField('F', (uint8_t) 3);
    }

    void setLocks(ZcodeCommandSlot<ZcodeParameters> *slot, ZcodeLockSet<ZcodeParameters> *locks) const {
        locks->addLock(ZcodeParameters::i2cPhyBus0Lock, true);
        locks->addLock(ZcodeParameters::i2cPhyBus1Lock, true);
        locks->addLock(ZcodeParameters::i2cPhyBus2Lock, true);
        locks->addLock(ZcodeParameters::i2cPhyBus3Lock, true);
    }

    uint8_t getCode() const {
        return code;
    }

    bool matchesCode(uint8_t code[], uint8_t length) const {
        return length == 1 && code[0] == ZcodeI2cSetupCommand::code;
    }

    uint8_t getCodeLength() const {
        return 1;
    }

    uint8_t
    const* getFullCode() const {
        return &code;
    }
}
;

#endif /* SRC_TEST_CPP_COMMANDS_I2C_RCODEI2CSETUPCOMMAND_HPP_ */
