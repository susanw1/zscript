/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ZCODE_MODULES_ZCODECOMMAND_HPP_
#define SRC_MAIN_CPP_ZCODE_MODULES_ZCODECOMMAND_HPP_

#include "../ZcodeIncludes.hpp"
#include "../running/ZcodeExecutionCommandSlot.hpp"

#define MODULE_CAPABILITIES_UTIL 0,

template<class ZP>
class ZcodeCommand {
protected:
    ZcodeCommand() {
        // prevent instantiation, only subclassing
    }

public:

};

#endif /* SRC_MAIN_CPP_ZCODE_MODULES_ZCODECOMMAND_HPP_ */
