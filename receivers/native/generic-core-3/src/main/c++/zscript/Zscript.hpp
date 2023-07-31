/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_ZSCRIPT_HPP_
#define SRC_MAIN_C___ZSCRIPT_ZSCRIPT_HPP_

#include "ZscriptIncludes.hpp"
namespace Zscript {

template<class ZP>
class Zscript {
private:
    ZcodeModuleRegistry moduleRegistry = new ZcodeModuleRegistry();
    ZcodeLocks locks = new ZcodeLocks();
    List<ZcodeChannel> channels = new ArrayList<>();
    List<ActionSource> sources = new ArrayList<>();
    ZcodeExecutor executor;

public:
    Zscript() {
//        executor = new ZcodeExecutor(this);
    }

    void addModule(ZcodeModule m) {
        moduleRegistry.addModule(m);
    }

    void addChannel(ZcodeChannel ch) {
        ch.setChannelIndex((byte) channels.size());
        channels.add(ch);
        sources.add(ch);
    }

    void addNotificationSource(ZcodeNotificationSource s) {
        sources.add(s);
    }

    bool lock(LockSet<ZP> *locks) {
        return locks.lock(l);
    }

    bool canLock(LockSet<ZP> *locks) {
        return locks.canLock(l);
    }

    void unlock(LockSet<ZP> *locks) {
        locks.unlock(l);
    }

    bool progress() {
        for (ZcodeChannel channel : channels) {
            channel.moveAlong();
        }
        return executor.progress(sources);
    }

    List<ZcodeChannel> getChannels() {
        return channels;
    }

    ZcodeModuleRegistry getModuleRegistry() {
        return moduleRegistry;
    }

    ZcodeOutStream getNotificationOutStream() {
        return notificationOutStream;
    }

    void setNotificationOutStream(ZcodeOutStream out) {
        this.notificationOutStream = out;
    }

};
}

#endif /* SRC_MAIN_C___ZSCRIPT_ZSCRIPT_HPP_ */
