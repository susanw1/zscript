package org.zcode.javareceiver.modules.core;

import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.execution.ZcodeCommandEnviroment;
import org.zcode.javareceiver.modules.ZcodeModule;

public class ZcodeCoreModule implements ZcodeModule {

    @Override
    public int getModuleID() {
        return 0;
    }

    @Override
    public void execute(ZcodeCommandEnviroment env, int command) {
        switch (command) {
        case 0x0:
            ZcodeCapabilitiesCommand.execute(env);
            break;
        case 0x1:
            ZcodeEchoCommand.execute(env);
            break;
        case 0x2:
            ZcodeActivateCommand.execute(env);
            break;
        case 0xc:
            ZcodeMakeCodeCommand.execute(env);
            break;
        case 0xd:
            ZcodeMatchCodeCommand.execute(env);
            break;
        default:
            env.status(ZcodeStatus.COMMAND_FORMAT_ERROR);
        }
    }

    @Override
    public void moveAlong(ZcodeCommandEnviroment env, int command) {
    }

}
