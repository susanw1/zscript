/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#include "zcode/modules/core/ZcodeCoreModule.hpp"
ZcodeCoreModule<ZcodeParams> ZcodeCoreModuleI; 

#ifdef ZCODE_HPP_INCLUDED
#error ZcodeCoreModule.hpp needs to be included before Zcode.hpp
#endif
