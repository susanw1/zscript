/*
 * ZcodeAddressRouter.hpp
 *
 *  Created on: 18 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_ZCODEDEBUGADDRESSROUTER_HPP_
#define SRC_TEST_CPP_ZCODE_ZCODEDEBUGADDRESSROUTER_HPP_

#include "../../ZcodeIncludes.hpp"
#include "../ZcodeCommand.hpp"
#include "../../addressing/ZcodeAddressRouter.hpp"

template<class ZP>
class ZcodeOutStream;

template<class ZP>
class ZcodeDebugOutput;

template<class ZP>
class Zcode;

template<class ZP>
class ZcodeDebugAddressRouter: public ZcodeAddressRouter<ZP> {
    typedef typename ZP::debugOutputBufferLength_t debugOutputBufferLength_t;

public:

    void routeAddress(ZcodeExecutionCommandSlot<ZP> slot, uint16_t startPos, uint16_t address, bool hasHadDot) {
        (void) (address);
        (void) (hasHadDot);

        ZcodeOutStream<ZP> *out = slot.getOut();
        slot.getZcode()->getDebug() << (char) ADDRESS_PREFIX;
        slot.getZcode()->getDebug().println((const char*) (slot.getBigField()->getData() + startPos),
                (debugOutputBufferLength_t) (slot.getBigField()->getLength() - startPos));
        out->writeStatus(OK);
    }

    void routeResponse(ZcodeOutStream<ZP> *out, ZcodeBusInterrupt<ZP> interrupt) {
        (void) out;
        (void) interrupt;
    }
};
#endif /* SRC_TEST_CPP_ZCODE_ZCODEDEBUGADDRESSROUTER_HPP_ */
