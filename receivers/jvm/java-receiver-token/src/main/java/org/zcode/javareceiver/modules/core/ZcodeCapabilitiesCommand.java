package org.zcode.javareceiver.modules.core;

import org.zcode.javareceiver.core.ZcodeOutStream;
import org.zcode.javareceiver.execution.ZcodeCommandEnviroment;

public class ZcodeCapabilitiesCommand {

    public static void execute(ZcodeCommandEnviroment env) {
        ZcodeOutStream out = env.getOutStream();
        out.writeField('C', 0x6107);
        out.writeField('M', 0x03);
        out.writeString("Java implementation of Zcode");
    }

}
