package org.zcode.javareceiver.main;

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
import org.zcode.javareceiver.util.DirectCommandChannel;
import org.zcode.javareceiver.util.PrintingOutStream;
import org.zcode.javareceiver.util.TestInterruptSource;

public class Main {

    public static void main(final String[] args) {
        ZcodeParameters     params = new ZcodeParameters(true);
        TestInterruptSource intSrc = new TestInterruptSource();

        Zcode z = new Zcode(params, new ZcodeBusInterruptSource[] { intSrc });

        z.setChannels(new ZcodeCommandChannel[] {
                new DirectCommandChannel(params, z, "Z1&Z8&Z9\n", new PrintingOutStream(), false, 80),
                new ZcodeExecutionSpaceChannel(params, z, z.getSpace())
        });

        z.getSpace().write("Z1+10a0&Z13A1\nZ1+10a1&Z11A2\nZ33".getBytes(StandardCharsets.US_ASCII), 0, true);

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
