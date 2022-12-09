/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ZCODE_MODULES_CORE_ZCODEDEBUGADDRESSINGSYSTEM_HPP_
#define SRC_MAIN_CPP_ZCODE_MODULES_CORE_ZCODEDEBUGADDRESSINGSYSTEM_HPP_

#include "../../ZcodeIncludes.hpp"
#include "../ZcodeCommand.hpp"
#include "../../addressing/ZcodeModuleAddressingSystem.hpp"

#ifndef ZCODE_SUPPORT_DEBUG
#error Cannot use debug addressing system without debug
#endif

#define ADDRESSING_SWITCH000 ADDRESSING_SWITCH_UTIL(ZcodeDebugAddressingSystem<ZP>::routeAddress)
#define ADDRESSING_RESP_SWITCH000 ADDRESSING_RESP_SWITCH_UTIL(ZcodeDebugAddressingSystem<ZP>::routeResponse)
#define ADDRESSING_IS_ADDRESSED_SWITCH000 ADDRESSING_IS_ADDRESSED_SWITCH_UTIL(ZcodeDebugAddressingSystem<ZP>::isAddressed)

#define ADDRESSING_LEVEL000 0

template<class ZP>
class ZcodeOutStream;

template<class ZP>
class ZcodeDebugOutput;

template<class ZP>
class Zcode;

template<class ZP>
class ZcodeDebugAddressingSystem: public ZcodeModuleAddressingSystem<ZP> {
    typedef typename ZP::debugOutputBufferSize_t debugOutputBufferSize_t;

public:

    static void routeAddress(ZcodeExecutionCommandSlot<ZP> slot, ZcodeAddressingInfo<ZP> *addressingInfo) {
        slot.getZcode()->getDebug().print((const char*) (slot.getBigField()->getData() + addressingInfo->start),
                (debugOutputBufferSize_t) (slot.getBigField()->getLength() - addressingInfo->start));
        slot.getZcode()->getDebug().attemptFlush();
    }

    static void routeResponse(ZcodeBusInterrupt<ZP> *interrupt, ZcodeOutStream<ZP> *out) {
        (void) interrupt;
        (void) out;
    }

    static bool isAddressed(ZcodeBusInterrupt<ZP> *interrupt) {
        (void) interrupt;
        return true;
    }
};

#endif /* SRC_MAIN_CPP_ZCODE_MODULES_CORE_ZCODEDEBUGADDRESSINGSYSTEM_HPP_ */
