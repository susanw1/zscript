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

    uint8_t big[1000];
    const uint8_t *data;
    uint16_t length;
    uint16_t pos = 0;
    uint16_t timer = 0;

public:
    ZcodeTestChannelInStream(Zcode<TestParams> *zcode, ZcodeCommandChannel<TestParams> *channel) :
            ZcodeChannelInStream<TestParams>(zcode, channel, big, 200), data(NULL), length(0) {
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
                    this->slot.acceptByte(EOL_SYMBOL);
                }
            }
            if (timer < 100) {
                timer++;
            }
            return false;
        } else {
            return this->slot.acceptByte(data[pos++]);
        }
    }
    bool isDone() {
        return timer >= 100;
    }

};

class ZcodeTestOutStream: public ZcodeOutStream<TestParams> {
private:
    bool openB = false;

public:
    uint8_t *received;
    uint16_t outLength;
    uint16_t pos = 0;
    bool lengthExceeded = false;

    ZcodeTestOutStream(uint8_t *received, uint16_t outLength) :
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
    ZcodeTestChannel(Zcode<TestParams> *zcode, const char *data, uint8_t *received, uint16_t outLength) :
            ZcodeCommandChannel<TestParams>(zcode, &seqin, &out, false), seqin(zcode, this), out(received, outLength) {
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
};

#endif /* SRC_TEST_CPP_ZCODE_TEST_ZCODELOCALCHANNEL_HPP_ */
