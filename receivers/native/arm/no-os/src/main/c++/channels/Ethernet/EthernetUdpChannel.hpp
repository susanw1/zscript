/*
 * EthernetUdpChannel.hpp
 *
 *  Created on: 6 Jul 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___CHANNELS_ETHERNET_ETHERNETUDPCHANNEL_HPP_
#define SRC_MAIN_C___CHANNELS_ETHERNET_ETHERNETUDPCHANNEL_HPP_

#include "../../LowLevel/ArduinoSpiLayer/src/Ethernet.h"
#include <ZcodeParameters.hpp>
#include <ZcodeIncludes.hpp>

#include <channels/ZcodeChannelInStream.hpp>
#include <channels/ZcodeCommandChannel.hpp>
#include <ZcodeOutStream.hpp>

class EthernetUdpChannel;

class EthernetUdpChannelInStream: public ZcodeChannelInStream<ZcodeParameters> {
private:

    uint8_t big[128];
    uint8_t buffer = 0;
    bool usingBuffer = false;
    EthernetUdpChannel *channel;

public:
    EthernetUdpChannelInStream(Zcode<ZcodeParameters> *zcode, EthernetUdpChannel *channel) :
            ZcodeChannelInStream<ZcodeParameters>(zcode, channel, big, 128), channel(channel) {
    }
    bool pushData() {
        if (usingBuffer) {
            if (this->slot.acceptByte(buffer)) {
                usingBuffer = false;
                return true;
            }
            return false;
        }
        if (channel->getUdp()->available() > 0) {
            int16_t tmp = channel->getUdp()->read();
            if (tmp == -1) {
                return false;
            }
            buffer = (uint8_t) tmp;
            if (!this->slot.acceptByte(buffer)) {
                usingBuffer = true;
                return false;
            }
            return true;
        } else {
            return false;
        }
    }
};

class EthernetUdpOutStream: public ZcodeOutStream<ZcodeParameters> {
private:
    bool openB = false;
    EthernetUdpChannel *channel;

public:
    EthernetUdpOutStream(EthernetUdpChannel *channel) :
            channel(channel) {
    }

    void open(ZcodeCommandChannel<TestParams> *target, ZcodeOutStreamOpenType t) {
        (void) target;
        (void) t;
        openB = true;
        channel->getUdp()->beginPacket(channel->getIp(), channel->getPort());
    }

    bool isOpen() {
        return openB;
    }

    void close() {
        channel->getUdp()->endPacket();
        channel->getUdp()->flush();
        openB = false;
    }

    void writeByte(uint8_t value) {
        channel->getUdp()->write(value);
    }

};

class EthernetUdpChannel: public ZcodeCommandChannel<ZcodeParameters> {
    EthernetUdpChannelInStream seqin;
    EthernetUdpOutStream out;
    EthernetUDP udp;
    IPAddress mostRecentIP;
    uint16_t mostRecentPort;

public:

    EthernetUdpChannel(uint16_t port, Zcode<ZcodeParameters> *zcode) :
            ZcodeCommandChannel<ZcodeParameters>(zcode, &seqin, &out, false), out(this), seqin(zcode, this), udp(), mostRecentIP(), mostRecentPort(0) {
        udp.begin(port);
    }

    EthernetUDP* getUdp() {
        return udp;
    }

    IPAddress getIp() {
        return mostRecentIP;
    }

    uint16_t getPort() {
        return mostRecentPort;
    }
};
#endif /* SRC_MAIN_C___CHANNELS_ETHERNET_ETHERNETUDPCHANNEL_HPP_ */
