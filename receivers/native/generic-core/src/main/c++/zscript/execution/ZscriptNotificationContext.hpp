/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_EXECUTION_ZSCRIPTNOTIFICATIONCONTEXT_HPP_
#define SRC_MAIN_C___ZSCRIPT_EXECUTION_ZSCRIPTNOTIFICATIONCONTEXT_HPP_

#include "../ZscriptIncludes.hpp"
#include "CommandOutStream.hpp"

namespace Zscript {

namespace GenericCore {

template<class ZP>
class ZscriptNotificationSource;
}

template<class ZP>
class ZscriptNotificationContext {
    GenericCore::ZscriptNotificationSource<ZP> *source;

public:
    ZscriptNotificationContext(GenericCore::ZscriptNotificationSource<ZP> *source) :
            source(source) {
        source->setNotificationComplete(true);
    }

    void notificationComplete() {
        source->setNotificationComplete(true);
    }

    void notificationNotComplete() {
        source->setNotificationComplete(false);
    }

    bool isNotificationComplete() {
        return source->isNotificationComplete();
    }

    bool isAddressing() {
        return source->isAddressing();
    }

    int getID() {
        return source->getID();
    }

    NotificationOutStream<ZP> getOutStream() {
        return NotificationOutStream<ZP>(Zscript<ZP>::zscript.getNotificationOutStream());
    }

    AsyncActionNotifier<ZP> getAsyncActionNotifier() {
        return source->getAsyncActionNotifier();
    }
};

}

#endif /* SRC_MAIN_C___ZSCRIPT_EXECUTION_ZSCRIPTNOTIFICATIONCONTEXT_HPP_ */
