package net.zscript.javareceiver.modules.scriptSpaces;

import java.util.Iterator;
import java.util.List;
import java.util.OptionalInt;

import net.zscript.javareceiver.core.ZscriptCommandOutStream;
import net.zscript.javareceiver.core.ZscriptStatus;
import net.zscript.javareceiver.execution.CommandContext;
import net.zscript.javareceiver.scriptSpaces.ScriptSpace;
import net.zscript.javareceiver.tokenizer.ScriptSpaceBuffer.ScriptSpaceWriterBuffer;
import net.zscript.javareceiver.tokenizer.Tokenizer;

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
        ZscriptCommandOutStream out    = ctx.getOutStream();
        ScriptSpace             target = spaces.get(spaceIndex.getAsInt());
        if (target.isRunning()) {
            ctx.status(ZscriptStatus.COMMAND_FAIL);
            return;
        }
        ScriptSpaceWriterBuffer writer;
        if (ctx.hasField((byte) 'R')) {
            writer = target.overwrite();
        } else {
            writer = target.append();
        }
        Tokenizer tok = new Tokenizer(writer.getTokenWriter(), 2);
        for (Iterator<Byte> iterator = ctx.getBigField(); iterator.hasNext();) {
            tok.accept(iterator.next());
        }
        out.writeField('L', 0xFFFF);
    }

}
