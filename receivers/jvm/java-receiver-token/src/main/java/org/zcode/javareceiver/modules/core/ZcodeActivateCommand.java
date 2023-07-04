package org.zcode.javareceiver.modules.core;

import org.zcode.javareceiver.execution.ZcodeCommandView;

public class ZcodeActivateCommand {

    public static void execute(ZcodeCommandView view) {
        view.getOutStream().writeField('A', view.isActivated() ? 1 : 0);
        view.activate();
    }

}
