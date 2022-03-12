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

template<class RP>
class ZcodeOutStream;

template<class RP>
class Zcode;

template<class RP>
class ZcodeCapabilitiesCommand: public ZcodeCommand<RP> {
    typedef typename RP::fieldUnit_t fieldUnit_t;
    private:
    Zcode<RP> *zcode;
    const uint8_t code = 0x02;
    public:
    ZcodeCapabilitiesCommand(Zcode<RP> *zcode) :
            zcode(zcode) {
    }

    void execute(ZcodeCommandSlot<RP> *slot, ZcodeCommandSequence<RP> *sequence, ZcodeOutStream<RP> *out);

    void setLocks(ZcodeCommandSlot<RP> *slot, ZcodeLockSet<RP> *locks) const {
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

template<class RP>
void ZcodeCapabilitiesCommand<RP>::execute(ZcodeCommandSlot<RP> *slot, ZcodeCommandSequence<RP> *sequence, ZcodeOutStream<RP> *out) {
    out->writeStatus(OK);
    if (RP::bigBigFieldLength > 0) {
        out->writeField('B', (uint32_t) RP::bigBigFieldLength);
    } else {
        out->writeField('B', (uint32_t) RP::bigFieldLength);
    }
    uint8_t capabilities = 0x01;
    if (RP::slotNum > 1) {
        capabilities |= 0x02;
    }
    if (RP::findInterruptSourceAddress) {
        capabilities |= 0x04;
    }
    if (RP::isUsingInterruptVector) {
        capabilities |= 0x08;
    }
    out->writeField('C', capabilities);
    if (RP::persistentMemorySize != 0) {
        out->writeField('P', RP::persistentMemorySize);
    }
    out->writeField('N', (uint32_t) RP::fieldNum);
    out->writeField('M', (uint8_t) 1);
    out->writeField('U', (uint8_t) sizeof(fieldUnit_t));
    ZcodeSupportedCommandArray<RP> supported = zcode->getCommandFinder()->getSupportedCommands();
    out->writeBigHexField(supported.cmds, supported.cmdNum);
    slot->setComplete(true);
}

#endif /* SRC_TEST_CPP_ZCODE_COMMANDS_ZCODECAPABILITIESCOMMAND_HPP_ */
