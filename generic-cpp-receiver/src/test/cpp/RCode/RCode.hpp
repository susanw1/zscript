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
#include "executionspace/RCodeExecutionSpace.hpp"
#include "RCodeNotificationManager.hpp"
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
    RCodeNotificationManager notificationManager;
    RCodeExecutionSpace space;
    RCodeCommandFinder finder;
    RCodeDebugOutput debug;
    RCodeVoidChannel voidChannel;
    RCodeLocks locks;
    RCodeCommandChannel **channels = NULL;
    uint8_t channelNum = 0;
public:
    RCode(RCodeBusInterruptSource *interruptSources, uint8_t interruptSourceNum) :
            parser(this), runner(this), notificationManager(this,
                    interruptSources, interruptSourceNum), space(
                    &notificationManager), finder(this), debug() {
    }
    void setChannels(RCodeCommandChannel **channels, uint8_t channelNum) {
        this->channels = channels;
        this->channelNum = channelNum;
    }

    void progressRCode();

    RCodeCommandChannel* getVoidChannel() {
        return &voidChannel;
    }

    RCodeExecutionSpace* getSpace() {
        return &space;
    }
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

    RCodeNotificationManager* getNotificationManager() {
        return &notificationManager;
    }
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
