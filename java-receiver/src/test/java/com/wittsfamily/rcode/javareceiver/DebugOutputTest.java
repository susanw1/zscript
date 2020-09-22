package com.wittsfamily.rcode.javareceiver;

import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.Test;

import com.wittsfamily.rcode.javareceiver.commands.RCodeActivateCommand;
import com.wittsfamily.rcode.javareceiver.commands.RCodeEchoCommand;
import com.wittsfamily.rcode.javareceiver.commands.RCodeSetDebugChannelCommand;
import com.wittsfamily.rcode.javareceiver.commands.RCodeTmpCommand;
import com.wittsfamily.rcode.javareceiver.commands.RCodeWriteToDebugCommand;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandChannel;
import com.wittsfamily.rcode.javareceiver.test.DirectCommandChannel;
import com.wittsfamily.rcode.javareceiver.test.StringOutStream;

class DebugOutputTest {

    @Test
    void shouldIgnoreComments() {
        RCodeActivateCommand.reset();
        RCode r = new RCode(new RCodeParameters(false), new RCodeBusInterruptSource[0]);
        StringOutStream out = new StringOutStream();
        r.setChannels(new RCodeCommandChannel[] { new DirectCommandChannel(new RCodeParameters(false), r, "#234342\nR6;R1B01\n#;1;2;3;1;2;3;\nR1CV40\"hello\"", out, false, 100) });
        r.getCommandFinder().registerCommand(new RCodeEchoCommand());
        r.getCommandFinder().registerCommand(new RCodeActivateCommand());
        r.getCommandFinder().registerCommand(new RCodeTmpCommand());
        r.getCommandFinder().registerCommand(new RCodeSetDebugChannelCommand(r));
        r.getCommandFinder().registerCommand(new RCodeWriteToDebugCommand(r));
        for (int i = 0; i < 100; i++) {
            r.progressRCode();
        }
        r.getDebug().println("aaaa");
        assertThat(out.getString()).isEqualTo("AS;SB1\nSCV40\"hello\"\n");
    }

    @Test
    void shouldOutputDebug() {
        RCodeActivateCommand.reset();
        RCode r = new RCode(new RCodeParameters(false), new RCodeBusInterruptSource[0]);
        StringOutStream out1 = new StringOutStream();
        StringOutStream out2 = new StringOutStream();
        r.setChannels(
                new RCodeCommandChannel[] { new DirectCommandChannel(new RCodeParameters(false), r, "R6;R9\nR60\"hello123123testing\\nhello123123testing\"", out1, false, 100),
                        new DirectCommandChannel(new RCodeParameters(false), r, "R60\"demodemo\"", out2, false, 100) });
        r.getCommandFinder().registerCommand(new RCodeEchoCommand());
        r.getCommandFinder().registerCommand(new RCodeActivateCommand());
        r.getCommandFinder().registerCommand(new RCodeTmpCommand());
        r.getCommandFinder().registerCommand(new RCodeSetDebugChannelCommand(r));
        r.getCommandFinder().registerCommand(new RCodeWriteToDebugCommand(r));
        for (int i = 0; i < 100; i++) {
            r.progressRCode();
        }
        r.getDebug().println("aaaa");
        assertThat(out1.getString()).isEqualTo("AS;S\n#Debug channel set...\n#demodemo\nS\n#hello123123testing\n#hello1\n#Debug buffer out of space, some data lost\n#aaaa\n");
        assertThat(out2.getString()).isEqualTo("S\n");
    }

    @Test
    void shouldIgnoreDebugWithNoChannel() {
        RCodeActivateCommand.reset();
        RCode r = new RCode(new RCodeParameters(false), new RCodeBusInterruptSource[0]);
        StringOutStream out1 = new StringOutStream();
        StringOutStream out2 = new StringOutStream();
        r.setChannels(new RCodeCommandChannel[] { new DirectCommandChannel(new RCodeParameters(false), r, "R6\nR60\"hello123123testing\\nhello123123testing\"", out1, false, 100),
                new DirectCommandChannel(new RCodeParameters(false), r, "R60\"demodemo\"", out2, false, 100) });
        r.getCommandFinder().registerCommand(new RCodeEchoCommand());
        r.getCommandFinder().registerCommand(new RCodeActivateCommand());
        r.getCommandFinder().registerCommand(new RCodeTmpCommand());
        r.getCommandFinder().registerCommand(new RCodeSetDebugChannelCommand(r));
        r.getCommandFinder().registerCommand(new RCodeWriteToDebugCommand(r));
        for (int i = 0; i < 100; i++) {
            r.progressRCode();
        }
        r.getDebug().println("aaaa");
        assertThat(out1.getString()).isEqualTo("AS\nS\n");
        assertThat(out2.getString()).isEqualTo("S\n");
    }

}
