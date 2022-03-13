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

template<class ZP>
class ZcodeNotificationManager;

template<class ZP>
class ZcodeInterruptVectorChannel;

template<class ZP>
class ZcodeInterruptVectorOut: public ZcodeOutStream<ZP> {
    typedef typename ZP::bigFieldAddress_t bigFieldAddress_t;
    typedef typename ZP::fieldUnit_t fieldUnit_t;
private:
    ZcodeNotificationManager<ZP> *notificationManager;
    ZcodeOutStream<ZP> *out = NULL;

public:
    ZcodeInterruptVectorOut(ZcodeNotificationManager<ZP> *notificationManager) :
            notificationManager(notificationManager) {
    }

    ZcodeOutStream<ZP>* markDebug() {
        return out->markDebug();
    }

    ZcodeOutStream<ZP>* markNotification() {
        return out->markNotification();
    }

    ZcodeOutStream<ZP>* markBroadcast() {
        return out->markBroadcast();
    }

    ZcodeOutStream<ZP>* writeStatus(ZcodeResponseStatus st) {
        return out->writeStatus(st);
    }

    ZcodeOutStream<ZP>* writeField(char f, fieldUnit_t v) {
        return out->writeField(f, v);
    }

    ZcodeOutStream<ZP>* continueField(fieldUnit_t v) {
        return out->continueField(v);
    }

    ZcodeOutStream<ZP>* writeBigHexField(uint8_t const *value, bigFieldAddress_t length) {
        return out->writeBigHexField(value, length);
    }

    ZcodeOutStream<ZP>* writeBigStringField(uint8_t const *value, bigFieldAddress_t length) {
        return out->writeBigStringField(value, length);
    }

    ZcodeOutStream<ZP>* writeBigStringField(char const *s) {
        return out->writeBigStringField(s);
    }

    ZcodeOutStream<ZP>* writeBytes(uint8_t const *value, bigFieldAddress_t length) {
        return out->writeBytes(value, length);
    }

    ZcodeOutStream<ZP>* writeCommandSeperator() {
        return out->writeCommandSeperator();
    }

    ZcodeOutStream<ZP>* writeCommandSequenceSeperator() {
        return out->writeCommandSequenceSeperator();
    }

    ZcodeOutStream<ZP>* writeCommandSequenceErrorHandler() {
        return out->writeCommandSequenceErrorHandler();
    }

    void openResponse(ZcodeCommandChannel<ZP> *target);

    void openNotification(ZcodeCommandChannel<ZP> *target) {
    }

    void openDebug(ZcodeCommandChannel<ZP> *target) {
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

template<class ZP>
void ZcodeInterruptVectorOut<ZP>::openResponse(ZcodeCommandChannel<ZP> *target) {
    ZcodeInterruptVectorChannel<ZP> *channel = (ZcodeInterruptVectorChannel<ZP>*) target;
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
    if (ZP::findInterruptSourceAddress && channel->getInterrupt()->getSource()->hasAddress()) {
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

template<class ZP>
bool ZcodeInterruptVectorOut<ZP>::lock() {
    if (out == NULL) {
        out = notificationManager->getNotificationChannel()->acquireOutStream();
    }
    return out->lock();
}

template<class ZP>
bool ZcodeInterruptVectorOut<ZP>::isLocked() {
    if (out == NULL) {
        out = notificationManager->getNotificationChannel()->acquireOutStream();
    }
    return out->isLocked();
}

template<class ZP>
void ZcodeInterruptVectorOut<ZP>::unlock() {
    out->unlock();
    notificationManager->getNotificationChannel()->releaseOutStream();
    out = NULL;
}

#include "ZcodeInterruptVectorChannel.hpp"

#endif /* SRC_MAIN_CPP_ZCODE_EXECUTIONSPACE_ZCODEINTERRUPTVECTOROUT_HPP_ */
