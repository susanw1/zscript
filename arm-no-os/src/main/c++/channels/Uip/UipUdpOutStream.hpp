/*
 * UipUdpOutStream.hpp
 *
 *  Created on: 10 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_CHANNELS_UIPUDPOUTSTREAM_HPP_
#define SRC_TEST_CPP_CHANNELS_UIPUDPOUTSTREAM_HPP_

#include <AbstractZcodeOutStream.hpp>
#include "UipUdpWrapper.hpp"

class UipUdpOutStream: public AbstractZcodeOutStream {
    private:
        UipUdpWriteWrapper write;
        bool open = false;

    public:
        UipUdpOutStream(UdpSocket *socket) :
                write(socket) {
        }

        virtual void writeByte(uint8_t value) {
            write.write(value);
        }

        virtual ZcodeOutStream* writeBytes(uint8_t const *value, uint16_t length) {
            write.write(value, length);
            return this;
        }

        virtual void openResponse(ZcodeCommandChannel *target);

        virtual void openNotification(ZcodeCommandChannel *target);

        virtual void openDebug(ZcodeCommandChannel *target);

        virtual bool isOpen() {
            return open;
        }

        virtual void close() {
            write.endPacket();
            open = false;
        }
};

#include "UipUdpCommandChannel.hpp"

#endif /* SRC_TEST_CPP_CHANNELS_UIPUDPOUTSTREAM_HPP_ */
