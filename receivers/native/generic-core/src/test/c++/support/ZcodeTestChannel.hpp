/*
 * ZcodeTestChannel.hpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_TEST_ZCODELOCALCHANNEL_HPP_
#define SRC_TEST_CPP_ZCODE_TEST_ZCODELOCALCHANNEL_HPP_

#include "ZcodeParameters.hpp"
#include "ZcodeIncludes.hpp"

#include <iostream>

#include "channels/ZcodeChannelInStream.hpp"
#include "channels/ZcodeCommandChannel.hpp"
#include "ZcodeOutStream.hpp"

class ZcodeTestChannelInStream: public ZcodeChannelInStream<TestParams> {
private:

    uint8_t big[1024];
    const uint8_t *data;
    uint16_t length;
    uint16_t pos = 0;
    uint16_t timer = 0;

public:
    ZcodeTestChannelInStream(ZcodeCommandChannel<TestParams> *channel) :
            ZcodeChannelInStream<TestParams>(channel, big, 1024), data(NULL), length(0) {
    }
    void setData(const uint8_t *data, uint16_t length) {
        this->data = data;
        this->length = length;
    }

    virtual ~ZcodeTestChannelInStream() {
    }

    bool pushData() {
        if (pos >= length) {
            if (timer == 0) {
                if (data[length - 1] != EOL_SYMBOL) {
                    if (!this->slot.acceptByte(EOL_SYMBOL)) {
                        return false;
                    }
                }
            }
            if (timer < 100) {
                timer++;
            }
            return false;
        } else {
            if (!this->slot.acceptByte(data[pos++])) {
                pos--;
                return false;
            }
            return true;
        }
    }
    bool isDone() {
        return timer >= 100;
    }

};

class ZcodeTestOutStream: public ZcodeOutStream<TestParams> {
private:
    bool openB = false;
    uint8_t readBuffer[128];

public:
    uint8_t *received;
    uint16_t outLength;
    uint16_t pos = 0;
    bool lengthExceeded = false;

    ZcodeTestOutStream(uint8_t *received, uint16_t outLength) :
            ZcodeOutStream<TestParams>(readBuffer, 128),
                    received(received), outLength(outLength) {
    }
    virtual ~ZcodeTestOutStream() {
    }

    void open(ZcodeCommandChannel<TestParams> *target, ZcodeOutStreamOpenType t) {
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
        if (pos < outLength) {
            received[pos++] = value;
        } else {
            lengthExceeded = true;
        }
    }
    bool isLengthExceeded() {
        return lengthExceeded;
    }

};

class ZcodeTestChannel: public ZcodeCommandChannel<TestParams> {
    ZcodeTestChannelInStream seqin;
    ZcodeTestOutStream out;

public:
    ZcodeTestChannel(const char *data, uint8_t *received, uint16_t outLength) :
            ZcodeCommandChannel<TestParams>(&seqin, &out, false), seqin(this), out(received, outLength) {
        uint16_t inLen = 0;
        while (data[inLen++] != 0)
            ;
        inLen--;
        seqin.setData((const uint8_t*) data, inLen);
    }
    virtual ~ZcodeTestChannel() {
    }
    bool isLengthExceeded() {
        return out.isLengthExceeded();
    }

    bool isDone() {
        return seqin.isDone();
    }

    void giveInfo(ZcodeExecutionCommandSlot<TestParams> slot) {
        ZcodeOutStream<TestParams> *out = slot.getOut();
        out->writeField16('B', 1024);
        out->writeField16('F', TestParams::fieldNum);
        out->writeField8('N', 0);
        out->writeField8('M', 0);
        out->writeBigStringField("Local test channel on for comparing string input/output");
        out->writeStatus(OK);
    }

    void readSetup(ZcodeExecutionCommandSlot<TestParams> slot) {
        ZcodeOutStream<TestParams> *out = slot.getOut();
        out->writeStatus(OK);
    }
};

#endif /* SRC_TEST_CPP_ZCODE_TEST_ZCODELOCALCHANNEL_HPP_ */
