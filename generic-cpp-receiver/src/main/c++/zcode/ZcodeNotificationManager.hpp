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

template<class RP>
class ZcodeBusInterruptSource;

template<class RP>
class ZcodeBusInterrupt;

template<class RP>
class ZcodeInterruptVectorManager;

template<class RP>
class ZcodeCommandChannel;

template<class RP>
class Zcode;

template<class RP>
class ZcodeNotificationManager {
private:
    ZcodeBusInterruptSource<RP> **sources;
    ZcodeBusInterrupt<RP> waitingNotifications[RP::interruptStoreNum];
    uint8_t waitingNotificationNumber = 0;
    uint8_t sourceNum = 0;
    ZcodeInterruptVectorManager<RP> *vectorChannel;
    ZcodeCommandChannel<RP> *notificationChannel;

    bool canSendNotification();

    void sendNotification(ZcodeBusInterrupt<RP> interrupt);

public:
    ZcodeNotificationManager(Zcode<RP> *zcode, ZcodeBusInterruptSource<RP> **sources, uint8_t sourceNum);

    void setVectorChannel(ZcodeInterruptVectorManager<RP> *vectorChannel) {
        this->vectorChannel = vectorChannel;
    }

    void setNotificationChannel(ZcodeCommandChannel<RP> *notificationChannel);

    ZcodeCommandChannel<RP>* getNotificationChannel() {
        return notificationChannel;
    }

    ZcodeInterruptVectorManager<RP>* getVectorChannel() {
        return vectorChannel;
    }

    void manageNotifications();
};

template<class RP>
ZcodeNotificationManager<RP>::ZcodeNotificationManager(Zcode<RP> *zcode, ZcodeBusInterruptSource<RP> **sources, uint8_t sourceNum) :
        sources(sources), sourceNum(sourceNum), vectorChannel(NULL), notificationChannel(zcode->getVoidChannel()) {
}

template<class RP>
bool ZcodeNotificationManager<RP>::canSendNotification() {
    if (vectorChannel == NULL || !RP::isUsingInterruptVector) {
        return !notificationChannel->hasOutStream() || !notificationChannel->acquireOutStream()->isLocked();
    } else {
        return vectorChannel->canAccept();
    }
}

template<class RP>
void ZcodeNotificationManager<RP>::sendNotification(ZcodeBusInterrupt<RP> interrupt) { // Only called if canSendNotification()
    if (vectorChannel != NULL && (RP::isUsingInterruptVector && vectorChannel->hasVector(&interrupt))) {
        vectorChannel->acceptInterrupt(interrupt);
    } else {
        ZcodeOutStream<RP> *out = notificationChannel->acquireOutStream();
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
        if (RP::findInterruptSourceAddress && interrupt.getSource()->hasFindableAddress(interrupt.getId())) {
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

template<class RP>
void ZcodeNotificationManager<RP>::setNotificationChannel(ZcodeCommandChannel<RP> *notificationChannel) {
    if (this->notificationChannel != NULL) {
        this->notificationChannel->releaseFromNotificationChannel();
    }
    this->notificationChannel = notificationChannel;
    this->notificationChannel->setAsNotificationChannel();
}

template<class RP>
void ZcodeNotificationManager<RP>::manageNotifications() {
    if (waitingNotificationNumber > 0 && canSendNotification()) {
        for (int i = 0; i < waitingNotificationNumber; i++) {
            ZcodeBusInterrupt<RP> interrupt = waitingNotifications[i];
            if (RP::findInterruptSourceAddress && interrupt.hasFindableAddress() && !interrupt.hasStartedAddressFind()) {
                interrupt.findAddress();
            }
            if (!RP::findInterruptSourceAddress || !interrupt.hasFindableAddress() || interrupt.getFoundAddress() != -1) {
                sendNotification(interrupt);
                for (int j = i; j < waitingNotificationNumber - 1; j++) {
                    waitingNotifications[j] = waitingNotifications[j + 1];
                }
                waitingNotificationNumber--;
                i--;
            }
        }
    }
    if (waitingNotificationNumber < RP::interruptStoreNum) {
        for (int i = 0; i < sourceNum; ++i) {
            ZcodeBusInterruptSource<RP> *source = sources[i];
            int16_t id = source->takeUncheckedNotificationId();
            if (id != -1) {

                if (RP::findInterruptSourceAddress && source->hasFindableAddress(id)) {
                    source->findAddress(id);
                    if (canSendNotification() && source->getFoundAddress(id) != -1) {
                        sendNotification(ZcodeBusInterrupt<RP>(source, id));
                    } else {
                        waitingNotifications[waitingNotificationNumber++] = ZcodeBusInterrupt<RP>(source, id);
                        break;
                    }
                } else {
                    if (canSendNotification() && (!RP::findInterruptSourceAddress || source->getFoundAddress(id) != -1)) {
                        sendNotification(ZcodeBusInterrupt<RP>(source, id));
                    } else {
                        waitingNotifications[waitingNotificationNumber++] = ZcodeBusInterrupt<RP>(source, id);
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
