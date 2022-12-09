/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_SERIAL_MODULE_CHANNELS_SERIALCHANNEL_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_SERIAL_MODULE_CHANNELS_SERIALCHANNEL_HPP_

#include <arm-no-os/serial-module/lowlevel/Serial.hpp>
#include <arm-no-os/serial-module/lowlevel/UartManager.hpp>

#include <zcode/ZcodeIncludes.hpp>

#include <zcode/channels/ZcodeChannelInStream.hpp>
#include <zcode/channels/ZcodeCommandChannel.hpp>
#include <zcode/ZcodeOutStream.hpp>

template<class ZP>
class ZcodeSerialBusInterruptSource;

template<class ZP>
class ZcodeSerialModule;

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
    Serial* getSerial() {
        return serial;
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
        (void) t;
        (void) target;
        openB = true;
    }

    bool isOpen() {
        return openB;
    }

    void close() {
        if (openB) {
            serial->transmitWriteBuffer();
        }
        openB = false;
    }

    void writeByte(uint8_t value) {
        serial->write(value);
    }
};

template<class ZP>
class SerialChannel: public ZcodeCommandChannel<ZP> {
    typedef typename ZP::LL LL;

    SerialChannelInStream<ZP> seqin;
    SerialOutStream<ZP> out;

public:

    SerialChannel(Serial *serial) :
            ZcodeCommandChannel<ZP>(&seqin, &out, false), seqin(this, serial), out(serial) {
        ZcodeSerialModule<ZP>::maskSerial(serial->getId());
        seqin.getSerial()->init(NULL, 9600, false);
    }
    SerialChannel(Serial *serial, uint32_t baud) :
            ZcodeCommandChannel<ZP>(&seqin, &out, false), seqin(this, serial), out(serial) {
        ZcodeSerialModule<ZP>::maskSerial(serial->getId());
        seqin.getSerial()->init(NULL, baud, false);
    }

    void giveInfo(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        out->writeField32('H', seqin.getSerial()->getMaxBaud());
        out->writeField16('P', seqin.getSerial()->getId());
        out->writeField16('B', ZP::serialChannelBigFieldSize);
        out->writeField16('F', ZP::maxParams);
        out->writeField8('N', 0);
        out->writeField8('A', 0);
        out->writeField8('M', 0x7);
        out->writeBigStringField("Serial based channel");
        out->writeStatus(OK);
    }

    void readSetup(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        uint32_t baud = 0;
        bool singleNdoubleStop = false;

        for (uint8_t i = 0; i < slot.getBigField()->getLength(); ++i) {
            baud <<= 8;
            baud |= slot.getBigField()->getData()[i];
        }
        if (slot.getBigField()->getLength() == 0) {
            baud = 9600;
        } else if (slot.getBigField()->getLength() > 4 || baud <= seqin.getSerial()->getMinBaud() || baud >= seqin.getSerial()->getMaxBaud()) {
            slot.fail(BAD_PARAM, "Invalid baud rate");
            return;
        }
        if (slot.getFields()->has('C')) { //check parity
            slot.fail(CMD_FAIL, "Parity not supported");
            return;
        }
        if (slot.getFields()->has('O')) {
            slot.fail(CMD_FAIL, "Noise detection not supported");
            return;
        }
        if (slot.getFields()->has('S')) {
            singleNdoubleStop = true;
        }
        if (slot.getFields()->has('A')) {
            slot.fail(CMD_FAIL, "Auto baud rate detection not supported");
            return;
        }

        seqin.getSerial()->init(NULL, baud, singleNdoubleStop);
        out->writeStatus(OK);
    }
};

#endif /* SRC_MAIN_CPP_ARM_NO_OS_SERIAL_MODULE_CHANNELS_SERIALCHANNEL_HPP_ */
