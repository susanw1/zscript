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
#include <ZcodeParameters.hpp>
#include <parsing/ZcodeCommandChannel.hpp>
#include <parsing/ZcodeCommandSequence.hpp>
#include "../../LowLevel/UartLowLevel/Serial.hpp"
#include "SerialChannelInStream.hpp"
#include "SerialChannelOutStream.hpp"

void SerialCommandChannelNewlineCallback(SerialIdentifier id);

class SerialCommandChannel: public ZcodeCommandChannel<ZcodeParameters> {
    private:
        static SerialCommandChannel *channel;
        Serial *serial;
        ZcodeCommandSequence<ZcodeParameters> seq;
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
        SerialCommandChannel(Serial *serial, Zcode<ZcodeParameters> *zcode) :
                serial(serial), seq(zcode, this), in(this), out(this) {
            channel = this;
            serial->init(NULL, 0, false);
            serial->setTargetValue(&SerialCommandChannelNewlineCallback, Zchars::EOL_SYMBOL);
        }

        virtual ZcodeChannelInStream<ZcodeParameters>* acquireInStream() {
            return &in;
        }

        virtual ZcodeOutStream<ZcodeParameters>* acquireOutStream() {
            return &out;
        }

        virtual bool hasCommandSequence() {
            return hasNewline;
        }

        virtual ZcodeCommandSequence<ZcodeParameters>* getCommandSequence() {
            return &seq;
        }

        virtual bool isPacketBased() {
            return false;
        }

        virtual void releaseInStream() {
            serial->skip(serial->getDistance(Zchars::EOL_SYMBOL));
            hasNewline = channel->getSerial()->getDistance(Zchars::EOL_SYMBOL) >= 0;
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
