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

template<class ZP>
class EthernetUdpChannelInStream: public ZcodeChannelInStream<ZP> {
private:

    uint8_t big[ZP::ethernetUdpChannelBigFieldSize];
    uint8_t buffer = 0;
    bool usingBuffer = false;
    bool hasEndedPacket = false;

    EthernetUDP *udp;
    IPAddress mostRecentIP;
    uint16_t mostRecentPort;

public:
    EthernetUdpChannelInStream(ZcodeCommandChannel<ZP> *channel, EthernetUDP *udp) :
            ZcodeChannelInStream<ZP>(channel, big, ZP::ethernetUdpChannelBigFieldSize), udp(udp), mostRecentIP(), mostRecentPort(0) {
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
            if (buffer == EOL_SYMBOL) {
                hasEndedPacket = true;
            } else {
                hasEndedPacket = false;
            }
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

template<class ZP>
class EthernetUdpOutStream: public ZcodeOutStream<ZP> {
private:
    bool openB = false;
    EthernetUDP *udp;

public:
    EthernetUdpOutStream(EthernetUDP *udp) :
            udp(udp) {
    }

    void open(ZcodeCommandChannel<ZP> *target, ZcodeOutStreamOpenType t);

    bool isOpen() {
        return openB;
    }

    void close() {
        if (openB) {
            udp->endPacket();
            udp->flush();
        }
        openB = false;
    }

    void writeByte(uint8_t value) {
        if (openB) {
            udp->write(value);
        }
    }

};

template<class ZP>
class EthernetUdpChannel: public ZcodeCommandChannel<ZP> {
    EthernetUDP udp;
    EthernetUdpChannelInStream<ZP> seqin;
    EthernetUdpOutStream<ZP> out;
    IPAddress notificationIP;
    IPAddress debugIP;
    uint16_t notificationPort;
    uint16_t debugPort;

public:

    EthernetUdpChannel(uint16_t port) :
            ZcodeCommandChannel<ZP>(&seqin, &out, false), udp(), seqin(this, &udp), out(&udp), notificationIP(), debugIP(), notificationPort(
                    0), debugPort(0) {
        udp.begin(port);
    }

    void stateChange(ZcodeCommandChannelStateChange change) {
        if (change == SET_AS_NOTIFICATION) {
            notificationIP = seqin.getIp();
            notificationPort = seqin.getPort();
        } else if (change == RELEASED_FROM_NOTIFICATION) {
            notificationPort = 0;
        } else if (change == SET_AS_DEBUG) {
            debugIP = seqin.getIp();
            debugPort = seqin.getPort();
        } else if (change == RELEASED_FROM_DEBUG) {
            debugPort = 0;
        }
    }
    IPAddress getNotificationIp() {
        return notificationIP;
    }
    uint16_t getNotificationPort() {
        return notificationPort;
    }
    IPAddress getDebugIp() {
        return debugIP;
    }
    uint16_t getDebugPort() {
        return debugPort;
    }
    void giveInfo(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        out->writeField16('B', ZP::ethernetUdpChannelBigFieldSize);
        out->writeField16('F', ZP::fieldNum);
        out->writeField8('N', 0);
        out->writeField16('M', 0x111);
        out->writeBigStringField("UDP based channel");
        out->writeStatus(OK);
        //TODO: make this better
    }

    void readSetup(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        out->writeStatus(OK);
        //TODO: do this properly
    }
};

template<class ZP>
void EthernetUdpOutStream<ZP>::open(ZcodeCommandChannel<ZP> *target, ZcodeOutStreamOpenType t) {
    EthernetUdpChannel<ZP> *channel = (EthernetUdpChannel<ZP>*) target;
    EthernetUdpChannelInStream<ZP> *in = (EthernetUdpChannelInStream<ZP>*) target->in;
    if (t == RESPONSE) {
        openB = true;
        udp->beginPacket(in->getIp(), in->getPort());
    } else if (t == NOTIFICATION && channel->getNotificationPort() != 0) {
        openB = true;
        udp->beginPacket(channel->getNotificationIp(), channel->getNotificationPort());
    } else if (t == DEBUG && channel->getDebugPort() != 0) {
        openB = true;
        udp->beginPacket(channel->getDebugIp(), channel->getDebugPort());
    }
}
#endif /* SRC_MAIN_C___CHANNELS_ETHERNET_ETHERNETUDPCHANNEL_HPP_ */
