/*
 * ZcodeCapabilitiesCommand.hpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEINTERRUPTSETCOMMAND_HPP_
#define SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEINTERRUPTSETCOMMAND_HPP_

#include "../../ZcodeIncludes.hpp"
#include "../ZcodeCommand.hpp"

template<class ZP>
class ZcodeOutStream;
template<class ZP>
class ZcodeInterruptVectorMap;
template<class ZP>
class ZcodeScriptSpace;

template<class ZP>
class Zcode;

template<class ZP>
class ZcodeInterruptSetCommand: public ZcodeCommand<ZP> {
private:
    static const uint8_t CODE = 0x09;

public:

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        ZcodeInterruptVectorMap<ZP> *map = slot.getZcode()->getNotificationManager()->getVectorChannel()->getVectorMap();
        uint16_t type = 0;
        uint16_t bus = 0;
        bool hasBus = false;
        uint16_t address = 0;
        bool hasAddress = false;
        uint16_t vector = 0;
        if (!slot.getFields()->get('T', &type) || type > 0xFF) {
            slot.fail(BAD_PARAM, "Need valid interrupt type to set interrupt");
            return;
        }
        if (!slot.getFields()->get('V', &vector) || vector >= ZP::scriptLength) {
            slot.fail(BAD_PARAM, "Need valid interrupt vector to set interrupt");
            return;
        }
        if (slot.getFields()->get('B', &bus)) {
            hasBus = true;
        }
        if (bus > 0xFF) {
            slot.fail(BAD_PARAM, "Bus value needs to be 1 byte");
            return;
        }
        if (slot.getFields()->get('B', &address)) {
            hasAddress = true;
        }
        if (address > 0xFF) {
            slot.fail(BAD_PARAM, "Address value needs to be 1 byte");
            return;
        }
        if (hasAddress && !hasBus) {
            slot.fail(BAD_PARAM, "Address value needs to be 1 byte");
            return;
        }
        if (hasAddress) {
            map->setVector((uint8_t) type, (uint8_t) bus, (uint8_t) address, vector);
        } else if (hasBus) {
            map->setVector((uint8_t) type, (uint8_t) bus, vector);
        } else {
            map->setVector((uint8_t) type, vector);
        }
        out->writeStatus(OK);

    }

}
;

#endif /* SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEINTERRUPTSETCOMMAND_HPP_ */
