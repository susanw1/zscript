/*
 * EthernetChannelOutStream.hpp
 *
 *  Created on: 1 Jan 2021
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_CHANNELS_ETHERNET_ETHERNETUDPCHANNELOUTSTREAM_HPP_
#define SRC_TEST_CPP_CHANNELS_ETHERNET_ETHERNETUDPCHANNELOUTSTREAM_HPP_
#include <stdint.h>
#include <stdlib.h>
#include <RCodeParameters.hpp>
#include <AbstractRCodeOutStream.hpp>

class EthernetUdpCommandChannel;

class EthernetUdpChannelOutStream: public AbstractRCodeOutStream<RCodeParameters> {
    EthernetUdpCommandChannel *channel;
    bool open = false;

public:
    EthernetUdpChannelOutStream(EthernetUdpCommandChannel *channel) :
            channel(channel) {
        unlock();
        this->mostRecent = NULL;
    }

    virtual void writeByte(uint8_t value);

    virtual RCodeOutStream<RCodeParameters>* writeBytes(uint8_t const *value,
            RCodeParameters::bigFieldAddress_t length);

    virtual void openResponse(RCodeCommandChannel<RCodeParameters> *target);
    virtual void openNotification(RCodeCommandChannel<RCodeParameters> *target);
    virtual void openDebug(RCodeCommandChannel<RCodeParameters> *target);

    virtual bool isOpen();

    virtual void close();
};

#include "EthernetUdpCommandChannel.hpp"

#endif /* SRC_TEST_CPP_CHANNELS_ETHERNET_ETHERNETUDPCHANNELOUTSTREAM_HPP_ */
