/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#if !defined(ZSCRIPT_IDENTIFY_PLATFORM_FIRMWARE_STRING) || !defined(ZSCRIPT_IDENTIFY_PLATFORM_FIRMWARE_VERSION)
#define ZSCRIPT_IDENTIFY_PLATFORM_FIRMWARE_STRING  "zscript.net Arduino API"
#define ZSCRIPT_IDENTIFY_PLATFORM_FIRMWARE_VERSION 0x0001
#endif

#if !defined(ZSCRIPT_IDENTIFY_PLATFORM_HARDWARE_STRING) || !defined(ZSCRIPT_IDENTIFY_PLATFORM_HARDWARE_VERSION)
#define ZSCRIPT_IDENTIFY_PLATFORM_HARDWARE_STRING "Arduino"
#define ZSCRIPT_IDENTIFY_PLATFORM_HARDWARE_VERSION 0x0
#endif