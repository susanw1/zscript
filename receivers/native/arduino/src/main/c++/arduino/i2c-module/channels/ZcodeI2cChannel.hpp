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

#error The standard arduino libraries are so gutless we can't actually do this...

#include <ZcodeChannelBuilder.hpp>

template<class ZP>
class ZcodeI2cChannelInStream: public ZcodeChannelInStream<ZP> {
private:

    uint8_t big[ZP::i2cBigSize];
    uint8_t buffer = 0;
    bool usingBuffer = false;

public:
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
private:
    uint8_t readBuffer[ZP::i2cChannelReadBufferSize];
    static uint8_t outBuffer[ZP::i2cChannelOutputBufferSize];
    static uint16_t writePos;
    static uint16_t readPos;
    bool hasHitNewLine = false;
    bool openB = false;

    static void requestHandler() {
        if (hasHitNewLine) {
            Wire.write
        }
        Wire.write(outBuffer[readPos++]);
    }

public:

    ZcodeI2cOutStream() :
            ZcodeOutStream<ZP>(readBuffer, ZP::i2cChannelReadBufferSize) {
        Wire.onRequest(&ZcodeI2cOutStream<ZP>::requestHandler);
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
    }

    void writeByte(uint8_t value) {
        if (pos < ZP::i2cChannelOutputBufferSize) {
            outBuffer[pos++] = value;
        }
    }

};

template<class ZP>
class ZcodeI2cChannel: public ZcodeCommandChannel<ZP> {
    ZcodeI2cChannelInStream seqin;
    ZcodeI2cOutStream out;

public:
    ZcodeI2cChannel() :
            ZcodeCommandChannel<ZP>(&seqin, &out, false), seqin(this) {
    }
    void giveInfo(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream < ZP > *out = slot.getOut();
        out->writeField16('B', ZP::i2cBigSize);
        out->writeField16('F', ZP::fieldNum);
        out->writeField8('N', 0);
        out->writeField8('M', 0x5);
        out->writeStatus(OK);
    }

    void readSetup(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream < ZP > *out = slot.getOut();
        out->writeStatus(OK);
    }

};

#endif /* ARDUINO_I2C_CHANNEL_HPP_ */
