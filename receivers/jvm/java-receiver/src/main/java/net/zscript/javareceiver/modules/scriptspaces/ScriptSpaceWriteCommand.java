package net.zscript.javareceiver.modules.scriptspaces;

import java.util.Iterator;
import java.util.List;
import java.util.OptionalInt;

import net.zscript.javareceiver.core.CommandOutStream;
import net.zscript.javareceiver.execution.CommandContext;
import net.zscript.javareceiver.scriptspaces.ScriptSpace;
import net.zscript.javareceiver.scriptspaces.ScriptSpaceTokenBuffer.ScriptSpaceWriterTokenBuffer;
import net.zscript.model.components.ZscriptStatus;
import net.zscript.tokenizer.Tokenizer;

public class ScriptSpaceWriteCommand {
    public static void execute(List<ScriptSpace> spaces, CommandContext ctx) {
        OptionalInt spaceIndex = ctx.getField('P');
        if (spaceIndex.isEmpty()) {
            ctx.status(ZscriptStatus.MISSING_KEY);
            return;
        } else if (spaceIndex.getAsInt() < 0 || spaceIndex.getAsInt() >= spaces.size()) {
            ctx.status(ZscriptStatus.VALUE_OUT_OF_RANGE);
            return;
        }

        CommandOutStream out    = ctx.getOutStream();
        ScriptSpace      target = spaces.get(spaceIndex.getAsInt());
        if (target.isRunning()) {
            ctx.status(ZscriptStatus.COMMAND_FAIL);
            return;
        }

        ScriptSpaceWriterTokenBuffer writer;
        if (ctx.hasField((byte) 'R')) {
            writer = target.overwrite();
        } else {
            writer = target.append();
        }

        Tokenizer tok = new Tokenizer(writer.getTokenWriter(), false);
        for (Iterator<Byte> iterator = ctx.bigFieldDataIterator(); iterator.hasNext(); ) {
            tok.accept(iterator.next());
        }
        writer.commitChanges();
        out.writeField('L', 0xFFFF);
    }

}
