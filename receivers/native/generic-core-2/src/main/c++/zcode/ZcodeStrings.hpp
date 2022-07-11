/*
 * ZcodeStrings.hpp
 *
 *  Created on: 9 Jul 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___ZCODE_ZCODESTRINGS_HPP_
#define SRC_MAIN_C___ZCODE_ZCODESTRINGS_HPP_
#include "ZcodeIncludes.hpp"
class ZcodeString {
public:
    const char *str;

    constexpr ZcodeString(const char *str) :
            str(str) {
    }

    bool operator==(const char *str) {
        return this->str == str;
    }
    bool operator!=(const char *str) {
        return this->str != str;
    }
};
template<class ZP>
class ZcodeStrings {
public:
    typedef ZcodeString string_t;

    static char getChar(string_t str, uint16_t index) {
        return str.str[index];
    }

    static const string_t boolTrue;
    static const string_t boolFalse;

    static const string_t failAddressingOnlyFromNotificationChannel;
    static const string_t failAddressingInvalidModule;
    static const string_t failAddressTooShort;
    static const string_t failAddressingNotSetup;

    static const string_t failParseBadStringEnd;
    static const string_t failParseNoStringEnd;
    static const string_t failParseStringTooLong;
    static const string_t failParseStringEscaping;

    static const string_t failParseBigOdd;
    static const string_t failParseBigTooLong;

    static const string_t failParseFieldTooLong;
    static const string_t failParseFieldRepeated;

    static const string_t failParseUnknownChar;

    static const string_t failParseTooManyLocks;
    static const string_t failParseBroadcastMultiple;
    static const string_t failParseLocksMultiple;

    static const string_t failParseOther;

    static const string_t failParseNoZ;
    static const string_t failParseNotActivated;
    static const string_t failParseUnknownCommand;

    static const string_t failScriptUnknownChannel;
    static const string_t failScriptBadAddress;
    static const string_t failScriptBadDelay;
    static const string_t failScriptBadWrite;

    static const string_t failInterruptBadType;
    static const string_t failInterruptBadVector;
    static const string_t failInterruptBadBus;
    static const string_t failInterruptBadAddress;
    static const string_t failInterruptHaveAddressWithoutBus;

    static const string_t failMatchCodeNoCodeGiven;
    static const string_t failBadChannelTarget;

    static const string_t scriptSpaceChannelDescriptor;
    static const string_t interruptVectorChannelDescriptor;

    static const string_t debugOverrun;
};

template<class ZP>
const ZcodeString ZcodeStrings<ZP>::failParseOther("Internal error");

template<class ZP>
const ZcodeString ZcodeStrings<ZP>::boolTrue("true");
template<class ZP>
const ZcodeString ZcodeStrings<ZP>::boolFalse("false");

template<class ZP>
const ZcodeString ZcodeStrings<ZP>::failAddressingOnlyFromNotificationChannel("Addressing only allowed from notification channel");
template<class ZP>
const ZcodeString ZcodeStrings<ZP>::failAddressingInvalidModule("Invalid module address");
template<class ZP>
const ZcodeString ZcodeStrings<ZP>::failAddressTooShort("Too few addressing levels");
template<class ZP>
const ZcodeString ZcodeStrings<ZP>::failAddressingNotSetup("Addressing not set up");

template<class ZP>
const ZcodeString ZcodeStrings<ZP>::failParseBadStringEnd("Misplaced string field end");
template<class ZP>
const ZcodeString ZcodeStrings<ZP>::failParseNoStringEnd("String field not ended");
template<class ZP>
const ZcodeString ZcodeStrings<ZP>::failParseStringTooLong("String field too long");
template<class ZP>
const ZcodeString ZcodeStrings<ZP>::failParseStringEscaping("String escaping error");

template<class ZP>
const ZcodeString ZcodeStrings<ZP>::failParseBigOdd("Big field odd length");
template<class ZP>
const ZcodeString ZcodeStrings<ZP>::failParseBigTooLong("Big field too long");

template<class ZP>
const ZcodeString ZcodeStrings<ZP>::failParseFieldTooLong("Field too long");
template<class ZP>
const ZcodeString ZcodeStrings<ZP>::failParseFieldRepeated("Field repeated");

template<class ZP>
const ZcodeString ZcodeStrings<ZP>::failParseUnknownChar("Unknown character");

template<class ZP>
const ZcodeString ZcodeStrings<ZP>::failParseTooManyLocks("Too many locks");
template<class ZP>
const ZcodeString ZcodeStrings<ZP>::failParseBroadcastMultiple("Broadcast set twice");
template<class ZP>
const ZcodeString ZcodeStrings<ZP>::failParseLocksMultiple("Locks set twice");

template<class ZP>
const ZcodeString ZcodeStrings<ZP>::failParseNoZ("No Z field");
template<class ZP>
const ZcodeString ZcodeStrings<ZP>::failParseNotActivated("System not activated");
template<class ZP>
const ZcodeString ZcodeStrings<ZP>::failParseUnknownCommand("Unknown command");

template<class ZP>
const ZcodeString ZcodeStrings<ZP>::failScriptUnknownChannel("Invalid Script Channel");
template<class ZP>
const ZcodeString ZcodeStrings<ZP>::failScriptBadAddress("Address too large");
template<class ZP>
const ZcodeString ZcodeStrings<ZP>::failScriptBadDelay("Delay too large");
template<class ZP>
const ZcodeString ZcodeStrings<ZP>::failScriptBadWrite("Out of space for write");

template<class ZP>
const ZcodeString ZcodeStrings<ZP>::failInterruptBadType("Invalid interrupt type");
template<class ZP>
const ZcodeString ZcodeStrings<ZP>::failInterruptBadVector("Invalid interrupt vector");
template<class ZP>
const ZcodeString ZcodeStrings<ZP>::failInterruptBadBus("Bus value too large");
template<class ZP>
const ZcodeString ZcodeStrings<ZP>::failInterruptBadAddress("Address value too large");
template<class ZP>
const ZcodeString ZcodeStrings<ZP>::failInterruptHaveAddressWithoutBus("Address value necessitates bus");

template<class ZP>
const ZcodeString ZcodeStrings<ZP>::failMatchCodeNoCodeGiven("No code given");
template<class ZP>
const ZcodeString ZcodeStrings<ZP>::failBadChannelTarget("Invalid channel");

template<class ZP>
const ZcodeString ZcodeStrings<ZP>::scriptSpaceChannelDescriptor("Script space channel");
template<class ZP>
const ZcodeString ZcodeStrings<ZP>::interruptVectorChannelDescriptor("Interrupt vector channel");

template<class ZP>
const ZcodeString ZcodeStrings<ZP>::debugOverrun("#Debug buffer out of space, some data lost\n");

#endif /* SRC_MAIN_C___ZCODE_ZCODESTRINGS_HPP_ */
