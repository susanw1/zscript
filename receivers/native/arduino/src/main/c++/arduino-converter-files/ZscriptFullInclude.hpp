/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

//we do this to ensure we have a valid config...
#ifdef ZSCRIPT_SUPPORT_SCRIPT_SPACE
#define ZSCRIPT_SUPPORT_NOTIFICATIONS
#endif

#ifdef ZSCRIPT_SUPPORT_ADDRESSING
#define ZSCRIPT_SUPPORT_NOTIFICATIONS
#endif

#ifdef ZSCRIPT_HAVE_PIN_MODULE
#include "arduino/pins-module/ZscriptPinModule.hpp"
#endif
#ifdef ZSCRIPT_HAVE_I2C_MODULE
#include "arduino/i2c-module/ZscriptI2cModule.hpp"
#endif

#ifdef ZSCRIPT_SUPPORT_SCRIPT_SPACE
#include "ZscriptScriptModule.hpp"
#endif

#include "ZscriptOuterCoreModule.hpp"
#include "ZscriptCoreModule.hpp"

#include "Zscript.hpp"

#ifdef ZSCRIPT_HAVE_SERIAL_CHANNEL
#include "arduino/serial-module/channels/ZscriptSerialChannel.hpp"

Zscript::ZscriptSerialChannel<ZscriptParams> ZscriptSerialChannel;
#endif

#ifdef ZSCRIPT_HAVE_I2C_CHANNEL
#include "arduino/i2c-module/channels/ZscriptI2cChannel.hpp"

Zscript::ZscriptI2cChannel<ZscriptParams> ZscriptI2cChannel;
#endif

class ArduinoZscriptBasicSetup {
#if defined(ZSCRIPT_HAVE_SERIAL_CHANNEL) or defined(ZSCRIPT_HAVE_I2C_CHANNEL)
    Zscript::ZscriptChannel<ZscriptParams> *channels[0

#ifdef ZSCRIPT_HAVE_SERIAL_CHANNEL
		                                           +1
#endif

#ifdef ZSCRIPT_HAVE_I2C_CHANNEL
		                                           +1
#endif

		                                           ];
#endif

public:

    void setup() {
        uint8_t i = 0;
#ifdef ZSCRIPT_HAVE_SERIAL_CHANNEL
        channels[i++] = &ZscriptSerialChannel;
#endif
#ifdef ZSCRIPT_HAVE_I2C_CHANNEL
        channels[i++] = &ZscriptI2cChannel;
        ZscriptI2cChannel.setup();
#endif
#if defined(ZSCRIPT_HAVE_SERIAL_CHANNEL) or defined(ZSCRIPT_HAVE_I2C_CHANNEL)
        Zscript::Zscript<ZscriptParams>::zscript.setChannels(channels, i);
#endif
    }
};
ArduinoZscriptBasicSetup ZscriptSetup;
