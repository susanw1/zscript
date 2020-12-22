/*
 * RCode.hpp
 *
 *  Created on: 8 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_RCODE_HPP_
#define SRC_TEST_CPP_RCODE_RCODE_HPP_
#include "RCodeIncludes.hpp"
#include "RCodeParameters.hpp"
#include "parsing/RCodeParser.hpp"
#include "parsing/RCodeCommandFinder.hpp"
#ifdef NOTIFICATIONS
#include "executionspace/RCodeExecutionSpace.hpp"
#include "RCodeNotificationManager.hpp"
#endif
#include "RCodeRunner.hpp"
#include "RCodeDebugOutput.hpp"
#include "RCodeVoidChannel.hpp"
#include "RCodeLocks.hpp"

class RCodeCommandChannel;
class RCodeLockSet;

class RCode {
private:
    RCodeParser parser;
    RCodeRunner runner;
#ifdef NOTIFICATIONS
    RCodeNotificationManager notificationManager;
    RCodeExecutionSpace space;
#endif
    RCodeCommandFinder finder;
    RCodeDebugOutput debug;
    RCodeVoidChannel voidChannel;
    RCodeLocks locks;
    RCodeCommandChannel **channels = NULL;
    const char *configFailureState = NULL;
    uint8_t channelNum = 0;
public:
    RCode(
#ifdef NOTIFICATIONS
            RCodeBusInterruptSource *interruptSources, uint8_t interruptSourceNum
#endif
            ) :
            parser(this), runner(this),
#ifdef NOTIFICATIONS
                    notificationManager(this, interruptSources, interruptSourceNum), space(&notificationManager),
#endif
                    finder(this), debug() {
    }
    void setChannels(RCodeCommandChannel **channels, uint8_t channelNum) {
        this->channels = channels;
        this->channelNum = channelNum;
    }

    void configFail(const char *configFailureState) {
        this->configFailureState = configFailureState;
    }

    const char* getConfigFailureState() {
        return configFailureState;
    }

    void progressRCode();

    RCodeCommandChannel* getVoidChannel() {
        return &voidChannel;
    }

#ifdef NOTIFICATIONS
    RCodeExecutionSpace* getSpace() {
        return &space;
    }
#endif
    RCodeCommandChannel** getChannels() {
        return channels;
    }
    uint8_t getChannelNumber() {
        return channelNum;
    }
    RCodeCommandFinder* getCommandFinder() {
        return &finder;
    }

    RCodeDebugOutput& getDebug() {
        return debug;
    }

#ifdef NOTIFICATIONS
    RCodeNotificationManager* getNotificationManager() {
        return &notificationManager;
    }
#endif
    bool canLock(RCodeLockSet *lockset) {
        return locks.canLock(lockset);
    }

    void lock(RCodeLockSet *lockset) {
        locks.lock(lockset);
    }

    void unlock(RCodeLockSet *lockset) {
        locks.unlock(lockset);
    }
};

#endif /* SRC_TEST_CPP_RCODE_RCODE_HPP_ */
