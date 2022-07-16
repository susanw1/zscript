/*
 * ZcodeLocalChannel.hpp
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

class ZcodeLocalChannelInStream: public ZcodeChannelInStream<TestParams> {
private:
    static const int MAX_BUFLEN = 1000;

    char buffer[MAX_BUFLEN];
    uint8_t big[1024];

    std::streamsize lengthRead = 0;
    std::streamsize pos = 0;
    int timer = 100;

public:
    ZcodeLocalChannelInStream(ZcodeCommandChannel<TestParams> *channel) :
            ZcodeChannelInStream<TestParams>(channel, big, 1024) {
    }

    virtual ~ZcodeLocalChannelInStream() {
    }

    bool pushData() {
        char result;
        if (pos >= lengthRead) {
            if (timer > 0) {
                timer--;
                return false;
            }
            timer = 10;
            pos = 0;
            std::cin.getline(buffer, MAX_BUFLEN - 1);
            lengthRead = std::cin.gcount() - 1;
            if (buffer[lengthRead - 1] != EOL_SYMBOL) {
                buffer[lengthRead++] = EOL_SYMBOL;
            }
            if (lengthRead > 1 && buffer[0] == '\n') {
                pos++;
            }
            return false;
        } else {
            result = buffer[pos++];
        }
        if (!this->slot.acceptByte(result)) {
            pos--;
            return false;
        }
        return true;
    }

};

class ZcodeLocalOutStream: public ZcodeOutStream<TestParams> {
private:
    bool openB = false;
    uint8_t readBuffer[128];

public:
    ZcodeLocalOutStream() :
            ZcodeOutStream<TestParams>(readBuffer, 128) {
    }
    virtual ~ZcodeLocalOutStream() {
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
        std::cout << (char) value;
    }

};

class ZcodeLocalChannel: public ZcodeCommandChannel<TestParams> {
    ZcodeLocalChannelInStream seqin;
    ZcodeLocalOutStream out;

public:
    ZcodeLocalChannel() :
            ZcodeCommandChannel<TestParams>(&seqin, &out, false), seqin(this) {
    }

    void giveInfo(ZcodeExecutionCommandSlot<TestParams> slot) {
        ZcodeOutStream<TestParams> *out = slot.getOut();
        out->writeField16('B', 1024);
        out->writeField16('F', TestParams::fieldNum);
        out->writeField8('N', 0);
        out->writeField8('M', 0);
        out->writeBigStringField("Local test channel for standard in/out");
        out->writeStatus(OK);
    }

    void readSetup(ZcodeExecutionCommandSlot<TestParams> slot) {
        ZcodeOutStream<TestParams> *out = slot.getOut();
        out->writeStatus(OK);
    }
};

#endif /* SRC_TEST_CPP_ZCODE_TEST_ZCODELOCALCHANNEL_HPP_ */
