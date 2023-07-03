package org.zcode.javareceiver.modules.core;

import java.util.Random;

import org.zcode.javareceiver.core.ZcodeOutStream;
import org.zcode.javareceiver.execution.ZcodeCommandEnviroment;

public class ZcodeMakeCodeCommand {

    private static final Random r = new Random();
    private static int          code;

    public static void execute(ZcodeCommandEnviroment env) {
        ZcodeOutStream out = env.getOutStream();
        code = r.nextInt(0x1000);
        out.writeField('C', code);
    }

    public static int getCode() {
        return code;
    }
}
