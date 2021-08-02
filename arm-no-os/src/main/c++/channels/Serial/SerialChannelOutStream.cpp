/*
 * SerialChannelOutStream.cpp
 *
 *  Created on: 2 Aug 2021
 *      Author: robert
 */

#include "SerialChannelOutStream.hpp"

void SerialChannelOutStream::close() {
    open = false;
    channel->getSerial()->transmitWriteBuffer();
}
void SerialChannelOutStream::writeByte(uint8_t value) {
    channel->getSerial()->write(value);
}
RCodeOutStream<RCodeParameters>* SerialChannelOutStream::writeBytes(uint8_t const *value,
        RCodeParameters::bigFieldAddress_t length) {
    channel->getSerial()->write(value, length);
    return this;
}
