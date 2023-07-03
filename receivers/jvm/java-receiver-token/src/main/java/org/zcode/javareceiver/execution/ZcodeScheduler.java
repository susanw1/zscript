package org.zcode.javareceiver.execution;

import java.util.List;

import org.zcode.javareceiver.semanticParser.ZcodeAction;

public class ZcodeScheduler {
    int decide(List<ZcodeAction> possibilities) {
        // TODO: a better one
        for (int i = 0; i < possibilities.size(); i++) {
            if (possibilities.get(i).needsPerforming()) {
                return i;
            }
        }
        return 0;
    }
}
