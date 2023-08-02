/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_MODULES_CORE_CAPABILITIESCOMMAND_HPP_
#define SRC_MAIN_C___ZSCRIPT_MODULES_CORE_CAPABILITIESCOMMAND_HPP_
#include "../../ZscriptIncludes.hpp"
#include "../../execution/ZscriptCommandContext.hpp"
#include "../../LanguageVersion.hpp"
#include "../ZscriptModule.hpp"

#define COMMAND_EXISTS_0000 EXISTENCE_MARKER_UTIL

namespace Zscript {
namespace GenericCore {
template<class ZP>
class CapabilitiesCommand {
public:
    enum VersionType {
        UserFirmware, UserHardware, PlatformFirmware, PlatformHardware, CoreZcodeLanguage
    };

    static void execute(ZscriptCommandContext<ZP> ctx) {
        AbstractOutStream<ZP> *out = ctx.getOutStream();

        uint16_t versionType = ctx.getField('V', UserFirmware);
        const char *ident = NULL;
        uint16_t version;

        switch (versionType) {
        case UserFirmware:
            ident = "aa";
            version = 1;
            break;
//        case UserHardware:
//            ident = ZP::Strings::identifyUserHardware;
//            version = ZCODE_IDENTIFY_USER_HARDWARE_VERSION;
//            break;
//        case PlatformFirmware:
//            ident = ZP::Strings::identifyPlatformFirmware;
//            version = ZCODE_IDENTIFY_PLATFORM_FIRMWARE_VERSION;
//            break;
//        case PlatformHardware:
//            ident = ZP::Strings::identifyPlatformHardware;
//            version = ZCODE_IDENTIFY_PLATFORM_HARDWARE_VERSION;
//            break;
        case CoreZcodeLanguage:
            ident = ZSCRIPT_IDENTIFY_CORE_ZSCRIPT_LANG_STRING;
            version = ZSCRIPT_IDENTIFY_CORE_ZSCRIPT_LANG_VERSION;
            break;
        }

        if (ident != NULL) {
//            out->writeBigStringField(ident);
            out->writeField('V', version);
        } else {
            ctx.status(ResponseStatus::VALUE_OUT_OF_RANGE);
        }
        out->writeField('C', MODULE_CAPABILITIES(000));
        out->writeField('M', COMMAND_SWITCH_EXISTS_BOTTOM_BYTE(00));
    }

};
}
}

#endif /* SRC_MAIN_C___ZSCRIPT_MODULES_CORE_CAPABILITIESCOMMAND_HPP_ */
