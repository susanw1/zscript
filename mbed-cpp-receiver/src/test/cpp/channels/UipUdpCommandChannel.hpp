/*
 * UipUdpCommandChannel.hpp
 *
 *  Created on: 10 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_CHANNELS_UIPUDPCOMMANDCHANNEL_HPP_
#define SRC_TEST_CPP_CHANNELS_UIPUDPCOMMANDCHANNEL_HPP_

#include "../RCode/parsing/RCodeCommandChannel.hpp"
#include "../RCode/parsing/RCodeCommandSequence.hpp"
#include "../RCode/RCodeParameters.hpp"
#include "../UIP/UipEthernet.h"
#include "UipUdpWrapper.hpp"
#include "UipUdpSequenceInStream.hpp"
#include "UipUdpOutStream.hpp"

class UipUdpCommandChannel: public RCodeCommandChannel {
private:
    RCodeCommandSequence sequence;
    UdpSocket socket;
    UipUdpSequenceInStream in;
    RCodeInStream inSt;
    UipUdpOutStream out;
    uint8_t ip[4];
    uint16_t port = 0;

public:
    UipUdpCommandChannel(RCode *rcode, UipEthernet *eth, uint16_t localPort) :
            sequence(rcode, this), socket(eth, 1000), in(&socket), inSt(&in), out(
                    &socket) {
        socket.begin(localPort);
        for (int i = 0; i < 4; i++) {
            ip[i] = 0;
        }
    }
    void open(UipUdpWriteWrapper *toOpen) {
        toOpen->beginPacket(IpAddress(ip), port);
    }

    bool matches(uint8_t *addr, uint16_t remotePort) {
        for (int i = 0; i < 4; i++) {
            if (ip[i] != addr[i]) {
                return false;
            }
        }
        return port == remotePort;
    }
    void setAddress(uint8_t *addr, uint16_t remotePort) {
        for (int i = 0; i < 4; i++) {
            ip[i] = addr[i];
        }
        port = remotePort;
    }
    virtual RCodeInStream* getInStream();

    virtual RCodeOutStream* getOutStream();

    virtual bool hasCommandSequence();

    virtual RCodeCommandSequence* getCommandSequence() {
        return &sequence;
    }

    virtual bool isPacketBased() {
        return true;
    }

    virtual void releaseInStream();

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
#include "UipUdpSequenceInStream.hpp"

#endif /* SRC_TEST_CPP_CHANNELS_UIPUDPCOMMANDCHANNEL_HPP_ */