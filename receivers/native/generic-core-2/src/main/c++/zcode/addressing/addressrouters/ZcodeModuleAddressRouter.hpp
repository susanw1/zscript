/*
 * ZcodeModuleAddressRouter.hpp
 *
 *  Created on: 21 Jul 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___ZCODE_ADDRESSING_ZCODEMODULEADDRESSROUTER_HPP_
#define SRC_MAIN_C___ZCODE_ADDRESSING_ZCODEMODULEADDRESSROUTER_HPP_

#include "../../ZcodeIncludes.hpp"
#include "../../ZcodeBusInterrupt.hpp"
#include "../ZcodeAddressRouter.hpp"

template<class ZP>
class ZcodeModuleAddressRouter: public ZcodeAddressRouter<ZP> {
    typedef typename ZcodeAddressRouter<ZP>::AddressSectionReading AddressSectionReading;

public:
    static void route(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeAddressingInfo<ZP> addrInfo;
        AddressSectionReading moduleReading = ZcodeAddressRouter<ZP>::readAddressSection12(slot.getBigField()->getData(), 0, slot.getBigField()->getLength());
        uint16_t module = moduleReading.addr;
        int8_t sections = ZcodeAddressRouter<ZP>::getAddressingLevel(module);

        if (sections == 0) {
            addrInfo.start = moduleReading.offset;
            addrInfo.addr = 0;
            addrInfo.port = 0;
        } else if (sections == 1 || sections == 2) {
            AddressSectionReading portReading = ZcodeAddressRouter<ZP>::readAddressSection8(slot.getBigField()->getData(), moduleReading.offset, slot.getBigField()->getLength());
            addrInfo.port = (uint8_t) portReading.addr;
            if (sections == 1) {
                addrInfo.start = portReading.offset;
                addrInfo.addr = 0;
            } else {
                AddressSectionReading addrReading =
                        ZcodeAddressRouter<ZP>::readAddressSection12(slot.getBigField()->getData(), portReading.offset, slot.getBigField()->getLength());
                addrInfo.start = addrReading.offset;
                addrInfo.addr = addrReading.addr;
            }
        } else {
            slot.fail(BAD_ADDRESSING, "Invalid Address");
            return;
        }
        ZcodeAddressRouter<ZP>::addressingSwitch(module, slot, addrInfo);
    }

    static void response(ZcodeBusInterrupt<ZP> interrupt, ZcodeOutStream<ZP> *out) {
        int8_t sections = ZcodeAddressRouter<ZP>::getAddressingLevel(interrupt.getNotificationModule());
        if (sections == -1) {
            interrupt.clear();
            return;
        }
        out->markAddressing();
        out->writeField16(interrupt.getNotificationModule());
        if (sections > 0) {
            out->markAddressingContinue();
            out->writeField8(interrupt.getNotificationPort());
        }
        if (sections == 2 && interrupt.hasFindableAddress() && interrupt.hasFoundAddress()) {
            out->markAddressingContinue();
            out->writeField16(interrupt.getFoundAddress());
        }
        ZcodeAddressRouter<ZP>::responseSwitch(interrupt, out);
    }
    static bool isAddressed(ZcodeBusInterrupt<ZP> interrupt) {
        int8_t sections = ZcodeAddressRouter<ZP>::getAddressingLevel(interrupt.getNotificationModule());
        return sections != -1;
    }

    static void addressingControlCommand(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        out->writeField16('R', 0); // some kind of route id...
        out->writeStatus(OK);
    }

};

#endif /* SRC_MAIN_C___ZCODE_ADDRESSING_ZCODEMODULEADDRESSROUTER_HPP_ */
