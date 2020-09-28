/*
 * UipUdpSequenceInStream.hpp
 *
 *  Created on: 10 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_CHANNELS_UIPUDPSEQUENCEINSTREAM_HPP_
#define SRC_TEST_CPP_CHANNELS_UIPUDPSEQUENCEINSTREAM_HPP_

#include "../RCode/parsing/RCodeSequenceInStream.hpp"
#include "UipUdpWrapper.hpp"

class UipUdpSequenceInStream;
class UipUdpLookaheadStream: public RCodeLookaheadStream {
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
class UipUdpSequenceInStream: public RCodeSequenceInStream {
private:
    UipUdpReadWrapper reader;
    UipUdpLookaheadStream lookahead;
    int last = -1;
    bool open = false;
public:
    UipUdpSequenceInStream(UdpSocket *socket) :
            reader(socket), lookahead(&reader) {
    }
    virtual char next() {
        int read = reader.read();
        if (read < 0 || read == '\n') {
            open = false;
            if (read == '\n' && reader.peek() == -1) {
                reader.read();
            }
        }
        last = read;
        return open ? read : '\n';
    }
    UipUdpReadWrapper* getReader() {
        return &reader;
    }

    virtual bool hasNext() {
        return reader.available() > 0;
    }

    virtual RCodeLookaheadStream* getLookahead() {
        lookahead.reset();
        return &lookahead;
    }

    virtual bool hasNextCommandSequence() {
        return reader.available() > 0 || reader.open();
    }

    virtual void openCommandSequence() {
        open = true;
    }

    virtual void closeCommandSequence() {
        open = false;
        reader.close();
    }

    virtual bool isCommandSequenceOpen() const {
        return open;
    }
};
#endif /* SRC_TEST_CPP_CHANNELS_UIPUDPSEQUENCEINSTREAM_HPP_ */
