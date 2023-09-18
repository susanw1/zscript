/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_ZSCRIPT_HPP_
#define SRC_MAIN_C___ZSCRIPT_ZSCRIPT_HPP_

#define ZSCRIPT_HPP_INCLUDED

#include "ZscriptIncludes.hpp"
#include "execution/LockSet.hpp"
#include "execution/LockSystem.hpp"
#include "execution/ZscriptExecutor.hpp"
#include "ZscriptChannel.hpp"

#ifdef ZSCRIPT_SUPPORT_SCRIPT_SPACE

#include "scriptSpace/ScriptSpace.hpp"

#endif

#ifdef ZSCRIPT_SUPPORT_SCRIPT_SPACE
#ifndef ZSCRIPT_SUPPORT_NOTIFICATIONS
#error Cannot use script space without notifications
#endif
#endif
#ifdef ZSCRIPT_SUPPORT_ADDRESSING
#ifndef ZSCRIPT_SUPPORT_NOTIFICATIONS
#error Cannot use addressing without notifications
#endif
#endif

namespace Zscript {

template<class ZP>
class Zscript {
public:
    static Zscript<ZP> zscript;

private:

    Zscript<ZP> *operator&() {
        return this;
    }

    Zscript<ZP> operator*() {
        return this;
    }

    ZscriptChannel<ZP> **channels = NULL;

#ifdef ZSCRIPT_SUPPORT_NOTIFICATIONS
    GenericCore::ZscriptNotificationSource<ZP> **notifSources = NULL;

#ifdef ZSCRIPT_SUPPORT_SCRIPT_SPACE
    ScriptSpace<ZP> **scriptSpaces = NULL;
    uint8_t scriptSpaceCount = 0;
#endif
    uint8_t notifSrcCount = 0;
    uint8_t notificationChannelIndex = 0xFF;
#endif
    GenericCore::LockSystem<ZP> locks;
    uint8_t channelCount = 0;

    Zscript() {
    }

public:

//
    void setChannels(ZscriptChannel<ZP> **channels, uint8_t channelCount) {
        this->channels = channels;
        this->channelCount = channelCount;
        for (uint8_t i = 0; i < channelCount; ++i) {
            channels[i]->setChannelIndex(i);
        }
    }

#ifdef ZSCRIPT_SUPPORT_NOTIFICATIONS

    void setNotificationSources(GenericCore::ZscriptNotificationSource<ZP> **notifSources, uint8_t notifSrcCount) {
        this->notifSources = notifSources;
        this->notifSrcCount = notifSrcCount;
    }

#endif

#ifdef ZSCRIPT_SUPPORT_SCRIPT_SPACE

    void setScriptSpaces(ScriptSpace<ZP> **scriptSpaces, uint8_t scriptSpaceCount) {
        this->scriptSpaces = scriptSpaces;
        this->scriptSpaceCount = scriptSpaceCount;
    }

#endif

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
        return GenericCore::ZscriptExecutor<ZP>::progress(channels, channelCount

#ifdef ZSCRIPT_SUPPORT_NOTIFICATIONS
                , notifSources, notifSrcCount
#endif
        );
    }

#ifdef ZSCRIPT_SUPPORT_NOTIFICATIONS

    AbstractOutStream<ZP> *getNotificationOutStream() {
        return channels[notificationChannelIndex]->getOutStream();
    }

    bool hasNotificationOutStream() {
        return notificationChannelIndex != 0xFF;
    }

    void setNotificationChannelIndex(uint8_t index) {
        if (index < channelCount && channels[index]->canBeNotifChannel()) {
            notificationChannelIndex = index;
        } else {
            notificationChannelIndex = 0xFF;
        }
    }

    uint8_t getNotificationChannelIndex() {
        return notificationChannelIndex;
    }

#endif

#ifdef ZSCRIPT_SUPPORT_SCRIPT_SPACE

    ScriptSpace<ZP> **getScriptSpaces() {
        return scriptSpaces;
    }

    uint8_t getScriptSpaceCount() {
        return scriptSpaceCount;
    }

#endif

    uint8_t getChannelCount() {
        return channelCount;
    }

    ZscriptChannel<ZP> **getChannels() {
        return channels;
    }

    void resetChannel(uint8_t index) {
        channels[index]->reset();
#ifdef ZSCRIPT_SUPPORT_NOTIFICATIONS
        if (notificationChannelIndex == index) {
            notificationChannelIndex = 0xFF;
        }
#endif
    }

};

template<class ZP>
Zscript<ZP> Zscript<ZP>::zscript;
}
#endif /* SRC_MAIN_C___ZSCRIPT_ZSCRIPT_HPP_ */
