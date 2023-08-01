package net.zscript.javareceiver.execution;

import java.util.ArrayList;
import java.util.List;

import net.zscript.javareceiver.core.Zscript;

public class ZscriptExecutor {
    private final Zscript zscript;

    private final ZscriptScheduler scheduler = new ZscriptScheduler();

    public ZscriptExecutor(Zscript zscript) {
        this.zscript = zscript;
    }

    public boolean progress(List<? extends ActionSource> sources) {
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

        int         indexToExec = scheduler.decide(zscript, possibleActions);
        ZscriptAction action      = possibleActions.get(indexToExec);

        if (!action.isEmptyAction() && action.lock(zscript)) {
            action.performAction(zscript, sources.get(indexToExec).getOutStream(zscript));
        }
        return hasNonWait;
    }

}
