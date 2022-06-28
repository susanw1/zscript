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
private:
    typedef typename ZP::bigFieldAddress_t bigFieldAddress_t;
    typedef typename ZP::fieldUnit_t fieldUnit_t;

    ZcodeNotificationManager<ZP> *notificationManager;
    ZcodeOutStream<ZP> *out = NULL;

public:
    ZcodeInterruptVectorOut(ZcodeNotificationManager<ZP> *notificationManager) :
            notificationManager(notificationManager) {
    }

    void writeByte(uint8_t value) {
        out->writeByte(value);
    }

    void open(ZcodeCommandChannel<ZP> *target, ZcodeOutStreamOpenType type);

    bool isOpen() {
        return out != NULL && out->isOpen() && out->mostRecent == this;
    }

    void close() {
        out->close();
    }

    bool lock();

    bool isLocked();

    void unlock();
};

template<class ZP>
void ZcodeInterruptVectorOut<ZP>::open(ZcodeCommandChannel<ZP> *target, ZcodeOutStreamOpenType type) {
    if (type == RESPONSE) {
        ZcodeInterruptVectorChannel<ZP> *channel = (ZcodeInterruptVectorChannel<ZP>*) target;
        if (out->isOpen()) {
            out->close();
        }
        out->mostRecent = this;
        out->openNotification(notificationManager->getNotificationChannel());
        out->markNotification();
        out->writeField(Zchars::NOTIFY_TYPE_PARAM, 1);
        out->writeField('A', 1);
        out->writeField('T', channel->getInterrupt()->getNotificationType());
        out->writeField('I', channel->getInterrupt()->getNotificationBus());
        out->writeStatus(OK);
        if (channel->getInterrupt()->hasFindableAddress()) {
            out->writeCommandSeparator();
            out->writeField('A', channel->getInterrupt()->getFoundAddress());
            out->writeStatus(OK);
        }
        out->writeCommandSeparator();
        channel->clear();
    }
}

template<class ZP>
bool ZcodeInterruptVectorOut<ZP>::lock() {
    if (out == NULL) {
        out = notificationManager->getNotificationChannel()->out;
    }
    return out->lock();
}

template<class ZP>
bool ZcodeInterruptVectorOut<ZP>::isLocked() {
    if (out == NULL) {
        out = notificationManager->getNotificationChannel()->out;
    }
    return out->isLocked();
}

template<class ZP>
void ZcodeInterruptVectorOut<ZP>::unlock() {
    out->unlock();
    out = NULL;
}

#include "../scriptspace/ZcodeInterruptVectorChannel.hpp"

#endif /* SRC_MAIN_CPP_ZCODE_EXECUTIONSPACE_ZCODEINTERRUPTVECTOROUT_HPP_ */
