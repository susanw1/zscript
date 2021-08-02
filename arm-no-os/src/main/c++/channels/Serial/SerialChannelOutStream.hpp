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
#include <RCodeParameters.hpp>
#include <AbstractRCodeOutStream.hpp>

class SerialCommandChannel;

class SerialChannelOutStream: public AbstractRCodeOutStream<RCodeParameters> {
    SerialCommandChannel *channel;
    bool open = false;

public:
    SerialChannelOutStream(SerialCommandChannel *channel) :
            channel(channel) {
        unlock();
        this->mostRecent = NULL;
    }

    virtual void writeByte(uint8_t value);

    virtual RCodeOutStream<RCodeParameters>* writeBytes(uint8_t const *value,
            RCodeParameters::bigFieldAddress_t length);

    virtual void openResponse(RCodeCommandChannel<RCodeParameters> *target) {
        open = true;
    }
    virtual void openNotification(RCodeCommandChannel<RCodeParameters> *target) {
        open = true;
    }
    virtual void openDebug(RCodeCommandChannel<RCodeParameters> *target) {
        open = true;
    }

    virtual bool isOpen() {
        return open;
    }

    virtual void close();
};
#include "SerialCommandChannel.hpp"

#endif /* CHANNELS_SERIAL_SERIALCHANNELOUTSTREAM_HPP_ */
