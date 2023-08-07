package net.zscript.javareceiver.modules.scriptSpaces;

import java.util.OptionalInt;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

import net.zscript.javareceiver.core.ZscriptStatus;
import net.zscript.javareceiver.execution.CommandContext;
import net.zscript.javareceiver.semanticParser.ContextView.AsyncActionNotifier;

public class ZscriptSleepCommand {
    private static final ScheduledExecutorService service = Executors.newSingleThreadScheduledExecutor();

    public static void execute(CommandContext ctx) {
        OptionalInt delay = ctx.getField('M');
        if (!delay.isPresent()) {
            ctx.status(ZscriptStatus.MISSING_KEY);
            return;
        }
        int                 msDelay  = delay.getAsInt();
        AsyncActionNotifier callback = ctx.getAsyncActionNotifier();
        ctx.commandNotComplete();
        service.schedule(() -> {
            callback.notifyNeedsAction();
        }, msDelay, TimeUnit.MILLISECONDS);
    }

    public static void moveAlong(CommandContext ctx) {
        ctx.commandComplete();
    }

}
