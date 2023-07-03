package org.zcode.javareceiver.modules.core;

import org.zcode.javareceiver.core.ZcodeOutStream;
import org.zcode.javareceiver.execution.ZcodeCommandView;

public class ZcodeCapabilitiesCommand {

    public static void execute(ZcodeCommandView view) {
        ZcodeOutStream out = view.getOutStream();
        out.writeField('C', 0x6107);
        out.writeField('M', 0x03);
        out.writeString("Java implementation of Zcode");
    }

}
