package org.zcode.javareceiver.execution;

import java.util.ArrayList;
import java.util.List;

import org.zcode.javareceiver.core.Zcode;

public class ZcodeExecutor {
    private final Zcode zcode;

    private final ZcodeScheduler scheduler = new ZcodeScheduler();

    public ZcodeExecutor(Zcode zcode) {
        this.zcode = zcode;
    }

    public void progress(List<? extends ActionSource> sources) {
        if (sources.isEmpty()) {
            return;
        }
        List<ZcodeAction> possibleActions = new ArrayList<>();

        for (ActionSource source : sources) {
            ZcodeAction action = source.getAction();
            if (action.isEmptyAction()) {
                action.performAction(zcode, null);
            }
            possibleActions.add(action);
        }

        int         indexToExec = scheduler.decide(zcode, possibleActions);
        ZcodeAction action      = possibleActions.get(indexToExec);

        if (!action.isEmptyAction() && action.lock(zcode)) {
            action.performAction(zcode, sources.get(indexToExec).getOutStream(zcode));
        }
    }

}
