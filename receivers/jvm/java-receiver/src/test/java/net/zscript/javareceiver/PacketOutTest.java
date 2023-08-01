package net.zscript.javareceiver;

import static org.assertj.core.api.Assertions.assertThat;

import java.util.List;

import org.junit.jupiter.api.Test;

import net.zscript.javareceiver.Zcode;
import net.zscript.javareceiver.ZcodeBusInterruptSource;
import net.zscript.javareceiver.ZcodeParameters;
import net.zscript.javareceiver.commands.ZcodeActivateCommand;
import net.zscript.javareceiver.commands.ZcodeEchoCommand;
import net.zscript.javareceiver.parsing.ZcodeCommandChannel;
import net.zscript.javareceiver.util.DirectCommandChannel;
import net.zscript.javareceiver.util.StringPacketOutStream;
import net.zscript.javareceiver.util.ZcodeTmpCommand;

class PacketOutTest {
    @Test
    void shouldWorkForBasicEcho() {
        testCommand("Z1E0020000Y72 W00X0G\"aaaaaaaaaaaa\"", List.of("E20000SY72WXG\"aaaaaaaaaaaa\"\n"), 1);
    }

    @Test
    void shouldWorkForMultipleCommands() {
        testCommand("Z1EY20 & Z1H33\nZ3&Z1B", List.of("ESY20&SH33\nAS&SB\n"), 12);
    }

    private void testCommand(final String input, final List<String> output, final int cmdNum) {
        ZcodeActivateCommand.reset();
        Zcode                 z   = new Zcode(new ZcodeParameters(false), new ZcodeBusInterruptSource[0]);
        StringPacketOutStream out = new StringPacketOutStream();

        z.setChannels(new ZcodeCommandChannel[] { new DirectCommandChannel(new ZcodeParameters(false), z, input, out, true, cmdNum) });
        z.getCommandFinder()
                .registerCommand(new ZcodeEchoCommand())
                .registerCommand(new ZcodeActivateCommand())
                .registerCommand(new ZcodeTmpCommand());
        z.progressZcode(20);
        assertThat(out.getPackets()).isEqualTo(output);
    }
}
