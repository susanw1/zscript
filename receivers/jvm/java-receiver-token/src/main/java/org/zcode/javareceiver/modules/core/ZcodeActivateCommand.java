package org.zcode.javareceiver.modules.core;

import org.zcode.javareceiver.execution.ZcodeCommandContext;

public class ZcodeActivateCommand {

    public static void execute(ZcodeCommandContext ctx) {
        ctx.getOutStream().writeField('A', ctx.isActivated() ? 1 : 0);
        ctx.activate();
    }

}
