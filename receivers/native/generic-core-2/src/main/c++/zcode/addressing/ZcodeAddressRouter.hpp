/*
 * ZcodeAddressRouter.hpp
 *
 *  Created on: 18 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_ZCODEADDRESSROUTER_HPP_
#define SRC_TEST_CPP_ZCODE_ZCODEADDRESSROUTER_HPP_

#include "../ZcodeIncludes.hpp"

template<class ZP>
class ZcodeExecutionCommandSlot;

template<class ZP>
class ZcodeAddressRouter {

    int8_t getHex(char c) {
        if (c >= 'a') {
            return (int8_t) (c <= 'f' ? c - 'a' + 10 : -1);
        } else {
            return (int8_t) (c >= '0' && c <= '9' ? c - '0' : -1);
        }
    }
public:
    void route(ZcodeExecutionCommandSlot<ZP> slot, uint16_t startPos) {
        const uint8_t *data = slot.getBigField()->getData() + startPos;
        uint16_t lengthToTransmit = (uint16_t) (slot.getBigField()->getLength() - startPos);
        if (slot.getChannel() != slot.getZcode()->getNotificationManager()->getNotificationChannel()) {
            slot.fail(BAD_ADDRESSING, "Addressing only allowed from notification channel");
            return;
        }
        uint16_t addr = 0;
        uint8_t i;
        for (i = 0; i < 4 && i < lengthToTransmit; i++) {
            int8_t v = getHex(data[i]);
            if (v == -1) {
                break;
            } else {
                addr = (uint16_t) ((addr << 4) | v);
            }
        }
        if (data[i] == '.') {
            i++;
            routeAddress(slot, (uint16_t) (startPos + i), addr, true);
        } else {
            routeAddress(slot, (uint16_t) (startPos + i), addr, false);
        }
    }

    virtual void routeAddress(ZcodeExecutionCommandSlot<ZP> slot, uint16_t startPos, uint16_t address, bool hasHadDot) = 0;

};
#endif /* SRC_TEST_CPP_ZCODE_ZCODEADDRESSROUTER_HPP_ */
