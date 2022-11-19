/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#if defined(ZCODE_SUPPORT_ADDRESSING) && defined(ZCODE_GENERATE_NOTIFICATIONS)
#include "zcode/addressing/addressrouters/ZcodeVoidAddressRouter.hpp"

#ifdef ZCODE_HPP_INCLUDED
#error ZcodeVoidAddressRouter.hpp needs to be included before Zcode.hpp
#endif

#define ZCODE_ARDUINO_USE_MODULE_ADDRESS_ROUTER
#endif
