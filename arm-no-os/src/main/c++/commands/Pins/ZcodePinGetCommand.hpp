/*
 * ZcodePinGetCommand.hpp
 *
 *  Created on: 12 Jan 2021
 *      Author: robert
 */

#ifndef SRC_TEST_C___COMMANDS_PINS_ZCODEPINGETCOMMAND_HPP_
#define SRC_TEST_C___COMMANDS_PINS_ZCODEPINGETCOMMAND_HPP_

#include "ZcodeIncludes.hpp"
#include "ZcodeParameters.hpp"
#include <Zcode.hpp>
#include "ZcodePinSystem.hpp"
#include "../../LowLevel/AToDLowLevel/AtoD.hpp"
#include "../../LowLevel/AToDLowLevel/AtoDManager.hpp"

class ZcodePinGetCommand: public ZcodeCommand<ZcodeParameters> {
private:
    const uint8_t code = 0x42;

public:

    void execute(ZcodeCommandSlot<ZcodeParameters> *slot, ZcodeCommandSequence<ZcodeParameters> *sequence, ZcodeOutStream<ZcodeParameters> *out);

    void setLocks(ZcodeCommandSlot<ZcodeParameters> *slot, ZcodeLockSet<ZcodeParameters> *locks) const {
    }

    uint8_t getCode() const {
        return code;
    }

    bool matchesCode(uint8_t code[], uint8_t length) const {
        return length == 1 && code[0] == ZcodePinGetCommand::code;
    }

    uint8_t getCodeLength() const {
        return 1;
    }

    uint8_t const* getFullCode() const {
        return &code;
    }

};

#endif /* SRC_TEST_C___COMMANDS_PINS_ZCODEPINGETCOMMAND_HPP_ */
