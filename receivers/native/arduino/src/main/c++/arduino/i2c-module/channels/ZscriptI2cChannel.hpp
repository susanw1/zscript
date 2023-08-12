/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARDUINO_I2C_MODULE_CHANNELS_ZSCRIPTI2CCHANNEL_HPP_
#define SRC_MAIN_CPP_ARDUINO_I2C_MODULE_CHANNELS_ZSCRIPTI2CCHANNEL_HPP_

#ifndef ZSCRIPT_HPP_INCLUDED
#error ZscriptI2cChannel.hpp needs to be included after Zscript.hpp
#endif

#include <ZscriptChannelBuilder.hpp>
#include <Wire.h>

namespace Zscript {

template<class ZP>
class ZscriptI2cOutStream: public AbstractOutStream<ZP> {
public:
    static uint8_t outBuffer[ZP::i2cChannelOutputBufferSize];
    static uint16_t writePos;
    static uint16_t readPos;
    static uint8_t valueThingTest;
    static uint8_t addr;
    static bool usingMagicAddr;
    bool openB = false;

public:
    void setAddr(uint8_t addr) {
        I2C_SET_ADDRESS(addr);
        I2C_ENABLE_GENERAL_CALL();
        ZscriptI2cOutStream<ZP>::addr = addr;
    }

    static void requestHandler() {
        if (usingMagicAddr) {
            Wire.write(addr << 1);
            Wire.write(addr << 1);
            I2C_SET_ADDRESS(addr);
            I2C_ENABLE_GENERAL_CALL();
            usingMagicAddr = false;
            return;
        }
        bool hasHitNewLine = false;
        for (uint8_t i = 0; i < 8; ++i) {
            if (hasHitNewLine || readPos >= writePos) {
                Wire.write('\n');
                hasHitNewLine = true;
            } else {
                Wire.write(outBuffer[readPos++]);
                if (outBuffer[readPos - 1] == '\n') {
                    hasHitNewLine = true;
                }
            }
        }
        if (readPos >= writePos) {
            writePos = 0;
            readPos = 0;
            for (uint8_t i = 0; i < ZP::i2cChannelOutputBufferSize; ++i) {
                outBuffer[i] = 0x00;
            }
            pinMode(ZP::i2cAlertPin, INPUT);
        } else if (outBuffer[readPos - 1] == '\n') {
            usingMagicAddr = true;
            I2C_SET_ADDRESS(0x0C);
            I2C_ENABLE_GENERAL_CALL();
        }
    }

    void open() {
        openB = true;
    }

    bool isOpen() {
        return openB;
    }

    void close() {
        openB = false;
        if (writePos > 0) {
            I2C_SET_ADDRESS(0x0C);
            I2C_ENABLE_GENERAL_CALL();
            pinMode(ZP::i2cAlertPin, OUTPUT);
            usingMagicAddr = true;
        }
    }

    void writeBytes(const uint8_t *bytes, uint16_t count, bool hexMode) {
        if (hexMode) {
            for (uint16_t i = 0; i < count; i++) {
                outBuffer[writePos++] = AbstractOutStream < ZP > ::toHexChar(bytes[i] >> 4);
                outBuffer[writePos++] = AbstractOutStream < ZP > ::toHexChar(bytes[i] & 0xf);
            }
        } else {
            for (uint16_t i = 0; i < count; ++i) {
                outBuffer[writePos++] = bytes[i];
            }
        }
    }

};
template<class ZP>
uint8_t ZscriptI2cOutStream<ZP>::outBuffer[ZP::i2cChannelOutputBufferSize];
template<class ZP>
uint16_t ZscriptI2cOutStream<ZP>::writePos = 0;
template<class ZP>
uint16_t ZscriptI2cOutStream<ZP>::readPos = 0;
template<class ZP>
uint8_t ZscriptI2cOutStream<ZP>::valueThingTest = 0;
template<class ZP>
uint8_t ZscriptI2cOutStream<ZP>::addr = 0;
template<class ZP>
bool ZscriptI2cOutStream<ZP>::usingMagicAddr = false;

template<class ZP>
class ZscriptI2cChannel: public ZscriptChannel<ZP> {
public:
    ZscriptI2cOutStream<ZP> out;
    GenericCore::TokenRingBuffer<ZP> tBuffer;
    ZscriptTokenizer<ZP> tokenizer;

    uint8_t buffer[ZP::i2cBufferSize];
    uint8_t tmp = 0;
    bool usingTmp = false;

public:
    ZscriptI2cChannel() :
            ZscriptChannel<ZP>(&out, &tBuffer, true), tBuffer(buffer, ZP::i2cBufferSize), tokenizer(tBuffer.getWriter(), 2) {
    }

    void setup() {
        pinMode(ZP::i2cAlertPin, INPUT);
        digitalWrite(ZP::i2cAlertPin, LOW);
        Wire.onRequest(&ZscriptI2cOutStream<ZP>::requestHandler);
    }

    void setAddress(uint8_t addr) {
        Wire.begin(0x61);
        out.setAddr(addr);
    }

    void moveAlong() {
        if (usingTmp) {
            if (tokenizer.offer(tmp)) {
                usingTmp = false;
            }
        }
        while (Wire.available() > 0) {
            tmp = (uint8_t) Wire.read();
            if (!tokenizer.offer(tmp)) {
                usingTmp = true;
                return;
            }
        }

    }

    void channelInfo(ZscriptCommandContext<ZP> ctx) {
        CommandOutStream < ZP > out = ctx.getOutStream();
        out.writeField('N', 0);
        out.writeField('M', 0x5);
    }

    void channelSetup(ZscriptCommandContext<ZP> ctx) {
        (void) ctx;
    }

};
}

#endif /* SRC_MAIN_CPP_ARDUINO_I2C_MODULE_CHANNELS_ZSCRIPTI2CCHANNEL_HPP_ */
