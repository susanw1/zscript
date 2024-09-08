package net.zscript.javareceiver.modules.core;

import java.util.Optional;

import net.zscript.javareceiver.core.CommandOutStream;
import net.zscript.javareceiver.execution.CommandContext;
import net.zscript.javareceiver.execution.ZscriptTokenField;
import net.zscript.model.components.Zchars;
import net.zscript.util.OptIterator;

public class ZscriptEchoCommand {

    public static void execute(CommandContext ctx) {
        CommandOutStream               out      = ctx.getOutStream();
        OptIterator<ZscriptTokenField> iterator = ctx.fieldIterator();
        for (Optional<ZscriptTokenField> opt = iterator.next(); opt.isPresent(); opt = iterator.next()) {
            ZscriptTokenField field = opt.get();
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
