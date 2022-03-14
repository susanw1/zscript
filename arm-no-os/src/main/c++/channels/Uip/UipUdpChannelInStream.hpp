/*
 * UipUdpSequenceInStream.hpp
 *
 *  Created on: 10 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_CHANNELS_UIPUDPCHANNELINSTREAM_HPP_
#define SRC_TEST_CPP_CHANNELS_UIPUDPCHANNELINSTREAM_HPP_

#include "instreams/ZcodeChannelInStream.hpp"
#include "UipUdpWrapper.hpp"

class UipUdpChannelInStream;
class UipUdpLookaheadStream: public ZcodeLookaheadStream {
private:
    UipUdpReadWrapper *reader;
    int pos = 0;
public:
    UipUdpLookaheadStream(UipUdpReadWrapper *reader) :
            reader(reader) {
    }
    virtual char read() {
        int result = reader->peek(pos++);
        return result == -1 ? '\n' : result;
    }

    void reset() {
        pos = 0;
    }
};
class UipUdpChannelInStream: public ZcodeChannelInStream {
private:
    UipUdpReadWrapper reader;
    UipUdpLookaheadStream lookahead;
public:
    UipUdpChannelInStream(UdpSocket *socket) :
            reader(socket), lookahead(&reader) {
    }
    UipUdpReadWrapper* getReader() {
        return &reader;
    }

    virtual int16_t read() {
        if (!reader.isReading()) {
            reader.open();
        }
        int read = reader.read();
        if (read < 0) {
            reader.close();
        }
        return read;
    }
    virtual ZcodeLookaheadStream* getLookahead() {
        lookahead.reset();
        return &lookahead;
    }

    bool hasNextCommandSequence() {
        return reader.available() > 0 || reader.open();
    }
    void close() {
        if (reader.peek() == -1) {
            reader.close();
        }
    }
};
#endif /* SRC_TEST_CPP_CHANNELS_UIPUDPCHANNELINSTREAM_HPP_ */
