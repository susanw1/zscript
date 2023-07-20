package org.zcode.javareceiver.execution;

import java.util.List;

import org.zcode.javareceiver.core.Zcode;
import org.zcode.javareceiver.semanticParser.ZcodeAction;

public class ZcodeScheduler {

    int decide(Zcode zcode, List<ZcodeAction> possibilities) {
        // TODO: a better one
        for (int i = 0; i < possibilities.size(); i++) {
            ZcodeAction r = possibilities.get(i);
            if (possibilities.get(i).needsPerforming() && possibilities.get(i).canLock(zcode)) {
                return i;
            }
        }
        return 0;
    }
}
