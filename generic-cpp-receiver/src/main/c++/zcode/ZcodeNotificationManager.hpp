/*
 * ZcodeNotificationManager.hpp
 *
 *  Created on: 28 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_ZCODENOTIFICATIONMANAGER_HPP_
#define SRC_TEST_CPP_ZCODE_ZCODENOTIFICATIONMANAGER_HPP_
#include "ZcodeIncludes.hpp"
#include "ZcodeBusInterruptSource.hpp"
#include "ZcodeBusInterrupt.hpp"

template<class ZP>
class ZcodeBusInterruptSource;

template<class ZP>
class ZcodeBusInterrupt;

template<class ZP>
class ZcodeInterruptVectorManager;

template<class ZP>
class ZcodeCommandChannel;

template<class ZP>
class Zcode;

template<class ZP>
class ZcodeNotificationManager {
private:
    ZcodeBusInterruptSource<ZP> **sources;
    ZcodeBusInterrupt<ZP> waitingNotifications[ZP::interruptStoreNum];
    uint8_t waitingNotificationNumber = 0;
    uint8_t sourceNum = 0;
    ZcodeInterruptVectorManager<ZP> *vectorChannel;
    ZcodeCommandChannel<ZP> *notificationChannel;

    bool canSendNotification();

    void sendNotification(ZcodeBusInterrupt<ZP> interrupt);

public:
    ZcodeNotificationManager(Zcode<ZP> *zcode, ZcodeBusInterruptSource<ZP> **sources, uint8_t sourceNum);

    void setVectorChannel(ZcodeInterruptVectorManager<ZP> *vectorChannel) {
        this->vectorChannel = vectorChannel;
    }

    void setNotificationChannel(ZcodeCommandChannel<ZP> *notificationChannel);

    ZcodeCommandChannel<ZP>* getNotificationChannel() {
        return notificationChannel;
    }

    ZcodeInterruptVectorManager<ZP>* getVectorChannel() {
        return vectorChannel;
    }

    void manageNotifications();
};

template<class ZP>
ZcodeNotificationManager<ZP>::ZcodeNotificationManager(Zcode<ZP> *zcode, ZcodeBusInterruptSource<ZP> **sources, uint8_t sourceNum) :
        sources(sources), sourceNum(sourceNum), vectorChannel(NULL), notificationChannel(zcode->getNoopChannel()) {
}

template<class ZP>
bool ZcodeNotificationManager<ZP>::canSendNotification() {
    if (vectorChannel == NULL || !ZP::isUsingInterruptVector) {
        return !notificationChannel->hasOutStream() || !notificationChannel->acquireOutStream()->isLocked();
    } else {
        return vectorChannel->canAccept();
    }
}

template<class ZP>
void ZcodeNotificationManager<ZP>::sendNotification(ZcodeBusInterrupt<ZP> interrupt) { // Only called if canSendNotification()
    if (vectorChannel != NULL && (ZP::isUsingInterruptVector && vectorChannel->hasVector(&interrupt))) {
        vectorChannel->acceptInterrupt(interrupt);
    } else {
        ZcodeOutStream<ZP> *out = notificationChannel->acquireOutStream();
        out->lock();
        if (out->isOpen()) {
            out->close();
            out->mostRecent = interrupt.getSource();
        }
        out->openNotification(notificationChannel);
        out->markNotification();
        out->writeField('Z', (uint8_t) 1);
        out->writeField('T', interrupt.getNotificationType());
        out->writeField('I', interrupt.getNotificationBus());
        out->writeStatus(OK);
        if (ZP::findInterruptSourceAddress && interrupt.getSource()->hasFindableAddress(interrupt.getId())) {
            out->writeCommandSeperator();
            out->writeField('A', interrupt.getFoundAddress());
            if (interrupt.hasFindableAddress()) {
                out->writeStatus(OK);
            } else {
                out->writeStatus(CMD_FAIL);
            }
        }
        out->writeCommandSequenceSeperator();
        out->close();
        out->unlock();
        interrupt.clear();
    }
}

template<class ZP>
void ZcodeNotificationManager<ZP>::setNotificationChannel(ZcodeCommandChannel<ZP> *notificationChannel) {
    if (this->notificationChannel != NULL) {
        this->notificationChannel->releaseFromNotificationChannel();
    }
    this->notificationChannel = notificationChannel;
    this->notificationChannel->setAsNotificationChannel();
}

template<class ZP>
void ZcodeNotificationManager<ZP>::manageNotifications() {
    if (waitingNotificationNumber > 0 && canSendNotification()) {
        for (int i = 0; i < waitingNotificationNumber; i++) {
            ZcodeBusInterrupt<ZP> interrupt = waitingNotifications[i];
            if (ZP::findInterruptSourceAddress && interrupt.hasFindableAddress() && !interrupt.hasStartedAddressFind()) {
                interrupt.findAddress();
            }
            if (!ZP::findInterruptSourceAddress || !interrupt.hasFindableAddress() || interrupt.getFoundAddress() != -1) {
                sendNotification(interrupt);
                for (int j = i; j < waitingNotificationNumber - 1; j++) {
                    waitingNotifications[j] = waitingNotifications[j + 1];
                }
                waitingNotificationNumber--;
                i--;
            }
        }
    }
    if (waitingNotificationNumber < ZP::interruptStoreNum) {
        for (int i = 0; i < sourceNum; ++i) {
            ZcodeBusInterruptSource<ZP> *source = sources[i];
            int16_t id = source->takeUncheckedNotificationId();
            if (id != -1) {

                if (ZP::findInterruptSourceAddress && source->hasFindableAddress(id)) {
                    source->findAddress(id);
                    if (canSendNotification() && source->getFoundAddress(id) != -1) {
                        sendNotification(ZcodeBusInterrupt<ZP>(source, id));
                    } else {
                        waitingNotifications[waitingNotificationNumber++] = ZcodeBusInterrupt<ZP>(source, id);
                        break;
                    }
                } else {
                    if (canSendNotification() && (!ZP::findInterruptSourceAddress || source->getFoundAddress(id) != -1)) {
                        sendNotification(ZcodeBusInterrupt<ZP>(source, id));
                    } else {
                        waitingNotifications[waitingNotificationNumber++] = ZcodeBusInterrupt<ZP>(source, id);
                        break;
                    }
                }
            }
        }
    }
}

#include "Zcode.hpp"
#include "parsing/ZcodeCommandChannel.hpp"
#include "ZcodeOutStream.hpp"
#include "executionspace/ZcodeInterruptVectorManager.hpp"

#endif /* SRC_TEST_CPP_ZCODE_ZCODENOTIFICATIONMANAGER_HPP_ */
