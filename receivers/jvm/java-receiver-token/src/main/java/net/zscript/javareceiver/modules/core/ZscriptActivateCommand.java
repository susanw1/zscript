package net.zscript.javareceiver.modules.core;

import net.zscript.javareceiver.execution.CommandContext;

public class ZscriptActivateCommand {

    public static void execute(CommandContext ctx) {
        ctx.getOutStream().writeField('A', ctx.isActivated() ? 1 : 0);
        ctx.activate();
    }

}
