/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARDUINO_SERIAL_MODULE_CHANNELS_ZSCRIPTSERIALCHANNEL_HPP_
#define SRC_MAIN_CPP_ARDUINO_SERIAL_MODULE_CHANNELS_ZSCRIPTSERIALCHANNEL_HPP_

#ifndef ZSCRIPT_HPP_INCLUDED
#error ZscriptSerialChannel.hpp needs to be included after Zscript.hpp
#endif

#include <ZscriptChannelBuilder.hpp>

namespace Zscript {

template<class ZP>
class ZscriptSerialOutStream: public AbstractOutStream<ZP> {
private:
    bool openB = false;

public:
    ZscriptSerialOutStream() {
    }

    void open() {
        openB = true;
    }

    bool isOpen() {
        return openB;
    }

    void close() {
        openB = false;
    }

    void writeBytes(const uint8_t *bytes, uint16_t count, bool hexMode) {
        if (hexMode) {
            for (uint16_t i = 0; i < count; i++) {
                Serial.print((char) AbstractOutStream < ZP > ::toHexChar(bytes[i] >> 4));
                Serial.print((char) AbstractOutStream < ZP > ::toHexChar(bytes[i] & 0xf));
            }
        } else {
            for (uint16_t i = 0; i < count; ++i) {
                Serial.print((char) bytes[i]);
            }
        }
    }
    void writeByte(uint8_t byte) {
        Serial.print((char) byte);
    }

};

template<class ZP>
class ZscriptSerialChannel: public ZscriptChannel<ZP> {
    ZscriptSerialOutStream<ZP> out;
    GenericCore::TokenRingBuffer<ZP> tBuffer;
    ZscriptTokenizer<ZP> tokenizer;

    uint8_t buffer[ZP::serialBufferSize];
    uint8_t tmp = 0;
    bool usingTmp = false;

public:
    ZscriptSerialChannel() :
            ZscriptChannel<ZP>(&out, &tBuffer, true), tBuffer(buffer, ZP::serialBufferSize), tokenizer(tBuffer.getWriter(), 2) {
    }
    void channelInfo(ZscriptCommandContext<ZP> ctx) {
        CommandOutStream < ZP > out = ctx.getOutStream();
        out.writeField('N', 0);
        out.writeField('M', 0x7);
    }

    void channelSetup(ZscriptCommandContext<ZP> ctx) {
        (void) ctx;
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