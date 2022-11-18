/*
 * ZcodeSerialChannel.hpp
 *
 *  Created on: 1 Jul 2022
 *      Author: robert
 */

#ifndef ARDUINO_SERIAL_CHANNEL_HPP_
#define ARDUINO_SERIAL_CHANNEL_HPP_

#ifndef ZCODE_HPP_INCLUDED
#error ZcodeSerialChannel.hpp needs to be included after Zcode.hpp
#endif

#include <ZcodeChannelBuilder.hpp>

template<class ZP>
class ZcodeSerialChannelInStream: public ZcodeChannelInStream<ZP> {
private:

    uint8_t big[ZP::serialBigSize];
    uint8_t buffer = 0;
    bool usingBuffer = false;

public:
    ZcodeSerialChannelInStream(ZcodeCommandChannel<ZP> *channel) :
            ZcodeChannelInStream<ZP>(channel, big, ZP::serialBigSize) {
    }

    bool pushData() {
        if (usingBuffer) {
            if (this->slot.acceptByte(buffer)) {
                usingBuffer = false;
                return true;
            }
            return false;
        }
        if (Serial.available() > 0) {
            buffer = (uint8_t) Serial.read();
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
class ZcodeSerialOutStream: public ZcodeOutStream<ZP> {
private:
    bool openB = false;
    uint8_t readBuffer[ZP::serialChannelReadBufferSize];
    public:
    ZcodeSerialOutStream() :
            ZcodeOutStream<ZP>(readBuffer, ZP::serialChannelReadBufferSize) {
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
        Serial.print((char) value);
    }

};

template<class ZP>
class ZcodeSerialChannel: public ZcodeCommandChannel<ZP> {
    ZcodeSerialChannelInStream<ZP> seqin;
    ZcodeSerialOutStream<ZP> out;

public:
    ZcodeSerialChannel() :
            ZcodeCommandChannel<ZP>(&seqin, &out, false), seqin(this) {
    }
    void giveInfo(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream < ZP > *out = slot.getOut();
        out->writeField16('B', ZP::serialBigSize);
        out->writeField16('F', ZP::fieldNum);
        out->writeField8('N', 0);
        out->writeField8('M', 0x7);
        out->writeStatus(OK);
    }

    void readSetup(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream < ZP > *out = slot.getOut();
        out->writeStatus(OK);
    }

};
#endif /* ARDUINO_SERIAL_CHANNEL_HPP_ */
