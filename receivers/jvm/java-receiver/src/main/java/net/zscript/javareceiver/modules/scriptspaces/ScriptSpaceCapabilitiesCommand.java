package net.zscript.javareceiver.modules.scriptspaces;

import java.util.List;

import net.zscript.javareceiver.core.CommandOutStream;
import net.zscript.javareceiver.execution.CommandContext;
import net.zscript.javareceiver.scriptspaces.ScriptSpace;

public class ScriptSpaceCapabilitiesCommand {
    public static void execute(List<ScriptSpace> spaces, CommandContext ctx) {
        CommandOutStream out = ctx.getOutStream();
        out.writeField('C', ScriptSpaceModule.getCommands());
        out.writeField('P', spaces.size());
    }
}
