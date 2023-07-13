package org.zcode.javareceiver.modules.outerCore;

import java.util.Optional;

import org.zcode.javareceiver.core.ZcodeCommandOutStream;
import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.execution.ZcodeCommandContext;
import org.zcode.javareceiver.modules.ZcodeCommandFinder;

public class ZcodeExtendedCapabilitiesCommand {

    public static void execute(ZcodeCommandContext ctx) {
        ZcodeCommandOutStream out = ctx.getOutStream();
        out.writeField('C', ZcodeOuterCoreModule.getCommands());
        out.writeField('M', ZcodeCommandFinder.getCommandSwitchExistsBroad());
        Optional<Integer> targetOpt = ctx.getField((byte) 'M');

        if (targetOpt.isPresent()) {
            int target = targetOpt.get();
            if (target > 16) {
                ctx.status(ZcodeStatus.VALUE_OUT_OF_RANGE);
                return;
            }
            int result = ZcodeCommandFinder.getCommandSwitchExistsTop(target);
            out.writeField('L', result);
        }
    }

}
