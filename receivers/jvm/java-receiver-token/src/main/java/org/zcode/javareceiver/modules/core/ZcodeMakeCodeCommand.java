package org.zcode.javareceiver.modules.core;

import java.util.Random;

import org.zcode.javareceiver.core.ZcodeOutStream;
import org.zcode.javareceiver.execution.ZcodeCommandView;

public class ZcodeMakeCodeCommand {

    private static final Random r = new Random();
    private static int          code;

    public static void execute(ZcodeCommandView view) {
        ZcodeOutStream out = view.getOutStream();
        code = r.nextInt(0x1000);
        out.writeField('C', code);
    }

    public static int getCode() {
        return code;
    }
}
