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

#include "arduino/arduino-core-module/persistence/PersistenceSystem.hpp"

#ifdef ZSCRIPT_SUPPORT_ADDRESSING
#define ZSCRIPT_SUPPORT_NOTIFICATIONS
#endif

#ifdef ZSCRIPT_HAVE_PIN_MODULE

#include "arduino/pins-module/PinModule.hpp"

#endif
#ifdef ZSCRIPT_HAVE_SERVO_MODULE
#include "arduino/servo-module/ServoModule.hpp"
#endif
#ifdef ZSCRIPT_HAVE_I2C_MODULE

#include "arduino/i2c-module/I2cModule.hpp"

#endif

#ifdef ZSCRIPT_SUPPORT_SCRIPT_SPACE
#include "ZscriptScriptModule.hpp"
#endif

#include "ZscriptOuterCoreModule.hpp"
#include "ZscriptCoreModule.hpp"

#include "Zscript.hpp"

#ifdef ZSCRIPT_HAVE_UDP_CHANNEL

#include <arduino/ethernet-module/channels/ZscriptUdpChannel.hpp>

#endif
#ifdef ZSCRIPT_HAVE_TCP_CHANNEL
#include <arduino/ethernet-module/channels/ZscriptTcpChannel.hpp>
Zscript::ZscriptTcpChannel<ZscriptParams> ZscriptTcpChannels[ZscriptParams::tcpChannelCount];
#endif

#ifdef ZSCRIPT_HAVE_SERIAL_CHANNEL

#include "arduino/serial-module/channels/ZscriptSerialChannel.hpp"

Zscript::ZscriptSerialChannel<ZscriptParams> ZscriptSerialChannel;
#endif

#ifdef ZSCRIPT_HAVE_I2C_CHANNEL
#include "arduino/i2c-module/channels/I2cChannel.hpp"

Zscript::I2cChannel<ZscriptParams> ZscriptI2cChannel;
#endif

class ArduinoZscriptBasicSetup {
#if defined(ZSCRIPT_HAVE_SERIAL_CHANNEL) or defined(ZSCRIPT_HAVE_I2C_CHANNEL) or defined(ZSCRIPT_HAVE_UDP_CHANNEL) or defined(ZSCRIPT_HAVE_TCP_CHANNEL)
    Zscript::ZscriptChannel<ZscriptParams> *channels[0

                                                     #ifdef ZSCRIPT_HAVE_SERIAL_CHANNEL
                                                     + 1
                                                     #endif

                                                     #ifdef ZSCRIPT_HAVE_I2C_CHANNEL
                                                     +1
                                                     #endif
                                                     #ifdef ZSCRIPT_HAVE_UDP_CHANNEL
                                                     + ZscriptParams::udpChannelCount
#endif
#ifdef ZSCRIPT_HAVE_TCP_CHANNEL
            +ZscriptParams::tcpChannelCount
#endif

    ];
#endif
#ifdef ZSCRIPT_SUPPORT_NOTIFICATIONS
#if defined(ZSCRIPT_I2C_SUPPORT_NOTIFICATIONS)
    Zscript::GenericCore::ZscriptNotificationSource<ZscriptParams> *notifSrcs[0

#ifdef ZSCRIPT_I2C_SUPPORT_NOTIFICATIONS
                                                   +1
#endif

                                                   ];
#endif
#endif

public:

    void setup() {
        uint8_t notifPersistLength = 0;
#ifdef ZSCRIPT_HAVE_UDP_CHANNEL
        if (Zscript::ZscriptUdpManager<ZscriptParams>::getNotifChannelPersistMaxLength() > notifPersistLength) {
            notifPersistLength = Zscript::ZscriptUdpManager<ZscriptParams>::getNotifChannelPersistMaxLength();
        }
#endif
#ifdef ZSCRIPT_HAVE_SERIAL_CHANNEL
        if (Zscript::ZscriptSerialChannel<ZscriptParams>::getNotifChannelPersistMaxLength() > notifPersistLength) {
            notifPersistLength = Zscript::ZscriptSerialChannel<ZscriptParams>::getNotifChannelPersistMaxLength();
        }
#endif
#ifdef ZSCRIPT_HAVE_I2C_CHANNEL
        if (Zscript::ZscriptI2cChannel<ZscriptParams>::getNotifChannelPersistMaxLength() > notifPersistLength) {
            notifPersistLength = Zscript::ZscriptI2cChannel<ZscriptParams>::getNotifChannelPersistMaxLength();
        }
#endif
        Zscript::PersistenceSystem<ZscriptParams>::reserveNotifChannelData(notifPersistLength);
#ifdef ZSCRIPT_HAVE_SERVO_MODULE
        Zscript::ZscriptServoModule<ZscriptParams>::setup();
#endif
        uint8_t i = 0;
#ifdef ZSCRIPT_HAVE_SERIAL_CHANNEL
        channels[i++] = &ZscriptSerialChannel;
#endif
#ifdef ZSCRIPT_HAVE_I2C_CHANNEL
        channels[i++] = &ZscriptI2cChannel;
        ZscriptI2cChannel.setup();
        ZscriptI2cChannel.setAddress(ZscriptParams::i2cChannelAddress);
#else
#ifdef ZSCRIPT_HAVE_I2C_MODULE
        Wire.begin();
#endif
#endif
#ifdef ZSCRIPT_HAVE_I2C_MODULE
        Zscript::I2cModule<ZscriptParams>::setup();
#endif
#ifdef ZSCRIPT_HAVE_UDP_CHANNEL
        Zscript::ZscriptUdpManager<ZscriptParams>::setup();
        for (uint8_t j = 0; j < ZscriptParams::udpChannelCount; j++) {
            channels[i++] = Zscript::ZscriptUdpManager<ZscriptParams>::channels + j;
        }

#endif
#ifdef ZSCRIPT_HAVE_TCP_CHANNEL
        Zscript::ZscriptTcpManager<ZscriptParams>::setup();
        for(uint8_t j = 0; j < ZscriptParams::tcpChannelCount; j++){
            channels[i++] = ZscriptTcpChannels+j;
        }
#endif
#if defined(ZSCRIPT_HAVE_SERIAL_CHANNEL) or defined(ZSCRIPT_HAVE_I2C_CHANNEL) or defined(ZSCRIPT_HAVE_UDP_CHANNEL) or defined(ZSCRIPT_HAVE_TCP_CHANNEL)
        Zscript::Zscript<ZscriptParams>::zscript.setChannels(channels, i);
#endif
#ifdef ZSCRIPT_SUPPORT_NOTIFICATIONS
        uint8_t j = 0;
#ifdef ZSCRIPT_I2C_SUPPORT_NOTIFICATIONS
        notifSrcs[j++] = &Zscript::I2cModule<ZscriptParams>::notifSrc;
#endif
#if defined(ZSCRIPT_I2C_SUPPORT_NOTIFICATIONS)
        Zscript::Zscript<ZscriptParams>::zscript.setNotificationSources(notifSrcs, j);
#endif
#endif
        uint8_t notifChannelIndex = 0xFF;
        if (Zscript::PersistenceSystem<ZscriptParams>::readSection(
                Zscript::PersistenceSystem<ZscriptParams>::getNotifChannelIdOffset(), 1, &notifChannelIndex) &&
            notifChannelIndex <= i && channels[notifChannelIndex]->setupStartupNotificationChannel()) {

            Zscript::Zscript<ZscriptParams>::zscript.setNotificationChannelIndex(notifChannelIndex);
        }
    }
};

ArduinoZscriptBasicSetup ZscriptSetup;
