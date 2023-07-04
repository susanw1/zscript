package org.zcode.javareceiver.modules.core;

import org.zcode.javareceiver.core.ZcodeOutStream;
import org.zcode.javareceiver.execution.ZcodeCommandView;
import org.zcode.javareceiver.modules.ZcodeCommandFinder;

public class ZcodeCapabilitiesCommand {

    public static void execute(ZcodeCommandView view) {
        ZcodeOutStream out = view.getOutStream();
        out.writeField('C', ZcodeCoreModule.getCommands());
        out.writeField('M', ZcodeCommandFinder.getCommandSwitchExistsBottom(0));
        out.writeString("Java implementation of Zcode");
    }

}
