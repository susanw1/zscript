package org.zcode.javareceiver.modules.outerCore;

import java.util.Optional;

import org.zcode.javareceiver.core.ZcodeOutStream;
import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.execution.ZcodeCommandView;
import org.zcode.javareceiver.modules.ZcodeCommandFinder;

public class ZcodeExtendedCapabilitiesCommand {

    public static void execute(ZcodeCommandView view) {
        ZcodeOutStream out = view.getOutStream();
        out.writeField('C', ZcodeOuterCoreModule.getCommands());
        out.writeField('M', ZcodeCommandFinder.getCommandSwitchExistsBroad());
        Optional<Integer> targetOpt = view.getField((byte) 'M');

        if (targetOpt.isPresent()) {
            int target = targetOpt.get();
            if (target > 16) {
                view.status(ZcodeStatus.COMMAND_FORMAT_ERROR);
                return;
            }
            int result = ZcodeCommandFinder.getCommandSwitchExistsTop(target);
            out.writeField('L', result);
        }
    }

}
