package net.zscript.javareceiver.modules.core;

import java.util.Optional;

import net.zscript.javareceiver.core.ZscriptCommandOutStream;
import net.zscript.javareceiver.execution.CommandContext;
import net.zscript.javareceiver.execution.ZscriptField;
import net.zscript.javareceiver.tokenizer.OptIterator;
import net.zscript.javareceiver.tokenizer.Zchars;

public class ZscriptEchoCommand {

    public static void execute(CommandContext ctx) {
        ZscriptCommandOutStream   out      = ctx.getOutStream();
        OptIterator<ZscriptField> iterator = ctx.fieldIterator();
        for (Optional<ZscriptField> opt = iterator.next(); opt.isPresent(); opt = iterator.next()) {
            ZscriptField field = opt.get();
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
