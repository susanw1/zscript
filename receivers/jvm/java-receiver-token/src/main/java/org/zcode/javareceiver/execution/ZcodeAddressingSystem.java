package org.zcode.javareceiver.execution;

import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.modules.ZcodeCommandFinder;
import org.zcode.javareceiver.modules.ZcodeModule;

public class ZcodeAddressingSystem {

    public static void execute(ZcodeAddressingView view) {
        if (!view.isActivated()) {
            view.status(ZcodeStatus.NOT_ACTIVATED);
            return;
        }
        int addr = view.getAddressSegments().next().get();

        ZcodeModule module = ZcodeCommandFinder.getModule(addr);
        if (module == null) {
            view.status(ZcodeStatus.ADDRESSING_ERROR);
            return;
        }
        view.setComplete();
        module.address(view);
    }

    public static void moveAlong(ZcodeAddressingView view) {
        int addr = view.getAddressSegments().next().get();
        ZcodeCommandFinder.getModule(addr).addressMoveAlong(view);
    }
}
