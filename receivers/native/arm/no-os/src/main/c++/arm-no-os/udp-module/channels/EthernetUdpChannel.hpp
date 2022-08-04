/*
 * EthernetUdpChannel.hpp
 *
 *  Created on: 6 Jul 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___CHANNELS_ETHERNET_ETHERNETUDPCHANNEL_HPP_
#define SRC_MAIN_C___CHANNELS_ETHERNET_ETHERNETUDPCHANNEL_HPP_

#include <arm-no-os/udp-module/lowlevel/src/Ethernet.h>
#include <zcode/ZcodeIncludes.hpp>

#include <zcode/channels/ZcodeChannelInStream.hpp>
#include <zcode/channels/ZcodeCommandChannel.hpp>
#include <zcode/ZcodeOutStream.hpp>
#include <arm-no-os/arm-core-module/persistence/ZcodeFlashPersistence.hpp>

template<class ZP>
class EthernetUdpChannelInStream: public ZcodeChannelInStream<ZP> {
private:
    typedef typename ZP::LL LL;

    uint8_t big[ZP::ethernetUdpChannelBigFieldSize];
    uint8_t buffer = 0;
    bool usingBuffer = false;
    bool hasEndedPacket = false;

    EthernetUDP<LL> *udp;
    IPAddress mostRecentIP;
    uint16_t mostRecentPort;

public:
    EthernetUdpChannelInStream(ZcodeCommandChannel<ZP> *channel, EthernetUDP<LL> *udp) :
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
            Ethernet<LowLevelConfiguration> .maintain();
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
                Ethernet<LowLevelConfiguration> .maintain();
                return false;
            }
            return true;
        } else {
            if (!hasEndedPacket) {
                hasEndedPacket = true;
                buffer = EOL_SYMBOL;
                if (!this->slot.acceptByte(buffer)) {
                    usingBuffer = true;
                    Ethernet<LowLevelConfiguration> .maintain();
                    return false;
                }
                return true;
            } else if (udp->parsePacket()) {
                hasEndedPacket = false;
                mostRecentIP = udp->remoteIP();
                mostRecentPort = udp->remotePort();
                return true;
            } else {
                Ethernet<LowLevelConfiguration> .maintain();
                return false;
            }
        }
    }
};

template<class ZP>
class EthernetUdpChannel;

template<class ZP>
class EthernetUdpOutStream: public ZcodeOutStream<ZP> {
private:
    typedef typename ZP::LL LL;
    struct UdpOutStatus {
        bool openB :1;
        bool openFrozen :1;
        ZcodeOutStreamOpenType openType :4;
    };
    EthernetUdpChannel<ZP> *channel;
    uint8_t readBuffer[ZP::ethernetUdpChannelReadBufferSize];
    UdpOutStatus status;

public:
    EthernetUdpOutStream(EthernetUdpChannel<ZP> *channel) :
            ZcodeOutStream<ZP>(readBuffer, ZP::ethernetUdpChannelReadBufferSize), channel(channel), status( { false, false, RESPONSE }) {
    }
    void actualOpen();

    void open(ZcodeCommandChannel<ZP> *target, ZcodeOutStreamOpenType t) {
        if (!status.openB && target == channel) {
            status.openType = t;
            status.openB = true;
            status.openFrozen = true;
        }
    }

    bool isOpen() {
        return status.openB;
    }

    void close() {
        if (status.openB && !status.openFrozen) {
            channel->getEthernetUdp()->endPacket();
            channel->getEthernetUdp()->flush();
        }
        status.openB = false;
        status.openFrozen = false;
    }

    void writeByte(uint8_t value) {
        if (status.openB) {
            if (status.openFrozen) {
                actualOpen();
            }
            channel->getEthernetUdp()->write(value);
        }
    }

};

template<class ZP>
class EthernetUdpChannel: public ZcodeCommandChannel<ZP> {
    typedef typename ZP::LL LL;
    EthernetUDP<LL> udp;
    EthernetUdpChannelInStream<ZP> seqin;
    EthernetUdpOutStream<ZP> out;
    IPAddress notificationIP;
    IPAddress debugIP;
    uint16_t notificationPort;
    uint16_t debugPort;

    uint16_t port;

public:

    EthernetUdpChannel(uint16_t port) :
            ZcodeCommandChannel<ZP>(&seqin, &out, false), udp(), seqin(this, &udp), out(this), notificationIP(), debugIP(), notificationPort(
                    0), debugPort(0), port(port) {
        uint8_t *mac;
        uint8_t macHardCoded[6] = { 0xde, 0xad, 0xbe, 0xef, 0xfe, 0xad };
        if (ZcodeFlashPersistence<LL>::persist.hasMac()) {
            mac = ZcodeFlashPersistence<LL>::persist.getMac();
        } else {
            mac = macHardCoded;
        }
        while (!Ethernet<LL> .begin(mac, 5000, 5000)) {
            //TODO: not just loop forever, instead come back later...
        }
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
        uint32_t addr = Ethernet<LL> .localIP();
        out->writeBigHexField((uint8_t*) &addr, 4);
        out->writeField16('P', port);
        out->writeStatus(OK);
        //TODO: make this better
    }

    void readSetup(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        out->writeBigHexField(ZcodeFlashPersistence<LL>::persist.getMac(), 6);
        if (slot.getBigField()->getLength() != 6 && slot.getBigField()->getLength() != 0) {
            slot.fail(BAD_PARAM, "MAC addresses must be 6 bytes long");
            return;
        } else if (slot.getBigField()->getLength() != 0) {
            ZcodeFlashPersistence<LL>::persist.writeMac(slot.getBigField()->getData());
        }
        out->writeStatus(OK);
    }
    bool reset() {
        udp.flush();
        udp.stop();
        Ethernet<LL> .init();
        uint8_t *mac;
        uint8_t macHardCoded[6] = { 0xde, 0xad, 0xbe, 0xef, 0xfe, 0xad };
        if (ZcodeFlashPersistence<LL>::persist.hasMac()) {
            mac = ZcodeFlashPersistence<LL>::persist.getMac();
        } else {
            mac = macHardCoded;
        }
        while (!Ethernet<LL> .begin(mac, 5000, 5000)) {
        }
        udp.begin(port);
        return true;
    }
    EthernetUDP<LL>* getEthernetUdp() {
        return &udp;
    }
};

template<class ZP>
void EthernetUdpOutStream<ZP>::actualOpen() {
    status.openFrozen = false;
    EthernetUdpChannelInStream<ZP> *in = (EthernetUdpChannelInStream<ZP>*) channel->in;
    if (status.openType == RESPONSE) {
        status.openB = true;
        channel->getEthernetUdp()->beginPacket(in->getIp(), in->getPort());
    } else if (status.openType == NOTIFICATION && channel->getNotificationPort() != 0) {
        status.openB = true;
        channel->getEthernetUdp()->beginPacket(channel->getNotificationIp(), channel->getNotificationPort());
    } else if (status.openType == DEBUG && channel->getDebugPort() != 0) {
        status.openB = true;
        channel->getEthernetUdp()->beginPacket(channel->getDebugIp(), channel->getDebugPort());
    }
}
#endif /* SRC_MAIN_C___CHANNELS_ETHERNET_ETHERNETUDPCHANNEL_HPP_ */
