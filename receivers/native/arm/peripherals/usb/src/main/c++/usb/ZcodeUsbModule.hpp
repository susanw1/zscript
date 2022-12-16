/*
 * ZcodeUsbModule.hpp
 *
 *  Created on: 16 Dec 2022
 *      Author: alicia
 */

#ifndef PERIPHERALS_USB_SRC_MAIN_C___USB_ZCODEUSBMODULE_HPP_
#define PERIPHERALS_USB_SRC_MAIN_C___USB_ZCODEUSBMODULE_HPP_

#if defined(USB_SERIAL_LL)
#include <usb-serial-ll/ZcodeUsbModule.hpp>

#else
#error No Serial system specified for usage
#endif

#endif /* PERIPHERALS_USB_SRC_MAIN_C___USB_ZCODEUSBMODULE_HPP_ */
