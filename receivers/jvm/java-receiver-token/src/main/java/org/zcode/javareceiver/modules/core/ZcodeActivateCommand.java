package org.zcode.javareceiver.modules.core;

import org.zcode.javareceiver.execution.ZcodeCommandEnviroment;

public class ZcodeActivateCommand {

    public static void execute(ZcodeCommandEnviroment env) {
        env.getOutStream().writeField('A', env.isActivated() ? 1 : 0);
        env.activate();
    }

}
