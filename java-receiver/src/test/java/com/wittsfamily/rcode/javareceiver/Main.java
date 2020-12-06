package com.wittsfamily.rcode.javareceiver;

import com.wittsfamily.rcode.javareceiver.commands.RCodeActivateCommand;
import com.wittsfamily.rcode.javareceiver.commands.RCodeEchoCommand;
import com.wittsfamily.rcode.javareceiver.commands.RCodeExecutionSpaceCommand;
import com.wittsfamily.rcode.javareceiver.commands.RCodeSetNotificationHostCommand;
import com.wittsfamily.rcode.javareceiver.executionspace.RCodeExecutionSpaceChannel;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandChannel;

public class Main {

    public static void main(String[] args) {
        RCodeParameters params = new RCodeParameters(true);
        TestInterruptSource intS = new TestInterruptSource();
        RCode r = new RCode(params, new RCodeBusInterruptSource[] { intS });
        r.setChannels(new RCodeCommandChannel[] { new DirectCommandChannel(params, r, "R1A10&R1S10+21BC&R1|R1A2&R1+44\n", new PrintingOutStream(), false, 80),
                new RCodeExecutionSpaceChannel(params, r, r.getSpace()) });
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