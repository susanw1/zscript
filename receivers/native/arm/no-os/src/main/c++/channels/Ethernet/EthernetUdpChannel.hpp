/*
 * EthernetUdpChannel.hpp
 *
 *  Created on: 6 Jul 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___CHANNELS_ETHERNET_ETHERNETUDPCHANNEL_HPP_
#define SRC_MAIN_C___CHANNELS_ETHERNET_ETHERNETUDPCHANNEL_HPP_

#include "../../LowLevel/ArduinoSpiLayer/src/Ethernet.h"
#include <ZcodeIncludes.hpp>

#include <channels/ZcodeChannelInStream.hpp>
#include <channels/ZcodeCommandChannel.hpp>
#include <ZcodeOutStream.hpp>

class EthernetUdpChannelInStream: public ZcodeChannelInStream<ZcodeParameters> {
private:

    uint8_t big[128];
    uint8_t buffer = 0;
    bool usingBuffer = false;
    bool hasEndedPacket = false;

    EthernetUDP *udp;
    IPAddress mostRecentIP;
    uint16_t mostRecentPort;

public:
    EthernetUdpChannelInStream(Zcode<ZcodeParameters> *zcode, ZcodeCommandChannel<ZcodeParameters> *channel, EthernetUDP *udp) :
            ZcodeChannelInStream<ZcodeParameters>(zcode, channel, big, 128), udp(udp), mostRecentIP(), mostRecentPort(0) {
    }
    IPAddress getIp() {
        return mostRecentIP;
    }

    uint16_t getPort() {
        return mostRecentPort;
    }
    bool pushData() {
        if (usingBuffer) {
            if (this->slot.acceptByte(buffer)) {
                usingBuffer = false;
                return true;
            }
            return false;
        }
        if (udp->available()) {
            buffer = (uint8_t) udp->read();
            if (!this->slot.acceptByte(buffer)) {
                usingBuffer = true;
                return false;
            }
            return true;
        } else {
            if (!hasEndedPacket) {
                hasEndedPacket = true;
                buffer = EOL_SYMBOL;
                if (!this->slot.acceptByte(buffer)) {
                    usingBuffer = true;
                    return false;
                }
                return true;
            } else if (udp->parsePacket()) {
                hasEndedPacket = false;
                mostRecentIP = udp->remoteIP();
                mostRecentPort = udp->remotePort();
                return true;
            } else {
                return false;
            }
        }
    }
};

class EthernetUdpOutStream: public ZcodeOutStream<ZcodeParameters> {
private:
    bool openB = false;
    EthernetUDP *udp;

public:
    EthernetUdpOutStream(EthernetUDP *udp) :
            udp(udp) {
    }

    void open(ZcodeCommandChannel<ZcodeParameters> *target, ZcodeOutStreamOpenType t) {
        (void) target;
        (void) t;
        openB = true;
        EthernetUdpChannelInStream *in = (EthernetUdpChannelInStream*) target->in;
        udp->beginPacket(in->getIp(), in->getPort());
    }

    bool isOpen() {
        return openB;
    }

    void close() {
        udp->endPacket();
        udp->flush();
        openB = false;
    }

    void writeByte(uint8_t value) {
        udp->write(value);
    }

};

class EthernetUdpChannel: public ZcodeCommandChannel<ZcodeParameters> {
    EthernetUDP udp;
    EthernetUdpChannelInStream seqin;
    EthernetUdpOutStream out;

public:

    EthernetUdpChannel(uint16_t port, Zcode<ZcodeParameters> *zcode) :
            ZcodeCommandChannel<ZcodeParameters>(zcode, &seqin, &out, false), udp(), seqin(zcode, this, &udp), out(&udp) {
        udp.begin(port);
    }

};
#endif /* SRC_MAIN_C___CHANNELS_ETHERNET_ETHERNETUDPCHANNEL_HPP_ */
