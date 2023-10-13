package net.zscript.javareceiver.execution;

import java.util.List;

import net.zscript.javareceiver.core.Zscript;

public class ZscriptScheduler {

    int decide(Zscript zscript, List<ZscriptAction> possibilities) {
        // TODO: a better one
        for (int i = 0; i < possibilities.size(); i++) {
            ZscriptAction action = possibilities.get(i);
            if (!action.isEmptyAction() && action.canLock(zscript)) {
                return i;
            }
        }
        return 0;
    }
}
