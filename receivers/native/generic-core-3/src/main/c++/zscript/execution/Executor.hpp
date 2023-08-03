/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_EXECUTION_EXECUTOR_HPP_
#define SRC_MAIN_C___ZSCRIPT_EXECUTION_EXECUTOR_HPP_

namespace Zscript {
namespace GenericCore {
class ZscriptExecutor {
public:

    static bool progress(List<? extends ActionSource> sources) {
        if (sources.isEmpty()) {
            return false;
        }
        List<ZscriptAction> possibleActions = new ArrayList<>();

        boolean hasNonWait = false;
        for (ActionSource source : sources) {
            ZscriptAction action = source.getAction();
            if (action.isEmptyAction()) {
                action.performAction(zscript, null);
            } else {
                hasNonWait = true;
            }
            possibleActions.add(action);
        }

        int indexToExec = scheduler.decide(zscript, possibleActions);
        ZscriptAction action = possibleActions.get(indexToExec);

        if (!action.isEmptyAction() && action.lock(zscript)) {
            action.performAction(zscript, sources.get(indexToExec).getOutStream(zscript));
        }
        return hasNonWait;
    }

};
}
}

#endif /* SRC_MAIN_C___ZSCRIPT_EXECUTION_EXECUTOR_HPP_ */
