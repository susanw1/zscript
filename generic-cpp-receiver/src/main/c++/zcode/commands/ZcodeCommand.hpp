/*
 * ZcodeCommand.hpp
 *
 *  Created on: 8 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_COMMANDS_ZCODECOMMAND_HPP_
#define SRC_TEST_CPP_ZCODE_COMMANDS_ZCODECOMMAND_HPP_
#include "../ZcodeIncludes.hpp"

template<class RP>
class ZcodeCommandChannel;

template<class RP>
class ZcodeCommandSlot;

template<class RP>
class ZcodeOutStream;

template<class RP>
class ZcodeCommandSequence;

template<class RP>
class ZcodeLockSet;

template<class RP>
class ZcodeCommand {
public:
    virtual void finish(ZcodeCommandSlot<RP> *rCodeCommandSlot, ZcodeOutStream<RP> *out) const {
    }

    virtual void moveAlong(ZcodeCommandSlot<RP> *rCodeCommandSlot) const {
    }

    virtual void execute(ZcodeCommandSlot<RP> *slot, ZcodeCommandSequence<RP> *sequence, ZcodeOutStream<RP> *out) = 0;

    virtual void setLocks(ZcodeCommandSlot<RP> *slot, ZcodeLockSet<RP> *locks) const = 0;

    virtual uint8_t getCode() const = 0;

    virtual bool matchesCode(uint8_t code[], uint8_t length) const = 0;

    virtual uint8_t getCodeLength() const = 0;

    virtual uint8_t const* getFullCode() const = 0;

};

#endif /* SRC_TEST_CPP_ZCODE_COMMANDS_ZCODECOMMAND_HPP_ */
