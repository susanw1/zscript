/*
 * RCodeNotificationManager.hpp
 *
 *  Created on: 28 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_RCODENOTIFICATIONMANAGER_HPP_
#define SRC_TEST_CPP_RCODE_RCODENOTIFICATIONMANAGER_HPP_
#include "RCodeIncludes.hpp"
#include "RCodeParameters.hpp"
#include "RCodeBusInterruptSource.hpp"
#include "RCodeBusInterrupt.hpp"

class RCodeBusInterruptSource;
class RCodeBusInterrupt;
class RCodeInterruptVectorManager;
class RCodeCommandChannel;
class RCode;

class RCodeNotificationManager {
private:
    RCodeBusInterruptSource *sources;
    RCodeBusInterrupt *waitingNotifications[RCodeParameters::interruptStoreNum];
    uint8_t waitingNotificationNumber = 0;
    uint8_t sourceNum = 0;
    RCodeInterruptVectorManager *vectorChannel;
    RCodeCommandChannel *notificationChannel;

    bool canSendNotification();

    void sendNotification(RCodeBusInterrupt *interrupt);

public:
    RCodeNotificationManager(RCode *rcode, RCodeBusInterruptSource *sources,
            uint8_t sourceNum);

    void setVectorChannel(RCodeInterruptVectorManager *vectorChannel) {
        this->vectorChannel = vectorChannel;
    }

    void setNotificationChannel(RCodeCommandChannel *notificationChannel);

    RCodeCommandChannel* getNotificationChannel() {
        return notificationChannel;
    }

    RCodeInterruptVectorManager* getVectorChannel() {
        return vectorChannel;
    }

    void manageNotifications();

};
#include "RCode.hpp"
#include "parsing/RCodeCommandChannel.hpp"
#include "RCodeOutStream.hpp"
#include "executionspace/RCodeInterruptVectorManager.hpp"

#endif /* SRC_TEST_CPP_RCODE_RCODENOTIFICATIONMANAGER_HPP_ */
