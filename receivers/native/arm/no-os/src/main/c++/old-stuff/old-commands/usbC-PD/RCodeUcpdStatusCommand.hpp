/*
 * RCodeUcpdStatusCommand.hpp
 *
 *  Created on: 6 Apr 2021
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_USB_PD_COMMANDS_RCODEUCPDSTATUSCOMMAND_HPP_
#define SRC_TEST_CPP_USB_PD_COMMANDS_RCODEUCPDSTATUSCOMMAND_HPP_
#include "RCodeUcpdSystem.hpp"
#include "RCodeIncludes.hpp"
#include "RCodeParameters.hpp"
#include "RCodeOutStream.hpp"
#include "commands/RCodeCommand.hpp"

class RCodeUcpdStatusCommand: public RCodeCommand<RCodeParameters> {
private:
    const uint8_t code[2] = { 0x11, 0x10 };

public:

    void execute(RCodeCommandSlot<RCodeParameters> *slot, RCodeCommandSequence<RCodeParameters> *sequence, RCodeOutStream<RCodeParameters> *out) {
        out->writeStatus(OK);
        out->writeField('I', (uint16_t) Ucpd::getMaxCurrent());
        out->writeField('V', (uint8_t) (Ucpd::getMinVoltage() / 20));

        out->writeField('A', (uint16_t) Ucpd::getMinVoltage());
        out->writeField('M', (uint16_t) Ucpd::getMaxVoltage());

        out->writeField('P', (uint16_t) Ucpd::getPowerStatus());
        slot->setComplete(true);
    }

    void setLocks(RCodeCommandSlot<RCodeParameters> *slot, RCodeLockSet<RCodeParameters> *locks) const {
    }

    uint8_t getCode() const {
        return code[0];
    }

    bool matchesCode(uint8_t code[], uint8_t length) const {
        return length == 2 && code[0] == RCodeUcpdStatusCommand::code[0] && code[1] == RCodeUcpdStatusCommand::code[1];
    }

    uint8_t getCodeLength() const {
        return 2;
    }

    uint8_t const* getFullCode() const {
        return code;
    }
};

#endif /* SRC_TEST_CPP_USB_PD_COMMANDS_RCODEUCPDSTATUSCOMMAND_HPP_ */
