/*
 * ZcodeUdpModule.hpp
 *
 *  Created on: 4 Aug 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___ARM_NO_OS_UDP_MODULE_ZCODEUDPMODULE_HPP_
#define SRC_MAIN_C___ARM_NO_OS_UDP_MODULE_ZCODEUDPMODULE_HPP_

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

#endif /* SRC_MAIN_C___ARM_NO_OS_UDP_MODULE_ZCODEUDPMODULE_HPP_ */
