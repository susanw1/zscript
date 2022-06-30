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
    uint8_t big[1000];

    std::streamsize lengthRead = 0;
    std::streamsize pos = 0;
    int timer = 100;

public:
    ZcodeLocalChannelInStream(Zcode<TestParams> *zcode, ZcodeCommandChannel<TestParams> *channel) :
            ZcodeChannelInStream<TestParams>(zcode, channel, big, 200) {
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
        return this->slot.acceptByte(result);
    }

};

class ZcodeLocalOutStream: public ZcodeOutStream<TestParams> {
private:
    bool openB = false;

public:
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
    ZcodeLocalChannel(Zcode<TestParams> *zcode) :
            ZcodeCommandChannel<TestParams>(zcode, &seqin, &out, false), seqin(zcode, this) {
    }

};

#endif /* SRC_TEST_CPP_ZCODE_TEST_ZCODELOCALCHANNEL_HPP_ */
