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

namespace Zscript {

template<class ZP>
class Zscript {
private:
    //    ZcodeModuleRegistry moduleRegistry = new ZcodeModuleRegistry();
    GenericCore::LockSystem<ZP> locks;
    //    List<ZcodeChannel> channels = new ArrayList<>();
//    List<ActionSource> sources = new ArrayList<>();
//    ZcodeExecutor executor;

public:
    Zscript() {
//        executor = new ZcodeExecutor(this);
    }

//    void addModule(ZcodeModule m) {
//        moduleRegistry.addModule(m);
//    }
//
//    void addChannel(ZcodeChannel ch) {
//        ch.setChannelIndex((byte) channels.size());
//        channels.add(ch);
//        sources.add(ch);
//    }
//
//    void addNotificationSource(ZcodeNotificationSource s) {
//        sources.add(s);
//    }

    bool lock(GenericCore::LockSet<ZP> *l) {
        return locks.lock(l);
    }

    bool canLock(GenericCore::LockSet<ZP> *l) {
        return locks.canLock(l);
    }

    void unlock(GenericCore::LockSet<ZP> *l) {
        locks.unlock(l);
    }

//    bool progress() {
//        for (ZcodeChannel channel : channels) {
//            channel.moveAlong();
//        }
//        return executor.progress(sources);
//    }
//
//    List<ZcodeChannel> getChannels() {
//        return channels;
//    }

//    ZcodeModuleRegistry getModuleRegistry() {
//        return moduleRegistry;
//    }

//    ZcodeOutStream getNotificationOutStream() {
//        return notificationOutStream;
//    }
//
//    void setNotificationOutStream(ZcodeOutStream out) {
//        this.notificationOutStream = out;
//    }

};
}

#endif /* SRC_MAIN_C___ZSCRIPT_ZSCRIPT_HPP_ */
