package net.zscript.javareceiver.execution;

import java.util.ArrayList;
import java.util.List;

import net.zscript.javareceiver.core.Zcode;

public class ZcodeExecutor {
    private final Zcode zcode;

    private final ZcodeScheduler scheduler = new ZcodeScheduler();

    public ZcodeExecutor(Zcode zcode) {
        this.zcode = zcode;
    }

    public boolean progress(List<? extends ActionSource> sources) {
        if (sources.isEmpty()) {
            return false;
        }
        List<ZcodeAction> possibleActions = new ArrayList<>();

        boolean hasNonWait = false;
        for (ActionSource source : sources) {
            ZcodeAction action = source.getAction();
            if (action.isEmptyAction()) {
                action.performAction(zcode, null);
            } else {
                hasNonWait = true;
            }
            possibleActions.add(action);
        }

        int         indexToExec = scheduler.decide(zcode, possibleActions);
        ZcodeAction action      = possibleActions.get(indexToExec);

        if (!action.isEmptyAction() && action.lock(zcode)) {
            action.performAction(zcode, sources.get(indexToExec).getOutStream(zcode));
        }
        return hasNonWait;
    }

}
