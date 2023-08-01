package net.zscript.javareceiver.modules.outerCore;

import java.util.OptionalInt;

import net.zscript.javareceiver.core.ZcodeCommandOutStream;
import net.zscript.javareceiver.core.ZcodeStatus;
import net.zscript.javareceiver.execution.ZcodeCommandContext;

public class ZcodeExtendedCapabilitiesCommand {

    public static void execute(ZcodeCommandContext ctx) {
        ZcodeCommandOutStream out = ctx.getOutStream();
        out.writeField('C', ZcodeOuterCoreModule.getCommands());
        out.writeField('M', ctx.getZcode().getModuleRegistry().getCommandSwitchExistsBroad());
        OptionalInt targetOpt = ctx.getField((byte) 'M');

        if (targetOpt.isPresent()) {
            int target = targetOpt.getAsInt();
            if (target > 16) {
                ctx.status(ZcodeStatus.VALUE_OUT_OF_RANGE);
                return;
            }
            int result = ctx.getZcode().getModuleRegistry().getCommandSwitchExistsTop(target);
            out.writeField('L', result);
        }
    }

}
