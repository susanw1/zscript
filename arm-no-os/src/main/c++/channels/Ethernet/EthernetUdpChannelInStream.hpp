/*
 * EthernetChannelInStream.hpp
 *
 *  Created on: 1 Jan 2021
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_CHANNELS_ETHERNET_ETHERNETUDPCHANNELINSTREAM_HPP_
#define SRC_TEST_CPP_CHANNELS_ETHERNET_ETHERNETUDPCHANNELINSTREAM_HPP_

#include <stdint.h>
#include <stdlib.h>
#include "../../LowLevel/ArduinoSpiLayer/src/Ethernet.h"
#include <instreams/ZcodeChannelInStream.hpp>
#include <ZcodeParameters.hpp>

class EthernetUdpCommandChannel;
class EthernetUdpChannelInStream;

class EthernetUdpChannelLookahead: public ZcodeLookaheadStream<ZcodeParameters> {
    EthernetUdpChannelInStream *parent;
    uint8_t dist = 0;
    friend EthernetUdpChannelInStream;
    public:
    EthernetUdpChannelLookahead(EthernetUdpChannelInStream *parent) :
            parent(parent) {
    }
    virtual char read();
};
class EthernetUdpChannelInStream: public ZcodeChannelInStream<ZcodeParameters> {
    EthernetUdpCommandChannel *channel;
    EthernetUdpChannelLookahead lookahead;
    friend EthernetUdpChannelLookahead;
    EthernetUdpCommandChannel* getChannel() {
        return channel;
    }

public:
    EthernetUdpChannelInStream(EthernetUdpCommandChannel *channel) :
            channel(channel), lookahead(this) {
    }
    virtual int16_t read();

    virtual ZcodeLookaheadStream<ZcodeParameters>* getLookahead() {
        lookahead.dist = 0;
        return &lookahead;
    }
};

#include "EthernetUdpCommandChannel.hpp"

#endif /* SRC_TEST_CPP_CHANNELS_ETHERNET_ETHERNETUDPCHANNELINSTREAM_HPP_ */
