/*
 * RCodeInterruptVectorOut.cpp
 *
 *  Created on: 9 Dec 2020
 *      Author: robert
 */

#include "RCodeInterruptVectorOut.hpp"

void RCodeInterruptVectorOut::openResponse(RCodeCommandChannel *target) {
    RCodeInterruptVectorChannel *channel = (RCodeInterruptVectorChannel*) target;
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
    if (RCodeParameters::findInterruptSourceAddress
            && channel->getInterrupt()->getSource()->hasAddress()) {
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

bool RCodeInterruptVectorOut::lock() {
    if (out == NULL) {
        out = notificationManager->getNotificationChannel()->acquireOutStream();
    }
    return out->lock();
}

bool RCodeInterruptVectorOut::isLocked() {
    if (out == NULL) {
        out = notificationManager->getNotificationChannel()->acquireOutStream();
    }
    return out->isLocked();
}
void RCodeInterruptVectorOut::unlock() {
    out->unlock();
    notificationManager->getNotificationChannel()->releaseOutStream();
    out = NULL;
}
