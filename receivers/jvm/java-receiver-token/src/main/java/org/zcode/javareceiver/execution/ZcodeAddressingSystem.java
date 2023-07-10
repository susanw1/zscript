package org.zcode.javareceiver.execution;

import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.modules.ZcodeCommandFinder;
import org.zcode.javareceiver.modules.ZcodeModule;

public class ZcodeAddressingSystem {

    public static void execute(ZcodeAddressingContext ctx) {
        if (!ctx.isActivated()) {
            ctx.status(ZcodeStatus.NOT_ACTIVATED);
            return;
        }
        int addr = ctx.getAddressSegments().next().get();

        ZcodeModule module = ZcodeCommandFinder.getModule(addr);
        if (module == null) {
            ctx.status(ZcodeStatus.ADDRESSING_ERROR);
            return;
        }
        ctx.setComplete();
        module.address(ctx);
    }

    public static void moveAlong(ZcodeAddressingContext ctx) {
        int addr = ctx.getAddressSegments().next().get();
        ZcodeCommandFinder.getModule(addr).addressMoveAlong(ctx);
    }
}
