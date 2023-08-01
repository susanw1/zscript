package net.zscript.javareceiver.modules.core;

import java.util.Optional;

import net.zscript.javareceiver.core.ZcodeCommandOutStream;
import net.zscript.javareceiver.execution.ZcodeCommandContext;
import net.zscript.javareceiver.execution.ZcodeField;
import net.zscript.javareceiver.tokenizer.OptIterator;
import net.zscript.javareceiver.tokenizer.Zchars;

public class ZcodeEchoCommand {

    public static void execute(ZcodeCommandContext ctx) {
        ZcodeCommandOutStream   out      = ctx.getOutStream();
        OptIterator<ZcodeField> iterator = ctx.fieldIterator();
        for (Optional<ZcodeField> opt = iterator.next(); opt.isPresent(); opt = iterator.next()) {
            ZcodeField field = opt.get();
            if (field.getKey() == Zchars.Z_STATUS) {
                int status = field.getValue();
                ctx.status((byte) status);
                continue;
            }
            if (field.getKey() != Zchars.Z_CMD) {
                out.writeField(field);
            }
        }
    }

}
