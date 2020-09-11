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
#include "../RCode/parsing/RCodeInStream.hpp"
#include "../RCode/AbstractRCodeOutStream.hpp"
#include "../RCode/RCode.hpp"

#include <iostream>

class RCodeLocalSequenceInStream;
class RCodeLocalLookaheadStream: public RCodeLookaheadStream {
public:
    RCodeLocalSequenceInStream *parent;
    int relativePos = 0;
    virtual char read();
    RCodeLocalLookaheadStream(RCodeLocalSequenceInStream *parent) :
            parent(parent) {

    }
};
class RCodeLocalSequenceInStream: public RCodeSequenceInStream {
private:
    char buffer[500];
    int lengthRead = 0;
    int pos = 0;
    bool opened = false;
    RCodeLocalLookaheadStream l;
public:
    RCodeLocalSequenceInStream() :
            l(this) {

    }
    char charAt(int relative) {
        return relative + pos - 1 >= lengthRead ?
                '\n' : buffer[relative + pos - 1];
    }
    virtual char next() {
        return pos >= lengthRead ? '\n' : buffer[pos++];
    }

    virtual bool hasNext() {
        return pos < lengthRead;
    }

    virtual RCodeLookaheadStream* getLookahead() {
        l.relativePos = 0;
        return &l;
    }

    virtual bool hasNextCommandSequence() {
        return !opened;
    }

    virtual void openCommandSequence() {
        std::cin.getline(buffer, 499);
        lengthRead = std::cin.gcount();
        buffer[lengthRead - 1] = '\n';
        opened = true;
        pos = 0;
    }

    virtual void closeCommandSequence() {
        opened = false;
    }
    virtual bool isCommandSequenceOpen() const {
        return opened;
    }
    virtual ~RCodeLocalSequenceInStream() {

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
    RCodeInStream in;
    RCodeLocalSequenceInStream seqin;
    RCodeLocalOutStream out;
    RCodeCommandSequence seq;
public:
    RCodeLocalChannel(RCode *rcode) :
            in(&seqin), seq(rcode, this) {

    }
    virtual RCodeInStream* getInStream() {
        return &in;
    }
    virtual RCodeOutStream* getOutStream() {
        return &out;
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
        seqin.closeCommandSequence();
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

    virtual void setLocks(RCodeLockSet *locks) {

    }

    virtual ~RCodeLocalChannel() {

    }
};

#endif /* SRC_TEST_CPP_RCODE_TEST_RCODELOCALCHANNEL_HPP_ */
