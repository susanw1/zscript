/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifdef ZSCRIPT_SUPPORT_SCRIPT_SPACE
#include "zscript/modules/script/ZscriptScriptModule.hpp"

ZscriptScriptModule<ZscriptParams> ZscriptScriptModuleI; 

#ifdef ZSCRIPT_HPP_INCLUDED
#error ZscriptScriptModule.hpp needs to be included before Zscript.hpp
#endif

#define ZSCRIPT_ARDUINO_USE_SCRIPT_MODULE
#endif
