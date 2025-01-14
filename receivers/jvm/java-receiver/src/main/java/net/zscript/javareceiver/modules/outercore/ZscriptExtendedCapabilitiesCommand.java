package net.zscript.javareceiver.modules.outercore;

import java.util.OptionalInt;

import net.zscript.javareceiver.core.CommandOutStream;
import net.zscript.javareceiver.execution.CommandContext;
import net.zscript.model.components.ZscriptStatus;

public class ZscriptExtendedCapabilitiesCommand {

    public static void execute(CommandContext ctx) {
        CommandOutStream out = ctx.getOutStream();
        out.writeField('C', ZscriptOuterCoreModule.getCommands());
        out.writeField('M', ctx.getZscript().getModuleRegistry().getCommandSwitchExistsBroad());
        OptionalInt targetOpt = ctx.getField((byte) 'M');

        if (targetOpt.isPresent()) {
            int target = targetOpt.getAsInt();
            if (target > 16) {
                ctx.status(ZscriptStatus.VALUE_OUT_OF_RANGE);
                return;
            }
            int result = ctx.getZscript().getModuleRegistry().getCommandSwitchExistsTop(target);
            out.writeField('L', result);
        }
    }

}
