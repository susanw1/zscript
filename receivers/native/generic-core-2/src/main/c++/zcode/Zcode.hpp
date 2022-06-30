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
private:
    ZcodeLocks<ZP> locks; // the stupid ordering here is to make the #if's happy without increasing the size...
    uint8_t channelCount = 0;
    uint8_t moduleNum = 0;
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

#ifdef ZCODE_SUPPORT_ADDRESSING
    ZcodeAddressRouter<ZP> *addrRouter;
#endif
    ZcodeCommandChannel<ZP> **channels = NULL;
    //    const char *configFailureState = NULL;
    ZcodeModule<ZP> **modules = NULL;

public:

#ifdef ZCODE_GENERATE_NOTIFICATIONS
#ifdef ZCODE_SUPPORT_ADDRESSING
    Zcode(ZcodeBusInterruptSource<ZP> **interruptSources, uint8_t interruptSourceNum, ZcodeAddressRouter<ZP> *addrRouter) :
             notificationManager(this, interruptSources, interruptSourceNum),

#ifdef ZCODE_SUPPORT_SCRIPT_SPACE
            space(this),
#endif
#ifdef ZCODE_SUPPORT_DEBUG
                    debug(),
#endif
                    addrRouter(addrRouter) {
    }
#endif

    Zcode(ZcodeBusInterruptSource<ZP> **interruptSources, uint8_t interruptSourceNum) :
             notificationManager(this, interruptSources, interruptSourceNum)

#ifdef ZCODE_SUPPORT_SCRIPT_SPACE
            ,space(this)
#endif
#ifdef ZCODE_SUPPORT_DEBUG
                    ,debug()
#endif


#ifdef ZCODE_SUPPORT_ADDRESSING
    , addrRouter(NULL)
#endif
    {
    }
#ifdef ZCODE_SUPPORT_ADDRESSING
    Zcode(ZcodeAddressRouter<ZP> *addrRouter) :
             notificationManager(this, NULL, 0),

#ifdef ZCODE_SUPPORT_SCRIPT_SPACE
            space(this),
#endif
#ifdef ZCODE_SUPPORT_DEBUG
                    debug(),
#endif
                    addrRouter(addrRouter) {
    }
#endif
#endif

    Zcode() :
            locks()

#ifdef ZCODE_GENERATE_NOTIFICATIONS
    , notificationManager(this, NULL, 0)
#endif
#ifdef ZCODE_SUPPORT_SCRIPT_SPACE
    , space(this)
#endif
#ifdef ZCODE_SUPPORT_DEBUG
    , debug()
#endif

#ifdef ZCODE_SUPPORT_ADDRESSING
    , addrRouter(NULL)
#endif
    {
    }

#ifdef ZCODE_SUPPORT_ADDRESSING
    ZcodeAddressRouter<ZP>* getAddressRouter() {
        return addrRouter;
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

    void setModules(ZcodeModule<ZP> **modules, uint8_t moduleNum) {
        this->modules = modules;
        this->moduleNum = moduleNum;
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
        ZcodeParser<ZP>::parseNext(this);
        ZcodeRunner<ZP>::runNext(this);
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

    ZcodeModule<ZP>** getModules() {
        return modules;
    }

    uint8_t getModuleNumber() {
        return moduleNum;
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

