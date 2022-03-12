/*
 * UipUdpChannelManager.hpp
 *
 *  Created on: 10 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_MAIN_UIPUDPCHANNELMANAGER_HPP_
#define SRC_TEST_CPP_MAIN_UIPUDPCHANNELMANAGER_HPP_

#include "../RCode/RCodeParameters.hpp"
#include "../RCode/RCodeOutStream.hpp"
#include "../RCode/parsing/RCodeInStream.hpp"
#include "../UIP/UdpSocket.h"
#include "UipUdpChannelInStream.hpp"
#include "UipUdpCommandChannel.hpp"
#include "UipUdpOutStream.hpp"
#include "UipUdpWrapper.hpp"

class UipUdpChannelManager {
    RCode *zcode;
    UipUdpChannelInStream seqIn;
    RCodeInStream in;
    UipUdpOutStream out;
    UipUdpCommandChannel channels[RCodeParameters::uipChannelNum];
    UdpSocket sockets[RCodeParameters::uipChannelNum];
    bool waitingForSlot = false;
    friend void setupChannels(UipUdpChannelManager *manager, UipEthernet *eth,
            RCode *zcode);
//    void findChannelForRun();
public:
    UipUdpChannelManager(UipEthernet *eth, UdpSocket *socket, RCode *zcode) :
            zcode(zcode), seqIn(), in(&seqIn), out(socket) {
        setupChannels(this, eth, zcode);
    }

    UipUdpCommandChannel* getChannels() {
        return channels;
    }

    RCodeInStream* getInStream() {
        return &in;
    }

    RCodeOutStream* getOutStream() {
        return &out;
    }
    void inReleased() {
    }
//    void checkSequences();
};

#endif /* SRC_TEST_CPP_MAIN_UIPUDPCHANNELMANAGER_HPP_ */
