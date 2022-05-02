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
#include "ZcodeNoopChannel.hpp"
#include "ZcodeLocks.hpp"

template<class ZP>
class ZcodeCommandChannel;

template<class ZP>
class ZcodeLockSet;

template<class ZP>
class ZcodeAddressingCommandConsumer;

template<class ZP>
class Zcode {
    private:
        ZcodeParser<ZP> parser;
        ZcodeRunner<ZP> runner;
        ZcodeNotificationManager<ZP> notificationManager;
        ZcodeExecutionSpace<ZP> space;
        ZcodeCommandFinder<ZP> finder;
        ZcodeDebugOutput<ZP> debug;
        ZcodeNoopChannel<ZP> noopChannel;
        ZcodeLocks<ZP> locks;
        ZcodeAddressingCommandConsumer<ZP> *addressingCommandConsumer = NULL;
        ZcodeCommandChannel<ZP> **channels = NULL;
        const char *configFailureState = NULL;
        uint8_t channelNum = 0;

    public:
        Zcode(ZcodeBusInterruptSource<ZP> **interruptSources, uint8_t interruptSourceNum) :
                parser(this), runner(this), notificationManager(this, interruptSources, interruptSourceNum), space(&notificationManager), finder(this), debug() {
        }

        void setChannels(ZcodeCommandChannel<ZP> **channels, uint8_t channelNum) {
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

        ZcodeCommandChannel<ZP>* getNoopChannel() {
            return &noopChannel;
        }

        ZcodeExecutionSpace<ZP>* getSpace() {
            return &space;
        }

        ZcodeCommandChannel<ZP>** getChannels() {
            return channels;
        }

        uint8_t getChannelNumber() {
            return channelNum;
        }

        ZcodeCommandFinder<ZP>* getCommandFinder() {
            return &finder;
        }

        ZcodeDebugOutput<ZP>& getDebug() {
            return debug;
        }

        ZcodeNotificationManager<ZP>* getNotificationManager() {
            return &notificationManager;
        }

        void setAddressingCommandConsumer(ZcodeAddressingCommandConsumer<ZP> *addressingCommandConsumer) {
            this->addressingCommandConsumer = addressingCommandConsumer;
        }

        ZcodeAddressingCommandConsumer<ZP>* getAddressingCommandConsumer() {
            return addressingCommandConsumer;
        }

        bool canLock(ZcodeLockSet<ZP> *lockset) {
            return locks.canLock(lockset);
        }

        void lock(ZcodeLockSet<ZP> *lockset) {
            locks.lock(lockset);
        }

        void unlock(ZcodeLockSet<ZP> *lockset) {
            locks.unlock(lockset);
        }
};

#endif /* SRC_TEST_CPP_ZCODE_ZCODE_HPP_ */
