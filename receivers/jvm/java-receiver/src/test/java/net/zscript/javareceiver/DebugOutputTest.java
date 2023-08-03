package net.zscript.javareceiver;

import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.Test;

import net.zscript.javareceiver.Zcode;
import net.zscript.javareceiver.ZcodeBusInterruptSource;
import net.zscript.javareceiver.ZcodeParameters;
import net.zscript.javareceiver.commands.ZcodeActivateCommand;
import net.zscript.javareceiver.commands.ZcodeEchoCommand;
import net.zscript.javareceiver.commands.ZcodeSetDebugChannelCommand;
import net.zscript.javareceiver.parsing.ZcodeCommandChannel;
import net.zscript.javareceiver.util.DirectCommandChannel;
import net.zscript.javareceiver.util.StringOutStream;
import net.zscript.javareceiver.util.ZcodeTmpCommand;
import net.zscript.javareceiver.util.ZcodeWriteToDebugCommand;

class DebugOutputTest {

    @Test
    void shouldIgnoreComments() {
        ZcodeActivateCommand.reset();
        Zcode           z   = new Zcode(new ZcodeParameters(false), new ZcodeBusInterruptSource[0]);
        StringOutStream out = new StringOutStream();
        z.setChannels(new ZcodeCommandChannel[] { new DirectCommandChannel(new ZcodeParameters(false), z, "#234342\nZ3&Z1B01\n#&1&2&3&1&2&3&\nZ1CV40\"hello\"", out, false, 100) });
        z.getCommandFinder().registerCommand(new ZcodeEchoCommand())
                .registerCommand(new ZcodeActivateCommand())
                .registerCommand(new ZcodeTmpCommand())
                .registerCommand(new ZcodeSetDebugChannelCommand(z))
                .registerCommand(new ZcodeWriteToDebugCommand(z));
        z.progressZcode(100);
        z.getDebug().println("aaaa");
        assertThat(out.getString()).isEqualTo("AS&SB1\nSCV40\"hello\"\n");
    }

    @Test
    void shouldOutputDebug() {
        ZcodeActivateCommand.reset();
        Zcode           z    = new Zcode(new ZcodeParameters(false), new ZcodeBusInterruptSource[0]);
        StringOutStream out1 = new StringOutStream();
        StringOutStream out2 = new StringOutStream();
        z.setChannels(
                new ZcodeCommandChannel[] { new DirectCommandChannel(new ZcodeParameters(false), z, "Z3&Z9\nZ60\"hello123123testing\\nhello123123testing\"", out1, false, 100),
                        new DirectCommandChannel(new ZcodeParameters(false), z, "Z60\"demodemo\"", out2, false, 100) });
        z.getCommandFinder().registerCommand(new ZcodeEchoCommand())
                .registerCommand(new ZcodeActivateCommand())
                .registerCommand(new ZcodeTmpCommand())
                .registerCommand(new ZcodeSetDebugChannelCommand(z))
                .registerCommand(new ZcodeWriteToDebugCommand(z));
        z.progressZcode(100);
        z.getDebug().println("aaaa");
        assertThat(out1.getString()).isEqualTo("AS&S\n#Debug channel set...\n#demodemo\nS\n#hello123123testing\n#hello1\n#Debug buffer out of space, some data lost\n#aaaa\n");
        assertThat(out2.getString()).isEqualTo("S\n");
    }

    @Test
    void shouldIgnoreDebugWithNoChannel() {
        ZcodeActivateCommand.reset();
        Zcode           z    = new Zcode(new ZcodeParameters(false), new ZcodeBusInterruptSource[0]);
        StringOutStream out1 = new StringOutStream();
        StringOutStream out2 = new StringOutStream();
        z.setChannels(new ZcodeCommandChannel[] { new DirectCommandChannel(new ZcodeParameters(false), z, "Z3\nZ60\"hello123123testing\\nhello123123testing\"", out1, false, 100),
                new DirectCommandChannel(new ZcodeParameters(false), z, "Z60\"demodemo\"", out2, false, 100) });
        z.getCommandFinder().registerCommand(new ZcodeEchoCommand())
                .registerCommand(new ZcodeActivateCommand())
                .registerCommand(new ZcodeTmpCommand())
                .registerCommand(new ZcodeSetDebugChannelCommand(z))
                .registerCommand(new ZcodeWriteToDebugCommand(z));
        z.progressZcode(100);
        z.getDebug().println("aaaa");
        assertThat(out1.getString()).isEqualTo("AS\nS\n");
        assertThat(out2.getString()).isEqualTo("S\n");
    }

}