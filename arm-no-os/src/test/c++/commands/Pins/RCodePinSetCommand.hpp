/*
 * RCodePinSetCommand.hpp
 *
 *  Created on: 12 Jan 2021
 *      Author: robert
 */

#ifndef SRC_TEST_C___COMMANDS_PINS_RCODEPINSETCOMMAND_HPP_
#define SRC_TEST_C___COMMANDS_PINS_RCODEPINSETCOMMAND_HPP_
#include "RCodeIncludes.hpp"
#include "RCodeParameters.hpp"
#include <RCode.hpp>
#include "RCodePinSystem.hpp"
#include <AToDLowLevel/AtoD.hpp>
#include <AToDLowLevel/AtoDManager.hpp>

class RCodePinSetCommand: public RCodeCommand<RCodeParameters> {
private:
    const uint8_t code = 0x41;

public:

    void execute(RCodeCommandSlot<RCodeParameters> *slot, RCodeCommandSequence<RCodeParameters> *sequence, RCodeOutStream<RCodeParameters> *out);

    void setLocks(RCodeCommandSlot<RCodeParameters> *slot, RCodeLockSet<RCodeParameters> *locks) const {
    }

    uint8_t getCode() const {
        return code;
    }

    bool matchesCode(uint8_t code[], uint8_t length) const {
        return length == 1 && code[0] == RCodePinSetCommand::code;
    }

    uint8_t getCodeLength() const {
        return 1;
    }

    uint8_t const* getFullCode() const {
        return &code;
    }

};
#endif /* SRC_TEST_C___COMMANDS_PINS_RCODEPINSETCOMMAND_HPP_ */
