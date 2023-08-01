package net.zscript.javareceiver.modules.core;

import java.util.OptionalInt;
import java.util.Random;

import net.zscript.javareceiver.core.ZcodeCommandOutStream;
import net.zscript.javareceiver.core.ZcodeStatus;
import net.zscript.javareceiver.execution.ZcodeCommandContext;

public class ZcodeRandomCodeCommand {

    private final Random r = new Random();
    private int          code;

    public void make(ZcodeCommandContext ctx) {
        ZcodeCommandOutStream out = ctx.getOutStream();
        code = r.nextInt(0x1000);
        out.writeField('C', code);
    }

    public void match(ZcodeCommandContext ctx) {
        OptionalInt givenCode = ctx.getField((byte) 'C');
        if (givenCode.isEmpty()) {
            ctx.status(ZcodeStatus.MISSING_KEY);
            return;
        }
        if (givenCode.getAsInt() != code) {
            ctx.status(ZcodeStatus.COMMAND_FAIL);
        }
    }

}
