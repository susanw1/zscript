/*
 * EthernetChannelOutStream.cpp
 *
 *  Created on: 1 Jan 2021
 *      Author: robert
 */

#include "EthernetUdpChannelOutStream.hpp"

void EthernetUdpChannelOutStream::writeByte(uint8_t value) {
    channel->getUdp()->write(value);
}

RCodeOutStream<RCodeParameters>* EthernetUdpChannelOutStream::writeBytes(uint8_t const *value,
        RCodeParameters::bigFieldAddress_t length) {
    channel->getUdp()->write(value, length);
    return this;
}

void EthernetUdpChannelOutStream::openResponse(RCodeCommandChannel<RCodeParameters> *target) {
    open = true;
    channel->getUdp()->beginPacket(channel->getIp(), channel->getPort());
}

void EthernetUdpChannelOutStream::openNotification(RCodeCommandChannel<RCodeParameters> *target) {
    open = true;
    channel->getUdp()->beginPacket(channel->getIp(), channel->getPort());
}
void EthernetUdpChannelOutStream::openDebug(RCodeCommandChannel<RCodeParameters> *target) {
    open = true;
    channel->getUdp()->beginPacket(channel->getIp(), channel->getPort());
}

bool EthernetUdpChannelOutStream::isOpen() {
    return open;
}

void EthernetUdpChannelOutStream::close() {
    open = false;
    channel->getUdp()->endPacket();
}
