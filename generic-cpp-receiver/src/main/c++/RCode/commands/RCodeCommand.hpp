/*
 * RCodeCommand.hpp
 *
 *  Created on: 8 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_COMMANDS_RCODECOMMAND_HPP_
#define SRC_TEST_CPP_RCODE_COMMANDS_RCODECOMMAND_HPP_
#include "../RCodeIncludes.hpp"

template<class RP>
class RCodeCommandChannel;

template<class RP>
class RCodeCommandSlot;

template<class RP>
class RCodeOutStream;

template<class RP>
class RCodeCommandSequence;

template<class RP>
class RCodeLockSet;

template<class RP>
class RCodeCommand {
public:
    virtual void finish(RCodeCommandSlot<RP> *rCodeCommandSlot, RCodeOutStream<RP> *out) const {
    }

    virtual void moveAlong(RCodeCommandSlot<RP> *rCodeCommandSlot) const {
    }

    virtual void execute(RCodeCommandSlot<RP> *slot, RCodeCommandSequence<RP> *sequence, RCodeOutStream<RP> *out) = 0;

    virtual void setLocks(RCodeCommandSlot<RP> *slot, RCodeLockSet<RP> *locks) const = 0;

    virtual uint8_t getCode() const = 0;

    virtual bool matchesCode(uint8_t code[], uint8_t length) const = 0;

    virtual uint8_t getCodeLength() const = 0;

    virtual uint8_t const* getFullCode() const = 0;

};

#endif /* SRC_TEST_CPP_RCODE_COMMANDS_RCODECOMMAND_HPP_ */
