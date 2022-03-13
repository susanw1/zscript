/*
 * ZcodeCapabilitiesCommand.hpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_COMMANDS_ZCODECAPABILITIESCOMMAND_HPP_
#define SRC_TEST_CPP_ZCODE_COMMANDS_ZCODECAPABILITIESCOMMAND_HPP_

#include "../ZcodeIncludes.hpp"
#include "ZcodeCommand.hpp"

template<class ZP>
class ZcodeOutStream;

template<class ZP>
class Zcode;

template<class ZP>
class ZcodeCapabilitiesCommand: public ZcodeCommand<ZP> {
    typedef typename ZP::fieldUnit_t fieldUnit_t;
    private:
    Zcode<ZP> *zcode;
    const uint8_t code = 0x02;
    public:
    ZcodeCapabilitiesCommand(Zcode<ZP> *zcode) :
            zcode(zcode) {
    }

    void execute(ZcodeCommandSlot<ZP> *slot, ZcodeCommandSequence<ZP> *sequence, ZcodeOutStream<ZP> *out);

    void setLocks(ZcodeCommandSlot<ZP> *slot, ZcodeLockSet<ZP> *locks) const {
    }

    uint8_t getCode() const {
        return code;
    }

    bool matchesCode(uint8_t code[], uint8_t length) const {
        return length == 1 && code[0] == ZcodeCapabilitiesCommand::code;
    }

    uint8_t getCodeLength() const {
        return 1;
    }

    uint8_t const* getFullCode() const {
        return &code;
    }
};

#include "../ZcodeOutStream.hpp"
#include "../parsing/ZcodeCommandSlot.hpp"
#include "../parsing/ZcodeCommandFinder.hpp"

template<class ZP>
void ZcodeCapabilitiesCommand<ZP>::execute(ZcodeCommandSlot<ZP> *slot, ZcodeCommandSequence<ZP> *sequence, ZcodeOutStream<ZP> *out) {
    out->writeStatus(OK);
    if (ZP::hugeFieldLength > 0) {
        out->writeField('B', (uint32_t) ZP::hugeFieldLength);
    } else {
        out->writeField('B', (uint32_t) ZP::bigFieldLength);
    }
    uint8_t capabilities = 0x01;
    if (ZP::slotNum > 1) {
        capabilities |= 0x02;
    }
    if (ZP::findInterruptSourceAddress) {
        capabilities |= 0x04;
    }
    if (ZP::isUsingInterruptVector) {
        capabilities |= 0x08;
    }
    out->writeField('C', capabilities);
    if (ZP::persistentMemorySize != 0) {
        out->writeField('P', ZP::persistentMemorySize);
    }
    out->writeField('N', (uint32_t) ZP::fieldNum);
    out->writeField('M', (uint8_t) 1);
    out->writeField('U', (uint8_t) sizeof(fieldUnit_t));
    ZcodeSupportedCommandArray<ZP> supported = zcode->getCommandFinder()->getSupportedCommands();
    out->writeBigHexField(supported.cmds, supported.cmdNum);
    slot->setComplete(true);
}

#endif /* SRC_TEST_CPP_ZCODE_COMMANDS_ZCODECAPABILITIESCOMMAND_HPP_ */
