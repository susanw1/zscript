/*
 * RCodeI2cSendReceiveCommand.hpp
 *
 *  Created on: 12 Apr 2021
 *      Author: robert
 */

#ifndef SRC_TEST_C___COMMANDS_I2C_RCODEI2CSENDRECEIVECOMMAND_HPP_
#define SRC_TEST_C___COMMANDS_I2C_RCODEI2CSENDRECEIVECOMMAND_HPP_
#include "RCodeIncludes.hpp"
#include "RCodeParameters.hpp"
#include "commands/RCodeCommand.hpp"
#include "RCodeI2cSubsystem.hpp"
#include "RCodeI2cBus.hpp"

class RCodeI2cSendReceiveCommand: public RCodeCommand<RCodeParameters> {
private:
    const uint8_t code = 0x51;

    static void setAsFinished(I2cTerminationStatus status, RCodeCommandSlot<RCodeParameters> *slot, uint8_t retries);
    public:
    void finish(RCodeCommandSlot<RCodeParameters> *rCodeCommandSlot, RCodeOutStream<RCodeParameters> *out) const;

    void moveAlong(RCodeCommandSlot<RCodeParameters> *rCodeCommandSlot) const;

    void execute(RCodeCommandSlot<RCodeParameters> *slot, RCodeCommandSequence<RCodeParameters> *sequence, RCodeOutStream<RCodeParameters> *out);

    void setLocks(RCodeCommandSlot<RCodeParameters> *slot, RCodeLockSet<RCodeParameters> *locks) const;

    uint8_t getCode() const {
        return code;
    }

    bool matchesCode(uint8_t code[], uint8_t length) const {
        return length == 1 && code[0] == RCodeI2cSendReceiveCommand::code;
    }

    uint8_t getCodeLength() const {
        return 1;
    }

    uint8_t
    const* getFullCode() const {
        return &code;
    }
};
#include "parsing/RCodeCommandSlot.hpp"
#include "RCodeOutStream.hpp"
#include "RCodeLockSet.hpp"

#endif /* SRC_TEST_C___COMMANDS_I2C_RCODEI2CSENDRECEIVECOMMAND_HPP_ */
