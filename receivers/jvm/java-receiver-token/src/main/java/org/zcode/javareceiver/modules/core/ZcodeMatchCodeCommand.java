package org.zcode.javareceiver.modules.core;

import java.util.Optional;

import org.zcode.javareceiver.core.ZcodeOutStream;
import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.execution.ZcodeCommandView;

public class ZcodeMatchCodeCommand {

    public static void execute(ZcodeCommandView view) {
        ZcodeOutStream    out       = view.getOutStream();
        Optional<Integer> givenCode = view.getField((byte) 'C');
        if (givenCode.isEmpty()) {
            view.status(ZcodeStatus.COMMAND_FORMAT_ERROR);
            return;
        }
        if (givenCode.get() != ZcodeMakeCodeCommand.getCode()) {
            view.status(ZcodeStatus.COMMAND_FAIL);
        }
    }

}
