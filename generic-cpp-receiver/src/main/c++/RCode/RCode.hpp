/*
 * RCode.hpp
 *
 *  Created on: 8 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_RCODE_HPP_
#define SRC_TEST_CPP_RCODE_RCODE_HPP_

#include "RCodeIncludes.hpp"
#include "parsing/RCodeParser.hpp"
#include "parsing/RCodeCommandFinder.hpp"
#include "executionspace/RCodeExecutionSpace.hpp"
#include "RCodeNotificationManager.hpp"
#include "RCodeRunner.hpp"
#include "RCodeDebugOutput.hpp"
#include "RCodeVoidChannel.hpp"
#include "RCodeLocks.hpp"

template<class RP>
class RCodeCommandChannel;

template<class RP>
class RCodeLockSet;

template<class RP>
class RCode {
private:
    RCodeParser<RP> parser;
    RCodeRunner<RP> runner;
    RCodeNotificationManager<RP> notificationManager;
    RCodeExecutionSpace<RP> space;
    RCodeCommandFinder<RP> finder;
    RCodeDebugOutput<RP> debug;
    RCodeVoidChannel<RP> voidChannel;
    RCodeLocks<RP> locks;
    RCodeCommandChannel<RP> **channels = NULL;
    const char *configFailureState = NULL;
    uint8_t channelNum = 0;
    public:
    RCode(RCodeBusInterruptSource<RP> *interruptSources, uint8_t interruptSourceNum) :
            parser(this), runner(this), notificationManager(this, interruptSources, interruptSourceNum), space(&notificationManager), finder(this), debug() {
    }
    void setChannels(RCodeCommandChannel<RP> **channels, uint8_t channelNum) {
        this->channels = channels;
        this->channelNum = channelNum;
    }

    void configFail(const char *configFailureState) {
        this->configFailureState = configFailureState;
    }

    const char* getConfigFailureState() {
        return configFailureState;
    }

    void progressRCode() {
        debug.attemptFlush();
        notificationManager.manageNotifications();
        parser.parseNext();
        runner.runNext();
    }

    RCodeCommandChannel<RP>* getVoidChannel() {
        return &voidChannel;
    }

    RCodeExecutionSpace<RP>* getSpace() {
        return &space;
    }

    RCodeCommandChannel<RP>** getChannels() {
        return channels;
    }
    uint8_t getChannelNumber() {
        return channelNum;
    }
    RCodeCommandFinder<RP>* getCommandFinder() {
        return &finder;
    }

    RCodeDebugOutput<RP>& getDebug() {
        return debug;
    }

    RCodeNotificationManager<RP>* getNotificationManager() {
        return &notificationManager;
    }

    bool canLock(RCodeLockSet<RP> *lockset) {
        return locks.canLock(lockset);
    }

    void lock(RCodeLockSet<RP> *lockset) {
        locks.lock(lockset);
    }

    void unlock(RCodeLockSet<RP> *lockset) {
        locks.unlock(lockset);
    }
};

#endif /* SRC_TEST_CPP_RCODE_RCODE_HPP_ */
