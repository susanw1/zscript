/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ZCODE_ADDRESSING_ZCODEADDRESSROUTER_HPP_
#define SRC_MAIN_CPP_ZCODE_ADDRESSING_ZCODEADDRESSROUTER_HPP_

#include "../ZcodeIncludes.hpp"
#include "../ZcodeBusInterrupt.hpp"
#include "../modules/ZcodeModule.hpp"
#include "ZcodeModuleAddressingSystem.hpp"

#define ADDRESSING_LEVEL_HELPER___0 0,
#define ADDRESSING_LEVEL_HELPER___1 0,
#define ADDRESSING_LEVEL_HELPER___2 0,
#define ADDRESSING_LEVEL_HELPER_0_0 0,
#define ADDRESSING_LEVEL_HELPER_1_1 0,
#define ADDRESSING_LEVEL_HELPER_2_2 0,

#define ADDRESSING_LEVEL_HELPER(x, l) ADDRESSING_LEVEL_HELPER_##x##_ ## l
#define ADDRESSING_LEVEL_HELPERI(x, l) ADDRESSING_LEVEL_HELPER(x, l)
#define ADDRESSING_LEVEL_HELPERII(x, l) ADDRESSING_LEVEL_HELPERI(x, l)
#define ADDRESSING_LEVEL_HELPERIII(x, l) ADDRESSING_LEVEL_HELPERII(x, l)
#define ADDRESSING_LEVEL_HELPERIV(x, l) ADDRESSING_LEVEL_HELPERIII(x, l)
#define ADDRESSING_LEVEL_HELPERV(x, l) ADDRESSING_LEVEL_HELPERIV(x, l)
//
//

#define ADDRESSING_SWITCH_CHECK(x, y) ADDRESSING_LEVEL_HELPERV(_, ADDRESSING_LEVEL##x##y)

#define ADDRESSING_SWITCH_SWITCH(x, y) ADDRESSING_SWITCH##x##y break;
//

#define ADDRESSING_RESP_SWITCH_SWITCH(x, y) ADDRESSING_RESP_SWITCH##x##y break;
//

#define ADDRESSING_IS_ADDRESSED_SWITCH_SWITCH(x, y) ADDRESSING_IS_ADDRESSED_SWITCH##x##y
//

#define ADDRESSING_LEVEL_CHECK0(x, y) ADDRESSING_LEVEL_HELPERV(0, ADDRESSING_LEVEL##x##y)
#define ADDRESSING_LEVEL_CHECK1(x, y) ADDRESSING_LEVEL_HELPERV(1, ADDRESSING_LEVEL##x##y)
#define ADDRESSING_LEVEL_CHECK2(x, y) ADDRESSING_LEVEL_HELPERV(2, ADDRESSING_LEVEL##x##y)

#define ADDRESSING_LEVEL_SWITCH(x, y)
//

#define ADDRESSING_SWITCH() MODULE_SWITCHING_GENERIC(ADDRESSING_SWITCH_CHECK, ADDRESSING_SWITCH_SWITCH)

#define ADDRESSING_RESP_SWITCH() MODULE_SWITCHING_GENERIC(ADDRESSING_SWITCH_CHECK, ADDRESSING_RESP_SWITCH_SWITCH)

#define ADDRESSING_LEVEL(v) MODULE_SWITCHING_GENERIC(ADDRESSING_LEVEL_CHECK##v, ADDRESSING_LEVEL_SWITCH)
//

//

#define ADDRESSING_IS_ADDRESSED() MODULE_SWITCHING_GENERIC(ADDRESSING_SWITCH_CHECK, ADDRESSING_IS_ADDRESSED_SWITCH_SWITCH)

//This sets up the control command
#define COMMAND_VALUE_0012 MODULE_CAPABILITIES_UTIL
template<class ZP>
class ZcodeAddressRouter {
    typedef typename ZP::Strings::string_t string_t;

    static int8_t getHex(char c) {
        if (c >= 'a') {
            return (int8_t) (c <= 'f' ? c - 'a' + 10 : -1);
        } else {
            return (int8_t) (c >= '0' && c <= '9' ? c - '0' : -1);
        }
    }
protected:
    struct AddressSectionReading {
        uint16_t addr;
        uint16_t offset;
        bool ignoreDot;
    };

    //has functions of form:    static void route(ZcodeExecutionCommandSlot<ZP> slot);
    //                          static void response(ZcodeBusInterrupt<ZP> *interrupt, ZcodeOutStream<ZP> *out);
    //                          static bool isAddressed(ZcodeBusInterrupt<ZP> *interrupt);
    //                          static void addressingControlCommand(ZcodeExecutionCommandSlot<ZP> slot);

    static void addressingSwitch(uint16_t module, ZcodeExecutionCommandSlot<ZP> slot, ZcodeAddressingInfo<ZP> addressingInfo) {
        (void) addressingInfo;
        switch (module) {
        ADDRESSING_SWITCH()

    default:
        slot.fail(BAD_ADDRESSING, (string_t) ZP::Strings::failAddressingInvalid);
        }
    }
    static void responseSwitch(ZcodeBusInterrupt<ZP> *interrupt, ZcodeOutStream<ZP> *out) {
        (void) out;
        switch (interrupt->getNotificationModule()) {
        ADDRESSING_RESP_SWITCH()

    default:
        return;
        }
    }
    static int8_t getAddressingLevel(uint16_t module) {
        switch (module) {
        ADDRESSING_LEVEL(0)
        return 0;
        ADDRESSING_LEVEL(1)
        return 1;
        ADDRESSING_LEVEL(2)
        return 2;
    default:
        return -1;
        }
    }
    static bool isAddressed(ZcodeBusInterrupt<ZP> *interrupt) {
        switch (interrupt->getNotificationModule()) {
        ADDRESSING_IS_ADDRESSED()

    default:
        return false;
        }
    }

    //Yes this method is total evil. idk what else to do...
    static void overwriteWithAddressingSymbol(ZcodeExecutionCommandSlot<ZP> slot, ZcodeAddressingInfo<ZP> *addrInfo) {
        if (addrInfo->start > 0 && slot.getBigField()->getData()[addrInfo->start - 1] == '.') {
            slot.overrideProtections()->getInternalSlot()->getBigField()->getData()[addrInfo->start - 1] = '@';
            addrInfo->start--;
        }
    }
    static AddressSectionReading readAddressSection8(const uint8_t *data, uint16_t start, uint16_t length) {
        return readAddressSectionGeneral(data, start, length, 2);
    }
    static AddressSectionReading readAddressSection12(const uint8_t *data, uint16_t start, uint16_t length) {
        return readAddressSectionGeneral(data, start, length, 3);
    }
    static AddressSectionReading readAddressSection16(const uint8_t *data, uint16_t start, uint16_t length) {
        return readAddressSectionGeneral(data, start, length, 4);
    }

private:
    static AddressSectionReading readAddressSectionGeneral(const uint8_t *data, uint16_t start, uint16_t length, uint8_t nibbles) {
        uint16_t addr = 0;
        uint8_t i;
        for (i = 0; i < nibbles && i < length - start; i++) {
            int8_t v = getHex(data[i + start]);
            if (v == -1) {
                break;
            } else {
                addr = (uint16_t) ((addr << 4) | v);
            }
        }
        AddressSectionReading reading;
        reading.ignoreDot = i == 0;
        if (data[i + start] == '.') {
            i++;
            reading.ignoreDot = false;
        }
        reading.addr = addr;
        reading.offset = (uint16_t) (start + i);
        return reading;
    }
};

#endif /* SRC_MAIN_CPP_ZCODE_ADDRESSING_ZCODEADDRESSROUTER_HPP_ */
