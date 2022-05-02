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
#include "parsing/ZcodeCommandChannel.hpp"
#include "instreams/ZcodeChannelInStream.hpp"
#include "AbstractZcodeOutStream.hpp"
#include "Zcode.hpp"

#include <iostream>

class ZcodeLocalChannelInStream;

class ZcodeLocalLookaheadStream: public ZcodeLookaheadStream<TestParams> {
    public:
        ZcodeLocalChannelInStream *parent;
        int relativePos = 0;
        virtual char read();
        ZcodeLocalLookaheadStream(ZcodeLocalChannelInStream *parent) :
                parent(parent) {
        }
};

class ZcodeLocalChannelInStream: public ZcodeChannelInStream<TestParams> {
    private:
        static const int MAX_BUFLEN = 10000;

        char buffer[MAX_BUFLEN];

        std::streamsize lengthRead = 0;
        std::streamsize pos = 0;
        int timer = 100;
        bool opened = false;
        ZcodeLocalLookaheadStream lookAhead;
        public:
        ZcodeLocalChannelInStream() :
                lookAhead(this) {
        }

        virtual ~ZcodeLocalChannelInStream() {
        }

        char charAt(int relative) {
            return relative + pos >= lengthRead ? Zchars::EOL_SYMBOL : buffer[relative + pos];
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

        virtual ZcodeLookaheadStream<TestParams>* getLookahead() {
            lookAhead.relativePos = 0;
            return &lookAhead;
        }

        bool hasNextCommandSequence() {
            if (!opened && timer <= 0) {
                std::cin.getline(buffer, MAX_BUFLEN - 1);
                lengthRead = std::cin.gcount() - 1;
                buffer[lengthRead] = Zchars::EOL_SYMBOL;
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

class ZcodeLocalOutStream: public AbstractZcodeOutStream<TestParams> {
    private:
        bool open = false;

    public:
        virtual void openResponse(ZcodeCommandChannel<TestParams> *target) {
            open = true;
        }

        virtual void openNotification(ZcodeCommandChannel<TestParams> *target) {
            open = true;
        }

        virtual void openDebug(ZcodeCommandChannel<TestParams> *target) {
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

        virtual ZcodeOutStream<TestParams>* writeBytes(uint8_t const *value, uint16_t length) {
            for (int i = 0; i < length; ++i) {
                std::cout << (char) value[i];
            }
            return this;
        }
};

class ZcodeLocalChannel: public ZcodeCommandChannel<TestParams> {
        ZcodeLocalChannelInStream seqin;
        ZcodeLocalOutStream out;
        ZcodeCommandSequence<TestParams> seq;

    public:
        ZcodeLocalChannel(Zcode<TestParams> *zcode) :
                seq(zcode, this) {
        }

        virtual ~ZcodeLocalChannel() {

        }

        virtual ZcodeLocalChannelInStream* acquireInStream() {
            return &seqin;
        }

        bool hasInStream() {
            return true;
        }

        virtual ZcodeOutStream<TestParams>* acquireOutStream() {
            return &out;
        }

        virtual bool hasOutStream() {
            return true;
        }

        virtual bool hasCommandSequence() {
            return seqin.hasNextCommandSequence();
        }

        virtual ZcodeCommandSequence<TestParams>* getCommandSequence() {
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

#endif /* SRC_TEST_CPP_ZCODE_TEST_ZCODELOCALCHANNEL_HPP_ */
