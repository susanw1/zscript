package net.zscript.javareceiver.modules.core;

import java.util.Optional;

import net.zscript.javareceiver.core.CommandOutStream;
import net.zscript.javareceiver.execution.CommandContext;
import net.zscript.model.components.Zchars;
import net.zscript.tokenizer.ZscriptField;
import net.zscript.util.OptIterator;

public class ZscriptEchoCommand {

    public static void execute(CommandContext ctx) {
        CommandOutStream          out      = ctx.getOutStream();
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
