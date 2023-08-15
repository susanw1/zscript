/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ARDUINO_UDP_MODULE_CHANNELS_ZSCRIPTUDPCHANNEL_HPP_
#define SRC_MAIN_C___ARDUINO_UDP_MODULE_CHANNELS_ZSCRIPTUDPCHANNEL_HPP_

#include <Ethernet.h>
#include <EthernetUdp.h>

#include <ZscriptChannelBuilder.hpp>

EthernetUDP Udp;

uint8_t macAddress[6] = { 0xde, 0xad, 0xbe, 0xef, 0xfe, 0xad };

namespace Zscript {

template<class ZP>
class Zscript;

template<class ZP>
class ZscriptUdpChannel;

template<class ZP>
class AbstractOutStream;

template<class ZP>
class ZscriptUdpOutStream: public AbstractOutStream<ZP> {
private:
    bool openB = false;

public:
    ZscriptUdpOutStream() {
    }

    void open(uint8_t source) {
        ZscriptUdpChannel<ZP> *sourceCh = (ZscriptUdpChannel<ZP>*) Zscript<ZP>::zscript.getChannels()[source];
        Udp.beginPacket(sourceCh->getAddress(), sourceCh->getPort());
        openB = true;
    }

    bool isOpen() {
        return openB;
    }

    void close() {
        Udp.endPacket();
        openB = false;
    }

    void writeBytes(const uint8_t *bytes, uint16_t count, bool hexMode) {
        if (hexMode) {
            for (uint16_t i = 0; i < count; i++) {
                Udp.write(AbstractOutStream<ZP>::toHexChar(bytes[i] >> 4));
                Udp.write(AbstractOutStream<ZP>::toHexChar(bytes[i] & 0xf));
            }
        } else {
            Udp.write(bytes, count);
        }
    }

};
template<class ZP>
class ZscriptUdpManager {
    static uint16_t port;
    static bool hasLink;
    static bool hasMessageB;

public:
    static ZscriptUdpChannel<ZP> channels[ZP::udpChannelCount];

    static ZscriptUdpOutStream<ZP> out;

    static void setup() {
        port = ZP::udpLocalPort;
//        if (Ethernet.hardwareStatus() == EthernetNoHardware) {
//            return;
//        }
//        if (Ethernet.linkStatus() == LinkOFF) {
//            return;
//        }
        if (Ethernet.begin(macAddress)) {
            Udp.begin(port);
            hasLink = true;
        }
    }
    static void setup(uint16_t portI) {
        port = portI;
//        if (!Ethernet.begin(macAddress, 2, 2)) {
//            if (Ethernet.hardwareStatus() == EthernetNoHardware) {
//                return;
//            }
//        if (Ethernet.linkStatus() == LinkOFF) {
//                return;
//            }
//        }
        if (Ethernet.begin(macAddress)) {
            Udp.begin(port);
            hasLink = true;
        }
    }

    static bool hasMessage() {
        return hasMessageB;
    }
    static void messageOver() {
        hasMessageB = false;
    }
    static void reset() {
        Udp.flush();
        Udp.stop();
        hasLink = false;
//        if (!Ethernet.begin(macAddress, 2, 2)) {
//            if (Ethernet.hardwareStatus() == EthernetNoHardware) {
//                return;
//            }
//        if (Ethernet.linkStatus() == LinkOFF) {
//                return;
//            }
//        }
        if (Ethernet.begin(macAddress)) {
            Udp.begin(port);
            hasLink = true;
        }
    }

    static void moveAlong() {
        if (!hasLink) {
            if (Ethernet.hardwareStatus() == EthernetNoHardware) {
                return;
            }
            if (Ethernet.linkStatus() == LinkOFF) {
                return;
            }
            if (Ethernet.begin(macAddress)) {
                Udp.begin(ZP::udpLocalPort);
                hasLink = true;
            }
            return;
        }
        uint8_t result = Ethernet.maintain();
        if (result == 1 || result == 3) {
            hasLink = false;
            Udp.stop();
            return;
        }
        if (hasMessageB) {
            return;
        }
        uint16_t size = Udp.parsePacket();
        if (size != 0) {
            IPAddress remote = Udp.remoteIP();
            uint32_t currentTime = millis();
            for (uint8_t i = 0; i < ZP::udpChannelCount; ++i) {
                if (channels[i].matches()) {
                    channels[i].messageIncoming(currentTime);
                    hasMessageB = true;
                    return;
                }
            }
            for (uint8_t i = 0; i < ZP::udpChannelCount; ++i) {
                if (channels[i].isAvailable(currentTime)) {
                    channels[i].assign();
                    channels[i].messageIncoming(currentTime);
                    hasMessageB = true;
                    return;
                }
            }
            Udp.flush();
        }
    }
};

template<class ZP>
bool ZscriptUdpManager<ZP>::hasLink = false;
template<class ZP>
uint16_t ZscriptUdpManager<ZP>::port = ZP::udpLocalPort;
template<class ZP>
bool ZscriptUdpManager<ZP>::hasMessageB = false;
template<class ZP>
ZscriptUdpOutStream<ZP> ZscriptUdpManager<ZP>::out;
template<class ZP>
ZscriptUdpChannel<ZP> ZscriptUdpManager<ZP>::channels[ZP::udpChannelCount];

template<class ZP>
class ZscriptUdpChannel: public ZscriptChannel<ZP> {
    GenericCore::TokenRingBuffer<ZP> tBuffer;
    ZscriptTokenizer<ZP> tokenizer;

    uint32_t lastMilliAccess;
    uint32_t remoteAddr;
    uint16_t remotePort;
    uint8_t buffer[ZP::udpBufferSize];
    bool lastWasNL = false;

protected:
    bool resetInternal() {
        remotePort = 0;
        ZscriptUdpManager<ZP>::reset();
        return true;
    }

public:
    ZscriptUdpChannel() :
            ZscriptChannel<ZP>(&ZscriptUdpManager<ZP>::out, &tBuffer, true),
                    tBuffer(buffer, ZP::udpBufferSize),
                    tokenizer(tBuffer.getWriter(), 2),
                    lastMilliAccess(millis()), remoteAddr(), remotePort(0) {
    }

    IPAddress getAddress() {
        return remoteAddr;
    }
    uint16_t getPort() {
        return remotePort;
    }
    bool matches() {
        return remoteAddr == (uint32_t) Udp.remoteIP() && remotePort == Udp.remotePort();
    }
    bool isAvailable(uint32_t currentTime) {
        return (remotePort == 0) || (!this->parser.isActivated()) && currentTime - lastMilliAccess < ZP::nonActivatedChannelTimeout;
    }
    void messageIncoming(uint32_t currentTime) {
        lastMilliAccess = currentTime;
    }
    void assign() {
        remoteAddr = Udp.remoteIP();
        remotePort = Udp.remotePort();
    }

    void channelInfo(ZscriptCommandContext<ZP> ctx) {
        CommandOutStream<ZP> out = ctx.getOutStream();
        out.writeField('N', 0);
        uint32_t ipData = Ethernet.localIP();
        out.writeBigHex((uint8_t*) &ipData, 4);
        out.writeField('P', Udp.localPort());
        if (Ethernet.hardwareStatus() == EthernetNoHardware) {
            out.writeField('H', 0);
        } else if (Ethernet.linkStatus() == LinkOFF) {
            out.writeField('L', 0);
        }
    }

    void channelSetup(ZscriptCommandContext<ZP> ctx) {
        (void) ctx;
    }

    void moveAlong() {
        if (ZscriptUdpManager<ZP>::hasMessage()) {
            if (matches()) {
                int val;
                while ((val = Udp.peek()) != -1 && tokenizer.offer(val)) {
                    lastWasNL = val == '\n';
                    Udp.read();
                }
                if (val == -1 && (lastWasNL || tokenizer.offer('\n'))) {
                    ZscriptUdpManager<ZP>::messageOver();
                }
            }
        } else {
            ZscriptUdpManager<ZP>::moveAlong();
        }
    }

};

}

#endif /* SRC_MAIN_C___ARDUINO_UDP_MODULE_CHANNELS_ZSCRIPTUDPCHANNEL_HPP_ */
