/*
 * ZcodeInterruptVectorOut.hpp
 *
 *  Created on: 9 Dec 2020
 *      Author: robert
 */

#ifndef SRC_MAIN_CPP_ZCODE_EXECUTIONSPACE_ZCODEINTERRUPTVECTOROUT_HPP_
#define SRC_MAIN_CPP_ZCODE_EXECUTIONSPACE_ZCODEINTERRUPTVECTOROUT_HPP_
#include "../ZcodeIncludes.hpp"
#include "../ZcodeOutStream.hpp"

template<class RP>
class ZcodeNotificationManager;

template<class RP>
class ZcodeInterruptVectorChannel;

template<class RP>
class ZcodeInterruptVectorOut: public ZcodeOutStream<RP> {
    typedef typename RP::bigFieldAddress_t bigFieldAddress_t;
    typedef typename RP::fieldUnit_t fieldUnit_t;
private:
    ZcodeNotificationManager<RP> *notificationManager;
    ZcodeOutStream<RP> *out = NULL;

public:
    ZcodeInterruptVectorOut(ZcodeNotificationManager<RP> *notificationManager) :
            notificationManager(notificationManager) {
    }

    ZcodeOutStream<RP>* markDebug() {
        return out->markDebug();
    }

    ZcodeOutStream<RP>* markNotification() {
        return out->markNotification();
    }

    ZcodeOutStream<RP>* markBroadcast() {
        return out->markBroadcast();
    }

    ZcodeOutStream<RP>* writeStatus(ZcodeResponseStatus st) {
        return out->writeStatus(st);
    }

    ZcodeOutStream<RP>* writeField(char f, fieldUnit_t v) {
        return out->writeField(f, v);
    }

    ZcodeOutStream<RP>* continueField(fieldUnit_t v) {
        return out->continueField(v);
    }

    ZcodeOutStream<RP>* writeBigHexField(uint8_t const *value, bigFieldAddress_t length) {
        return out->writeBigHexField(value, length);
    }

    ZcodeOutStream<RP>* writeBigStringField(uint8_t const *value, bigFieldAddress_t length) {
        return out->writeBigStringField(value, length);
    }

    ZcodeOutStream<RP>* writeBigStringField(char const *s) {
        return out->writeBigStringField(s);
    }

    ZcodeOutStream<RP>* writeBytes(uint8_t const *value, bigFieldAddress_t length) {
        return out->writeBytes(value, length);
    }

    ZcodeOutStream<RP>* writeCommandSeperator() {
        return out->writeCommandSeperator();
    }

    ZcodeOutStream<RP>* writeCommandSequenceSeperator() {
        return out->writeCommandSequenceSeperator();
    }

    ZcodeOutStream<RP>* writeCommandSequenceErrorHandler() {
        return out->writeCommandSequenceErrorHandler();
    }

    void openResponse(ZcodeCommandChannel<RP> *target);

    void openNotification(ZcodeCommandChannel<RP> *target) {
    }

    void openDebug(ZcodeCommandChannel<RP> *target) {
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
void ZcodeInterruptVectorOut<RP>::openResponse(ZcodeCommandChannel<RP> *target) {
    ZcodeInterruptVectorChannel<RP> *channel = (ZcodeInterruptVectorChannel<RP>*) target;
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
bool ZcodeInterruptVectorOut<RP>::lock() {
    if (out == NULL) {
        out = notificationManager->getNotificationChannel()->acquireOutStream();
    }
    return out->lock();
}

template<class RP>
bool ZcodeInterruptVectorOut<RP>::isLocked() {
    if (out == NULL) {
        out = notificationManager->getNotificationChannel()->acquireOutStream();
    }
    return out->isLocked();
}

template<class RP>
void ZcodeInterruptVectorOut<RP>::unlock() {
    out->unlock();
    notificationManager->getNotificationChannel()->releaseOutStream();
    out = NULL;
}

#include "ZcodeInterruptVectorChannel.hpp"

#endif /* SRC_MAIN_CPP_ZCODE_EXECUTIONSPACE_ZCODEINTERRUPTVECTOROUT_HPP_ */
