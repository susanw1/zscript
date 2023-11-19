/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_MODULES_CORE_CAPABILITIESCOMMAND_HPP_
#define SRC_MAIN_C___ZSCRIPT_MODULES_CORE_CAPABILITIESCOMMAND_HPP_

#include <net/zscript/model/modules/base/CoreModule.hpp>
#include "../../ZscriptIncludes.hpp"
#include "../../execution/ZscriptCommandContext.hpp"
#include "../../LanguageVersion.hpp"
#include "../ZscriptModule.hpp"

#define COMMAND_EXISTS_0000 EXISTENCE_MARKER_UTIL

namespace Zscript {

namespace GenericCore {

template<class ZP>
class CapabilitiesCommand: public core_module::Capabilities_CommandDefs {
public:
    static void execute(ZscriptCommandContext<ZP> ctx) {
        CommandOutStream<ZP> out = ctx.getOutStream();

        uint16_t versionType = ctx.getField(ReqVersionType__V, userFirmware_Value);
        const char *ident = NULL;
        uint16_t version;

        switch (versionType) {
        case userFirmware_Value:
            ident = ZSCRIPT_IDENTIFY_USER_FIRMWARE_STRING;
            version = ZSCRIPT_IDENTIFY_USER_FIRMWARE_VERSION;
            break;
        case userHardware_Value:
            ident = ZSCRIPT_IDENTIFY_USER_HARDWARE_STRING;
            version = ZSCRIPT_IDENTIFY_USER_HARDWARE_VERSION;
            break;
        case platformFirmware_Value:
            ident = ZSCRIPT_IDENTIFY_PLATFORM_FIRMWARE_STRING;
            version = ZSCRIPT_IDENTIFY_PLATFORM_FIRMWARE_VERSION;
            break;
        case platformHardware_Value:
            ident = ZSCRIPT_IDENTIFY_PLATFORM_HARDWARE_STRING;
            version = ZSCRIPT_IDENTIFY_PLATFORM_HARDWARE_VERSION;
            break;
        case coreZscriptLanguage_Value:
            ident = ZSCRIPT_IDENTIFY_CORE_ZSCRIPT_LANG_STRING;
            version = ZSCRIPT_IDENTIFY_CORE_ZSCRIPT_LANG_VERSION;
            break;
        }

        if (ident != NULL) {
            out.writeQuotedString(ident);
            out.writeField(RespVersion__V, version);
        } else {
            ctx.status(ResponseStatus::VALUE_OUT_OF_RANGE);
        }
        out.writeField(RespCommandsSet__C, MODULE_CAPABILITIES(000));
        out.writeField(RespModulesBankSet__M, COMMAND_SWITCH_EXISTS_BOTTOM_BYTE(00));
#ifdef ZSCRIPT_SUPPORT_NOTIFICATIONS
        out.writeField(RespNotificationsSupported__N, 0);
#endif
#ifdef ZSCRIPT_SUPPORT_ADDRESSING
        out.writeField(RespAddressingSupported__A, 0);
#endif
    }
};

}

}

#endif /* SRC_MAIN_C___ZSCRIPT_MODULES_CORE_CAPABILITIESCOMMAND_HPP_ */
