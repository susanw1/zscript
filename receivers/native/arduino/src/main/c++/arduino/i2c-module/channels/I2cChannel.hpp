/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARDUINO_I2C_MODULE_CHANNELS_ZSCRIPTI2CCHANNEL_HPP_
#define SRC_MAIN_CPP_ARDUINO_I2C_MODULE_CHANNELS_ZSCRIPTI2CCHANNEL_HPP_


#include <ZscriptChannelBuilder.hpp>
#include <Wire.h>

#include "../I2cManager.hpp"

namespace Zscript {

template<class ZP>
class I2cOutStream : public AbstractOutStream<ZP> {
public:
    static const uint8_t SmBusAlertAddr;
    static uint8_t outBuffer[ZP::i2cChannelOutputBufferSize];
    static uint16_t writePos;
    static uint16_t readPos;
    static uint8_t addr;
    static bool usingMagicAddr;
    bool openB = false;

public:
    void setAddr(uint8_t addr) {
        if (!usingMagicAddr) {
            I2C_ADDRESS_AND_GENERAL_CALL(addr);
        }
        I2cOutStream<ZP>::addr = addr;
    }

    uint8_t getAddr() {
        return addr;
    }

    static void requestHandler() {
        if (usingMagicAddr && I2C_WAS_SMBUS_ALERT_READ()) {
            Wire.write(addr << 1);
            Wire.write(0);
            I2C_END_SMBUS_ALERT(addr);
            pinMode(ZP::i2cAlertOutPin, INPUT);
            usingMagicAddr = false;
            return;
        }
        bool hasHitNewLine = false;
        for (uint8_t i = 0; i < ZP::i2cAddressingReadBlockLength; ++i) {
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
            if (writePos >= ZP::i2cChannelOutputBufferSize) {
                // return error...
                writePos = 0;
                readPos = 0;
                outBuffer[writePos++] = '!';
                outBuffer[writePos++] = 'S';
                outBuffer[writePos++] = '1';
                outBuffer[writePos++] = '0';
                outBuffer[writePos++] = '\n';
                for (uint8_t i = writePos; i < ZP::i2cChannelOutputBufferSize; ++i) {
                    outBuffer[i] = 0x00;
                }
                I2C_BEGIN_SMBUS_ALERT();
                pinMode(ZP::i2cAlertOutPin, OUTPUT);
                usingMagicAddr = true;
                return;
            }
            writePos = 0;
            readPos = 0;
            for (uint8_t i = 0; i < ZP::i2cChannelOutputBufferSize; ++i) {
                outBuffer[i] = 0x00;
            }
        } else if (outBuffer[readPos - 1] == '\n') {
            I2C_BEGIN_SMBUS_ALERT();
            pinMode(ZP::i2cAlertOutPin, OUTPUT);
            usingMagicAddr = true;
        }
    }

    void open(uint8_t source) {
        (void) source;
        openB = true;
    }

    bool isOpen() {
        return openB;
    }

    void close() {
        openB = false;
        if (writePos > 0) {
            I2C_BEGIN_SMBUS_ALERT();
            pinMode(ZP::i2cAlertOutPin, OUTPUT);
            usingMagicAddr = true;
        }
    }

    void writeBytes(const uint8_t *bytes, uint16_t count, bool hexMode) {
        if (writePos == ZP::i2cChannelOutputBufferSize) {
            return;
        }
        if (hexMode) {
            for (uint16_t i = 0; i < count; i++) {
                if (writePos == ZP::i2cChannelOutputBufferSize - 1) {
                    outBuffer[writePos++] = '\n';
                    return;
                }
                outBuffer[writePos++] = AbstractOutStream<ZP>::toHexChar(bytes[i] >> 4);
                if (writePos == ZP::i2cChannelOutputBufferSize - 1) {
                    outBuffer[writePos++] = '\n';
                    return;
                }
                outBuffer[writePos++] = AbstractOutStream<ZP>::toHexChar(bytes[i] & 0xf);
            }
        } else {
            for (uint16_t i = 0; i < count; ++i) {
                if (writePos == ZP::i2cChannelOutputBufferSize - 1) {
                    outBuffer[writePos++] = '\n';
                    return;
                }
                outBuffer[writePos++] = bytes[i];
            }
        }
    }

};

template<class ZP>
const uint8_t I2cOutStream<ZP>::SmBusAlertAddr = 0x0C;

template<class ZP>
uint8_t I2cOutStream<ZP>::outBuffer[ZP::i2cChannelOutputBufferSize];

template<class ZP>
uint16_t I2cOutStream<ZP>::writePos = 0;

template<class ZP>
uint16_t I2cOutStream<ZP>::readPos = 0;

template<class ZP>
uint8_t I2cOutStream<ZP>::addr = 0;

template<class ZP>
bool I2cOutStream<ZP>::usingMagicAddr = false;

template<class ZP>
class I2cChannel : public ZscriptChannel<ZP> {
public:
    I2cOutStream<ZP> out;
    static GenericCore::TokenRingBuffer<ZP> tBuffer;
    static ZscriptTokenizer<ZP> tokenizer;

    static uint8_t buffer[ZP::i2cBufferSize];
    static uint8_t tmp;
    static bool usingTmp;

    static void dataHandler(int a) {
        (void) a;
        if (usingTmp) {
            if (tokenizer.offer(tmp)) {
                usingTmp = false;
            } else {
                return;
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

public:
    static uint8_t getNotifChannelPersistMaxLength() {
        return 0;
    }

    I2cChannel() :
            ZscriptChannel<ZP>(&out, &tBuffer, 0x5, true) {
    }

    bool setupStartupNotificationChannel() {
        return true;
    }

    void setup() {
        pinMode(ZP::i2cAlertOutPin, INPUT);
        digitalWrite(ZP::i2cAlertOutPin, LOW);
        Wire.onRequest(&I2cOutStream<ZP>::requestHandler);
        Wire.onReceive(&I2cChannel<ZP>::dataHandler);
    }

    void setAddress(uint8_t addr) {
        Wire.begin(addr);
        out.setAddr(addr);
    }

    uint8_t getAddress() {
        return out.getAddr();
    }

    void moveAlong() {
        if (usingTmp) {
            if (tokenizer.offer(tmp)) {
                usingTmp = false;
            } else {
                return;
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

//    void channelInfo(ZscriptCommandContext<ZP> ctx) {
//        CommandOutStream<ZP> ctxOut = ctx.getOutStream();
//        ctxOut.writeField('N', 0);
//        ctxOut.writeField('M', 0x5);
//    }
//
//    void channelSetup(ZscriptCommandContext<ZP> ctx) {
//        uint16_t addr;
//        if (ctx.getField('A', &addr)) {
//            if (addr >= 128) {
//                ctx.status(ResponseStatus::VALUE_OUT_OF_RANGE);
//                return;
//            }
//            out.setAddr((uint8_t) addr);
//        }
//        if (ctx.hasField('P')) {
//            uint8_t index = this->parser.getChannelIndex();
//            PersistenceSystem<ZP>::writeSection(PersistenceSystem<ZP>::getNotifChannelIdOffset(), 1, &index);
//        }
//    }

};

template<class ZP>
GenericCore::TokenRingBuffer<ZP> I2cChannel<ZP>::tBuffer(I2cChannel<ZP>::buffer, ZP::i2cBufferSize);

template<class ZP>
ZscriptTokenizer<ZP> I2cChannel<ZP>::tokenizer(I2cChannel<ZP>::tBuffer.getWriter(), 2);

template<class ZP>
uint8_t I2cChannel<ZP>::buffer[ZP::i2cBufferSize];

template<class ZP>
uint8_t I2cChannel<ZP>::tmp = 0;

template<class ZP>
bool I2cChannel<ZP>::usingTmp = false;

}

#endif /* SRC_MAIN_CPP_ARDUINO_I2C_MODULE_CHANNELS_ZSCRIPTI2CCHANNEL_HPP_ */
