/*
 * RCodeOutStream.hpp
 *
 *  Created on: 7 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_RCODEOUTSTREAM_HPP_
#define SRC_TEST_CPP_RCODE_RCODEOUTSTREAM_HPP_
#include "RCodeIncludes.hpp"
#include "RCodeParameters.hpp"
#include "RCodeResponseStatus.hpp"

class RCodeCommandChannel;

class RCodeOutStream {
public:
    bool lockVal;
    void *mostRecent;

    bool lock() {
        if (!lockVal) {
            lockVal = true;
            return true;
        }
        return false;

    }

    void unlock() {
        lockVal = false;
    }

    bool isLocked() {
        return lockVal;
    }
    virtual RCodeOutStream* markNotification() = 0;

    virtual RCodeOutStream* markBroadcast() = 0;

    virtual RCodeOutStream* writeStatus(RCodeResponseStatus st) = 0;

    virtual RCodeOutStream* writeField(char f, uint8_t v) = 0;

    virtual RCodeOutStream* continueField(uint8_t v) = 0;

    virtual RCodeOutStream* writeBigHexField(uint8_t const *value,
            uint16_t length) = 0;

    virtual RCodeOutStream* writeBigStringField(uint8_t const *value,
            uint16_t length) = 0;

    virtual RCodeOutStream* writeBigStringField(char const *s) = 0;

    virtual RCodeOutStream* writeBytes(uint8_t const *value,
            uint16_t length) = 0;

    virtual RCodeOutStream* writeCommandSeperator() = 0;

    virtual RCodeOutStream* writeCommandSequenceSeperator() = 0;

    virtual void openResponse(RCodeCommandChannel *target) = 0;

    virtual void openNotification(RCodeCommandChannel *target) = 0;

    virtual void openDebug(RCodeCommandChannel *target) = 0;

    virtual bool isOpen() = 0;

    virtual void close() = 0;

    virtual ~RCodeOutStream() {

    }
};

#endif /* SRC_TEST_CPP_RCODE_RCODEOUTSTREAM_HPP_ */
