/*
 * RCodeNotificationManager.hpp
 *
 *  Created on: 28 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_RCODENOTIFICATIONMANAGER_HPP_
#define SRC_TEST_CPP_RCODE_RCODENOTIFICATIONMANAGER_HPP_
#include "RCodeIncludes.hpp"
#include "RCodeBusInterruptSource.hpp"
#include "RCodeBusInterrupt.hpp"

template<class RP>
class RCodeBusInterruptSource;

template<class RP>
class RCodeBusInterrupt;

template<class RP>
class RCodeInterruptVectorManager;

template<class RP>
class RCodeCommandChannel;

template<class RP>
class RCode;

template<class RP>
class RCodeNotificationManager {
private:
    RCodeBusInterruptSource<RP> *sources;
    RCodeBusInterrupt<RP> waitingNotifications[RP::interruptStoreNum];
    uint8_t waitingNotificationNumber = 0;
    uint8_t sourceNum = 0;
    RCodeInterruptVectorManager<RP> *vectorChannel;
    RCodeCommandChannel<RP> *notificationChannel;

    bool canSendNotification();

    void sendNotification(RCodeBusInterrupt<RP> interrupt);

public:
    RCodeNotificationManager(RCode<RP> *rcode, RCodeBusInterruptSource<RP> *sources, uint8_t sourceNum);

    void setVectorChannel(RCodeInterruptVectorManager<RP> *vectorChannel) {
        this->vectorChannel = vectorChannel;
    }

    void setNotificationChannel(RCodeCommandChannel<RP> *notificationChannel);

    RCodeCommandChannel<RP>* getNotificationChannel() {
        return notificationChannel;
    }

    RCodeInterruptVectorManager<RP>* getVectorChannel() {
        return vectorChannel;
    }

    void manageNotifications();
};

template<class RP>
RCodeNotificationManager<RP>::RCodeNotificationManager(RCode<RP> *rcode, RCodeBusInterruptSource<RP> *sources, uint8_t sourceNum) :
        sources(sources), sourceNum(sourceNum), vectorChannel(NULL), notificationChannel(rcode->getVoidChannel()) {
}

template<class RP>
bool RCodeNotificationManager<RP>::canSendNotification() {
    if (vectorChannel == NULL || !RP::isUsingInterruptVector) {
        return !notificationChannel->hasOutStream() || !notificationChannel->acquireOutStream()->isLocked();
    } else {
        return vectorChannel->canAccept();
    }
}

template<class RP>
void RCodeNotificationManager<RP>::sendNotification(RCodeBusInterrupt<RP> interrupt) { // Only called if canSendNotification()
    if (vectorChannel != NULL && (RP::isUsingInterruptVector && vectorChannel->hasVector(&interrupt))) {
        vectorChannel->acceptInterrupt(interrupt);
    } else {
        RCodeOutStream<RP> *out = notificationChannel->acquireOutStream();
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
        interrupt.clear();
    }
}

template<class RP>
void RCodeNotificationManager<RP>::setNotificationChannel(RCodeCommandChannel<RP> *notificationChannel) {
    if (this->notificationChannel != NULL) {
        this->notificationChannel->releaseFromNotificationChannel();
    }
    this->notificationChannel = notificationChannel;
    this->notificationChannel->setAsNotificationChannel();
}

template<class RP>
void RCodeNotificationManager<RP>::manageNotifications() {
    if (waitingNotificationNumber > 0 && canSendNotification()) {
        for (int i = 0; i < waitingNotificationNumber; i++) {
            RCodeBusInterrupt<RP> interrupt = waitingNotifications[i];
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
            RCodeBusInterruptSource<RP> *source = sources + i;
            int16_t id = source->takeUncheckedNotificationId();
            if (id != -1) {

                if (RP::findInterruptSourceAddress && source->hasFindableAddress(id)) {
                    source->findAddress(id);
                    if (canSendNotification() && source->getFoundAddress(id) != -1) {
                        sendNotification(RCodeBusInterrupt<RP>(source, id));
                    } else {
                        waitingNotifications[waitingNotificationNumber++] = RCodeBusInterrupt<RP>(source, id);
                        break;
                    }
                } else {
                    if (canSendNotification() && (!RP::findInterruptSourceAddress || source->getFoundAddress(id) != -1)) {
                        sendNotification(RCodeBusInterrupt<RP>(source, id));
                    } else {
                        waitingNotifications[waitingNotificationNumber++] = RCodeBusInterrupt<RP>(source, id);
                        break;
                    }
                }
            }
        }
    }
}

#include "RCode.hpp"
#include "parsing/RCodeCommandChannel.hpp"
#include "RCodeOutStream.hpp"
#include "executionspace/RCodeInterruptVectorManager.hpp"

#endif /* SRC_TEST_CPP_RCODE_RCODENOTIFICATIONMANAGER_HPP_ */
