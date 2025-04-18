package net.zscript.javareceiver.modules.scriptspaces;

import java.util.List;
import java.util.OptionalInt;

import net.zscript.javareceiver.core.CommandOutStream;
import net.zscript.javareceiver.execution.CommandContext;
import net.zscript.javareceiver.scriptspaces.ScriptSpace;
import net.zscript.model.components.ZscriptStatus;

public class ScriptSpaceSetupCommand {
    public static void execute(List<ScriptSpace> spaces, CommandContext ctx) {
        OptionalInt spaceIndex = ctx.getFieldValue('P');
        if (spaceIndex.isEmpty()) {
            ctx.status(ZscriptStatus.MISSING_KEY);
            return;
        } else if (spaceIndex.getAsInt() < 0 || spaceIndex.getAsInt() >= spaces.size()) {
            ctx.status(ZscriptStatus.VALUE_OUT_OF_RANGE);
            return;
        }

        CommandOutStream out         = ctx.getOutStream();
        ScriptSpace      targetSpace = spaces.get(spaceIndex.getAsInt());
        out.writeField('P', targetSpace.getCurrentLength());

        if (targetSpace.isRunning()) {
            out.writeField('R', 0);
        }

        if (targetSpace.isWritable()) {
            out.writeField('W', 0);
        }

        out.writeField('L', 0xFFFF);
        OptionalInt runOpt = ctx.getFieldValue('R');
        if (runOpt.isPresent()) {
            if (runOpt.getAsInt() != 0) {
                targetSpace.run();
            } else {
                targetSpace.stop();
            }
        }
    }
}
