package org.zcode.javareceiver.modules.core;

import java.util.Optional;
import java.util.Random;

import org.zcode.javareceiver.core.ZcodeOutStream;
import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.execution.ZcodeCommandView;

public class ZcodeRandomCodeCommand {

    private static final Random r = new Random();
    private static int          code;

    public static void make(ZcodeCommandView view) {
        ZcodeOutStream out = view.getOutStream();
        code = r.nextInt(0x1000);
        out.writeField('C', code);
    }

    public static void match(ZcodeCommandView view) {
        Optional<Integer> givenCode = view.getField((byte) 'C');
        if (givenCode.isEmpty()) {
            view.status(ZcodeStatus.COMMAND_FORMAT_ERROR);
            return;
        }
        if (givenCode.get() != code) {
            view.status(ZcodeStatus.COMMAND_FAIL);
        }
    }

}
