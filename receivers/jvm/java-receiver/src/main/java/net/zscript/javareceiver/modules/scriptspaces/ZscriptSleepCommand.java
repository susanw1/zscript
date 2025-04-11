package net.zscript.javareceiver.modules.scriptspaces;

import java.util.OptionalInt;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

import net.zscript.javareceiver.execution.CommandContext;
import net.zscript.javareceiver.semanticparser.ContextView.AsyncActionNotifier;
import net.zscript.model.components.ZscriptStatus;

public class ZscriptSleepCommand {
    private static final ScheduledExecutorService service = Executors.newSingleThreadScheduledExecutor();

    public static void execute(CommandContext ctx) {
        OptionalInt delay = ctx.getFieldValue('M');
        if (delay.isEmpty()) {
            ctx.status(ZscriptStatus.MISSING_KEY);
            return;
        }
        int                 msDelay  = delay.getAsInt();
        AsyncActionNotifier callback = ctx.getAsyncActionNotifier();
        ctx.commandNotComplete();
        service.schedule(callback::notifyNeedsAction, msDelay, TimeUnit.MILLISECONDS);
    }

    public static void moveAlong(CommandContext ctx) {
        ctx.commandComplete();
    }

}
