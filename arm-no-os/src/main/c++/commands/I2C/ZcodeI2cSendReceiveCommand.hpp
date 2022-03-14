/*
 * ZcodeI2cSendReceiveCommand.hpp
 *
 *  Created on: 12 Apr 2021
 *      Author: robert
 */

#ifndef SRC_TEST_C___COMMANDS_I2C_ZCODEI2CSENDRECEIVECOMMAND_HPP_
#define SRC_TEST_C___COMMANDS_I2C_ZCODEI2CSENDRECEIVECOMMAND_HPP_
#include "ZcodeIncludes.hpp"
#include "ZcodeParameters.hpp"
#include "commands/ZcodeCommand.hpp"
#include "ZcodeI2cSubsystem.hpp"
#include "ZcodeI2cBus.hpp"

class ZcodeI2cSendReceiveCommand: public ZcodeCommand<ZcodeParameters> {
private:
    const uint8_t code = 0x53;

    static void setAsFinished(I2cTerminationStatus status, ZcodeCommandSlot<ZcodeParameters> *slot, uint8_t retries);
    public:
    void finish(ZcodeCommandSlot<ZcodeParameters> *zcodeCommandSlot, ZcodeOutStream<ZcodeParameters> *out) const;

    void moveAlong(ZcodeCommandSlot<ZcodeParameters> *zcodeCommandSlot) const;

    void execute(ZcodeCommandSlot<ZcodeParameters> *slot, ZcodeCommandSequence<ZcodeParameters> *sequence, ZcodeOutStream<ZcodeParameters> *out);

    void setLocks(ZcodeCommandSlot<ZcodeParameters> *slot, ZcodeLockSet<ZcodeParameters> *locks) const;

    uint8_t getCode() const {
        return code;
    }

    bool matchesCode(uint8_t code[], uint8_t length) const {
        return length == 1 && code[0] == ZcodeI2cSendReceiveCommand::code;
    }

    uint8_t getCodeLength() const {
        return 1;
    }

    uint8_t
    const* getFullCode() const {
        return &code;
    }
};
#include "parsing/ZcodeCommandSlot.hpp"
#include "ZcodeOutStream.hpp"
#include "ZcodeLockSet.hpp"

#endif /* SRC_TEST_C___COMMANDS_I2C_ZCODEI2CSENDRECEIVECOMMAND_HPP_ */
