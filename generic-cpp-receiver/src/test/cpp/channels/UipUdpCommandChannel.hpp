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
#include "UipUdpWrapperSoon.hpp"

class UipUdpChannelManager;

class UipUdpCommandChannel: public RCodeCommandChannel {
private:
    UipUdpChannelManager *manager = NULL;
    RCodeCommandSequence sequence;
    uint8_t ip[4];
    uint16_t port = 0;
    bool isNotificationSet = false;
    bool isDebugSet = false;
    bool hasSequence = false;

    void setup(RCode *rcode, UipUdpChannelManager *manager) {
        this->sequence.setRCode(rcode);
        this->manager = manager;
    }
    friend void setupChannels(UipUdpChannelManager *manager, RCode *rcode);
public:
    UipUdpCommandChannel() :
            sequence(NULL, this) {
        for (int i = 0; i < 4; i++) {
            ip[i] = 0;
        }
    }
    void open(UdpSocket *toOpen) {
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
    bool isInUse() {
        return isDebugSet || isNotificationSet || sequence.peekFirst() != NULL;
    }
    void setHasSequence() {
        hasSequence = true;
    }
    void unsetHasSequence() {
        hasSequence = false;
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
        isNotificationSet = true;
    }

    virtual void releaseFromNotificationChannel() {
        isNotificationSet = false;
    }

    virtual void setAsDebugChannel() {
        isDebugSet = true;
    }

    virtual void releaseFromDebugChannel() {
        isDebugSet = false;
    }

    virtual void setLocks(RCodeLockSet *locks) {
    }
};
#include "UipUdpChannelManager.hpp"

#endif /* SRC_TEST_CPP_CHANNELS_UIPUDPCOMMANDCHANNEL_HPP_ */
