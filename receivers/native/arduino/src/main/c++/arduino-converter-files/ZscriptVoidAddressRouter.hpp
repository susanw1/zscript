/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#if defined(ZSCRIPT_SUPPORT_ADDRESSING) && defined(ZSCRIPT_GENERATE_NOTIFICATIONS)
#include "zscript/addressing/addressrouters/ZscriptVoidAddressRouter.hpp"

#ifdef ZSCRIPT_HPP_INCLUDED
#error ZscriptVoidAddressRouter.hpp needs to be included before Zscript.hpp
#endif

#define ZSCRIPT_ARDUINO_USE_MODULE_ADDRESS_ROUTER
#endif
