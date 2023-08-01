package net.zscript.javareceiver.modules.outerCore;

import java.util.OptionalInt;

import net.zscript.javareceiver.core.ZscriptCommandOutStream;
import net.zscript.javareceiver.core.ZscriptStatus;
import net.zscript.javareceiver.execution.CommandContext;

public class ZscriptExtendedCapabilitiesCommand {

    public static void execute(CommandContext ctx) {
        ZscriptCommandOutStream out = ctx.getOutStream();
        out.writeField('C', ZscriptOuterCoreModule.getCommands());
        out.writeField('M', ctx.getZcode().getModuleRegistry().getCommandSwitchExistsBroad());
        OptionalInt targetOpt = ctx.getField((byte) 'M');

        if (targetOpt.isPresent()) {
            int target = targetOpt.getAsInt();
            if (target > 16) {
                ctx.status(ZscriptStatus.VALUE_OUT_OF_RANGE);
                return;
            }
            int result = ctx.getZcode().getModuleRegistry().getCommandSwitchExistsTop(target);
            out.writeField('L', result);
        }
    }

}
