package com.wittsfamily.rcode.javareceiver.tests;

import java.util.List;

import org.assertj.core.api.Assertions;
import org.junit.jupiter.api.Test;

import com.wittsfamily.rcode.javareceiver.DirectCommandChannel;
import com.wittsfamily.rcode.javareceiver.RCode;
import com.wittsfamily.rcode.javareceiver.RCodeBusInterruptSource;
import com.wittsfamily.rcode.javareceiver.RCodeParameters;
import com.wittsfamily.rcode.javareceiver.RCodeTmpCommand;
import com.wittsfamily.rcode.javareceiver.StringPacketOutStream;
import com.wittsfamily.rcode.javareceiver.commands.RCodeActivateCommand;
import com.wittsfamily.rcode.javareceiver.commands.RCodeEchoCommand;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandChannel;

class PacketOutTest {
    private void testCommand(String input, List<String> output, int cmdNum) {
        RCodeActivateCommand.reset();
        RCode r = new RCode(new RCodeParameters(false), new RCodeBusInterruptSource[0]);
        StringPacketOutStream out = new StringPacketOutStream();
        r.setChannels(new RCodeCommandChannel[] { new DirectCommandChannel(new RCodeParameters(false), r, input, out, true, cmdNum) });
        r.getCommandFinder().registerCommand(new RCodeEchoCommand());
        r.getCommandFinder().registerCommand(new RCodeActivateCommand());
        r.getCommandFinder().registerCommand(new RCodeTmpCommand());
        for (int i = 0; i < 20; i++) {
            r.progressRCode();
        }
        Assertions.assertThat(out.getPackets()).isEqualTo(output);
    }

    @Test
    void shouldWorkForBasicEcho() {
        testCommand("R1E0020000Y72Z00X0G\"aaaaaaaaaaaa\"", List.of("E20000SY72ZXG\"aaaaaaaaaaaa\"\n"), 1);
    }

    @Test
    void shouldWorkForMultipleCommands() {
        testCommand("R1EY20&R1H33\nR3&R1B", List.of("ESY20&SH33\nAS&SB\n"), 12);
    }

}
