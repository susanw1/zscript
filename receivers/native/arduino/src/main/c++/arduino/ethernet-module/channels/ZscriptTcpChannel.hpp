/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ARDUINO_TCP_MODULE_CHANNELS_ZSCRIPTTCPCHANNEL_HPP_
#define SRC_MAIN_C___ARDUINO_TCP_MODULE_CHANNELS_ZSCRIPTTCPCHANNEL_HPP_

#include "../ZscriptEthernetSystem.hpp"
#include <ZscriptChannelBuilder.hpp>

namespace Zscript {
EthernetServer server(23);

template<class ZP>
class Zscript;

template<class ZP>
class ZscriptUdpChannel;

template<class ZP>
class AbstractOutStream;

template<class ZP>
class ZscriptTcpOutStream: public AbstractOutStream<ZP> {
    EthernetClient *client;
    bool openB = false;

public:
    ZscriptTcpOutStream(EthernetClient *client) :
            client(client) {
    }

    void open(uint8_t source) {
        (void) source;
        openB = true;
    }

    bool isOpen() {
        return openB;
    }

    void close() {
        openB = false;
    }

    void writeBytes(const uint8_t *bytes, uint16_t count, bool hexMode) {
        if (hexMode) {
            for (uint16_t i = 0; i < count; i++) {
                client->write(AbstractOutStream<ZP>::toHexChar(bytes[i] >> 4));
                client->write(AbstractOutStream<ZP>::toHexChar(bytes[i] & 0xf));
            }
        } else {
            client->write(bytes, count);
        }
    }
};

template<class ZP>
class ZscriptTcpChannel: public ZscriptChannel<ZP> {
    GenericCore::TokenRingBuffer<ZP> tBuffer;
    ZscriptTokenizer<ZP> tokenizer;
    ZscriptTcpOutStream<ZP> out;

    EthernetClient client;
    uint8_t buffer[ZP::udpBufferSize];

protected:
    bool resetInternal() {
        client.stop();
        return true;
    }

public:
    ZscriptTcpChannel() :
            ZscriptChannel<ZP>(&out, &tBuffer, true),
                    tBuffer(buffer, ZP::udpBufferSize),
                    tokenizer(tBuffer.getWriter(), 2),
                    out(&client) {
    }

    void channelInfo(ZscriptCommandContext<ZP> ctx) {
        CommandOutStream < ZP > out = ctx.getOutStream();
        out.writeField('N', 0);
        uint32_t ipData = Ethernet.localIP();
        out.writeBigHex((uint8_t*) &ipData, 4);
        out.writeField('P', client.localPort());
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
        if (client.connected()) {
            int val;
            while ((val = client.peek()) != -1 && tokenizer.offer(val)) {
                client.read();
            }
        } else {
            client = server.available();
        }
    }

};

}

#endif /* SRC_MAIN_C___ARDUINO_TCP_MODULE_CHANNELS_ZSCRIPTTCPCHANNEL_HPP_ */
