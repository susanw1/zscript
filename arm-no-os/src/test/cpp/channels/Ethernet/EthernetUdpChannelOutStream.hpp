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
#include <AbstractRCodeOutStream.hpp>

class EthernetUdpCommandChannel;

class EthernetUdpChannelOutStream: public AbstractRCodeOutStream {
    EthernetUdpCommandChannel *channel;
    bool open = false;

public:
    EthernetUdpChannelOutStream(EthernetUdpCommandChannel *channel) :
            channel(channel) {
    }

    virtual void writeByte(uint8_t value);

    virtual RCodeOutStream* writeBytes(uint8_t const *value,
            bigFieldAddress_t length);

    virtual void openResponse(RCodeCommandChannel *target);
    virtual void openNotification(RCodeCommandChannel *target);
    virtual void openDebug(RCodeCommandChannel *target);

    virtual bool isOpen();

    virtual void close();
};

#include "EthernetUdpCommandChannel.hpp"

#endif /* SRC_TEST_CPP_CHANNELS_ETHERNET_ETHERNETUDPCHANNELOUTSTREAM_HPP_ */
