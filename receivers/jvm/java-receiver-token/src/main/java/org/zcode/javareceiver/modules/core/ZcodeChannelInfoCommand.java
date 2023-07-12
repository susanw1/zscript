package org.zcode.javareceiver.modules.core;

import java.util.List;

import org.zcode.javareceiver.core.Zcode;
import org.zcode.javareceiver.core.ZcodeChannel;
import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.execution.ZcodeCommandContext;

public class ZcodeChannelInfoCommand {

    public static void execute(ZcodeCommandContext ctx) {
        List<ZcodeChannel> chs = Zcode.getZcode().getChannels();

        int current = 0;
        for (ZcodeChannel ch : chs) {
            if (ch.getOutStream() == ctx.getOutStream()) {
                break;
            }
            current++;
        }
        ctx.getOutStream().writeField('C', chs.size());
        if (current <= chs.size()) {
            ctx.getOutStream().writeField('U', current);
        }
        int target = ctx.getField((byte) 'C', current);
        if (target >= chs.size()) {
            ctx.status(ZcodeStatus.COMMAND_FORMAT_ERROR);
            return;
        }
        chs.get(target).channelInfo(ctx);
    }

}
