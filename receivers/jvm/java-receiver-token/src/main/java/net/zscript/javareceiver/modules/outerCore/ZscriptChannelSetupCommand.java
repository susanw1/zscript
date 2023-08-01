package net.zscript.javareceiver.modules.outerCore;

import java.util.List;

import net.zscript.javareceiver.core.ZscriptChannel;
import net.zscript.javareceiver.core.ZscriptStatus;
import net.zscript.javareceiver.execution.CommandContext;

public class ZscriptChannelSetupCommand {

    public static void execute(CommandContext ctx) {
        List<ZscriptChannel> chs = ctx.getZcode().getChannels();

        int current = ctx.getChannelIndex();
        int target  = ctx.getField((byte) 'C', current);
        if (target >= chs.size()) {
            ctx.status(ZscriptStatus.VALUE_OUT_OF_RANGE);
            return;
        }
        chs.get(target).channelSetup(ctx);
        if (ctx.hasField((byte) 'N')) {
            ctx.getZcode().setNotificationOutStream(ctx.getZcode().getChannels().get(target).getOutStream(ctx.getZcode()));
        }
    }

}
