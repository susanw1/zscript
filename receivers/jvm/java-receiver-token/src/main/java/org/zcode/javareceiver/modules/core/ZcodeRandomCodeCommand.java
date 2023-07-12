package org.zcode.javareceiver.modules.core;

import java.util.Optional;
import java.util.Random;

import org.zcode.javareceiver.core.ZcodeCommandOutStream;
import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.execution.ZcodeCommandContext;

public class ZcodeRandomCodeCommand {

    private static final Random r = new Random();
    private static int          code;

    public static void make(ZcodeCommandContext ctx) {
        ZcodeCommandOutStream out = ctx.getOutStream();
        code = r.nextInt(0x1000);
        out.writeField('C', code);
    }

    public static void match(ZcodeCommandContext ctx) {
        Optional<Integer> givenCode = ctx.getField((byte) 'C');
        if (givenCode.isEmpty()) {
            ctx.status(ZcodeStatus.COMMAND_FORMAT_ERROR);
            return;
        }
        if (givenCode.get() != code) {
            ctx.status(ZcodeStatus.COMMAND_FAIL);
        }
    }

}
