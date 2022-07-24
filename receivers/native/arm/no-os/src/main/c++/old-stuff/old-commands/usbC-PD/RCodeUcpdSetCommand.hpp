/*
 * RCodeUcpdSetCommand.hpp
 *
 *  Created on: 6 Apr 2021
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_USB_PD_COMMANDS_RCODEUCPDSETCOMMAND_HPP_
#define SRC_TEST_CPP_USB_PD_COMMANDS_RCODEUCPDSETCOMMAND_HPP_
#include "RCodeUcpdSystem.hpp"
#include "RCodeIncludes.hpp"
#include "RCodeParameters.hpp"
#include "RCodeOutStream.hpp"
#include "commands/RCodeCommand.hpp"

class RCodeUcpdSetCommand: public RCodeCommand<RCodeParameters> {
private:
    const uint8_t code[2] = { 0x11, 0x11 };

public:

    void execute(RCodeCommandSlot<RCodeParameters> *slot, RCodeCommandSequence<RCodeParameters> *sequence, RCodeOutStream<RCodeParameters> *out) {
        uint16_t current = Ucpd::getMaxCurrent();
        if (slot->getFields()->has('I')) {
            current = slot->getFields()->getByte('I', 0, 0);
            if (slot->getFields()->getByteCount('I') > 1) {
                current <<= 8;
                current |= slot->getFields()->getByte('I', 1, 0);
            }
        }
        uint16_t minVoltage = Ucpd::getMinVoltage();
        uint16_t maxVoltage = Ucpd::getMaxVoltage();
        if (slot->getFields()->has('V')) {
            minVoltage = slot->getFields()->getByte('V', 0, 0) * 20;
            maxVoltage = minVoltage;
        }
        if (slot->getFields()->has('A')) {
            if (slot->getFields()->getByteCount('A') > 1) {
                minVoltage = (slot->getFields()->getByte('A', 0, 0) << 8) | slot->getFields()->getByte('A', 1, 0);
            } else {
                minVoltage = slot->getFields()->getByte('A', 0, 0);
            }
            maxVoltage = minVoltage;
        }
        if (slot->getFields()->has('M')) {
            if (slot->getFields()->getByteCount('M') > 1) {
                maxVoltage = (slot->getFields()->getByte('M', 0, 0) << 8) | slot->getFields()->getByte('M', 1, 0);
            } else {
                maxVoltage = slot->getFields()->getByte('M', 0, 0);
            }
        }
        if (minVoltage == Ucpd::getMinVoltage() && maxVoltage == Ucpd::getMaxVoltage()) {
            Ucpd::renegotiateCurrent(current);
        } else if (current == Ucpd::getMaxCurrent()) {
            Ucpd::renegotiateVoltage(minVoltage, maxVoltage);
        } else {
            Ucpd::renegotiateAll(current, minVoltage, maxVoltage);
        }
        out->writeStatus(OK);
        slot->setComplete(true);
    }

    void setLocks(RCodeCommandSlot<RCodeParameters> *slot, RCodeLockSet<RCodeParameters> *locks) const {
    }

    uint8_t getCode() const {
        return code[0];
    }

    bool matchesCode(uint8_t code[], uint8_t length) const {
        return length == 2 && code[0] == RCodeUcpdSetCommand::code[0] && code[1] == RCodeUcpdSetCommand::code[1];
    }

    uint8_t getCodeLength() const {
        return 2;
    }

    uint8_t const* getFullCode() const {
        return code;
    }
};

#endif /* SRC_TEST_CPP_USB_PD_COMMANDS_RCODEUCPDSETCOMMAND_HPP_ */
