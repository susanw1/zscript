/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifdef ZCODE_SUPPORT_SCRIPT_SPACE
#include "zcode/modules/script/ZcodeScriptModule.hpp"

ZcodeScriptModule<ZcodeParams> ZcodeScriptModuleI; 

#ifdef ZCODE_HPP_INCLUDED
#error ZcodeScriptModule.hpp needs to be included before Zcode.hpp
#endif

#define ZCODE_ARDUINO_USE_SCRIPT_MODULE
#endif
