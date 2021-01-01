/*
 * RCodeInterruptVectorOut.hpp
 *
 *  Created on: 9 Dec 2020
 *      Author: robert
 */

#ifndef SRC_MAIN_CPP_RCODE_EXECUTIONSPACE_RCODEINTERRUPTVECTOROUT_HPP_
#define SRC_MAIN_CPP_RCODE_EXECUTIONSPACE_RCODEINTERRUPTVECTOROUT_HPP_
#include "../RCodeIncludes.hpp"
#include "RCodeParameters.hpp"
#include "../RCodeOutStream.hpp"

class RCodeNotificationManager;
class RCodeInterruptVectorChannel;

class RCodeInterruptVectorOut: public RCodeOutStream {
private:
    RCodeNotificationManager *notificationManager;
    RCodeOutStream *out = NULL;

public:
    RCodeInterruptVectorOut(RCodeNotificationManager *notificationManager) :
            notificationManager(notificationManager) {
    }

    RCodeOutStream* markDebug() {
        return out->markDebug();
    }

    RCodeOutStream* markNotification() {
        return out->markNotification();
    }

    RCodeOutStream* markBroadcast() {
        return out->markBroadcast();
    }

    RCodeOutStream* writeStatus(RCodeResponseStatus st) {
        return out->writeStatus(st);
    }

    RCodeOutStream* writeField(char f, fieldUnit v) {
        return out->writeField(f, v);
    }

    RCodeOutStream* continueField(fieldUnit v) {
        return out->continueField(v);
    }

    RCodeOutStream* writeBigHexField(uint8_t const *value,
            bigFieldAddress_t length) {
        return out->writeBigHexField(value, length);
    }

    RCodeOutStream* writeBigStringField(uint8_t const *value,
            bigFieldAddress_t length) {
        return out->writeBigStringField(value, length);
    }

    RCodeOutStream* writeBigStringField(char const *s) {
        return out->writeBigStringField(s);
    }

    RCodeOutStream* writeBytes(uint8_t const *value, bigFieldAddress_t length) {
        return out->writeBytes(value, length);
    }

    RCodeOutStream* writeCommandSeperator() {
        return out->writeCommandSeperator();
    }

    RCodeOutStream* writeCommandSequenceSeperator() {
        return out->writeCommandSequenceSeperator();
    }

    RCodeOutStream* writeCommandSequenceErrorHandler() {
        return out->writeCommandSequenceErrorHandler();
    }

    void openResponse(RCodeCommandChannel *target);

    void openNotification(RCodeCommandChannel *target) {
    }

    void openDebug(RCodeCommandChannel *target) {
    }

    bool isOpen() {
        return out->isOpen() && out->mostRecent == this;
    }

    void close() {
        out->close();
    }

    bool lock();

    bool isLocked();

    void unlock();

};
#include "RCodeInterruptVectorChannel.hpp"

#endif /* SRC_MAIN_CPP_RCODE_EXECUTIONSPACE_RCODEINTERRUPTVECTOROUT_HPP_ */
