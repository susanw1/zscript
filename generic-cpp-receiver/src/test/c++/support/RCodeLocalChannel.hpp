/*
 * RCodeLocalChannel.hpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_TEST_RCODELOCALCHANNEL_HPP_
#define SRC_TEST_CPP_RCODE_TEST_RCODELOCALCHANNEL_HPP_

#include "RCodeParameters.hpp"
#include "RCodeIncludes.hpp"
#include "parsing/RCodeCommandChannel.hpp"
#include "instreams/RCodeChannelInStream.hpp"
#include "AbstractRCodeOutStream.hpp"
#include "RCode.hpp"

#include <iostream>

class RCodeLocalChannelInStream;

class RCodeLocalLookaheadStream: public RCodeLookaheadStream<TestParams> {
public:
    RCodeLocalChannelInStream *parent;
    int relativePos = 0;
    virtual char read();
    RCodeLocalLookaheadStream(RCodeLocalChannelInStream *parent) :
            parent(parent) {
    }
};

class RCodeLocalChannelInStream: public RCodeChannelInStream<TestParams> {
private:
    static const int MAX_BUFLEN = 10000;

    char buffer[MAX_BUFLEN];

    std::streamsize lengthRead = 0;
    std::streamsize pos = 0;
    int timer = 100;
    bool opened = false;
    RCodeLocalLookaheadStream lookAhead;
public:
    RCodeLocalChannelInStream() :
            lookAhead(this) {
    }

    virtual ~RCodeLocalChannelInStream() {
    }

    char charAt(int relative) {
        return relative + pos >= lengthRead ? '\n' : buffer[relative + pos];
    }

    virtual int16_t read() {
        if (pos >= lengthRead) {
            opened = false;
            return -1;
        } else {
            return buffer[pos++];
        }
    }

    virtual bool hasNext() {
        return pos < lengthRead;
    }

    virtual RCodeLookaheadStream<TestParams>* getLookahead() {
        lookAhead.relativePos = 0;
        return &lookAhead;
    }

    bool hasNextCommandSequence() {
        if (!opened && timer <= 0) {
            std::cin.getline(buffer, MAX_BUFLEN - 1);
            lengthRead = std::cin.gcount() - 1;
            buffer[lengthRead] = '\n';
            opened = true;
            pos = 0;
            timer = 100;
        } else {
            timer--;
            return false;
        }
        return true;
    }
};

class RCodeLocalOutStream: public AbstractRCodeOutStream<TestParams> {
private:
    bool open = false;
public:
    virtual void openResponse(RCodeCommandChannel<TestParams> *target) {
        open = true;
    }

    virtual void openNotification(RCodeCommandChannel<TestParams> *target) {
        open = true;
    }

    virtual void openDebug(RCodeCommandChannel<TestParams> *target) {
        open = true;
    }

    virtual bool isOpen() {
        return open;
    }

    virtual void close() {
        open = false;
    }

    virtual void writeByte(uint8_t value) {
        std::cout << (char) value;
    }

    virtual RCodeOutStream<TestParams>* writeBytes(uint8_t const *value, uint16_t length) {
        for (int i = 0; i < length; ++i) {
            std::cout << (char) value[i];
        }
        return this;
    }
};

class RCodeLocalChannel: public RCodeCommandChannel<TestParams> {
    RCodeLocalChannelInStream seqin;
    RCodeLocalOutStream out;
    RCodeCommandSequence<TestParams> seq;
public:
    RCodeLocalChannel(RCode<TestParams> *rcode) :
            seq(rcode, this) {
    }

    virtual ~RCodeLocalChannel() {

    }

    virtual RCodeLocalChannelInStream* acquireInStream() {
        return &seqin;
    }

    bool hasInStream() {
        return true;
    }

    virtual RCodeOutStream<TestParams>* acquireOutStream() {
        return &out;
    }

    virtual bool hasOutStream() {
        return true;
    }

    virtual bool hasCommandSequence() {
        return seqin.hasNextCommandSequence();
    }

    virtual RCodeCommandSequence<TestParams>* getCommandSequence() {
        return &seq;
    }

    virtual bool isPacketBased() {
        return false;
    }

    virtual void releaseInStream() {
    }

    virtual void releaseOutStream() {
    }

    virtual void setAsNotificationChannel() {
    }

    virtual void releaseFromNotificationChannel() {
    }

    virtual void setAsDebugChannel() {
    }

    virtual void releaseFromDebugChannel() {
    }

    virtual void lock() {
    }

    virtual bool canLock() {
        return true;
    }

    virtual void unlock() {
    }
};

#endif /* SRC_TEST_CPP_RCODE_TEST_RCODELOCALCHANNEL_HPP_ */
