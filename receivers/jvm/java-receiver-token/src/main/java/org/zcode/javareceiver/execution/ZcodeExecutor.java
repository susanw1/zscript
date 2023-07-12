package org.zcode.javareceiver.execution;

import java.util.ArrayList;
import java.util.List;

import org.zcode.javareceiver.core.Zcode;
import org.zcode.javareceiver.core.ZcodeChannel;
import org.zcode.javareceiver.semanticParser.ZcodeAction;

public class ZcodeExecutor {
    private final Zcode zcode;

    private final ZcodeScheduler scheduler = new ZcodeScheduler();

    public ZcodeExecutor(Zcode zcode) {
        this.zcode = zcode;
    }

    public void progress(List<ZcodeChannel> channels) {
        List<ZcodeAction> possibleActions = new ArrayList<>();
        for (ZcodeChannel ch : channels) {
            possibleActions.add(ch.getParser().getAction());
        }

        int         toExec = scheduler.decide(zcode, possibleActions);
        ZcodeAction action = possibleActions.get(toExec);

        if (action.needsPerforming() && action.lock(zcode)) {
            action.performAction(zcode, channels.get(toExec).getOutStream());
        }
    }

}
