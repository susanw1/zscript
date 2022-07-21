/*
 * Zcode.hpp
 *
 *  Created on: 8 Sep 2020
 *      Author: robert
 */

#ifdef SRC_TEST_CPP_ZCODE_ZCODE_HPP_
#error Cannot include Zcode.hpp twice
#endif
#define SRC_TEST_CPP_ZCODE_ZCODE_HPP_

#ifndef ZCODE_GENERATE_NOTIFICATIONS
#ifdef ZCODE_SUPPORT_SCRIPT_SPACE
#error Cannot use script space without notifications
#endif
#endif
#ifndef ZCODE_SUPPORT_SCRIPT_SPACE
#ifdef ZCODE_SUPPORT_INTERRUPT_VECTOR
#error Cannot use interrupt vectors without script space
#endif
#endif
#ifndef ZCODE_GENERATE_NOTIFICATIONS
#ifdef ZCODE_SUPPORT_ADDRESSING
#error Cannot use addressing without notifications
#endif
#endif

#include "ZcodeIncludes.hpp"
#include "parsing/ZcodeParser.hpp"
#include "parsing/ZcodeChannelCommandSlot.hpp"
#include "channels/ZcodeCommandChannel.hpp"
#ifdef ZCODE_SUPPORT_SCRIPT_SPACE
#include "scriptspace/ZcodeScriptSpace.hpp"
#endif
#ifdef ZCODE_GENERATE_NOTIFICATIONS
#include "ZcodeNotificationManager.hpp"
#endif
#include "running/ZcodeRunner.hpp"
#ifdef ZCODE_SUPPORT_DEBUG
#include "ZcodeDebugOutput.hpp"
#endif
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
public:
    static Zcode<ZP> zcode;

private:
    ZcodeLocks<ZP> locks; // the stupid ordering here is to make the #if's happy without increasing the size...
    uint8_t channelCount = 0;
    bool activated = false;

#ifdef ZCODE_GENERATE_NOTIFICATIONS
    ZcodeNotificationManager<ZP> notificationManager;
#endif
#ifdef ZCODE_SUPPORT_SCRIPT_SPACE
    ZcodeScriptSpace<ZP> space;
#endif

#ifdef ZCODE_SUPPORT_DEBUG
    ZcodeDebugOutput<ZP> debug;
#endif

    ZcodeCommandChannel<ZP> **channels = NULL;
    //    const char *configFailureState = NULL;

public:

    Zcode() :
            locks()

#ifdef ZCODE_GENERATE_NOTIFICATIONS
    , notificationManager()
#endif
#ifdef ZCODE_SUPPORT_SCRIPT_SPACE
    , space()
#endif
#ifdef ZCODE_SUPPORT_DEBUG
    , debug()
#endif

    {
    }

#ifdef ZCODE_GENERATE_NOTIFICATIONS

    void setInterruptSources(ZcodeBusInterruptSource<ZP> **sources, uint8_t sourceNum) {
        notificationManager.setBusInterruptSources(sources, sourceNum);
    }
#endif

    bool isActivated() {
        return activated;
    }
    void setActivated() {
        activated = true;
    }
    void deactivate() {
        activated = false;
    }

    void setChannels(ZcodeCommandChannel<ZP> **channels, uint8_t channelNum) {
        this->channels = channels;
        this->channelCount = channelNum;
    }

//    void configFail(const char *configFailureState) {
//        this->configFailureState = configFailureState;
//    }
//
//    const char* getConfigFailureState() {
//        return configFailureState;
//    }

    void progressZcode() {

#ifdef ZCODE_SUPPORT_DEBUG
        debug.attemptFlush();
#endif
#ifdef ZCODE_GENERATE_NOTIFICATIONS
        notificationManager.manageNotifications();
#endif
        ZcodeParser<ZP>::parseNext();
        ZcodeRunner<ZP>::runNext();
    }

#ifdef ZCODE_SUPPORT_SCRIPT_SPACE
    ZcodeScriptSpace<ZP>* getSpace() {
        return &space;
    }
#endif

    ZcodeCommandChannel<ZP>* getChannel(uint8_t index) {
#ifdef ZCODE_SUPPORT_SCRIPT_SPACE
#ifdef ZCODE_SUPPORT_INTERRUPT_VECTOR
        if (index == 0) {
            return notificationManager.getVectorChannel()->getChannel();

        } else
#endif // ZCODE_SUPPORT_INTERRUPT_VECTOR
            if (index < channelCount
#ifdef ZCODE_SUPPORT_INTERRUPT_VECTOR
                    + 1
#endif
                    ) {
#endif
        return channels[index

#if defined(ZCODE_SUPPORT_SCRIPT_SPACE) && defined(ZCODE_SUPPORT_INTERRUPT_VECTOR)
                        - 1
#endif
        ];
#ifdef ZCODE_SUPPORT_SCRIPT_SPACE
        }
        else {
            return space.getChannel((uint8_t) (index - channelCount
#ifdef ZCODE_SUPPORT_INTERRUPT_VECTOR
                    - 1 // this compensates for the notification channel on the front.
#endif
            ));
        }
#endif
    }

    ZcodeLocks<ZP>* getLocks() {
        return &locks;
    }

    uint8_t getChannelCount() {
        return (uint8_t) (channelCount

#ifdef ZCODE_SUPPORT_SCRIPT_SPACE
            + space.getChannelCount()
#ifdef ZCODE_SUPPORT_INTERRUPT_VECTOR
            + 1
#endif
#endif
        );
    }

#ifdef ZCODE_SUPPORT_DEBUG
    ZcodeDebugOutput<ZP>& getDebug() {
        return debug;
    }
#endif

#ifdef ZCODE_GENERATE_NOTIFICATIONS
    ZcodeNotificationManager<ZP>* getNotificationManager() {
        return &notificationManager;
    }
#endif

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

template<class ZP>
Zcode<ZP> Zcode<ZP>::zcode;
