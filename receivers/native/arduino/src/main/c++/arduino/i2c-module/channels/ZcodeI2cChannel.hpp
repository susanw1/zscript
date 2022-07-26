/*
 * ZcodeI2cChannel.hpp
 *
 *  Created on: 1 Jul 2022
 *      Author: robert
 */

#ifndef ARDUINO_I2C_CHANNEL_HPP_
#define ARDUINO_I2C_CHANNEL_HPP_

#ifndef SRC_TEST_CPP_ZCODE_ZCODE_HPP_
#error ZcodeI2cChannel.hpp needs to be included after Zcode.hpp
#endif

#include <ZcodeChannelBuilder.hpp>
#include <Wire.h>

template<class ZP>
class ZcodeI2cChannelInStream: public ZcodeChannelInStream<ZP> {
private:

    uint8_t big[ZP::i2cBigSize];
    uint8_t buffer = 0;
    bool usingBuffer = false;

public:

    static void doNothing(int v) {
        (void) v;
    }
    ZcodeI2cChannelInStream(ZcodeCommandChannel<ZP> *channel) :
            ZcodeChannelInStream<ZP>(channel, big, ZP::i2cBigSize) {
    }

    bool pushData() {
        if (usingBuffer) {
            if (this->slot.acceptByte(buffer)) {
                usingBuffer = false;
                return true;
            }
            return false;
        }
        if (Wire.available() > 0) {
            buffer = (uint8_t) Wire.read();
            if (!this->slot.acceptByte(buffer)) {
                usingBuffer = true;
                return false;
            }
            return true;
        } else {
            return false;
        }
    }

};

template<class ZP>
class ZcodeI2cOutStream: public ZcodeOutStream<ZP> {
public:
    uint8_t readBuffer[ZP::i2cChannelReadBufferSize];
    static uint8_t outBuffer[ZP::i2cChannelOutputBufferSize];
    static uint16_t writePos;
    static uint16_t readPos;
    bool openB = false;
    static uint8_t valueThingTest;
    public:

    static void requestHandler() {
        bool hasHitNewLine = false;
        for (uint8_t i = 0; i < 8; ++i) {
            if (hasHitNewLine || readPos >= writePos) {
                Wire.write('\n');
                hasHitNewLine = true;
            } else {
                Wire.write(outBuffer[readPos++]);
                if (outBuffer[readPos] == '\n') {
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
        }
    }
    ZcodeI2cOutStream() :
            ZcodeOutStream<ZP>(readBuffer, ZP::i2cChannelReadBufferSize) {
    }

    void open(ZcodeCommandChannel<ZP> *target, ZcodeOutStreamOpenType t) {
        (void) target;
        (void) t;
        openB = true;
    }

    bool isOpen() {
        return openB;
    }

    void close() {
        openB = false;
        if (writePos > 0) {
            pinMode(ZP::i2cAlertPin, OUTPUT);
        }
    }

    void writeByte(uint8_t value) {
        if (writePos < ZP::i2cChannelOutputBufferSize) {
            outBuffer[writePos++] = value;
        }
    }

};
template<class ZP>
uint8_t ZcodeI2cOutStream<ZP>::outBuffer[ZP::i2cChannelOutputBufferSize];
template<class ZP>
uint16_t ZcodeI2cOutStream<ZP>::writePos = 0;
template<class ZP>
uint16_t ZcodeI2cOutStream<ZP>::readPos = 0;
template<class ZP>
uint8_t ZcodeI2cOutStream<ZP>::valueThingTest = 0;

template<class ZP>
class ZcodeI2cChannel: public ZcodeCommandChannel<ZP> {
public:
    ZcodeI2cChannelInStream<ZP> seqin;
    ZcodeI2cOutStream<ZP> out;

public:
    ZcodeI2cChannel() :
            ZcodeCommandChannel<ZP>(&seqin, &out, false), seqin(this) {
    }
    void setup() {
        pinMode(ZP::i2cAlertPin, INPUT);
        digitalWrite(ZP::i2cAlertPin, LOW);
        Wire.onRequest(&ZcodeI2cOutStream<ZP>::requestHandler);
        Wire.onReceive(&ZcodeI2cChannelInStream<ZP>::doNothing);
    }
    void giveInfo(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        out->writeField16('B', ZP::i2cBigSize);
        out->writeField16('F', ZP::fieldNum);
        out->writeField8('N', 0);
        out->writeField8('M', 0x5);
        out->writeStatus(OK);
    }

    void readSetup(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        out->writeStatus(OK);
    }

};

#endif /* ARDUINO_I2C_CHANNEL_HPP_ */
