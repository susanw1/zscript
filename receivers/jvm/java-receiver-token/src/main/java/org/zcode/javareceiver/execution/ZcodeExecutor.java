package org.zcode.javareceiver.execution;

import java.util.ArrayList;
import java.util.List;

import org.zcode.javareceiver.core.ZcodeChannel;
import org.zcode.javareceiver.semanticParser.ZcodeAction;

public class ZcodeExecutor {

    List<ZcodeChannel> channels  = new ArrayList<>();
    ZcodeScheduler     scheduler = new ZcodeScheduler();

    public void progress() {
        List<ZcodeAction> possibleActions = new ArrayList<>();
        for (ZcodeChannel ch : channels) {
            possibleActions.add(ch.getParser().getAction());
        }
        int toExec = scheduler.decide(possibleActions);
        if (possibleActions.get(toExec).needsPerforming()) {
            possibleActions.get(toExec).performAction(channels.get(toExec).getOutStream());
        }
    }

}
