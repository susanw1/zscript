/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ZCODE_MODULES_CORE_ZCODECAPABILITIESCOMMAND_HPP_
#define SRC_MAIN_CPP_ZCODE_MODULES_CORE_ZCODECAPABILITIESCOMMAND_HPP_

#include "../../ZcodeIncludes.hpp"
#include "../ZcodeCommand.hpp"
#include "../ZcodeModule.hpp"

#define COMMAND_EXISTS_0000 EXISTENCE_MARKER_UTIL

template<class ZP>
class ZcodeOutStream;

template<class ZP>
class Zcode;

template<class ZP>
class ZcodeCapabilitiesCommand: public ZcodeCommand<ZP> {
protected:
    typedef typename ZP::Strings::string_t string_t;

    enum VersionType {
        UserFirmware, UserHardware, PlatformFirmware, PlatformHardware, CoreZcodeLanguage
    };

    static constexpr char CMD_PARAM_VERSIONTYPE_V = 'V';
    static constexpr char CMD_RESP_COMMANDS_C = 'C';
    static constexpr char CMD_RESP_MODULES_PRESENT_M = 'M';
    static constexpr char CMD_RESP_VERSION_V = 'V';

public:
    static const uint8_t CODE = 0x00;

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        out->writeField16(CMD_RESP_COMMANDS_C, MODULE_CAPABILITIES(000));
        out->writeField16(CMD_RESP_MODULES_PRESENT_M, COMMAND_SWITCH_EXISTS_BOTTOM_BYTE(00));

        uint16_t versionType = slot.getFields()->get(CMD_PARAM_VERSIONTYPE_V, UserFirmware);
        string_t ident = NULL;
        uint16_t version;

        switch (versionType) {
        case UserFirmware:
            ident = (string_t) ZP::Strings::identifyUserFirmware;
            version = ZCODE_IDENTIFY_USER_FIRMWARE_VERSION;
            break;
        case UserHardware:
            ident = (string_t) ZP::Strings::identifyUserHardware;
            version = ZCODE_IDENTIFY_USER_HARDWARE_VERSION;
            break;
        case PlatformFirmware:
            ident = (string_t) ZP::Strings::identifyPlatformFirmware;
            version = ZCODE_IDENTIFY_PLATFORM_FIRMWARE_VERSION;
            break;
        case PlatformHardware:
            ident = (string_t) ZP::Strings::identifyPlatformHardware;
            version = ZCODE_IDENTIFY_PLATFORM_HARDWARE_VERSION;
            break;
        case CoreZcodeLanguage:
            ident = (string_t) ZP::Strings::identifyCoreZcodeLanguage;
            version = ZCODE_IDENTIFY_CORE_ZCODE_LANG_VERSION;
            break;
        }

        if (ident != NULL) {
            out->writeBigStringField(ident);
            out->writeField16(CMD_RESP_VERSION_V, version);
            out->writeStatus(OK);
        } else {
            out->writeStatus(BAD_PARAM);
        }
    }
};

#endif /* SRC_MAIN_CPP_ZCODE_MODULES_CORE_ZCODECAPABILITIESCOMMAND_HPP_ */
