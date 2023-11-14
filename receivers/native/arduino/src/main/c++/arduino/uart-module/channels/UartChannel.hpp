/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARDUINO_SERIAL_MODULE_CHANNELS_ZSCRIPTSERIALCHANNEL_HPP_
#define SRC_MAIN_CPP_ARDUINO_SERIAL_MODULE_CHANNELS_ZSCRIPTSERIALCHANNEL_HPP_

#include <ZscriptChannelBuilder.hpp>
#include <net/zscript/model/modules/base/UartModule.hpp>
#include "../../arduino-core-module/persistence/PersistenceSystem.hpp"

namespace Zscript {

template<class ZP>
class ZscriptUartOutStream : public AbstractOutStream<ZP> {
private:
    static constexpr uint8_t MODE_DATABITS = 0x06;
    static constexpr uint8_t MODE_STOPBITS = 0x08;
    static constexpr uint8_t MODE_PARITY = 0x30;

    // index into the ZP::uartSupportedFreqs array
    uint8_t currentFrequencyIndex;
    // represents Arduino config: 00pp_sdd0 (pp=N,E,O parity; s=2 stop bits; dd=5,6,7,8 bits)
    uint8_t currentSerialMode;
    bool configToChange: 1;
    bool openB: 1;

public:
    ZscriptUartOutStream() : currentFrequencyIndex(0), currentSerialMode(SERIAL_8N1), configToChange(false), openB(false) {
    }

    void open(uint8_t source) {
        (void) source;
        openB = true;
    }

    bool isOpen() {
        return openB;
    }

    void close() {
        if (configToChange) {
            configure();
        }
        openB = false;
    }

    void configure() {
//        Serial.flush();
        Serial.begin(ZP::uartSupportedFreqs[currentFrequencyIndex], currentSerialMode);
        configToChange = false;
    }

    void writeBytes(const uint8_t *bytes, uint16_t count, bool hexMode) {
        if (hexMode) {
            for (uint16_t i = 0; i < count; i++) {
                Serial.print((char) AbstractOutStream<ZP>::toHexChar(bytes[i] >> 4));
                Serial.print((char) AbstractOutStream<ZP>::toHexChar(bytes[i] & 0xf));
            }
        } else {
            for (uint16_t i = 0; i < count; ++i) {
                Serial.print((char) bytes[i]);
            }
        }
    }

    uint8_t getFrequencyIndex() {
        return currentFrequencyIndex;
    }

    void setFrequencyIndex(uint8_t newIndex) {
        currentFrequencyIndex = newIndex;
        configToChange = true;
    }

    bool getConfigStopBits() {
        return (currentSerialMode & MODE_STOPBITS) != 0;
    }

    void setConfigStopBits(bool twoBits) {
        currentSerialMode = (currentSerialMode & ~MODE_STOPBITS) | (twoBits ? MODE_STOPBITS : 0);
        configToChange = true;
    }

    /** 0 => 5 bits, 1 => 6 bits, 2 => 7 bits, 3 => 8 bits */
    uint8_t getConfigDataBits() {
        return (currentSerialMode & MODE_DATABITS) >> 1;
    }

    /** 0 => parity disabled, 2 => Odd parity, 3 => Even parity */
    uint8_t getConfigParity() {
        return (currentSerialMode & MODE_PARITY) >> 4;
    }

    void setConfigParityMode(bool enabled, bool oddParity) {
        currentSerialMode = (currentSerialMode & ~MODE_PARITY) | (enabled ? 0x20 : 0) | (oddParity ? 0x10 : 0);
        configToChange = true;
    }
};

template<class ZP>
class UartChannel : public ZscriptChannel<ZP> {
    ZscriptUartOutStream<ZP> out;
    GenericCore::TokenRingBuffer<ZP> tBuffer;
    ZscriptTokenizer<ZP> tokenizer;

    uint8_t buffer[ZP::uartChannelBufferSize];
    uint8_t tmp = 0;
    bool usingTmp: 1;

public:
    static uint8_t getNotifChannelPersistMaxLength() {
        return 0;
    }

    UartChannel() :
            buffer(), ZscriptChannel<ZP>(&out, &tBuffer, uart_module::MODULE_FULL_ID, true),
            tBuffer(buffer, ZP::uartChannelBufferSize), tokenizer(tBuffer.getWriter(), 2), usingTmp(false) {
    }

    void setup() {
        out.configure();
    }

    bool setupStartupNotificationChannel() {
        return true;
    }

    ZscriptUartOutStream<ZP> *getUartOutStream() {
        return &out;
    }

    void moveAlong() {
        while (usingTmp || Serial.available() > 0) {
            if (!usingTmp) {
                tmp = (uint8_t) Serial.read();
            }
            if (!tokenizer.offer(tmp)) {
                usingTmp = true;
                return;
            }
        }
    }


};
}
#endif /* SRC_MAIN_CPP_ARDUINO_SERIAL_MODULE_CHANNELS_ZSCRIPTSERIALCHANNEL_HPP_ */
