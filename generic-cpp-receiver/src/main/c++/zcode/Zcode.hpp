/*
 * Zcode.hpp
 *
 *  Created on: 8 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_ZCODE_HPP_
#define SRC_TEST_CPP_ZCODE_ZCODE_HPP_

#include "ZcodeIncludes.hpp"
#include "parsing/ZcodeParser.hpp"
#include "parsing/ZcodeCommandFinder.hpp"
#include "executionspace/ZcodeExecutionSpace.hpp"
#include "ZcodeNotificationManager.hpp"
#include "ZcodeRunner.hpp"
#include "ZcodeDebugOutput.hpp"
#include "ZcodeVoidChannel.hpp"
#include "ZcodeLocks.hpp"

template<class RP>
class ZcodeCommandChannel;

template<class RP>
class ZcodeLockSet;

template<class RP>
class ZcodeAddressingCommandConsumer;

template<class RP>
class Zcode {
private:
    ZcodeParser<RP> parser;
    ZcodeRunner<RP> runner;
    ZcodeNotificationManager<RP> notificationManager;
    ZcodeExecutionSpace<RP> space;
    ZcodeCommandFinder<RP> finder;
    ZcodeDebugOutput<RP> debug;
    ZcodeVoidChannel<RP> voidChannel;
    ZcodeLocks<RP> locks;
    ZcodeAddressingCommandConsumer<RP> *addressingCommandConsumer = NULL;
    ZcodeCommandChannel<RP> **channels = NULL;
    const char *configFailureState = NULL;
    uint8_t channelNum = 0;
    public:
    Zcode(ZcodeBusInterruptSource<RP> **interruptSources, uint8_t interruptSourceNum) :
            parser(this), runner(this), notificationManager(this, interruptSources, interruptSourceNum), space(&notificationManager), finder(this), debug() {
    }
    void setChannels(ZcodeCommandChannel<RP> **channels, uint8_t channelNum) {
        this->channels = channels;
        this->channelNum = channelNum;
    }

    void configFail(const char *configFailureState) {
        this->configFailureState = configFailureState;
    }

    const char* getConfigFailureState() {
        return configFailureState;
    }

    void progressZcode() {
        debug.attemptFlush();
        notificationManager.manageNotifications();
        parser.parseNext();
        runner.runNext();
    }

    ZcodeCommandChannel<RP>* getVoidChannel() {
        return &voidChannel;
    }

    ZcodeExecutionSpace<RP>* getSpace() {
        return &space;
    }

    ZcodeCommandChannel<RP>** getChannels() {
        return channels;
    }
    uint8_t getChannelNumber() {
        return channelNum;
    }
    ZcodeCommandFinder<RP>* getCommandFinder() {
        return &finder;
    }

    ZcodeDebugOutput<RP>& getDebug() {
        return debug;
    }

    ZcodeNotificationManager<RP>* getNotificationManager() {
        return &notificationManager;
    }
    void setAddressingCommandConsumer(ZcodeAddressingCommandConsumer<RP> *addressingCommandConsumer) {
        this->addressingCommandConsumer = addressingCommandConsumer;
    }
    ZcodeAddressingCommandConsumer<RP>* getAddressingCommandConsumer() {
        return addressingCommandConsumer;
    }

    bool canLock(ZcodeLockSet<RP> *lockset) {
        return locks.canLock(lockset);
    }

    void lock(ZcodeLockSet<RP> *lockset) {
        locks.lock(lockset);
    }

    void unlock(ZcodeLockSet<RP> *lockset) {
        locks.unlock(lockset);
    }
};

#endif /* SRC_TEST_CPP_ZCODE_ZCODE_HPP_ */
