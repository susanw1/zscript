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
#include "parsing/ZcodeChannelCommandSlot.hpp"
#include "scriptspace/ZcodeScriptSpace.hpp"
#include "ZcodeNotificationManager.hpp"
#include "running/ZcodeRunner.hpp"
#include "ZcodeDebugOutput.hpp"
#include "channels/ZcodeNoopChannel.hpp"
#include "ZcodeLocks.hpp"
#include "modules/ZcodeModule.hpp"

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
    ZcodeScriptSpace<ZP> space;
    ZcodeDebugOutput<ZP> debug;
    ZcodeNoopChannel<ZP> noopChannel;
    ZcodeLocks<ZP> locks;
    ZcodeAddressingCommandConsumer<ZP> *addressingCommandConsumer = NULL;
    ZcodeCommandChannel<ZP> **channels = NULL;
    const char *configFailureState = NULL;
    ZcodeModule<ZP> **modules = NULL;
    uint8_t channelNum = 0;
    uint8_t moduleNum = 0;

public:
    Zcode(ZcodeBusInterruptSource<ZP> **interruptSources, uint8_t interruptSourceNum) :
            parser(this), runner(this), notificationManager(this, interruptSources, interruptSourceNum), space(&notificationManager), debug(),
                    noopChannel(this) {
    }

    void setChannels(ZcodeCommandChannel<ZP> **channels, uint8_t channelNum) {
        this->channels = channels;
        this->channelNum = channelNum;
    }

    void setModules(ZcodeModule<ZP> **modules, uint8_t moduleNum) {
        this->modules = modules;
        this->moduleNum = moduleNum;
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

    ZcodeScriptSpace<ZP>* getSpace() {
        return &space;
    }

    ZcodeCommandChannel<ZP>** getChannels() {
        return channels;
    }

    ZcodeLocks<ZP>* getLocks() {
        return &locks;
    }

    uint8_t getChannelNumber() {
        return channelNum;
    }

    ZcodeModule<ZP>** getModules() {
        return modules;
    }

    uint8_t getModuleNumber() {
        return moduleNum;
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
