/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_UDP_MODULE_ZCODEUDPMODULE_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_UDP_MODULE_ZCODEUDPMODULE_HPP_

#ifdef ZCODE_HPP_INCLUDED
#error Must be included before Zcode.hpp
#endif

#include <zcode/modules/ZcodeModule.hpp>

#include <arm-no-os/udp-module/channels/EthernetUdpChannel.hpp>

template<class ZP>
class ZcodeUdpModule: public ZcodeModule<ZP> {
    typedef typename ZP::Strings::string_t string_t;
    typedef typename ZP::LL LL;

public:

    typedef EthernetUdpChannel<ZP> channel;

};

#endif /* SRC_MAIN_CPP_ARM_NO_OS_UDP_MODULE_ZCODEUDPMODULE_HPP_ */
