/*
 * ZcodeNotificationManager.hpp
 *
 *  Created on: 28 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_ZCODENOTIFICATIONMANAGER_HPP_
#define SRC_TEST_CPP_ZCODE_ZCODENOTIFICATIONMANAGER_HPP_

#include "ZcodeIncludes.hpp"
#include "ZcodeOutStream.hpp"
#include "ZcodeBusInterruptSource.hpp"
#include "ZcodeBusInterrupt.hpp"

#if defined(ZCODE_SUPPORT_SCRIPT_SPACE) && defined(ZCODE_SUPPORT_INTERRUPT_VECTOR)
#include "scriptspace/ZcodeInterruptVectorManager.hpp"
#endif

template<class ZP>
class ZcodeBusInterruptSource;

template<class ZP>
class ZcodeBusInterrupt;

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

#if defined(ZCODE_SUPPORT_SCRIPT_SPACE) && defined(ZCODE_SUPPORT_INTERRUPT_VECTOR)
    ZcodeInterruptVectorManager<ZP> vectorChannel;
#endif
    ZcodeCommandChannel<ZP> *notificationChannel;

    bool canSendNotification();

    void sendNotification(ZcodeBusInterrupt<ZP> interrupt);

public:
    ZcodeNotificationManager() :
            sources(NULL), sourceNum(0),

#if defined(ZCODE_SUPPORT_SCRIPT_SPACE) && defined(ZCODE_SUPPORT_INTERRUPT_VECTOR)
            vectorChannel(),
    #endif
                    notificationChannel(NULL) {
    }
    void sendInitialInterruptInfo(ZcodeOutStream<ZP> *out, ZcodeBusInterrupt<ZP> interrupt) {
        out->openNotification(notificationChannel);
        out->markNotification(BusNotification);
        out->writeField16('M', interrupt.getNotificationModule());
        out->writeField8('P', interrupt.getNotificationPort());
        out->writeStatus(OK);
        if (interrupt.hasFindableAddress()) {
            out->writeCommandSeparator();
            out->writeField16('A', interrupt.getFoundAddress());
            out->writeStatus(OK);
        }
    }

    void setBusInterruptSources(ZcodeBusInterruptSource<ZP> **sources, uint8_t sourceNum) {
        this->sources = sources;
        this->sourceNum = sourceNum;
    }

#if defined(ZCODE_SUPPORT_SCRIPT_SPACE) && defined(ZCODE_SUPPORT_INTERRUPT_VECTOR)
    void setVectorChannel(ZcodeInterruptVectorManager<ZP> *vectorChannel) {
        this->vectorChannel = vectorChannel;
    }
#endif

    void setNotificationChannel(ZcodeCommandChannel<ZP> *notificationChannel);

    ZcodeCommandChannel<ZP>* getNotificationChannel() {
        return notificationChannel;
    }

#if defined(ZCODE_SUPPORT_SCRIPT_SPACE) && defined(ZCODE_SUPPORT_INTERRUPT_VECTOR)
    ZcodeInterruptVectorManager<ZP>* getVectorChannel() {
        return &vectorChannel;
    }
#endif

    void manageNotifications();
};

template<class ZP>
bool ZcodeNotificationManager<ZP>::canSendNotification() {

#if defined(ZCODE_SUPPORT_SCRIPT_SPACE) && defined(ZCODE_SUPPORT_INTERRUPT_VECTOR)
        return vectorChannel.canAccept() && !notificationChannel->out->isLocked();
#else
    return !notificationChannel->out->isLocked();
#endif
}

template<class ZP>
void ZcodeNotificationManager<ZP>::sendNotification(ZcodeBusInterrupt<ZP> interrupt) { // Only called if canSendNotification()
#if defined(ZCODE_SUPPORT_SCRIPT_SPACE) && defined(ZCODE_SUPPORT_INTERRUPT_VECTOR)
    if(ZP::AddressRouter::isAddressed(interrupt)){
        ZP::AddressRouter::response(interrupt, notificationChannel->out);
    } else if (vectorChannel.hasVector(&interrupt)) {
        vectorChannel.acceptInterrupt(interrupt);
    } else {
#endif
    ZcodeOutStream<ZP> *out = notificationChannel->out;
    out->lock();
    out->openNotification(notificationChannel);
    sendInitialInterruptInfo(out, interrupt);
    out->writeCommandSequenceSeparator();
    out->close();
    out->unlock();
    interrupt.clear();
#if defined(ZCODE_SUPPORT_SCRIPT_SPACE) && defined(ZCODE_SUPPORT_INTERRUPT_VECTOR)
    }
#endif
}

template<class ZP>
void ZcodeNotificationManager<ZP>::setNotificationChannel(ZcodeCommandChannel<ZP> *notificationChannel) {
    if (this->notificationChannel != NULL) {
        this->notificationChannel->stateChange(RELEASED_FROM_NOTIFICATION);
    }
    this->notificationChannel = notificationChannel;
    if (notificationChannel != NULL) {
        this->notificationChannel->stateChange(SET_AS_NOTIFICATION);
    }
}

template<class ZP>
void ZcodeNotificationManager<ZP>::manageNotifications() {
    if (waitingNotificationNumber > 0 && canSendNotification()) {
        for (uint8_t i = 0; i < waitingNotificationNumber; i++) {
            ZcodeBusInterrupt<ZP> interrupt = waitingNotifications[i];
            if (!interrupt.hasFindableAddress() || interrupt.hasFoundAddress()) {
                sendNotification(interrupt);
                if (!interrupt.isValid()) {
                    for (uint8_t j = i; j < waitingNotificationNumber - 1; j++) {
                        waitingNotifications[j] = waitingNotifications[j + 1];
                    }
                    waitingNotificationNumber--;
                    i--;
                }
            }
        }
    }
    if (waitingNotificationNumber < ZP::interruptStoreNum) {
        for (uint8_t i = 0; i < sourceNum; ++i) {
            ZcodeBusInterruptSource<ZP> *source = sources[i];
            ZcodeNotificationInfo info = source->takeUncheckedNotification();
            if (info.valid) {
                ZcodeNotificationAddressInfo addr;
                if (ZP::findInterruptSourceAddress) {
                    addr = source->getAddressInfo(info.id);
                } else {
                    addr.valid = false;
                }
                if (addr.valid) {
                    if (canSendNotification() && addr.hasFound) {
                        sendNotification(ZcodeBusInterrupt<ZP>(source, info, addr));
                    } else {
                        waitingNotifications[waitingNotificationNumber++] = ZcodeBusInterrupt<ZP>(source, info, addr);
                        break;
                    }
                } else {
                    if (canSendNotification()) {
                        sendNotification(ZcodeBusInterrupt<ZP>(source, info, addr));
                    } else {
                        waitingNotifications[waitingNotificationNumber++] = ZcodeBusInterrupt<ZP>(source, info, addr);
                        break;
                    }
                }
            }
        }
    }
#ifdef ZCODE_SUPPORT_INTERRUPT_VECTOR
    vectorChannel.activateInterrupt();
#endif
}

#endif /* SRC_TEST_CPP_ZCODE_ZCODENOTIFICATIONMANAGER_HPP_ */
