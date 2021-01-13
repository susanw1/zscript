/*
 * RCodePinSetupCommand.hpp
 *
 *  Created on: 8 Jan 2021
 *      Author: robert
 */

#ifndef SRC_TEST_C___COMMANDS_PINS_RCODEPINSETUPCOMMAND_HPP_
#define SRC_TEST_C___COMMANDS_PINS_RCODEPINSETUPCOMMAND_HPP_
#include "RCodeIncludes.hpp"
#include "RCodeParameters.hpp"
#include <RCode.hpp>
#include "RCodePinSystem.hpp"
#include "RCodePinInterruptSource.hpp"
#include <AToDLowLevel/AtoD.hpp>
#include <AToDLowLevel/AtoDManager.hpp>

class RCodePinSetupCommand: public RCodeCommand<RCodeParameters> {
private:
    const uint8_t code = 0x40;

public:

    void execute(RCodeCommandSlot<RCodeParameters> *slot, RCodeCommandSequence<RCodeParameters> *sequence, RCodeOutStream<RCodeParameters> *out);

    void setLocks(RCodeCommandSlot<RCodeParameters> *slot, RCodeLockSet<RCodeParameters> *locks) const {
    }

    uint8_t getCode() const {
        return code;
    }

    bool matchesCode(uint8_t code[], uint8_t length) const {
        return length == 1 && code[0] == RCodePinSetupCommand::code;
    }

    uint8_t getCodeLength() const {
        return 1;
    }

    uint8_t const* getFullCode() const {
        return &code;
    }

};

#endif /* SRC_TEST_C___COMMANDS_PINS_RCODEPINSETUPCOMMAND_HPP_ */
