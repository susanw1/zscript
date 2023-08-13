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
#include "../notifications/ZscriptNotificationSource.hpp"
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

    static bool progress(ZscriptChannel<ZP> **channels, GenericCore::ZscriptNotificationSource<ZP> **notifSources, uint8_t channelCount,
            uint8_t notifSrcCount) {
        if (channelCount == 0 && notifSrcCount == 0) {
            return false;
        }
        ZscriptAction<ZP> possibleActions[channelCount + notifSrcCount];

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
        for (uint8_t i = 0; i < notifSrcCount; ++i) {

            ZscriptAction<ZP> action = notifSources[i]->getAction();
            if (action.isEmptyAction()) {
                action.performAction( NULL);
            } else {
                hasNonWait = true;
            }
            possibleActions[channelCount + i] = action;
        }

        uint16_t indexToExec = decide(possibleActions, channelCount + notifSrcCount);
        ZscriptAction<ZP> action = possibleActions[indexToExec];

        if (!action.isEmptyAction() && action.lock()) {
            if (indexToExec >= channelCount) {
                if (Zscript<ZP>::zscript.getNotificationOutStream() == NULL) {
                    GenericCore::DiscardingOutStream<ZP> out;
                    action.performAction(&out);
                } else {
                    action.performAction(Zscript<ZP>::zscript.getNotificationOutStream());
                }
            } else {
                action.performAction(channels[indexToExec]->getOutStream());
            }
        }
        return hasNonWait;
    }

};
}
}

#endif /* SRC_MAIN_C___ZSCRIPT_EXECUTION_EXECUTOR_HPP_ */
