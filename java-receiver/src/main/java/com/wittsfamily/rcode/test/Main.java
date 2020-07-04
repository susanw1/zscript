package com.wittsfamily.rcode.test;

import com.wittsfamily.rcode.RCode;
import com.wittsfamily.rcode.RCodeBusInterruptSource;
import com.wittsfamily.rcode.RCodeParameters;
import com.wittsfamily.rcode.commands.RCodeEchoCommand;
import com.wittsfamily.rcode.parsing.RCodeCommandChannel;

public class Main {

    public static void main(String[] args) {
        RCode r = new RCode(new RCodeParameters(), new RCodeBusInterruptSource[0]);
        r.setChannels(new RCodeCommandChannel[] { new DirectCommandChannel(r) });
        r.getCommandFinder().registerCommand(new RCodeEchoCommand());
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
    }

}
