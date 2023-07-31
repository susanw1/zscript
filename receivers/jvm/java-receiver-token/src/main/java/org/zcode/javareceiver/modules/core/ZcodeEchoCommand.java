package org.zcode.javareceiver.modules.core;

import java.util.Optional;

import org.zcode.javareceiver.core.ZcodeCommandOutStream;
import org.zcode.javareceiver.execution.ZcodeCommandContext;
import org.zcode.javareceiver.execution.ZcodeField;
import org.zcode.javareceiver.tokenizer.OptIterator;
import org.zcode.javareceiver.tokenizer.Zchars;

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
