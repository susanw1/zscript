/*
 * ZcodeCommand.hpp
 *
 *  Created on: 8 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_COMMANDS_ZCODECOMMAND_HPP_
#define SRC_TEST_CPP_ZCODE_COMMANDS_ZCODECOMMAND_HPP_
#include "../ZcodeIncludes.hpp"

template<class ZP>
class ZcodeCommandChannel;

template<class ZP>
class ZcodeCommandSlot;

template<class ZP>
class ZcodeOutStream;

template<class ZP>
class ZcodeCommandSequence;

template<class ZP>
class ZcodeLockSet;

template<class ZP>
class ZcodeCommand {
public:
    virtual void finish(ZcodeCommandSlot<ZP> *zcodeCommandSlot, ZcodeOutStream<ZP> *out) const {
    }

    virtual void moveAlong(ZcodeCommandSlot<ZP> *zcodeCommandSlot) const {
    }

    virtual void execute(ZcodeCommandSlot<ZP> *slot, ZcodeCommandSequence<ZP> *sequence, ZcodeOutStream<ZP> *out) = 0;

    virtual void setLocks(ZcodeCommandSlot<ZP> *slot, ZcodeLockSet<ZP> *locks) const = 0;

    virtual uint8_t getCode() const = 0;

    virtual bool matchesCode(uint8_t code[], uint8_t length) const = 0;

    virtual uint8_t getCodeLength() const = 0;

    virtual uint8_t const* getFullCode() const = 0;

};

#endif /* SRC_TEST_CPP_ZCODE_COMMANDS_ZCODECOMMAND_HPP_ */
