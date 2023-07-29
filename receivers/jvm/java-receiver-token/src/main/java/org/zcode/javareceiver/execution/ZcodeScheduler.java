package org.zcode.javareceiver.execution;

import java.util.List;

import org.zcode.javareceiver.core.Zcode;

public class ZcodeScheduler {

    int decide(Zcode zcode, List<ZcodeAction> possibilities) {
        // TODO: a better one
        for (int i = 0; i < possibilities.size(); i++) {
            ZcodeAction action = possibilities.get(i);
            if (!action.isEmptyAction() && action.canLock(zcode)) {
                return i;
            }
        }
        return 0;
    }
}
