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

#if defined(ZSCRIPT_HAVE_I2C_MODULE) || defined(ZSCRIPT_HAVE_I2C_CHANNEL)

#include "arduino/i2c-module/I2cModule.hpp"

#endif

#if defined(ZSCRIPT_HAVE_I2C_MODULE) || defined(ZSCRIPT_HAVE_I2C_CHANNEL)
#include "arduino/i2c-module/I2cModule.hpp"
#endif

#if defined(ZSCRIPT_HAVE_UART_MODULE) || defined(ZSCRIPT_HAVE_UART_CHANNEL)
#include "arduino/uart-module/UartModule.hpp"
#endif

#ifdef ZSCRIPT_HAVE_SERVO_MODULE
#include "arduino/servo-module/ServoModule.hpp"
#endif


#ifdef ZSCRIPT_SUPPORT_SCRIPT_SPACE
#include "ZscriptScriptModule.hpp"
#endif

// INCLUDE ALL NON-CORE MODULES ABOVE HERE!

#include "ZscriptOuterCoreModule.hpp"
#include "ZscriptCoreModule.hpp"

#include "Zscript.hpp"

// INCLUDE ALL CHANNELS BELOW HERE!

#ifdef ZSCRIPT_HAVE_I2C_CHANNEL
#include <arduino/i2c-module/channels/I2cChannel.hpp>
#endif

#ifdef ZSCRIPT_HAVE_UDP_CHANNEL

#include <arduino/ethernet-module/channels/ZscriptUdpChannel.hpp>

#endif

#ifdef ZSCRIPT_HAVE_TCP_CHANNEL
#include <arduino/ethernet-module/channels/ZscriptTcpChannel.hpp>
Zscript::ZscriptTcpChannel<ZscriptParams> ZscriptTcpChannels[ZscriptParams::tcpChannelCount];
#endif

#if defined(ZSCRIPT_HAVE_UART_CHANNEL)

#include "arduino/uart-module/channels/UartChannel.hpp"

Zscript::UartChannel<ZscriptParams> zscriptUartChannel;
#endif

class ArduinoZscriptBasicSetup {
#if defined(ZSCRIPT_HAVE_UART_CHANNEL) or defined(ZSCRIPT_HAVE_I2C_CHANNEL) or defined(ZSCRIPT_HAVE_UDP_CHANNEL) or defined(ZSCRIPT_HAVE_TCP_CHANNEL)
    Zscript::ZscriptChannel<ZscriptParams> *channels[0

                                                     #ifdef ZSCRIPT_HAVE_UART_CHANNEL
                                                     + 1
                                                     #endif
                                                     #ifdef ZSCRIPT_HAVE_I2C_CHANNEL
                                                     + 1
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
#   if defined(ZSCRIPT_I2C_SUPPORT_NOTIFICATIONS) || defined(ZSCRIPT_PIN_SUPPORT_NOTIFICATIONS)
    Zscript::GenericCore::ZscriptNotificationSource<ZscriptParams> *notifSrcs[0
#       ifdef ZSCRIPT_I2C_SUPPORT_NOTIFICATIONS
                                                                        + 1
#       endif
#       ifdef ZSCRIPT_PIN_SUPPORT_NOTIFICATIONS
                                                                        + 1
#       endif
];
#   endif
#endif

public:

    void setup() {
        uint8_t notifPersistLength = 0;
#ifdef ZSCRIPT_HAVE_UDP_CHANNEL
        if (Zscript::ZscriptUdpManager<ZscriptParams>::getNotifChannelPersistMaxLength() > notifPersistLength) {
            notifPersistLength = Zscript::ZscriptUdpManager<ZscriptParams>::getNotifChannelPersistMaxLength();
        }
#endif

#ifdef ZSCRIPT_HAVE_UART_CHANNEL
        if (Zscript::UartChannel<ZscriptParams>::getNotifChannelPersistMaxLength() > notifPersistLength) {
            notifPersistLength = Zscript::UartChannel<ZscriptParams>::getNotifChannelPersistMaxLength();
        }
#endif
#ifdef ZSCRIPT_HAVE_I2C_CHANNEL
        if (Zscript::I2cChannel<ZscriptParams>::getNotifChannelPersistMaxLength() > notifPersistLength) {
            notifPersistLength = Zscript::I2cChannel<ZscriptParams>::getNotifChannelPersistMaxLength();
        }
#endif
        Zscript::PersistenceSystem<ZscriptParams>::reserveNotifChannelData(notifPersistLength);
#if defined(ZSCRIPT_HAVE_UDP_CHANNEL) || defined(ZSCRIPT_HAVE_TCP_CHANNEL)
        Zscript::EthernetSystem<ZscriptParams>::setup();
#endif
#ifdef ZSCRIPT_HAVE_PIN_MODULE
        Zscript::PinModule<ZscriptParams>::setup();
#endif
#ifdef ZSCRIPT_HAVE_SERVO_MODULE
        Zscript::ZscriptServoModule<ZscriptParams>::setup();
#endif
        uint8_t i = 0;
#if defined(ZSCRIPT_HAVE_I2C_MODULE) || defined(ZSCRIPT_HAVE_I2C_CHANNEL)
        Zscript::I2cModule<ZscriptParams>::setup();
#endif
#ifdef ZSCRIPT_HAVE_I2C_CHANNEL
        channels[i++] = &Zscript::I2cModule<ZscriptParams>::channel;
#endif
#ifdef ZSCRIPT_HAVE_UART_CHANNEL
        channels[i++] = &zscriptUartChannel;
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
#if defined(ZSCRIPT_HAVE_UART_CHANNEL) or defined(ZSCRIPT_HAVE_I2C_CHANNEL) or defined(ZSCRIPT_HAVE_UDP_CHANNEL) or defined(ZSCRIPT_HAVE_TCP_CHANNEL)
        Zscript::Zscript<ZscriptParams>::zscript.setChannels(channels, i);
#endif
#ifdef ZSCRIPT_SUPPORT_NOTIFICATIONS
        uint8_t srcCount = 0;
#ifdef ZSCRIPT_I2C_SUPPORT_NOTIFICATIONS
        notifSrcs[srcCount++] = &Zscript::I2cModule<ZscriptParams>::notifSrc;
#endif
#ifdef ZSCRIPT_PIN_SUPPORT_NOTIFICATIONS
        notifSrcs[srcCount++] = &Zscript::PinModule<ZscriptParams>::notificationSource;
#endif
#if defined(ZSCRIPT_I2C_SUPPORT_NOTIFICATIONS) || defined(ZSCRIPT_PIN_SUPPORT_NOTIFICATIONS)
        Zscript::Zscript<ZscriptParams>::zscript.setNotificationSources(notifSrcs, srcCount);
#endif
#endif

#ifdef ZSCRIPT_SUPPORT_PERSISTENCE
        uint8_t notifChannelIndex = 0xFF;
        if (Zscript::PersistenceSystem<ZscriptParams>::readSection(
                Zscript::PersistenceSystem<ZscriptParams>::getNotifChannelIdOffset(), 1, &notifChannelIndex) &&
            notifChannelIndex <= i && channels[notifChannelIndex]->setupStartupNotificationChannel()) {

            Zscript::Zscript<ZscriptParams>::zscript.setNotificationChannelIndex(notifChannelIndex);
        }
#endif
    }

    static void pollAll() {
#ifdef ZSCRIPT_HAVE_SERVO_MODULE
        Zscript::ZscriptServoModule<ZscriptParams>::moveAlongServos();
#endif
#ifdef ZSCRIPT_HAVE_PIN_MODULE
        Zscript::PinModule<ZscriptParams>::poll();
#endif
    }
};

ArduinoZscriptBasicSetup ZscriptSetup;
