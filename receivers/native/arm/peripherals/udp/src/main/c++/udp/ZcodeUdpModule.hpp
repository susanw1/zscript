/*
 * ZcodeUdpModule.hpp
 *
 *  Created on: 16 Dec 2022
 *      Author: alicia
 */

#ifndef PERIPHERALS_UDP_SRC_MAIN_C___UDP_ZCODEUDPMODULE_HPP_
#define PERIPHERALS_UDP_SRC_MAIN_C___UDP_ZCODEUDPMODULE_HPP_

#if defined(UDP_WIZNET_LL)
#include <wiznet-udp-ll/ZcodeUdpModule.hpp>
#else
#error No UDP system specified for usage
#endif

#endif /* PERIPHERALS_UDP_SRC_MAIN_C___UDP_ZCODEUDPMODULE_HPP_ */
