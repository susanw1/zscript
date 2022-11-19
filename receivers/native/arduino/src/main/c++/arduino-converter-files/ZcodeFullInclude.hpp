/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

//we do this to ensure we have a valid config...
#ifdef ZCODE_USE_MODULE_ADDRESSING
#define ZCODE_SUPPORT_ADDRESSING
#endif
#ifdef ZCODE_SUPPORT_INTERRUPT_VECTOR
#define ZCODE_SUPPORT_SCRIPT_SPACE
#endif
#ifdef ZCODE_SUPPORT_SCRIPT_SPACE
#define ZCODE_GENERATE_NOTIFICATIONS
#endif

#ifdef ZCODE_USE_DEBUG_ADDRESSING_SYSTEM
#include "ZcodeDebugAddressingSystem.hpp"
#define ZCODE_SUPPORT_DEBUG
#endif
#ifdef ZCODE_USE_MODULE_ADDRESSING
#include "ZcodeModuleAddressRouter.hpp"
#endif
#ifdef ZCODE_USE_MAPPING_ADDRESSING
#include "ZcodeMappingAddressRouter.hpp"
#endif

// makes the above #define's the only needed ones...
#ifdef ZCODE_ARDUINO_USE_MODULE_ADDRESS_ROUTER
#define ZCODE_SUPPORT_ADDRESSING
#endif

#ifdef ZCODE_SUPPORT_ADDRESSING
#define ZCODE_GENERATE_NOTIFICATIONS
#endif

#include "arduino/arduino-core-module/commands/ZcodeResetCommand.hpp"

#ifdef ZCODE_HAVE_PIN_MODULE
#include "arduino/pins-module/ZcodePinModule.hpp"
#endif
#ifdef ZCODE_HAVE_I2C_MODULE
#include "arduino/i2c-module/ZcodeI2cModule.hpp"
#endif

#include "ZcodeScriptModule.hpp"
#include "ZcodeOuterCoreModule.hpp"
#include "ZcodeCoreModule.hpp"

#include "Zcode.hpp"

#ifdef ZCODE_HAVE_SERIAL_CHANNEL
#include "arduino/serial-module/channels/ZcodeSerialChannel.hpp"

ZcodeSerialChannel<ZcodeParams> ZcodeSerialChannelI;
#endif

#ifdef ZCODE_HAVE_I2C_CHANNEL
#include "arduino/i2c-module/channels/ZcodeI2cChannel.hpp"

ZcodeI2cChannel<ZcodeParams> ZcodeI2cChannelI;
#endif

class ArduinoZcodeBasicSetup {
#if defined(ZCODE_HAVE_SERIAL_CHANNEL) or defined(ZCODE_HAVE_I2C_CHANNEL)
		ZcodeCommandChannel<ZcodeParams> *channels[0

#ifdef ZCODE_HAVE_SERIAL_CHANNEL
		                                           +1
#endif

#ifdef ZCODE_HAVE_I2C_CHANNEL
		                                           +1
#endif

		                                           ];
#endif

public:

    void setup() {
        uint8_t i = 0;
#ifdef ZCODE_HAVE_SERIAL_CHANNEL
        channels[i++] = &ZcodeSerialChannelI;
#endif
#ifdef ZCODE_HAVE_I2C_CHANNEL
        channels[i++] = &ZcodeI2cChannelI;
        ZcodeI2cChannelI.setup();
#endif
#if defined(ZCODE_HAVE_SERIAL_CHANNEL) or defined(ZCODE_HAVE_I2C_CHANNEL)
		Zcode<ZcodeParams>::zcode.setChannels(channels, i);
#endif
    }
};
ArduinoZcodeBasicSetup ZcodeSetup;
