package org.zcode.javareceiver.modules.outerCore;

import java.util.List;

import org.zcode.javareceiver.core.ZcodeChannel;
import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.execution.ZcodeCommandContext;

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
    }

}
