/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_ZSCRIPT_HPP_
#define SRC_MAIN_C___ZSCRIPT_ZSCRIPT_HPP_

#include "ZscriptIncludes.hpp"
#include "execution/LockSet.hpp"
#include "execution/LockSystem.hpp"
#include "execution/ZscriptExecutor.hpp"
#include "ZscriptChannel.hpp"
#include "scriptSpace/ScriptSpace.hpp"

namespace Zscript {

template<class ZP>
class Zscript {
public:
    static Zscript<ZP> zscript;

private:

    Zscript<ZP>* operator &() {
        return this;
    }
    Zscript<ZP> operator *() {
        return this;
    }

    //    ZcodeModuleRegistry moduleRegistry = new ZcodeModuleRegistry();
    ZscriptChannel<ZP> **channels = NULL;
    GenericCore::ZscriptNotificationSource<ZP> **notifSources = NULL;
    ScriptSpace<ZP> **scriptSpaces = NULL;
    GenericCore::LockSystem<ZP> locks;
    uint8_t notificationChannelIndex = 0xFF;
    uint8_t channelCount = 0;
    uint8_t notifSrcCount = 0;
    uint8_t scriptSpaceCount = 0;

    Zscript() {
//        executor = new ZcodeExecutor(this);
    }

//    ZcodeExecutor executor;

public:

//    void addModule(ZcodeModule m) {
//        moduleRegistry.addModule(m);
//    }
//
    void setChannels(ZscriptChannel<ZP> **channels, uint8_t channelCount) {
        this->channels = channels;
        this->channelCount = channelCount;
    }

    void setNotificationSources(GenericCore::ZscriptNotificationSource<ZP> **notifSources, uint8_t notifSrcCount) {
        this->notifSources = notifSources;
        this->notifSrcCount = notifSrcCount;
    }

    void setScriptSpaces(ScriptSpace<ZP> **scriptSpaces, uint8_t scriptSpaceCount) {
        this->scriptSpaces = scriptSpaces;
        this->scriptSpaceCount = scriptSpaceCount;
    }

    bool lock(GenericCore::LockSet<ZP> *l) {
        return locks.lock(l);
    }

    bool canLock(GenericCore::LockSet<ZP> *l) {
        return locks.canLock(l);
    }

    void unlock(GenericCore::LockSet<ZP> *l) {
        locks.unlock(l);
    }

    bool progress() {
        for (uint8_t i = 0; i < channelCount; ++i) {
            channels[i]->moveAlong();
        }
        return GenericCore::ZscriptExecutor<ZP>::progress(channels, notifSources, channelCount, notifSrcCount);
    }
//
//    List<ZcodeChannel> getChannels() {
//        return channels;
//    }

//    ZcodeModuleRegistry getModuleRegistry() {
//        return moduleRegistry;
//    }

    AbstractOutStream<ZP>* getNotificationOutStream() {
        return channels[notificationChannelIndex]->getOutStream();
    }
    bool hasNotificationOutStream() {
        return notificationChannelIndex != 0xFF;
    }
//
    void setNotificationChannelIndex(uint8_t index) {
        if (index < channelCount && channels[index]->canBeNotifChannel()) {
            notificationChannelIndex = index;
        } else {
            notificationChannelIndex = 0xFF;
        }
    }
    ScriptSpace<ZP>** getScriptSpaces() {
        return scriptSpaces;
    }
    uint8_t getScriptSpaceCount() {
        return scriptSpaceCount;
    }
    uint8_t getChannelCount() {
        return channelCount;
    }
    ZscriptChannel<ZP>** getChannels() {
        return channels;
    }

};
template<class ZP>
Zscript<ZP> Zscript<ZP>::zscript;
}

#endif /* SRC_MAIN_C___ZSCRIPT_ZSCRIPT_HPP_ */
