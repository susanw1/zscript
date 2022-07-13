/*
 * ZcodeAddressRouter.hpp
 *
 *  Created on: 18 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_ZCODEMODULEADDRESSROUTER_HPP_
#define SRC_TEST_CPP_ZCODE_ZCODEMODULEADDRESSROUTER_HPP_

#include "../ZcodeIncludes.hpp"
#include "ZcodeAddressRouter.hpp"

template<class ZP>
class ZcodeExecutionCommandSlot;
template<class ZP>
class Zcode;
template<class ZP>
class ZcodeModule;

template<class ZP>
class ZcodeModuleAddressRouter: public ZcodeAddressRouter<ZP> {
    typedef typename ZP::Strings::string_t string_t;

public:

    void routeAddress(ZcodeExecutionCommandSlot<ZP> slot, uint16_t startPos, uint16_t address, bool hasHadDot) {
        (void) hasHadDot;
        Zcode<ZP> *zcode = &Zcode<ZP>::zcode;
        ZcodeModule<ZP> *target = NULL;
        for (uint8_t i = 0; i < zcode->getModuleNumber(); ++i) {
            if (zcode->getModules()[i]->moduleId == address) {
                target = zcode->getModules()[i];
                break;
            }
        }
        if (target != NULL && target->addressRouter != NULL) {
            target->addressRouter->route(slot, startPos);
        } else {
            slot.fail(BAD_ADDRESSING, (string_t) ZP::Strings::failAddressingInvalidModule);
        }
    }
    void routeResponse(ZcodeOutStream<ZP> *out, ZcodeBusInterrupt<ZP> interrupt) {
        ZcodeModule<ZP> *target = NULL;
        for (uint8_t i = 0; i < Zcode<ZP>::zcode.getModuleNumber(); ++i) {
            if (Zcode<ZP>::zcode.getModules()[i]->moduleId == interrupt.getNotificationType()) {
                target = Zcode<ZP>::zcode.getModules()[i];
                break;
            }
        }
        if (target != NULL && target->addressRouter != NULL) {
            out->writeField16(interrupt.getNotificationType());
            target->addressRouter->routeResponse(out, interrupt);
        }
    }

};
#endif /* SRC_TEST_CPP_ZCODE_ZCODEMODULEADDRESSROUTER_HPP_ */
