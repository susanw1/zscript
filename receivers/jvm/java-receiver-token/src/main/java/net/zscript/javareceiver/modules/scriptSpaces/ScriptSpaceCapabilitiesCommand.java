package net.zscript.javareceiver.modules.scriptSpaces;

import java.util.List;

import net.zscript.javareceiver.core.ZscriptCommandOutStream;
import net.zscript.javareceiver.execution.CommandContext;
import net.zscript.javareceiver.scriptSpaces.ScriptSpace;

public class ScriptSpaceCapabilitiesCommand {
    public static void execute(List<ScriptSpace> spaces, CommandContext ctx) {
        ZscriptCommandOutStream out = ctx.getOutStream();
        out.writeField('C', ScriptSpaceModule.getCommands());
        out.writeField('P', spaces.size());
    }
}
