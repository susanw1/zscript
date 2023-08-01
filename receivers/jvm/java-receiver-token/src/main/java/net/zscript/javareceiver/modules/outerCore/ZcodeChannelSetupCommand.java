package net.zscript.javareceiver.modules.outerCore;

import java.util.List;

import net.zscript.javareceiver.core.ZcodeChannel;
import net.zscript.javareceiver.core.ZcodeStatus;
import net.zscript.javareceiver.execution.ZcodeCommandContext;

public class ZcodeChannelSetupCommand {

    public static void execute(ZcodeCommandContext ctx) {
        List<ZcodeChannel> chs = ctx.getZcode().getChannels();

        int current = ctx.getChannelIndex();
        int target  = ctx.getField((byte) 'C', current);
        if (target >= chs.size()) {
            ctx.status(ZcodeStatus.VALUE_OUT_OF_RANGE);
            return;
        }
        chs.get(target).channelSetup(ctx);
        if (ctx.hasField((byte) 'N')) {
            ctx.getZcode().setNotificationOutStream(ctx.getZcode().getChannels().get(target).getOutStream(ctx.getZcode()));
        }
    }

}
