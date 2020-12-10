package com.wittsfamily.rcode.javareceiver;

import java.nio.charset.StandardCharsets;

import com.wittsfamily.rcode.javareceiver.commands.RCodeActivateCommand;
import com.wittsfamily.rcode.javareceiver.commands.RCodeEchoCommand;
import com.wittsfamily.rcode.javareceiver.commands.RCodeExecutionSpaceCommand;
import com.wittsfamily.rcode.javareceiver.commands.RCodeSetDebugChannelCommand;
import com.wittsfamily.rcode.javareceiver.commands.RCodeSetNotificationHostCommand;
import com.wittsfamily.rcode.javareceiver.executionspace.RCodeExecutionSpaceChannel;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandChannel;

public class Main {

    public static void main(String[] args) {
        RCodeParameters params = new RCodeParameters(true);
        TestInterruptSource intS = new TestInterruptSource();
        RCode r = new RCode(params, new RCodeBusInterruptSource[] { intS });
        r.setChannels(new RCodeCommandChannel[] { new DirectCommandChannel(params, r, "R6&R8&R9\n", new PrintingOutStream(), false, 80),
                new RCodeExecutionSpaceChannel(params, r, r.getSpace()) });
        r.getSpace().write("R1+10a0&R13A1\nR1+10a1&R11A2\nR33".getBytes(StandardCharsets.US_ASCII), 0, true);
        r.getCommandFinder().registerCommand(new RCodeEchoCommand());
        r.getCommandFinder().registerCommand(new RCodeActivateCommand());
        r.getCommandFinder().registerCommand(new RCodeExecutionSpaceCommand(r.getSpace()));
        r.getCommandFinder().registerCommand(new RCodeSetDebugChannelCommand(r));
        r.getCommandFinder().registerCommand(new RCodeSetNotificationHostCommand(r));
        for (int i = 0; i < 10; i++) {
            r.progressRCode();
        }
        r.getSpace().setRunning(true);
        for (int i = 0; i < 50; i++) {
            r.progressRCode();
        }
        r.getSpace().setRunning(true);
        for (int i = 0; i < 50; i++) {
            r.progressRCode();
        }
    }

}
