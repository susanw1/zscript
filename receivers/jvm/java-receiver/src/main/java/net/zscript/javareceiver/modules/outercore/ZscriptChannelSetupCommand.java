package net.zscript.javareceiver.modules.outercore;

import java.util.List;

import net.zscript.javareceiver.core.ZscriptChannel;
import net.zscript.javareceiver.execution.CommandContext;
import net.zscript.model.components.ZscriptStatus;

public class ZscriptChannelSetupCommand {

    public static void execute(CommandContext ctx) {
        List<ZscriptChannel> chs = ctx.getZscript().getChannels();

        int current = ctx.getChannelIndex();
        int target  = ctx.getField((byte) 'C', current);
        if (target >= chs.size()) {
            ctx.status(ZscriptStatus.VALUE_OUT_OF_RANGE);
            return;
        }
        chs.get(target).channelSetup(ctx);
        if (ctx.hasField((byte) 'N')) {
            ctx.getZscript().setNotificationOutStream(ctx.getZscript().getChannels().get(target).getOutStream(ctx.getZscript()));
        }
    }

}
