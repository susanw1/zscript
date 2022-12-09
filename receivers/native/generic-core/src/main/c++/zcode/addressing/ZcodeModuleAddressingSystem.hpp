/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ZCODE_ADDRESSING_ZCODEMODULEADDRESSINGSYSTEM_HPP_
#define SRC_MAIN_CPP_ZCODE_ADDRESSING_ZCODEMODULEADDRESSINGSYSTEM_HPP_

#include "../ZcodeIncludes.hpp"
#include "../ZcodeBusInterrupt.hpp"

#ifndef ZCODE_SUPPORT_ADDRESSING
#error Cannot use addressing system without addressing
#endif

template<class ZP>
struct ZcodeAddressingInfo {
    typedef typename ZP::bigFieldAddress_t bigFieldAddress_t;

    uint16_t addr;
    bigFieldAddress_t start;
    uint8_t port;
};

#define ADDRESSING_SWITCH_UTIL(func) func(slot, &addressingInfo);
#define ADDRESSING_RESP_SWITCH_UTIL(func) func(interrupt, out);
#define ADDRESSING_IS_ADDRESSED_SWITCH_UTIL(func) return func(interrupt);

template<class ZP>
class ZcodeModuleAddressingSystem {
public:

};

//Need to set:  ADDRESSING_SWITCHxxx - will send the addressed message
//              ADDRESSING_RESP_SWITCHxxx - will parse the response
//              ADDRESSING_LEVELxxx - 0 if no port/addr, 1 if no addr, 2 if all

#endif /* SRC_MAIN_CPP_ZCODE_ADDRESSING_ZCODEMODULEADDRESSINGSYSTEM_HPP_ */
