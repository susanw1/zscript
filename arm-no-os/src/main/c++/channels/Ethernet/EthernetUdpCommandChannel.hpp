/*
 * EthernetCommandChannel.hpp
 *
 *  Created on: 1 Jan 2021
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_CHANNELS_ETHERNETCOMMANDCHANNEL_HPP_
#define SRC_TEST_CPP_CHANNELS_ETHERNETCOMMANDCHANNEL_HPP_
#include <stdint.h>
#include <stdlib.h>
#include <ZcodeParameters.hpp>
#include <parsing/ZcodeCommandChannel.hpp>
#include <parsing/ZcodeCommandSequence.hpp>
#include "../../LowLevel/ArduinoSpiLayer/src/Ethernet.h"
#include "EthernetUdpChannelInStream.hpp"
#include "EthernetUdpChannelOutStream.hpp"

class EthernetUdpCommandChannel: public ZcodeCommandChannel<ZcodeParameters> {
    EthernetUDP udp;
    ZcodeCommandSequence<ZcodeParameters> seq;
    EthernetUdpChannelInStream in;
    EthernetUdpChannelOutStream out;
    IPAddress mostRecentIP;
    uint16_t mostRecentPort;

    friend EthernetUdpChannelInStream;
    friend EthernetUdpChannelLookahead;
    friend EthernetUdpChannelOutStream;

    EthernetUDP* getUdp() {
        return &udp;
    }
public:
    EthernetUdpCommandChannel(uint16_t port, Zcode<ZcodeParameters> *zcode) :
            udp(), seq(zcode, this), in(this), out(this), mostRecentIP(), mostRecentPort(0) {
        udp.begin(port);
    }

    IPAddress getIp() {
        return mostRecentIP;
    }
    uint16_t getPort() {
        return mostRecentPort;
    }
    virtual ZcodeChannelInStream<ZcodeParameters>* acquireInStream() {
        return &in;
    }

    virtual ZcodeOutStream<ZcodeParameters>* acquireOutStream() {
        return &out;
    }

    virtual bool hasCommandSequence() {
        if (udp.available()) {
            return true;
        } else {
            if (udp.parsePacket()) {
                mostRecentIP = udp.remoteIP();
                mostRecentPort = udp.remotePort();
                return true;
            } else {
                return false;
            }
        }
    }

    virtual ZcodeCommandSequence<ZcodeParameters>* getCommandSequence() {
        return &seq;
    }

    virtual bool isPacketBased() {
        return true;
    }

    virtual void releaseInStream() {
        udp.flush();
    }

    virtual void releaseOutStream() {
    }

    virtual bool hasInStream() {
        return true;
    }
    virtual bool hasOutStream() {
        return true;
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

#endif /* SRC_TEST_CPP_CHANNELS_ETHERNETCOMMANDCHANNEL_HPP_ */
