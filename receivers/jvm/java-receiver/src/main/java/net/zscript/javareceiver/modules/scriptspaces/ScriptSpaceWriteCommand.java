package net.zscript.javareceiver.modules.scriptspaces;

import java.util.List;
import java.util.Optional;
import java.util.OptionalInt;

import net.zscript.javareceiver.core.CommandOutStream;
import net.zscript.javareceiver.execution.CommandContext;
import net.zscript.javareceiver.scriptspaces.ScriptSpace;
import net.zscript.javareceiver.scriptspaces.ScriptSpaceTokenBuffer.ScriptSpaceWriterTokenBuffer;
import net.zscript.model.components.ZscriptStatus;
import net.zscript.tokenizer.Tokenizer;
import net.zscript.util.BlockIterator;

public class ScriptSpaceWriteCommand {
    public static void execute(List<ScriptSpace> spaces, CommandContext ctx) {
        final OptionalInt spaceIndex = ctx.getField('P');
        if (spaceIndex.isEmpty()) {
            ctx.status(ZscriptStatus.MISSING_KEY);
            return;
        } else if (spaceIndex.getAsInt() < 0 || spaceIndex.getAsInt() >= spaces.size()) {
            ctx.status(ZscriptStatus.VALUE_OUT_OF_RANGE);
            return;
        }

        final CommandOutStream out    = ctx.getOutStream();
        final ScriptSpace      target = spaces.get(spaceIndex.getAsInt());
        if (target.isRunning()) {
            ctx.status(ZscriptStatus.COMMAND_FAIL);
            return;
        }

        final ScriptSpaceWriterTokenBuffer writer;
        if (ctx.hasField((byte) 'R')) {
            writer = target.overwrite();
        } else {
            writer = target.append();
        }

        final Optional<BlockIterator> scriptText = ctx.getFieldData('C');
        if (scriptText.isEmpty()) {
            ctx.status(ZscriptStatus.MISSING_KEY);
            return;
        }

        final Tokenizer tok = new Tokenizer(writer.getTokenWriter(), false);
        for (BlockIterator iterator = scriptText.get(); iterator.hasNext(); ) {
            tok.accept(iterator.next());
        }
        writer.commitChanges();
        out.writeField('L', 0xFFFF);
    }
}
