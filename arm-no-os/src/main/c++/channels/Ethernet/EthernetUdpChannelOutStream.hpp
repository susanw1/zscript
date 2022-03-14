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
#include <ZcodeParameters.hpp>
#include <AbstractZcodeOutStream.hpp>

class EthernetUdpCommandChannel;

class EthernetUdpChannelOutStream: public AbstractZcodeOutStream<ZcodeParameters> {
    EthernetUdpCommandChannel *channel;
    bool open = false;

public:
    EthernetUdpChannelOutStream(EthernetUdpCommandChannel *channel) :
            channel(channel) {
        unlock();
        this->mostRecent = NULL;
    }

    virtual void writeByte(uint8_t value);

    virtual ZcodeOutStream<ZcodeParameters>* writeBytes(uint8_t const *value,
            ZcodeParameters::bigFieldAddress_t length);

    virtual void openResponse(ZcodeCommandChannel<ZcodeParameters> *target);
    virtual void openNotification(ZcodeCommandChannel<ZcodeParameters> *target);
    virtual void openDebug(ZcodeCommandChannel<ZcodeParameters> *target);

    virtual bool isOpen();

    virtual void close();
};

#include "EthernetUdpCommandChannel.hpp"

#endif /* SRC_TEST_CPP_CHANNELS_ETHERNET_ETHERNETUDPCHANNELOUTSTREAM_HPP_ */
