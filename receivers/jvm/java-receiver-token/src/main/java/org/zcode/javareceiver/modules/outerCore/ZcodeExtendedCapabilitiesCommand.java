package org.zcode.javareceiver.modules.outerCore;

import java.util.OptionalInt;

import org.zcode.javareceiver.core.ZcodeCommandOutStream;
import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.execution.ZcodeCommandContext;

public class ZcodeExtendedCapabilitiesCommand {

    public static void execute(ZcodeCommandContext ctx) {
        ZcodeCommandOutStream out = ctx.getOutStream();
        out.writeField('C', ZcodeOuterCoreModule.getCommands());
        out.writeField('M', ctx.getZcode().getModuleFinder().getCommandSwitchExistsBroad());
        OptionalInt targetOpt = ctx.getField((byte) 'M');

        if (targetOpt.isPresent()) {
            int target = targetOpt.getAsInt();
            if (target > 16) {
                ctx.status(ZcodeStatus.VALUE_OUT_OF_RANGE);
                return;
            }
            int result = ctx.getZcode().getModuleFinder().getCommandSwitchExistsTop(target);
            out.writeField('L', result);
        }
    }

}
