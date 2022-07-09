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
public:

    void routeAddress(ZcodeExecutionCommandSlot<ZP> slot, uint16_t startPos, uint16_t address, bool hasHadDot) {
        if (hasHadDot) {
            Zcode<ZP> *zcode = slot.getZcode();
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
                slot.fail(BAD_ADDRESSING, ZP::Strings::failAddressingInvalidModule);
            }
        } else {
            slot.fail(BAD_ADDRESSING, ZP::Strings::failAddressTooShort);
        }
    }

};
#endif /* SRC_TEST_CPP_ZCODE_ZCODEMODULEADDRESSROUTER_HPP_ */
