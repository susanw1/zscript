/*
 * SerialCommandChannel.hpp
 *
 *  Created on: 2 Aug 2021
 *      Author: robert
 */

#ifndef CHANNELS_SERIAL_SERIALCOMMANDCHANNEL_HPP_
#define CHANNELS_SERIAL_SERIALCOMMANDCHANNEL_HPP_
#include <stdint.h>
#include <stdlib.h>
#include <RCodeParameters.hpp>
#include <parsing/RCodeCommandChannel.hpp>
#include <parsing/RCodeCommandSequence.hpp>
#include "../../LowLevel/UartLowLevel/Serial.hpp"
#include "SerialChannelInStream.hpp"
#include "SerialChannelOutStream.hpp"

void SerialCommandChannelNewlineCallback(SerialIdentifier id);

class SerialCommandChannel: public RCodeCommandChannel<RCodeParameters> {
    static SerialCommandChannel *channel;
    Serial *serial;
    RCodeCommandSequence<RCodeParameters> seq;
    SerialChannelInStream in;
    SerialChannelOutStream out;
    bool hasNewline = false;

    friend SerialChannelInStream;
    friend SerialChannelLookahead;
    friend SerialChannelOutStream;
    friend void SerialCommandChannelNewlineCallback(SerialIdentifier);

    Serial* getSerial() {
        return serial;
    }
public:
    SerialCommandChannel(Serial *serial, RCode<RCodeParameters> *rcode) :
            serial(serial), seq(rcode, this), in(this), out(this) {
        channel = this;
        serial->init(NULL, 0, false);
        serial->setTargetValue(&SerialCommandChannelNewlineCallback, '\n');
    }

    virtual RCodeChannelInStream<RCodeParameters>* acquireInStream() {
        return &in;
    }

    virtual RCodeOutStream<RCodeParameters>* acquireOutStream() {
        return &out;
    }

    virtual bool hasCommandSequence() {
        return hasNewline;
    }

    virtual RCodeCommandSequence<RCodeParameters>* getCommandSequence() {
        return &seq;
    }

    virtual bool isPacketBased() {
        return false;
    }

    virtual void releaseInStream() {
        serial->skip(serial->getDistance('\n'));
        hasNewline = channel->getSerial()->getDistance('\n') >= 0;
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
#endif /* CHANNELS_SERIAL_SERIALCOMMANDCHANNEL_HPP_ */
