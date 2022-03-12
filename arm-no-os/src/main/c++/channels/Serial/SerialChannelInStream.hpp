/*
 * SerialChannelInStream.hpp
 *
 *  Created on: 2 Aug 2021
 *      Author: robert
 */

#ifndef CHANNELS_SERIAL_SERIALCHANNELINSTREAM_HPP_
#define CHANNELS_SERIAL_SERIALCHANNELINSTREAM_HPP_
#include <stdint.h>
#include <stdlib.h>
#include "../../LowLevel/UartLowLevel/Serial.hpp"
#include <instreams/ZcodeChannelInStream.hpp>
#include <ZcodeParameters.hpp>

class SerialCommandChannel;
class SerialChannelInStream;

class SerialChannelLookahead: public ZcodeLookaheadStream<ZcodeParameters> {
    SerialChannelInStream *parent;
    friend SerialChannelInStream;
    public:
    SerialChannelLookahead(SerialChannelInStream *parent) :
            parent(parent) {
    }
    virtual char read();
};
class SerialChannelInStream: public ZcodeChannelInStream<ZcodeParameters> {
    SerialCommandChannel *channel;
    SerialChannelLookahead lookahead;
    friend SerialChannelLookahead;
    SerialCommandChannel* getChannel() {
        return channel;
    }

public:
    SerialChannelInStream(SerialCommandChannel *channel) :
            channel(channel), lookahead(this) {
    }
    virtual int16_t read();

    virtual ZcodeLookaheadStream<ZcodeParameters>* getLookahead();
};
#include "SerialCommandChannel.hpp"

#endif /* CHANNELS_SERIAL_SERIALCHANNELINSTREAM_HPP_ */
