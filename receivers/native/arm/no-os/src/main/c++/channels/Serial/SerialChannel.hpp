/*
 * SerialChannel.hpp
 *
 *  Created on: 14 Jul 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___CHANNELS_SERIAL_SERIALCHANNEL_HPP_
#define SRC_MAIN_C___CHANNELS_SERIAL_SERIALCHANNEL_HPP_

#include <UartLowLevel/Serial.hpp>

#include <ZcodeIncludes.hpp>

#include <channels/ZcodeChannelInStream.hpp>
#include <channels/ZcodeCommandChannel.hpp>
#include <ZcodeOutStream.hpp>

template<class ZP>
class SerialChannelInStream: public ZcodeChannelInStream<ZP> {
private:

    uint8_t big[ZP::serialChannelBigFieldSize];
    Serial *serial;

public:
    SerialChannelInStream(ZcodeCommandChannel<ZP> *channel, Serial *serial) :
            ZcodeChannelInStream<ZP>(channel, big, ZP::serialChannelBigFieldSize), serial(serial) {
    }
    bool pushData() {
        serial->resetPeek();
        int16_t result = serial->peek();
        if (result == -1) {
            return false;
        } else {
            if (this->slot.acceptByte((uint8_t) result)) {
                serial->skipToPeek();
                return true;
            }
            serial->resetPeek();
            return false;
        }
    }
};

template<class ZP>
class SerialOutStream: public ZcodeOutStream<ZP> {
private:
    bool openB = false;
    Serial *serial;
    uint8_t readBuffer[ZP::serialChannelReadBufferSize];

public:
    SerialOutStream(Serial *serial) :
            ZcodeOutStream<ZP>(readBuffer, ZP::serialChannelReadBufferSize), serial(serial) {
    }

    void open(ZcodeCommandChannel<ZP> *target, ZcodeOutStreamOpenType t) {
        openB = true;
    }

    bool isOpen() {
        return openB;
    }

    void close() {
        serial->transmitWriteBuffer();
    }

    void writeByte(uint8_t value) {
        serial->write(value);
    }

};

template<class ZP>
class SerialChannel: public ZcodeCommandChannel<ZP> {
    SerialChannelInStream<ZP> seqin;
    SerialOutStream<ZP> out;

public:

    SerialChannel(Serial *serial) :
            ZcodeCommandChannel<ZP>(&seqin, &out, false), seqin(this, serial), out(serial) {
        //TODO: Handle things like buffer overrun, etc.
    }

    void giveInfo(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        out->writeField16('B', ZP::serialChannelBigFieldSize);
        out->writeField16('F', ZP::fieldNum);
        out->writeField8('N', 0);
        out->writeField8('M', 0x7);
        out->writeBigStringField("Serial based channel");
        out->writeStatus(OK);
        //TODO: make this better
    }

    void readSetup(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        out->writeStatus(OK);
        //TODO: do this properly
    }
};

#endif /* SRC_MAIN_C___CHANNELS_SERIAL_SERIALCHANNEL_HPP_ */
