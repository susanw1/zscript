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

ZcodeOutStream<ZcodeParameters>* EthernetUdpChannelOutStream::writeBytes(uint8_t const *value,
        ZcodeParameters::bigFieldAddress_t length) {
    channel->getUdp()->write(value, length);
    return this;
}

void EthernetUdpChannelOutStream::openResponse(ZcodeCommandChannel<ZcodeParameters> *target) {
    open = true;
    channel->getUdp()->beginPacket(channel->getIp(), channel->getPort());
}

void EthernetUdpChannelOutStream::openNotification(ZcodeCommandChannel<ZcodeParameters> *target) {
    open = true;
    channel->getUdp()->beginPacket(channel->getIp(), channel->getPort());
}
void EthernetUdpChannelOutStream::openDebug(ZcodeCommandChannel<ZcodeParameters> *target) {
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
