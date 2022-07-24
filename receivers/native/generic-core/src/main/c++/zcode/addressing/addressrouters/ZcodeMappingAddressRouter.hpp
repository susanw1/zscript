/*
 * ZcodeMappingAddressRouter.hpp
 *
 *  Created on: 21 Jul 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___ZCODE_ADDRESSING_ZCODEMAPPINGADDRESSROUTER_HPP_
#define SRC_MAIN_C___ZCODE_ADDRESSING_ZCODEMAPPINGADDRESSROUTER_HPP_

#include "../../ZcodeIncludes.hpp"
#include "../../ZcodeBusInterrupt.hpp"
#include "../ZcodeAddressRouter.hpp"

#ifndef ZCODE_SUPPORT_ADDRESSING
#error Cannot use addressing system without addressing
#endif

template<class ZP>
class ZcodeMappingAddressRouter: public ZcodeAddressRouter<ZP> {
    typedef typename ZcodeAddressRouter<ZP>::AddressSectionReading AddressSectionReading;
    typedef typename ZP::Strings::string_t string_t;

    struct ZcodeAddressMapping {
        uint16_t module :12;
        uint16_t address :12;
        uint8_t port :8;
    };

    static ZcodeAddressMapping map[ZP::mappingAddressCount];

public:
    static void setAddress(uint16_t mapAddr, uint16_t module, uint8_t port, uint16_t address) {
        if (mapAddr < ZP::mappingAddressCount) {
            ZcodeAddressMapping mapping;
            mapping.module = module;
            mapping.port = port;
            mapping.address = address;
            map[mapAddr] = mapping;
        }
    }

    static void route(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeAddressingInfo<ZP> addrInfo;
        AddressSectionReading reading = ZcodeAddressRouter<ZP>::readAddressSection12(slot.getBigField()->getData(), 0, slot.getBigField()->getLength());

        if (reading.addr >= ZP::mappingAddressCount) {
            slot.fail(BAD_ADDRESSING, (string_t) ZP::Strings::failAddressingInvalid);
            return;
        }
        ZcodeAddressMapping mapping = map[reading.addr];
        int8_t sections = ZcodeAddressRouter<ZP>::getAddressingLevel(mapping.module);

        addrInfo.start = reading.offset;

        if (sections == 0) {
            addrInfo.addr = 0;
            addrInfo.port = 0;
        } else if (sections == 1) {
            addrInfo.addr = mapping.address;
            addrInfo.port = 0;
        } else if (sections == 2) {
            addrInfo.addr = mapping.address;
            addrInfo.port = mapping.port;
        } else {
            slot.fail(BAD_ADDRESSING, (string_t) ZP::Strings::failAddressingInvalid);
            return;
        }
        ZcodeAddressRouter<ZP>::overwriteWithAddressingSymbol(slot, &addrInfo);
        ZcodeAddressRouter<ZP>::addressingSwitch(mapping.module, slot, addrInfo);
    }

    static void response(ZcodeBusInterrupt<ZP> interrupt, ZcodeOutStream<ZP> *out) {
        int8_t sections = ZcodeAddressRouter<ZP>::getAddressingLevel(interrupt.getNotificationModule());
        if (sections == -1) {
            interrupt.clear();
            return;
        }
        uint16_t module = interrupt.getNotificationModule();
        uint8_t port = interrupt.getNotificationPort();
        uint16_t addr = interrupt.getFoundAddress();

        int16_t mappingAddress = -1;
        for (uint16_t i = 0; i < ZP::mappingAddressCount; ++i) {
            ZcodeAddressMapping mapping = map[i];
            if (mapping.module == module) {
                if (sections < 1 || mapping.port == port) {
                    if (sections < 2 || mapping.address == addr) {
                        mappingAddress = i;
                        break;
                    }
                }
            }
        }
        if (mappingAddress == -1) {
            interrupt.clear();
            return;
        }

        out->lock();
        out->markAddressing();
        out->writeField16((uint16_t) mappingAddress);
        ZcodeAddressRouter<ZP>::responseSwitch(interrupt, out);
        out->writeCommandSequenceSeparator();
        out->unlock();
        interrupt.clear();
    }

    static bool isAddressed(ZcodeBusInterrupt<ZP> interrupt) {
        int8_t sections = ZcodeAddressRouter<ZP>::getAddressingLevel(interrupt.getNotificationModule());
        if (sections == -1) {
            return false;
        }
        uint16_t module = interrupt.getNotificationModule();
        uint8_t port = interrupt.getNotificationPort();
        uint16_t addr = interrupt.getFoundAddress();

        for (uint16_t i = 0; i < ZP::mappingAddressCount; ++i) {
            ZcodeAddressMapping mapping = map[i];
            if (mapping.module == module) {
                if (sections < 1 || mapping.port == port) {
                    if (sections < 2 || mapping.address == addr) {
                        return true;
                    }
                }
            }
        }

        return false;
    }

    static void addressingControlCommand(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        uint16_t mappedAddress;
        uint16_t module;
        uint16_t port = 0;
        uint16_t address = 0;
        if (slot.getFields()->get('R', &mappedAddress)) {
            if (mappedAddress >= ZP::mappingAddressCount) {
                slot.fail(BAD_ADDRESSING, (string_t) ZP::Strings::failAddressingInvalid);
                return;
            }
            if (!slot.getFields()->get('M', &module)) {
                slot.fail(BAD_ADDRESSING, (string_t) ZP::Strings::failAddressingInvalid);
                return;
            }
            int8_t sections = ZcodeAddressRouter<ZP>::getAddressingLevel(module);
            if (sections == -1) {
                slot.fail(BAD_ADDRESSING, (string_t) ZP::Strings::failAddressingInvalid);
                return;
            }
            if (sections > 0) {
                if (!slot.getFields()->get('P', &port)) {
                    slot.fail(BAD_ADDRESSING, (string_t) ZP::Strings::failAddressingInvalid);
                    return;
                }
            }
            if (sections == 2) {
                if (!slot.getFields()->get('A', &address)) {
                    slot.fail(BAD_ADDRESSING, (string_t) ZP::Strings::failAddressingInvalid);
                    return;
                }
            }
            setAddress(mappedAddress, module, port, address);
        }
        out->writeField16('R', 1); // some kind of route id...
        out->writeField16('A', ZP::mappingAddressCount);
        out->writeStatus(OK);
    }

};

template<class ZP>
typename ZcodeMappingAddressRouter<ZP>::ZcodeAddressMapping ZcodeMappingAddressRouter<ZP>::map[ZP::mappingAddressCount];

#endif /* SRC_MAIN_C___ZCODE_ADDRESSING_ZCODEMAPPINGADDRESSROUTER_HPP_ */
