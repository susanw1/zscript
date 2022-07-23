/*
 * ZcodeVoidAddressRouter.hpp
 *
 *  Created on: 21 Jul 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___ZCODE_ADDRESSING_ZCODEVOIDADDRESSROUTER_HPP_
#define SRC_MAIN_C___ZCODE_ADDRESSING_ZCODEVOIDADDRESSROUTER_HPP_

#include "../ZcodeIncludes.hpp"
#include "../ZcodeBusInterrupt.hpp"
#include "ZcodeAddressRouter.hpp"

template<class ZP>
class ZcodeVoidAddressRouter {

public:
    static void route(ZcodeExecutionCommandSlot<ZP> slot) {
        slot.fail(BAD_ADDRESSING, "Addressing not set up");
    }
    static void response(ZcodeBusInterrupt<ZP> interrupt, ZcodeOutStream<ZP> *out) {

    }
    static bool isAddressed(ZcodeBusInterrupt<ZP> interrupt) {
        return false;
    }

};

#endif /* SRC_MAIN_C___ZCODE_ADDRESSING_ZCODEVOIDADDRESSROUTER_HPP_ */
