/*
 * UipUdpWrapper.hpp
 *
 *  Created on: 11 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_CHANNELS_UIPUDPWRAPPER_HPP_
#define SRC_TEST_CPP_CHANNELS_UIPUDPWRAPPER_HPP_

#include "../UIP/UdpSocket.h"

class UipUdpReadWrapper {
private:
    UdpSocket *socket;
    bool reading = false;
public:
    UipUdpReadWrapper(UdpSocket *socket) :
            socket(socket) {
    }
    bool isReading() {
        return reading && socket->available() > 0;
    }
    bool open() {
        if (!reading) {
            reading = socket->parsePacket();
            return reading;
        }
        return false;
    }
    int available() {
        if (reading) {
            return socket->available();
        }
        return 0;
    }
    void close() {
        if (reading) {
            socket->flush();
            reading = false;
        }
    }
    int read() {
        if (reading) {
            return socket->read();
        }
        return -1;
    }
    int peek() {
        if (reading) {
            return socket->peek();
        }
        return -1;
    }
    int peek(memaddress offset) {
        if (reading) {
            return socket->peek(offset);
        }
        return -1;
    }
    IpAddress remoteIP() {
        return socket->remoteIP();
    }
    uint16_t remotePort() {
        return socket->remotePort();
    }
};
class UipUdpWriteWrapper {
private:
    UdpSocket *const socket;
    bool isWriting = false;
public:
    UipUdpWriteWrapper(UdpSocket *socket) :
            socket(socket) {
    }
    bool beginPacket(IpAddress ip, uint16_t port) {
        if (!isWriting) {
            isWriting = socket->beginPacket(ip, port);
            return isWriting;
        }
        return false;
    }

    size_t write(uint8_t b) {
        if (isWriting) {
            return socket->write(b);
        }
        return 0;
    }
    size_t write(const uint8_t *buffer, size_t size) {
        if (isWriting) {
            return socket->write(buffer, size);
        }
        return 0;
    }

    void endPacket() {
        if (isWriting) {
            socket->endPacket();
            isWriting = false;
        }
    }

};
#endif /* SRC_TEST_CPP_CHANNELS_UIPUDPWRAPPER_HPP_ */
