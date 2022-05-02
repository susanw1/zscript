/*
 * UipUdpChannelManager.hpp
 *
 *  Created on: 10 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_MAIN_UIPUDPCHANNELMANAGER_HPP_
#define SRC_TEST_CPP_MAIN_UIPUDPCHANNELMANAGER_HPP_

#include "../Zcode/ZcodeParameters.hpp"
#include "../Zcode/ZcodeOutStream.hpp"
#include "../Zcode/parsing/ZcodeInStream.hpp"
#include "../UIP/UdpSocket.h"
#include "UipUdpChannelInStream.hpp"
#include "UipUdpCommandChannel.hpp"
#include "UipUdpOutStream.hpp"
#include "UipUdpWrapper.hpp"

class UipUdpChannelManager {
    private:
        Zcode *zcode;
        UipUdpChannelInStream seqIn;
        ZcodeInStream in;
        UipUdpOutStream out;
        UipUdpCommandChannel channels[ZcodeParameters::uipChannelNum];
        UdpSocket sockets[ZcodeParameters::uipChannelNum];
        bool waitingForSlot = false;
        friend void setupChannels(UipUdpChannelManager *manager, UipEthernet *eth,
                Zcode *zcode);
        //    void findChannelForRun();
    public:
        UipUdpChannelManager(UipEthernet *eth, UdpSocket *socket, Zcode *zcode) :
                zcode(zcode), seqIn(), in(&seqIn), out(socket) {
            setupChannels(this, eth, zcode);
        }

        UipUdpCommandChannel* getChannels() {
            return channels;
        }

        ZcodeInStream* getInStream() {
            return &in;
        }

        ZcodeOutStream* getOutStream() {
            return &out;
        }

        void inReleased() {
        }
//    void checkSequences();
};

#endif /* SRC_TEST_CPP_MAIN_UIPUDPCHANNELMANAGER_HPP_ */
