/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ZCODE_MODULES_SCRIPT_ZCODEINTERRUPTSETCOMMAND_HPP_
#define SRC_MAIN_CPP_ZCODE_MODULES_SCRIPT_ZCODEINTERRUPTSETCOMMAND_HPP_

#include "../../ZcodeIncludes.hpp"
#include "../ZcodeCommand.hpp"

#define COMMAND_EXISTS_0029 EXISTENCE_MARKER_UTIL

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
    typedef typename ZP::Strings::string_t string_t;

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
            slot.fail(BAD_PARAM, (string_t) ZP::Strings::failInterruptBadType);
            return;
        }
        if (!slot.getFields()->get('V', &vector) || vector >= ZP::maxScriptSize) {
            slot.fail(BAD_PARAM, (string_t) ZP::Strings::failInterruptBadVector);
            return;
        }
        if (slot.getFields()->get('B', &bus)) {
            hasBus = true;
        }
        if (bus > 0xFF) {
            slot.fail(BAD_PARAM, (string_t) ZP::Strings::failInterruptBadBus);
            return;
        }
        if (slot.getFields()->get('B', &address)) {
            hasAddress = true;
        }
        if (address > 0xFF) {
            slot.fail(BAD_PARAM, (string_t) ZP::Strings::failInterruptBadAddress);
            return;
        }
        if (hasAddress && !hasBus) {
            slot.fail(BAD_PARAM, (string_t) ZP::Strings::failInterruptHaveAddressWithoutBus);
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

#endif /* SRC_MAIN_CPP_ZCODE_MODULES_SCRIPT_ZCODEINTERRUPTSETCOMMAND_HPP_ */
