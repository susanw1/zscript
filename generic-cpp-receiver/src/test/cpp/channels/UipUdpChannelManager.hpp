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
#include "UipUdpCommandChannel.hpp"
#include "UipUdpOutStream.hpp"
#include "UipUdpSequenceInStream.hpp"
//#include "UipUdpWrapper.hpp"

class UipUdpChannelManager {
    UdpSocket *socket;
    UipUdpSequenceInStream seqIn;
    RCodeInStream in;
    UipUdpOutStream out;
    UipUdpCommandChannel channels[RCodeParameters::uipChannelNum];
    bool hasCheckedPackets = false;
    bool a = 0;
    friend void setupChannels(UipUdpChannelManager *manager, RCode *rcode);
public:
    UipUdpChannelManager(UdpSocket *socket, RCode *rcode) :
            socket(socket), seqIn(socket), in(&seqIn), out(socket) {
        setupChannels(this, rcode);
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
        hasCheckedPackets = false;
    }
    void checkSequences();
};

#endif /* SRC_TEST_CPP_MAIN_UIPUDPCHANNELMANAGER_HPP_ */
