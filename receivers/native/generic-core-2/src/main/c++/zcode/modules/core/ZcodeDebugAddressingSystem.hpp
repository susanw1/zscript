/*
 * ZcodeDebugAddressingSystem.hpp
 *
 *  Created on: 18 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_ZCODEDEBUGADDRESSINGSYSTEM_HPP_
#define SRC_TEST_CPP_ZCODE_ZCODEDEBUGADDRESSINGSYSTEM_HPP_

#include "../../ZcodeIncludes.hpp"
#include "../ZcodeCommand.hpp"
#include "../../addressing/ZcodeModuleAddressingSystem.hpp"

#ifndef ZCODE_SUPPORT_DEBUG
#error Cannot use debug addressing system without debug
#endif

#define ADDRESSING_SWITCH000 ADDRESSING_SWITCH_UTIL(ZcodeDebugAddressingSystem<ZP>::routeAddress)
#define ADDRESSING_RESP_SWITCH000 ADDRESSING_RESP_SWITCH_UTIL(ZcodeDebugAddressingSystem<ZP>::routeResponse)
#define ADDRESSING_LEVEL000 0

template<class ZP>
class ZcodeOutStream;

template<class ZP>
class ZcodeDebugOutput;

template<class ZP>
class Zcode;

template<class ZP>
class ZcodeDebugAddressingSystem: public ZcodeModuleAddressingSystem<ZP> {
    typedef typename ZP::debugOutputBufferLength_t debugOutputBufferLength_t;

public:

    static void routeAddress(ZcodeExecutionCommandSlot<ZP> slot, ZcodeAddressingInfo<ZP> *addressingInfo) {
        slot.getZcode()->getDebug() << (char) Zchars::ADDRESS_PREFIX;
        slot.getZcode()->getDebug().println((const char*) (slot.getBigField()->getData() + addressingInfo->start),
                (debugOutputBufferLength_t) (slot.getBigField()->getLength() - addressingInfo->start));
    }

    static void routeResponse(ZcodeBusInterrupt<ZP> interrupt, ZcodeOutStream<ZP> *out) {
        (void) interrupt;
        (void) out;
    }
};
#endif /* SRC_TEST_CPP_ZCODE_ZCODEDEBUGADDRESSINGSYSTEM_HPP_ */
