/*
 * ZcodeStrings.hpp
 *
 *  Created on: 9 Jul 2022
 *      Author: robert
 */

#ifndef ARDUINO_FLASH_ZCODESTRINGS_HPP_
#define ARDUINO_FLASH_ZCODESTRINGS_HPP_
#include <ZcodeIncludes.hpp>

template<class ZP>
class ZcodeStrings {
public:
    typedef const __FlashStringHelper* string_t;

    static char getChar(string_t str, uint16_t index) {
        return pgm_read_byte_near(((PGM_P) str) + index);
    }

    static const char boolTrue[] PROGMEM;
    static const char boolFalse[] PROGMEM;

    static const char identifyUserFirmware[] PROGMEM;
    static const char identifyCoreZcodeLanguage[] PROGMEM;
    static const char identifyPlatformFirmware[] PROGMEM;
    static const char identifyPlatformHardware[] PROGMEM;

    static const char failAddressingOnlyFromNotificationChannel[] PROGMEM;
    static const char failAddressingInvalid[] PROGMEM;
    static const char failAddressingNoNewLine[] PROGMEM;
    static const char failAddressingNotSetup[] PROGMEM;

    static const char failParseBadStringEnd[] PROGMEM;
    static const char failParseNoStringEnd[] PROGMEM;
    static const char failParseStringTooLong[] PROGMEM;
    static const char failParseStringEscaping[] PROGMEM;

    static const char failParseBigOdd[] PROGMEM;
    static const char failParseBigTooLong[] PROGMEM;

    static const char failParseFieldTooLong[] PROGMEM;
    static const char failParseFieldRepeated[] PROGMEM;

    static const char failParseUnknownChar[] PROGMEM;

    static const char failParseTooManyLocks[] PROGMEM;
    static const char failParseBroadcastMultiple[] PROGMEM;
    static const char failParseLocksMultiple[] PROGMEM;

    static const char failParseOther[] PROGMEM;

    static const char failParseNoZ[] PROGMEM;
    static const char failParseNotActivated[] PROGMEM;
    static const char failParseUnknownCommand[] PROGMEM;

    static const char failScriptUnknownChannel[] PROGMEM;
    static const char failScriptBadAddress[] PROGMEM;
    static const char failScriptBadDelay[] PROGMEM;
    static const char failScriptBadWrite[] PROGMEM;

    static const char failInterruptBadType[] PROGMEM;
    static const char failInterruptBadVector[] PROGMEM;
    static const char failInterruptBadBus[] PROGMEM;
    static const char failInterruptBadAddress[] PROGMEM;
    static const char failInterruptHaveAddressWithoutBus[] PROGMEM;

    static const char failMatchCodeNoCodeGiven[] PROGMEM;
    static const char failBadChannelTarget[] PROGMEM;

    static const char scriptSpaceChannelDescriptor[] PROGMEM;
    static const char interruptVectorChannelDescriptor[] PROGMEM;

    static const char badModuleAreaSpecifier[] PROGMEM;

    static const char debugOverrun[] PROGMEM;
};

template<class ZP>
const char ZcodeStrings<ZP>::failParseOther[] PROGMEM = "Internal error";

template<class ZP>
const char ZcodeStrings<ZP>::boolTrue[] PROGMEM = "true";
template<class ZP>
const char ZcodeStrings<ZP>::boolFalse[] PROGMEM = "false";

#ifdef ZCODE_IDENTIFY_USER_FIRMWARE_STRING
template<class ZP>
const char ZcodeStrings<ZP>::identifyUserFirmware[] PROGMEM = ZCODE_IDENTIFY_USER_FIRMWARE_STRING;
#endif
#ifdef ZCODE_IDENTIFY_CORE_ZCODE_LANG_STRING
template<class ZP>
const char ZcodeStrings<ZP>::identifyCoreZcodeLanguage[] PROGMEM = ZCODE_IDENTIFY_CORE_ZCODE_LANG_STRING;
#endif
#ifdef ZCODE_IDENTIFY_PLATFORM_FIRMWARE_STRING
template<class ZP>
const char ZcodeStrings<ZP>::identifyPlatformFirmware[] PROGMEM = ZCODE_IDENTIFY_PLATFORM_FIRMWARE_STRING;
#endif
#ifdef ZCODE_IDENTIFY_HARDWARE_PLATFORM_STRING
template<class ZP>
const char ZcodeStrings<ZP>::identifyPlatformHardware[] PROGMEM = ZCODE_IDENTIFY_HARDWARE_PLATFORM_STRING;
#endif

template<class ZP>
const char ZcodeStrings<ZP>::failAddressingOnlyFromNotificationChannel[] PROGMEM = "Addressing only allowed from notification channel";
template<class ZP>
const char ZcodeStrings<ZP>::failAddressingInvalid[] PROGMEM = "Invalid address";
template<class ZP>
const char ZcodeStrings<ZP>::failAddressingNoNewLine[] PROGMEM = "Addressing must end in new line";
template<class ZP>
const char ZcodeStrings<ZP>::failAddressingNotSetup[] PROGMEM = "Addressing not set up";

template<class ZP>
const char ZcodeStrings<ZP>::failParseBadStringEnd[] PROGMEM = "Misplaced string field end";
template<class ZP>
const char ZcodeStrings<ZP>::failParseNoStringEnd[] PROGMEM = "String field not ended";
template<class ZP>
const char ZcodeStrings<ZP>::failParseStringTooLong[] PROGMEM = "String field too long";
template<class ZP>
const char ZcodeStrings<ZP>::failParseStringEscaping[] PROGMEM = "String escaping error";

template<class ZP>
const char ZcodeStrings<ZP>::failParseBigOdd[] PROGMEM = "Big field odd length";
template<class ZP>
const char ZcodeStrings<ZP>::failParseBigTooLong[] PROGMEM = "Big field too long";

template<class ZP>
const char ZcodeStrings<ZP>::failParseFieldTooLong[] PROGMEM = "Field too long";
template<class ZP>
const char ZcodeStrings<ZP>::failParseFieldRepeated[] PROGMEM = "Field repeated";

template<class ZP>
const char ZcodeStrings<ZP>::failParseUnknownChar[] PROGMEM = "Unknown character";

template<class ZP>
const char ZcodeStrings<ZP>::failParseTooManyLocks[] PROGMEM = "Too many locks";
template<class ZP>
const char ZcodeStrings<ZP>::failParseBroadcastMultiple[] PROGMEM = "Broadcast set twice";
template<class ZP>
const char ZcodeStrings<ZP>::failParseLocksMultiple[] PROGMEM = "Locks set twice";

template<class ZP>
const char ZcodeStrings<ZP>::failParseNoZ[] PROGMEM = "No Z field";
template<class ZP>
const char ZcodeStrings<ZP>::failParseNotActivated[] PROGMEM = "System not activated";
template<class ZP>
const char ZcodeStrings<ZP>::failParseUnknownCommand[] PROGMEM = "Unknown command";

template<class ZP>
const char ZcodeStrings<ZP>::failScriptUnknownChannel[] PROGMEM = "Invalid Script Channel";
template<class ZP>
const char ZcodeStrings<ZP>::failScriptBadAddress[] PROGMEM = "Address too large";
template<class ZP>
const char ZcodeStrings<ZP>::failScriptBadDelay[] PROGMEM = "Delay too large";
template<class ZP>
const char ZcodeStrings<ZP>::failScriptBadWrite[] PROGMEM = "Out of space for write";

template<class ZP>
const char ZcodeStrings<ZP>::failInterruptBadType[] PROGMEM = "Invalid interrupt type";
template<class ZP>
const char ZcodeStrings<ZP>::failInterruptBadVector[] PROGMEM = "Invalid interrupt vector";
template<class ZP>
const char ZcodeStrings<ZP>::failInterruptBadBus[] PROGMEM = "Bus value too large";
template<class ZP>
const char ZcodeStrings<ZP>::failInterruptBadAddress[] PROGMEM = "Address value too large";
template<class ZP>
const char ZcodeStrings<ZP>::failInterruptHaveAddressWithoutBus[] PROGMEM = "Address value necessitates bus";

template<class ZP>
const char ZcodeStrings<ZP>::failMatchCodeNoCodeGiven[] PROGMEM = "No code given";
template<class ZP>
const char ZcodeStrings<ZP>::failBadChannelTarget[] PROGMEM = "Invalid channel";

template<class ZP>
const char ZcodeStrings<ZP>::scriptSpaceChannelDescriptor[] PROGMEM = "Script space channel";
template<class ZP>
const char ZcodeStrings<ZP>::interruptVectorChannelDescriptor[] PROGMEM = "Interrupt vector channel";

template<class ZP>
const char ZcodeStrings<ZP>::badModuleAreaSpecifier[] PROGMEM = "Module area is invalid";

template<class ZP>
const char ZcodeStrings<ZP>::debugOverrun[] PROGMEM = "#Debug buffer out of space, some data lost\n";

#endif /* SRC_MAIN_C___ZCODE_ZCODESTRINGS_HPP_ */
