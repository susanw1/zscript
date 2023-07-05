package org.zcode.javareceiver.modules.outerCore;

import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.execution.ZcodeAddressingView;
import org.zcode.javareceiver.execution.ZcodeCommandView;
import org.zcode.javareceiver.modules.ZcodeModule;
import org.zcode.javareceiver.modules.core.ZcodeGuidCommand;

public class ZcodeOuterCoreModule implements ZcodeModule {

    public static int getCommands() {
        return 0x0011;
    }

    @Override
    public int getModuleID() {
        return 1;
    }

    @Override
    public void execute(ZcodeCommandView view, int command) {
        switch (command) {
        case 0x0:
            ZcodeExtendedCapabilitiesCommand.execute(view);
            break;
        case 0x4:
            ZcodeGuidCommand.set(view);
            break;
        case 0x8:
            ZcodeChannelSetupCommand.execute(view);
            break;
        default:
            view.status(ZcodeStatus.COMMAND_FORMAT_ERROR);
        }
    }

    @Override
    public void moveAlong(ZcodeCommandView view, int command) {
    }

    @Override
    public void address(ZcodeAddressingView view) {
    }

    @Override
    public void addressMoveAlong(ZcodeAddressingView view) {
    }
}
