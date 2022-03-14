package org.zcode.javareceiver.util;

import java.nio.charset.StandardCharsets;

import org.zcode.javareceiver.Zcode;
import org.zcode.javareceiver.ZcodeBusInterruptSource;
import org.zcode.javareceiver.ZcodeParameters;
import org.zcode.javareceiver.commands.ZcodeActivateCommand;
import org.zcode.javareceiver.commands.ZcodeEchoCommand;
import org.zcode.javareceiver.commands.ZcodeExecutionSpaceCommand;
import org.zcode.javareceiver.commands.ZcodeSetDebugChannelCommand;
import org.zcode.javareceiver.commands.ZcodeSetNotificationHostCommand;
import org.zcode.javareceiver.executionspace.ZcodeExecutionSpaceChannel;
import org.zcode.javareceiver.parsing.ZcodeCommandChannel;

public class Main {

    public static void main(final String[] args) {
        ZcodeParameters     params = new ZcodeParameters(true);
        TestInterruptSource intSrc = new TestInterruptSource();

        Zcode z = new Zcode(params, new ZcodeBusInterruptSource[] { intSrc });

        z.setChannels(new ZcodeCommandChannel[] {
                new DirectCommandChannel(params, z, "R6&R8&R9\n", new PrintingOutStream(), false, 80),
                new ZcodeExecutionSpaceChannel(params, z, z.getSpace())
        });

        z.getSpace().write("R1+10a0&R13A1\nR1+10a1&R11A2\nR33".getBytes(StandardCharsets.US_ASCII), 0, true);

        z.getCommandFinder().registerCommand(new ZcodeEchoCommand())
                .registerCommand(new ZcodeActivateCommand())
                .registerCommand(new ZcodeExecutionSpaceCommand(z.getSpace()))
                .registerCommand(new ZcodeSetDebugChannelCommand(z))
                .registerCommand(new ZcodeSetNotificationHostCommand(z));

        z.progressZcode(10);
        z.getSpace().setRunning(true);
        z.progressZcode(50);
        z.getSpace().setRunning(true);
        z.progressZcode(50);
    }

}
