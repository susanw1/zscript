/*
 * RCodeNotificationManager.cpp
 *
 *  Created on: 28 Sep 2020
 *      Author: robert
 */

#include "RCodeNotificationManager.hpp"

RCodeNotificationManager::RCodeNotificationManager(RCode *rcode,
        RCodeBusInterruptSource *sources, uint8_t sourceNum) :
        sources(sources), sourceNum(sourceNum), vectorChannel(NULL), notificationChannel(
                rcode->getVoidChannel()) {
}
bool RCodeNotificationManager::canSendNotification() {
    return (!RCodeParameters::isUsingInterruptVector
            && !notificationChannel->getOutStream()->isLocked())
            || (RCodeParameters::isUsingInterruptVector);
    //&& vectorChannel->canAccept());
}

void RCodeNotificationManager::sendNotification(RCodeBusInterrupt *interrupt) {
    if (RCodeParameters::isUsingInterruptVector) {
        // && vectorChannel->hasVector(interrupt)) {
        //vectorChannel->acceptInterrupt(interrupt);
    } else {
        RCodeOutStream *out = notificationChannel->getOutStream();
        if (out->isOpen()) {
            out->close();
            out->mostRecent = interrupt->getSource();
        }
        out->openNotification(notificationChannel);
        out->markNotification();
        out->writeField('Z', 1);
        out->writeField('A', 1);
        out->writeField('T', interrupt->getNotificationType());
        out->writeField('I', interrupt->getNotificationBus());
        out->writeStatus(OK);
        if (RCodeParameters::findInterruptSourceAddress
                && interrupt->getSource()->hasAddress()) {
            out->writeCommandSeperator();
            out->writeField('A', interrupt->getFoundAddress());
            if (interrupt->hasFindableAddress()) {
                out->writeStatus(OK);
            } else {
                out->writeStatus(CMD_FAIL);
            }
        }
        out->writeCommandSequenceSeperator();
        out->close();
        interrupt->clear();
    }
}
void RCodeNotificationManager::setNotificationChannel(
        RCodeCommandChannel *notificationChannel) {
    if (this->notificationChannel != NULL) {
        this->notificationChannel->releaseFromNotificationChannel();
    }
    this->notificationChannel = notificationChannel;
    this->notificationChannel->setAsNotificationChannel();
}
void RCodeNotificationManager::manageNotifications() {
    if (waitingNotificationNumber > 0 && canSendNotification()) {
        for (int i = 0; i < waitingNotificationNumber; i++) {
            RCodeBusInterrupt *interrupt = waitingNotifications[i];
            if (RCodeParameters::findInterruptSourceAddress
                    && !interrupt->hasStartedAddressFind()) {
                if (interrupt->checkFindAddressLocks()) {
                    interrupt->findAddress();
                }
            }
            if (!RCodeParameters::findInterruptSourceAddress
                    || interrupt->hasFoundAddress()) {
                sendNotification(interrupt);
                for (int j = i; j < waitingNotificationNumber - 1; j++) {
                    waitingNotifications[j] = waitingNotifications[j + 1];
                }
                waitingNotificationNumber--;
                i--;
            }
        }
    }
    if (waitingNotificationNumber < RCodeParameters::interruptStoreNum) {
        for (int i = 0; i < sourceNum; ++i) {
            RCodeBusInterruptSource *source = sources + i;
            if (source->hasUncheckedNotifications()) {
                uint8_t id = source->takeUncheckedNotificationId();
                if (RCodeParameters::findInterruptSourceAddress
                        && source->hasAddress()) {
                    if (source->checkFindAddressLocks(id)) {
                        source->findAddress(id);
                    }
                    if (canSendNotification() && source->hasFoundAddress(id)) {
                        sendNotification(new RCodeBusInterrupt(source, id));
                    } else {
                        waitingNotifications[waitingNotificationNumber++] =
                                new RCodeBusInterrupt(source, id);
                        break;
                    }
                } else {
                    if (canSendNotification()
                            && (!RCodeParameters::findInterruptSourceAddress
                                    || source->hasFoundAddress(id))) {
                        sendNotification(new RCodeBusInterrupt(source, id));
                    } else {
                        waitingNotifications[waitingNotificationNumber++] =
                                new RCodeBusInterrupt(source, id);
                        break;
                    }
                }
            }
        }
    }
}
