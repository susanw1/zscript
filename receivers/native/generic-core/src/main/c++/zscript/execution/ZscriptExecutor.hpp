/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_EXECUTION_EXECUTOR_HPP_
#define SRC_MAIN_C___ZSCRIPT_EXECUTION_EXECUTOR_HPP_
#include "../ZscriptIncludes.hpp"
#include "../ZscriptChannel.hpp"
#ifdef ZSCRIPT_SUPPORT_NOTIFICATIONS
#include "../notifications/ZscriptNotificationSource.hpp"
#endif
#include "DiscardingOutStream.hpp"

namespace Zscript {
namespace GenericCore {
template<class ZP>
class ZscriptExecutor {
public:
    static uint16_t decide(ZscriptAction<ZP> *possibleActions, uint16_t actions) {
        // TODO: a better one
        for (uint16_t i = 0; i < actions; i++) {
            ZscriptAction<ZP> action = possibleActions[i];
            if (!action.isEmptyAction() && action.canLock()) {
                return i;
            }
        }
        return 0;
    }

    static bool progress(ZscriptChannel<ZP> **channels, uint8_t channelCount

#ifdef ZSCRIPT_SUPPORT_NOTIFICATIONS
    , GenericCore::ZscriptNotificationSource<ZP> **notifSources,
            uint8_t notifSrcCount
#endif
            ) {
        uint8_t totalCount = channelCount

#ifdef ZSCRIPT_SUPPORT_NOTIFICATIONS
                + notifSrcCount
#endif
        ;
        if (totalCount == 0) {
            return false;
        }
        ZscriptAction<ZP> possibleActions[totalCount];

        bool hasNonWait = false;
        for (uint8_t i = 0; i < channelCount; ++i) {

            ZscriptAction<ZP> action = channels[i]->getParser()->getAction();
            if (action.isEmptyAction()) {
                action.performEmptyAction();
            } else {
                hasNonWait = true;
            }
            possibleActions[i] = action;
        }
#ifdef ZSCRIPT_SUPPORT_NOTIFICATIONS
        for (uint8_t i = 0; i < notifSrcCount; ++i) {

            ZscriptAction<ZP> action = notifSources[i]->getAction();
            if (action.isEmptyAction()) {
                action.performAction( NULL);
            } else {
                hasNonWait = true;
            }
            possibleActions[channelCount + i] = action;
        }
#endif

        uint16_t indexToExec = decide(possibleActions, totalCount);
        ZscriptAction<ZP> action = possibleActions[indexToExec];

        if (!action.isEmptyAction() && action.lock()) {
#ifdef ZSCRIPT_SUPPORT_NOTIFICATIONS
            if (indexToExec >= channelCount) {
                if (Zscript<ZP>::zscript.getNotificationOutStream() == NULL) {
                    GenericCore::DiscardingOutStream<ZP> out;
                    action.performAction(&out);
                } else {
                    action.performAction(Zscript<ZP>::zscript.getNotificationOutStream());
                }
            } else {
#endif
            action.performAction(channels[indexToExec]->getOutStream());
#ifdef ZSCRIPT_SUPPORT_NOTIFICATIONS
            }
#endif
        }
        return hasNonWait;
    }

};
}
}

#endif /* SRC_MAIN_C___ZSCRIPT_EXECUTION_EXECUTOR_HPP_ */
