/*
 * UipUdpSequenceInStream.hpp
 *
 *  Created on: 10 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_CHANNELS_UIPUDPSEQUENCEINSTREAM_HPP_
#define SRC_TEST_CPP_CHANNELS_UIPUDPSEQUENCEINSTREAM_HPP_

#include "../RCode/parsing/RCodeSequenceInStream.hpp"

class UipUdpLookaheadStream: public RCodeLookaheadStream {
private:
    UdpSocket *socket;
    int pos = 0;
    int last = -2;
public:
    UipUdpLookaheadStream(UdpSocket *socket) :
            socket(socket) {
    }
    virtual char read() {
        int result;
        if (last != -2) {
            result = last;
            last = -2;
        } else {
            result = socket->peek(pos++);
        }
        return result == -1 ? '\n' : result;
    }
    void reset(int last) {
        pos = 0;
        this->last = last;
    }
};
class UipUdpSequenceInStream: public RCodeSequenceInStream {
private:
    UdpSocket *socket;
    UipUdpLookaheadStream lookahead;
    int last = -1;
    bool open = false;
public:
    UipUdpSequenceInStream(UdpSocket *socket) :
            socket(socket), lookahead(socket) {
    }
    virtual char next() {
        int read = socket->read();
        if (read < 0 || read == '\n') {
            open = false;
            if (read == '\n' && socket->peek() == -1) {
                socket->read();
            }
        }
        last = read;
        return open ? read : '\n';
    }

    virtual bool hasNext() {
        return socket->available() > 0;
    }

    virtual RCodeLookaheadStream* getLookahead() {
        lookahead.reset(last);
        return &lookahead;
    }

    virtual bool hasNextCommandSequence() {
        return socket->available() > 0;
    }

    virtual void openCommandSequence() {
        open = true;
    }

    virtual void closeCommandSequence() {
        open = false;
        socket->flush();
    }

    virtual bool isCommandSequenceOpen() const {
        return open;
    }
};
#endif /* SRC_TEST_CPP_CHANNELS_UIPUDPSEQUENCEINSTREAM_HPP_ */
