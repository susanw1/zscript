/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ZSCRIPT_ZSCRIPTRESPONSETYPES_HPP_
#define SRC_MAIN_CPP_ZSCRIPT_ZSCRIPTRESPONSETYPES_HPP_

namespace Zscript {
namespace GenericCore {

#error deprecated! delete me!

enum ResponseType {
    Response = 0,
    ResetNotification = 1,
    PeripheralNotification = 2,
    ScriptSpaceNotification = 3
};

}
}

#endif /* SRC_MAIN_CPP_ZSCRIPT_ZSCRIPTRESPONSETYPES_HPP_ */
