package net.zscript.javareceiver.modules.core;

import java.util.OptionalInt;
import java.util.Random;

import net.zscript.javareceiver.core.CommandOutStream;
import net.zscript.javareceiver.execution.CommandContext;
import net.zscript.model.components.ZscriptStatus;

public class ZscriptRandomCodeCommand {

    private final Random r = new Random();
    private       int    code;

    public void make(CommandContext ctx) {
        CommandOutStream out = ctx.getOutStream();
        code = r.nextInt(0x1000);
        out.writeField('C', code);
    }

    public void match(CommandContext ctx) {
        OptionalInt givenCode = ctx.getField((byte) 'C');
        if (givenCode.isEmpty()) {
            ctx.status(ZscriptStatus.MISSING_KEY);
            return;
        }
        if (givenCode.getAsInt() != code) {
            ctx.status(ZscriptStatus.COMMAND_FAIL);
        }
    }

}
