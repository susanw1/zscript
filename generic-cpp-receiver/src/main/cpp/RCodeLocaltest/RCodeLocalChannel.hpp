/*
 * RCodeLocalChannel.hpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_TEST_RCODELOCALCHANNEL_HPP_
#define SRC_TEST_CPP_RCODE_TEST_RCODELOCALCHANNEL_HPP_
#include "../RCode/RCodeIncludes.hpp"
#include "../RCode/RCodeParameters.hpp"
#include "../RCode/parsing/RCodeCommandChannel.hpp"
#include "../RCode/instreams/RCodeChannelInStream.hpp"
#include "../RCode/AbstractRCodeOutStream.hpp"
#include "../RCode/RCode.hpp"

#include <iostream>

class RCodeLocalChannelInStream;
class RCodeLocalLookaheadStream: public RCodeLookaheadStream {
public:
    RCodeLocalChannelInStream *parent;
    int relativePos = 0;
    virtual char read();
    RCodeLocalLookaheadStream(RCodeLocalChannelInStream *parent) :
            parent(parent) {

    }
};
class RCodeLocalChannelInStream: public RCodeChannelInStream {
private:
    char buffer[10000];
    int lengthRead = 0;
    int pos = 0;
    int timer = 100;
    bool opened = false;
    RCodeLocalLookaheadStream l;
public:
    RCodeLocalChannelInStream() :
            l(this) {

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

    virtual RCodeLookaheadStream* getLookahead() {
        l.relativePos = 0;
        return &l;
    }
    bool hasNextCommandSequence() {
        if (!opened && timer <= 0) {
            std::cin.getline(buffer, 9999);
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

    virtual ~RCodeLocalChannelInStream() {

    }
};
class RCodeLocalOutStream: public AbstractRCodeOutStream {
private:
    bool open = false;
public:
    virtual void openResponse(RCodeCommandChannel *target) {
        open = true;
    }

    virtual void openNotification(RCodeCommandChannel *target) {
        open = true;
    }

    virtual void openDebug(RCodeCommandChannel *target) {
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
    virtual RCodeOutStream* writeBytes(uint8_t const *value, uint16_t length) {
        for (int i = 0; i < length; ++i) {
            std::cout << (char) value[i];
        }
        return this;
    }
};
class RCodeLocalChannel: public RCodeCommandChannel {
    RCodeLocalChannelInStream seqin;
    RCodeLocalOutStream out;
    RCodeCommandSequence seq;
public:
    RCodeLocalChannel(RCode *rcode) :
            seq(rcode, this) {

    }
    virtual RCodeLocalChannelInStream* acquireInStream() {
        return &seqin;
    }
    bool hasInStream() {
        return true;
    }
    virtual RCodeOutStream* acquireOutStream() {
        return &out;
    }
    virtual bool hasOutStream() {
        return true;
    }
    virtual bool hasCommandSequence() {
        return seqin.hasNextCommandSequence();
    }

    virtual RCodeCommandSequence* getCommandSequence() {
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

    virtual ~RCodeLocalChannel() {

    }
};

#endif /* SRC_TEST_CPP_RCODE_TEST_RCODELOCALCHANNEL_HPP_ */
