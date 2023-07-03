package org.zcode.javareceiver.modules.core;

import java.util.Optional;

import org.zcode.javareceiver.core.ZcodeOutStream;
import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.execution.ZcodeCommandEnviroment;

public class ZcodeMatchCodeCommand {

    public static void execute(ZcodeCommandEnviroment env) {
        ZcodeOutStream    out       = env.getOutStream();
        Optional<Integer> givenCode = env.getField((byte) 'C');
        if (givenCode.isEmpty()) {
            env.status(ZcodeStatus.COMMAND_FORMAT_ERROR);
            return;
        }
        if (givenCode.get() != ZcodeMakeCodeCommand.getCode()) {
            env.status(ZcodeStatus.COMMAND_FAIL);
        }
    }

}
