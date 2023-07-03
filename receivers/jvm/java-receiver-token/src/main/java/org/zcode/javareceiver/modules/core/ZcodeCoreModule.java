package org.zcode.javareceiver.modules.core;

import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.execution.ZcodeCommandView;
import org.zcode.javareceiver.modules.ZcodeModule;

public class ZcodeCoreModule implements ZcodeModule {

    @Override
    public int getModuleID() {
        return 0;
    }

    @Override
    public void execute(ZcodeCommandView view, int command) {
        switch (command) {
        case 0x0:
            ZcodeCapabilitiesCommand.execute(view);
            break;
        case 0x1:
            ZcodeEchoCommand.execute(view);
            break;
        case 0x2:
            ZcodeActivateCommand.execute(view);
            break;
        case 0xc:
            ZcodeMakeCodeCommand.execute(view);
            break;
        case 0xd:
            ZcodeMatchCodeCommand.execute(view);
            break;
        default:
            view.status(ZcodeStatus.COMMAND_FORMAT_ERROR);
        }
    }

    @Override
    public void moveAlong(ZcodeCommandView view, int command) {
    }

}
