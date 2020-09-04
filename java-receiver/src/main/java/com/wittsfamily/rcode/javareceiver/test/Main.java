package com.wittsfamily.rcode.javareceiver.test;

import java.nio.charset.StandardCharsets;

import com.wittsfamily.rcode.javareceiver.RCode;
import com.wittsfamily.rcode.javareceiver.RCodeBusInterruptSource;
import com.wittsfamily.rcode.javareceiver.RCodeParameters;
import com.wittsfamily.rcode.javareceiver.commands.RCodeActivateCommand;
import com.wittsfamily.rcode.javareceiver.commands.RCodeEchoCommand;
import com.wittsfamily.rcode.javareceiver.commands.RCodeExecutionSpaceCommand;
import com.wittsfamily.rcode.javareceiver.commands.RCodeSetNotificationHostCommand;
import com.wittsfamily.rcode.javareceiver.commands.RCodeTmp2Command;
import com.wittsfamily.rcode.javareceiver.commands.RCodeTmpCommand;
import com.wittsfamily.rcode.javareceiver.executionspace.RCodeExecutionSpaceChannel;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandChannel;

public class Main {

    public static void main(String[] args) {
        RCodeParameters params = new RCodeParameters(true);
        TestInterruptSource intS = new TestInterruptSource();
        RCode r = new RCode(params, new RCodeBusInterruptSource[] { intS });
        r.setChannels(new RCodeCommandChannel[] { new DirectCommandChannel(params, r, "R8;R6\nR21G\n", new PrintingOutStream(), false, 3),
                new RCodeExecutionSpaceChannel(params, r, r.getSpace()) });
        r.getSpace().write("R1+10a0;R10A1\nR1+10a1;R11A2\nR33\nR1+10a2;R11A2\n".getBytes(StandardCharsets.US_ASCII), 0, true);
        r.getCommandFinder().registerCommand(new RCodeEchoCommand());
        r.getCommandFinder().registerCommand(new RCodeActivateCommand());
        r.getCommandFinder().registerCommand(new RCodeTmpCommand());
        r.getCommandFinder().registerCommand(new RCodeTmp2Command());
        r.getCommandFinder().registerCommand(new RCodeExecutionSpaceCommand(r.getSpace()));
        r.getCommandFinder().registerCommand(new RCodeSetNotificationHostCommand(r));
        for (int i = 0; i < 40; i++) {
            r.progressRCode();
        }
    }

}
