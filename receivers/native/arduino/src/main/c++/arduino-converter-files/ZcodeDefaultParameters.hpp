/*
 * ZcodeParameters.hpp
 *
 *  Created on: 7 Sep 2020
 *      Author: robert
 */

#ifndef ARDUINO_SRC_MAIN_CPP_DEFAULT_TEST_PARAMETERS_HPP_
#define ARDUINO_SRC_MAIN_CPP_DEFAULT_TEST_PARAMETERS_HPP_

#include <ZcodeIncludes.hpp>
#include "ZcodeStrings.hpp"

// Please note that ZcodeFullInclude will set any #defines necessary to other set #defines - so ZCODE_USE_DEBUG_ADDRESSING_SYSTEM enables ZCODE_SUPPORT_DEBUG

//#define ZCODE_SUPPORT_SCRIPT_SPACE
//#define ZCODE_SUPPORT_INTERRUPT_VECTOR

#define ZCODE_GENERATE_NOTIFICATIONS
//#define ZCODE_SUPPORT_DEBUG

//#define ZCODE_HAVE_I2C_MODULE

#define ZCODE_HAVE_SERIAL_CHANNEL
//#define ZCODE_HAVE_I2C_CHANNEL
//#define ZCODE_USE_DEBUG_ADDRESSING_SYSTEM

//This is only needed if an addressing system other than ZcodeModuleAddressRouter or ZcodeMappingAddressRouter is used. Otherwise use the below #define
//#define ZCODE_SUPPORT_ADDRESSING

//Please note this needs to line up with the typedef of ZcodeParams::AddressRouter below
#define ZCODE_USE_MODULE_ADDRESSING
//#define ZCODE_USE_MAPPING_ADDRESSING

template<class ZP>
class ZcodeModuleAddressRouter;
template<class ZP>
class ZcodeMappingAddressRouter;

class ZcodeParams {
public:
    typedef ZcodeStrings<ZcodeParams> Strings;
    typedef ZcodeModuleAddressRouter<ZcodeParams> AddressRouter;
    //    typedef ZcodeModuleAddressRouter<ZcodeParams> AddressRouter;

    static uint16_t numberGenerator() {
        return (uint16_t) millis();
    }

    typedef uint16_t scriptSpaceAddress_t;
    typedef uint16_t scriptSpaceOutLength_t;
    typedef uint16_t bigFieldAddress_t;
    typedef uint8_t fieldMapSize_t;
    typedef uint8_t commandNum_t;
    typedef uint16_t debugOutputBufferLength_t;
    typedef uint8_t lockNumber_t;

    static const int fieldNum = 10;

    static const bool findInterruptSourceAddress = true;
    static const int interruptStoreNum = 5;
    static const int interruptVectorWorkingNum = 5;
    static const int interruptVectorNum = 5;
    static const uint16_t scriptLength = 128;
    static const uint16_t scriptBigSize = 16;
    static const uint8_t scriptChannelCount = 1;

    static const uint16_t parseIterationMax = 128;

    static const int scriptOutBufferSize = 32;
    static const uint16_t scriptOutReadBufferSize = 16;
    static const uint16_t interruptVectorOutReadBufferSize = 32;
    static const int lockNum = 8;
    static const int debugBufferLength = 32;

    static const uint16_t serialBigSize = 32;
    static const uint16_t serialChannelReadBufferSize = 8;

    static const uint16_t mappingAddressCount = 32;
};

#endif /* ARDUINO_SRC_MAIN_CPP_DEFAULT_TEST_PARAMETERS_HPP_ */
