/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_PINS_MODULE_ZCODEPINMODULE_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_PINS_MODULE_ZCODEPINMODULE_HPP_

#if defined(PINS_LL)
#include <pins-ll/ZcodePinModule.hpp>

#else
#error No Pin system specified for usage
#endif

#endif /* SRC_MAIN_CPP_ARM_NO_OS_PINS_MODULE_ZCODEPINMODULE_HPP_ */
