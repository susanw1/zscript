/*
 * RCodeI2cSetupCommand.hpp
 *
 *  Created on: 13 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_COMMANDS_I2C_RCODEI2CSETUPCOMMAND_HPP_
#define SRC_TEST_CPP_COMMANDS_I2C_RCODEI2CSETUPCOMMAND_HPP_
#include "RCodeIncludes.hpp"
#include "RCodeParameters.hpp"
#include "commands/RCodeCommand.hpp"
#include "RCodeI2cSubsystem.hpp"

class RCodeI2cSetupCommand: public RCodeCommand {
private:
    RCodeI2cSubsystem *i2cSystem;
    const uint8_t code = 0x50;

public:
    RCodeI2cSetupCommand(RCodeI2cSubsystem *i2cSystem) :
            i2cSystem(i2cSystem) {
    }
    void finish(RCodeCommandSlot *rCodeCommandSlot, RCodeOutStream *out) const {
    }

    void execute(RCodeCommandSlot *slot, RCodeCommandSequence *sequence,
            RCodeOutStream *out) {
        slot->setComplete(true);
        uint8_t freqCount = slot->getFields()->countFieldSections('F');
        uint8_t freq;
        if (freqCount > 1) {
            out->writeStatus(BAD_PARAM);
            out->writeBigStringField("Frequency field must be 0-3");
            return;
        } else if (freqCount == 1) {
            freq = slot->getFields()->get('F', 0);
            if (freq > 3) {
                out->writeStatus(BAD_PARAM);
                out->writeBigStringField("Frequency field must be 0-3");
                return;
            }
        }
        if (slot->getFields()->has('A') || slot->getFields()->has('C')
                || slot->getFields()->has('B')) {
            out->writeStatus(CMD_FAIL);
            out->writeBigStringField(
                    "This receiver does not support I2C command channels");
            return;
        }
        i2cSystem->setCreateNotifications(slot->getFields()->has('N'));
        if (freq == 0) {
            i2cSystem->setFrequency(10000);
        } else if (freq == 1) {
            i2cSystem->setFrequency(100000);
        } else if (freq == 2) {
            i2cSystem->setFrequency(400000);
        } else if (freq == 3) {
            i2cSystem->setFrequency(1000000);
        }
        out->writeStatus(OK);
        out->writeField('N', 0);
        if (RCodeParameters::findInterruptSourceAddress) {
            out->writeField('A', 0);
        }
        if (RCodeParameters::isUsingInterruptVector) {
            out->writeField('I', 0);
        }
        out->writeField('C', 0x2F);
        out->writeField('B', RCodeI2cParameters::i2cBussesPerPhyBus * 4);
        out->writeField('F', 3);
    }

    void setLocks(RCodeLockSet *locks) const {
        locks->addLock(RCodeLockValues::i2cPhyBus0Lock, true);
        locks->addLock(RCodeLockValues::i2cPhyBus1Lock, true);
        locks->addLock(RCodeLockValues::i2cPhyBus2Lock, true);
        locks->addLock(RCodeLockValues::i2cPhyBus3Lock, true);
    }

    uint8_t getCode() const {
        return code;
    }

    bool matchesCode(uint8_t code[], uint8_t length) const {
        return length == 1 && code[0] == RCodeI2cSetupCommand::code;
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
