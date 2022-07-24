/*
 * ZcodeVoidAddressRouter.hpp
 *
 *  Created on: 21 Jul 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___ZCODE_ADDRESSING_ZCODEVOIDADDRESSROUTER_HPP_
#define SRC_MAIN_C___ZCODE_ADDRESSING_ZCODEVOIDADDRESSROUTER_HPP_

#include "../../ZcodeIncludes.hpp"
#include "../../ZcodeBusInterrupt.hpp"
#include "../ZcodeAddressRouter.hpp"

#ifndef ZCODE_SUPPORT_ADDRESSING
#error Cannot use addressing system without addressing
#endif

template<class ZP>
class ZcodeVoidAddressRouter: public ZcodeAddressRouter<ZP> {
    typedef typename ZP::Strings::string_t string_t;

public:
    static void route(ZcodeExecutionCommandSlot<ZP> slot) {
        slot.fail(BAD_ADDRESSING, (string_t) ZP::Strings::failAddressingNotSetup);
    }
    static void response(ZcodeBusInterrupt<ZP> interrupt, ZcodeOutStream<ZP> *out) {
        (void) interrupt;
        (void) out;
    }
    static bool isAddressed(ZcodeBusInterrupt<ZP> interrupt) {
        (void) interrupt;
        return false;
    }

};

#endif /* SRC_MAIN_C___ZCODE_ADDRESSING_ZCODEVOIDADDRESSROUTER_HPP_ */
