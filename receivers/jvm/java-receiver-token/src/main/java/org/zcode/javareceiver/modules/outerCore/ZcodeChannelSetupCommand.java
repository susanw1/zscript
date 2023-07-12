package org.zcode.javareceiver.modules.outerCore;

import java.util.List;

import org.zcode.javareceiver.core.Zcode;
import org.zcode.javareceiver.core.ZcodeChannel;
import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.execution.ZcodeCommandContext;

public class ZcodeChannelSetupCommand {

    public static void execute(ZcodeCommandContext ctx) {
        List<ZcodeChannel> chs = Zcode.getZcode().getChannels();

        int current = 0;
        for (ZcodeChannel ch : chs) {
            if (ch.getOutStream() == ctx.getOutStream()) {
                break;
            }
            current++;
        }
        int target = ctx.getField((byte) 'C', current);
        if (target >= chs.size()) {
            ctx.status(ZcodeStatus.COMMAND_FORMAT_ERROR);
            return;
        }
        chs.get(target).channelSetup(ctx);
    }

}
