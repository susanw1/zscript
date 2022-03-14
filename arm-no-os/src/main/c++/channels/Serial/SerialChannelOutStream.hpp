/*
 * SerialChannelOutStream.hpp
 *
 *  Created on: 2 Aug 2021
 *      Author: robert
 */

#ifndef CHANNELS_SERIAL_SERIALCHANNELOUTSTREAM_HPP_
#define CHANNELS_SERIAL_SERIALCHANNELOUTSTREAM_HPP_
#include <stdint.h>
#include <stdlib.h>
#include <ZcodeParameters.hpp>
#include <AbstractZcodeOutStream.hpp>

class SerialCommandChannel;

class SerialChannelOutStream: public AbstractZcodeOutStream<ZcodeParameters> {
    SerialCommandChannel *channel;
    bool open = false;

public:
    SerialChannelOutStream(SerialCommandChannel *channel) :
            channel(channel) {
        unlock();
        this->mostRecent = NULL;
    }

    virtual void writeByte(uint8_t value);

    virtual ZcodeOutStream<ZcodeParameters>* writeBytes(uint8_t const *value,
            ZcodeParameters::bigFieldAddress_t length);

    virtual void openResponse(ZcodeCommandChannel<ZcodeParameters> *target) {
        open = true;
    }
    virtual void openNotification(ZcodeCommandChannel<ZcodeParameters> *target) {
        open = true;
    }
    virtual void openDebug(ZcodeCommandChannel<ZcodeParameters> *target) {
        open = true;
    }

    virtual bool isOpen() {
        return open;
    }

    virtual void close();
};
#include "SerialCommandChannel.hpp"

#endif /* CHANNELS_SERIAL_SERIALCHANNELOUTSTREAM_HPP_ */
