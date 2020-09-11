/*
 * UipUdpOutStream.hpp
 *
 *  Created on: 10 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_CHANNELS_UIPUDPOUTSTREAM_HPP_
#define SRC_TEST_CPP_CHANNELS_UIPUDPOUTSTREAM_HPP_

#include "../RCode/AbstractRCodeOutStream.hpp"
#include "UipUdpCommandChannel.hpp"

class UipUdpOutStream: public AbstractRCodeOutStream {
private:
    UdpSocket *socket;
    bool open = false;
public:
    UipUdpOutStream(UdpSocket *socket) :
            socket(socket) {
    }

    virtual void writeByte(uint8_t value) {
        socket->write(value);
    }
    virtual RCodeOutStream* writeBytes(uint8_t const *value, uint16_t length) {
        socket->write(value, length);
        return this;
    }
    virtual void openResponse(RCodeCommandChannel *target) {
        ((UipUdpCommandChannel*) target)->open(socket);
        open = true;
    }

    virtual void openNotification(RCodeCommandChannel *target) {
        ((UipUdpCommandChannel*) target)->open(socket);
        open = true;
    }

    virtual void openDebug(RCodeCommandChannel *target) {
        ((UipUdpCommandChannel*) target)->open(socket);
        open = true;
    }

    virtual bool isOpen() {
        return open;
    }

    virtual void close() {
        socket->endPacket();
        open = false;
    }
};

#endif /* SRC_TEST_CPP_CHANNELS_UIPUDPOUTSTREAM_HPP_ */
