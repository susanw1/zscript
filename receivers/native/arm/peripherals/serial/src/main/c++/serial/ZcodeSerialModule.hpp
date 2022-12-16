/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_SERIAL_MODULE_ZCODESERIALMODULE_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_SERIAL_MODULE_ZCODESERIALMODULE_HPP_

#if defined(SERIAL_LL)
#include <serial-ll/ZcodeSerialModule.hpp>

#else
#error No Serial system specified for usage
#endif

#endif /* SRC_MAIN_CPP_ARM_NO_OS_SERIAL_MODULE_ZCODESERIALMODULE_HPP_ */
