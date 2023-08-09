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
private:
    //    ZcodeModuleRegistry moduleRegistry = new ZcodeModuleRegistry();
    GenericCore::LockSystem<ZP> locks;
    ZscriptChannel<ZP> **channels = NULL;
    GenericCore::ZscriptNotificationSource<ZP> **notifSources = NULL;
    ScriptSpace<ZP> **scriptSpaces = NULL;
    uint8_t channelCount = 0;
    uint8_t notifSrcCount = 0;
    uint8_t scriptSpaceCount = 0;
    AbstractOutStream<ZP> *notificationOutStream = NULL;

//    ZcodeExecutor executor;

public:
    Zscript() {
//        executor = new ZcodeExecutor(this);
    }

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
        return GenericCore::ZscriptExecutor<ZP>::progress(this, channels, notifSources, channelCount, notifSrcCount);
    }
//
//    List<ZcodeChannel> getChannels() {
//        return channels;
//    }

//    ZcodeModuleRegistry getModuleRegistry() {
//        return moduleRegistry;
//    }

    AbstractOutStream<ZP>* getNotificationOutStream() {
        return notificationOutStream;
    }
//
    void setNotificationOutStream(AbstractOutStream<ZP> *out) {
        this->notificationOutStream = out;
    }
    ScriptSpace<ZP>** getScriptSpaces() {
        return scriptSpaces;
    }
    uint8_t getScriptSpaceCount() {
        return scriptSpaceCount;
    }

};
}

#endif /* SRC_MAIN_C___ZSCRIPT_ZSCRIPT_HPP_ */
