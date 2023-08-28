package net.zscript.javareceiver.modules.scriptSpaces;

import java.util.ArrayList;
import java.util.List;

import net.zscript.javareceiver.execution.CommandContext;
import net.zscript.javareceiver.modules.ZscriptModule;
import net.zscript.javareceiver.scriptSpaces.ScriptSpace;
import net.zscript.model.components.ZscriptStatus;

public class ScriptSpaceModule implements ZscriptModule {
    private final List<ScriptSpace> spaces = new ArrayList<>();

    public void addScriptSpace(ScriptSpace s) {
        spaces.add(s);
    }

    public static int getCommands() {
        return 0x107;
    }

    @Override
    public int getModuleID() {
        return 2;
    }

    @Override
    public void execute(CommandContext ctx, int command) {
        switch (command) {
        case 0x0:
            ScriptSpaceCapabilitiesCommand.execute(spaces, ctx);
            break;
        case 0x1:
            ScriptSpaceSetupCommand.execute(spaces, ctx);
            break;
        case 0x2:
            ScriptSpaceWriteCommand.execute(spaces, ctx);
            break;
        case 0x8:
            ZscriptSleepCommand.execute(ctx);
            break;
        default:
            ctx.status(ZscriptStatus.COMMAND_NOT_FOUND);
        }
    }

    @Override
    public void moveAlong(CommandContext ctx, int command) {
        switch (command) {
        case 0x8:
            ZscriptSleepCommand.moveAlong(ctx);
            break;
        default:
            ctx.commandComplete();
            ctx.status(ZscriptStatus.COMMAND_NOT_FOUND);
        }
    }

}
