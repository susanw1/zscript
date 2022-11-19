/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#include "zcode/modules/outer-core/ZcodeOuterCoreModule.hpp"
ZcodeOuterCoreModule<ZcodeParams> ZcodeOuterCoreModuleI; 

#ifdef ZCODE_HPP_INCLUDED
#error ZcodeOuterCoreModule.hpp needs to be included before Zcode.hpp
#endif
