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

    uint8_t readBuffer[ZP::interruptVectorOutReadBufferSize];
    ZcodeOutStream<ZP> *out = NULL;
    bool openB = false;

public:
    ZcodeInterruptVectorOut() :
            ZcodeOutStream<ZP>(readBuffer, ZP::interruptVectorOutReadBufferSize) {
    }

    void writeByte(uint8_t value) {
        if (out != NULL) {
            out->writeByte(value);
        }
    }

    void open(ZcodeCommandChannel<ZP> *target, ZcodeOutStreamOpenType type);

    //FIXME: remove reliance on access to mostRecent
    bool isOpen() {
        return Zcode<ZP>::zcode.getNotificationManager()->getNotificationChannel() == NULL || (out != NULL && out->isOpen() && out->mostRecent == this);
    }

    void close() {
        if (out != NULL && out->mostRecent == this) {
            out->close();
        }
    }

    bool lock();

    bool isLocked();

    void unlock();
};

template<class ZP>
void ZcodeInterruptVectorOut<ZP>::open(ZcodeCommandChannel<ZP> *target, ZcodeOutStreamOpenType type) {
    if (!out->isOpen() && type == RESPONSE) {
        if (Zcode<ZP>::zcode.getNotificationManager()->getNotificationChannel() == NULL) {
            return;
        }
        ZcodeInterruptVectorChannel<ZP> *channel = (ZcodeInterruptVectorChannel<ZP>*) target;

        out->openNotification(Zcode<ZP>::zcode.getNotificationManager()->getNotificationChannel());
        Zcode<ZP>::zcode.getNotificationManager()->sendInitialInterruptInfo(out, channel->getInterrupt());
        out->writeCommandSeparator();
        channel->clear();
    }
}

template<class ZP>
bool ZcodeInterruptVectorOut<ZP>::lock() {
    if (out == NULL) {
        if (Zcode<ZP>::zcode.getNotificationManager()->getNotificationChannel() == NULL) {
            return true;
        }
        out = Zcode<ZP>::zcode.getNotificationManager()->getNotificationChannel()->out;
    }
    return out->lock();
}

template<class ZP>
bool ZcodeInterruptVectorOut<ZP>::isLocked() {
    if (out == NULL) {
        if (Zcode<ZP>::zcode.getNotificationManager()->getNotificationChannel() == NULL) {
            return true;
        }
        out = Zcode<ZP>::zcode.getNotificationManager()->getNotificationChannel()->out;
    }
    return out->isLocked();
}

template<class ZP>
void ZcodeInterruptVectorOut<ZP>::unlock() {
    if (out != NULL) {
        out->unlock();
    }
    out = NULL;
}

#include "../scriptspace/ZcodeInterruptVectorChannel.hpp"

#endif /* SRC_MAIN_CPP_ZCODE_EXECUTIONSPACE_ZCODEINTERRUPTVECTOROUT_HPP_ */
