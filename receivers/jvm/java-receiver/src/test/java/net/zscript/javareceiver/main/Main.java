package net.zscript.javareceiver.main;

import java.nio.charset.StandardCharsets;

import net.zscript.javareceiver.Zcode;
import net.zscript.javareceiver.ZcodeBusInterruptSource;
import net.zscript.javareceiver.ZcodeParameters;
import net.zscript.javareceiver.commands.ZcodeActivateCommand;
import net.zscript.javareceiver.commands.ZcodeEchoCommand;
import net.zscript.javareceiver.commands.ZcodeExecutionSpaceCommand;
import net.zscript.javareceiver.commands.ZcodeSetDebugChannelCommand;
import net.zscript.javareceiver.commands.ZcodeSetNotificationHostCommand;
import net.zscript.javareceiver.executionspace.ZcodeExecutionSpaceChannel;
import net.zscript.javareceiver.parsing.ZcodeCommandChannel;
import net.zscript.javareceiver.util.DirectCommandChannel;
import net.zscript.javareceiver.util.PrintingOutStream;
import net.zscript.javareceiver.util.TestInterruptSource;

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
