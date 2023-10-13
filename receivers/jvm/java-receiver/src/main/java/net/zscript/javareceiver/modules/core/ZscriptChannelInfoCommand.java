package net.zscript.javareceiver.modules.core;

import java.util.List;

import net.zscript.javareceiver.core.ZscriptChannel;
import net.zscript.javareceiver.execution.CommandContext;
import net.zscript.model.components.ZscriptStatus;

public class ZscriptChannelInfoCommand {

    public static void execute(CommandContext ctx) {
        List<ZscriptChannel> chs = ctx.getZscript().getChannels();

        int current = ctx.getChannelIndex();

        int target = ctx.getField((byte) 'C', current);
        if (target >= chs.size()) {
            ctx.status(ZscriptStatus.VALUE_OUT_OF_RANGE);
            return;
        }
        ctx.getOutStream().writeField('C', chs.size());
        if (current <= chs.size()) {
            ctx.getOutStream().writeField('U', current);
        }
        chs.get(target).channelInfo(ctx);
    }

}
