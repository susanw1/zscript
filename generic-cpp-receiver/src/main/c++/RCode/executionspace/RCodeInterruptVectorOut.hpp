/*
 * RCodeInterruptVectorOut.hpp
 *
 *  Created on: 9 Dec 2020
 *      Author: robert
 */

#ifndef SRC_MAIN_CPP_RCODE_EXECUTIONSPACE_RCODEINTERRUPTVECTOROUT_HPP_
#define SRC_MAIN_CPP_RCODE_EXECUTIONSPACE_RCODEINTERRUPTVECTOROUT_HPP_
#include "../RCodeIncludes.hpp"
#include "../RCodeOutStream.hpp"

template<class RP>
class RCodeNotificationManager;

template<class RP>
class RCodeInterruptVectorChannel;

template<class RP>
class RCodeInterruptVectorOut: public RCodeOutStream<RP> {
    typedef typename RP::bigFieldAddress_t bigFieldAddress_t;
    typedef typename RP::fieldUnit_t fieldUnit_t;
private:
    RCodeNotificationManager<RP> *notificationManager;
    RCodeOutStream<RP> *out = NULL;

public:
    RCodeInterruptVectorOut(RCodeNotificationManager<RP> *notificationManager) :
            notificationManager(notificationManager) {
    }

    RCodeOutStream<RP>* markDebug() {
        return out->markDebug();
    }

    RCodeOutStream<RP>* markNotification() {
        return out->markNotification();
    }

    RCodeOutStream<RP>* markBroadcast() {
        return out->markBroadcast();
    }

    RCodeOutStream<RP>* writeStatus(RCodeResponseStatus st) {
        return out->writeStatus(st);
    }

    RCodeOutStream<RP>* writeField(char f, fieldUnit_t v) {
        return out->writeField(f, v);
    }

    RCodeOutStream<RP>* continueField(fieldUnit_t v) {
        return out->continueField(v);
    }

    RCodeOutStream<RP>* writeBigHexField(uint8_t const *value, bigFieldAddress_t length) {
        return out->writeBigHexField(value, length);
    }

    RCodeOutStream<RP>* writeBigStringField(uint8_t const *value, bigFieldAddress_t length) {
        return out->writeBigStringField(value, length);
    }

    RCodeOutStream<RP>* writeBigStringField(char const *s) {
        return out->writeBigStringField(s);
    }

    RCodeOutStream<RP>* writeBytes(uint8_t const *value, bigFieldAddress_t length) {
        return out->writeBytes(value, length);
    }

    RCodeOutStream<RP>* writeCommandSeperator() {
        return out->writeCommandSeperator();
    }

    RCodeOutStream<RP>* writeCommandSequenceSeperator() {
        return out->writeCommandSequenceSeperator();
    }

    RCodeOutStream<RP>* writeCommandSequenceErrorHandler() {
        return out->writeCommandSequenceErrorHandler();
    }

    void openResponse(RCodeCommandChannel<RP> *target);

    void openNotification(RCodeCommandChannel<RP> *target) {
    }

    void openDebug(RCodeCommandChannel<RP> *target) {
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

template<class RP>
void RCodeInterruptVectorOut<RP>::openResponse(RCodeCommandChannel<RP> *target) {
    RCodeInterruptVectorChannel<RP> *channel = (RCodeInterruptVectorChannel<RP>*) target;
    if (out->isOpen()) {
        out->close();
    }
    out->mostRecent = this;
    out->openNotification(notificationManager->getNotificationChannel());
    out->markNotification();
    out->writeField('Z', 1);
    out->writeField('A', 1);
    out->writeField('T', channel->getInterrupt()->getNotificationType());
    out->writeField('I', channel->getInterrupt()->getNotificationBus());
    out->writeStatus(OK);
    if (RP::findInterruptSourceAddress && channel->getInterrupt()->getSource()->hasAddress()) {
        out->writeCommandSeperator();
        out->writeField('A', channel->getInterrupt()->getFoundAddress());
        if (channel->getInterrupt()->hasFindableAddress()) {
            out->writeStatus(OK);
        } else {
            out->writeStatus(CMD_FAIL);
        }
    }
    out->writeCommandSeperator();
    channel->getInterrupt()->clear();
}

template<class RP>
bool RCodeInterruptVectorOut<RP>::lock() {
    if (out == NULL) {
        out = notificationManager->getNotificationChannel()->acquireOutStream();
    }
    return out->lock();
}

template<class RP>
bool RCodeInterruptVectorOut<RP>::isLocked() {
    if (out == NULL) {
        out = notificationManager->getNotificationChannel()->acquireOutStream();
    }
    return out->isLocked();
}

template<class RP>
void RCodeInterruptVectorOut<RP>::unlock() {
    out->unlock();
    notificationManager->getNotificationChannel()->releaseOutStream();
    out = NULL;
}

#include "RCodeInterruptVectorChannel.hpp"

#endif /* SRC_MAIN_CPP_RCODE_EXECUTIONSPACE_RCODEINTERRUPTVECTOROUT_HPP_ */
