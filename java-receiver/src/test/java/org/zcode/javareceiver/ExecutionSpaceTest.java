package org.zcode.javareceiver;

import static org.assertj.core.api.Assertions.assertThat;

import java.nio.charset.StandardCharsets;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.zcode.javareceiver.commands.ZcodeActivateCommand;
import org.zcode.javareceiver.commands.ZcodeEchoCommand;
import org.zcode.javareceiver.commands.ZcodeExecutionSpaceCommand;
import org.zcode.javareceiver.commands.ZcodeSetNotificationHostCommand;
import org.zcode.javareceiver.executionspace.ZcodeExecutionSpaceChannel;
import org.zcode.javareceiver.parsing.ZcodeCommandChannel;
import org.zcode.javareceiver.util.DirectCommandChannel;
import org.zcode.javareceiver.util.StringOutStream;
import org.zcode.javareceiver.util.TestInterruptSource;
import org.zcode.javareceiver.util.ZcodeTmp2Command;
import org.zcode.javareceiver.util.ZcodeTmpCommand;

class ExecutionSpaceTest {
    @BeforeEach
    void reset() {
        ZcodeActivateCommand.reset();
    }

    @Test
    void shouldExecuteCommandsAndReport() {
        ZcodeParameters     params = new ZcodeParameters(true);
        TestInterruptSource intSrc = new TestInterruptSource();
        Zcode               z      = new Zcode(params, new ZcodeBusInterruptSource[] { intSrc });
        StringOutStream     out    = new StringOutStream();
        z.setChannels(new ZcodeCommandChannel[] { new DirectCommandChannel(params, z, "R8&R3\nR21G\n", out, false, 10), new ZcodeExecutionSpaceChannel(params, z, z.getSpace()) });
        z.getSpace().write("R1+10a0&R13A1\nR1+10a1&R11A2\nR33\nR1+10a2&R11A2\n".getBytes(StandardCharsets.US_ASCII), 0, true);
        z.getCommandFinder()
                .registerCommand(new ZcodeEchoCommand())
                .registerCommand(new ZcodeActivateCommand())
                .registerCommand(new ZcodeTmpCommand())
                .registerCommand(new ZcodeTmp2Command())
                .registerCommand(new ZcodeExecutionSpaceCommand(z.getSpace()))
                .registerCommand(new ZcodeSetNotificationHostCommand(z));
        z.progressZcode(80);
        assertThat(out.getString()).isEqualTo("S&AS\nS\n!Z2S+10a1&S10A2\n!Z2S4\"Command not known\"\n");
    }

    @Test
    void shouldNotExecuteCommandsIfNotRunning() {
        ZcodeParameters     params = new ZcodeParameters(true);
        TestInterruptSource intSrc = new TestInterruptSource();
        Zcode               z      = new Zcode(params, new ZcodeBusInterruptSource[] { intSrc });
        StringOutStream     out    = new StringOutStream();
        z.setChannels(new ZcodeCommandChannel[] { new DirectCommandChannel(params, z, "R8&R3\nR21\n", out, false, 10), new ZcodeExecutionSpaceChannel(params, z, z.getSpace()) });
        z.getSpace().write("R1+10a0&R13A1\nR1+10a1&R11A2\nR33\nR1+10a2&R11A2\n".getBytes(StandardCharsets.US_ASCII), 0, true);
        z.getCommandFinder()
                .registerCommand(new ZcodeEchoCommand())
                .registerCommand(new ZcodeActivateCommand())
                .registerCommand(new ZcodeTmpCommand())
                .registerCommand(new ZcodeTmp2Command())
                .registerCommand(new ZcodeExecutionSpaceCommand(z.getSpace()))
                .registerCommand(new ZcodeSetNotificationHostCommand(z));
        z.progressZcode(40);
        assertThat(out.getString()).isEqualTo("S&AS\nS\n");
    }

    @Test
    void shouldExecuteMultipleCommandChannels() {
        ZcodeParameters     params = new ZcodeParameters(true);
        TestInterruptSource intSrc = new TestInterruptSource();
        Zcode               z      = new Zcode(params, new ZcodeBusInterruptSource[] { intSrc });
        StringOutStream     out    = new StringOutStream();
        z.setChannels(new ZcodeCommandChannel[] { new DirectCommandChannel(params, z, "R8&R3\nR21G\nR1+00\nR1+01\nR1+02", out, false, 40),
                new ZcodeExecutionSpaceChannel(params, z, z.getSpace()) });
        z.getSpace().write("R11A2\nR11A3\nR11A4\nR11A5\nR1+10&R21\n".getBytes(StandardCharsets.US_ASCII), 0, true);
        z.getCommandFinder()
                .registerCommand(new ZcodeEchoCommand())
                .registerCommand(new ZcodeActivateCommand())
                .registerCommand(new ZcodeTmpCommand())
                .registerCommand(new ZcodeTmp2Command())
                .registerCommand(new ZcodeExecutionSpaceCommand(z.getSpace()))
                .registerCommand(new ZcodeSetNotificationHostCommand(z));
        z.progressZcode(100);
        assertThat(out.getString()).isEqualTo("S&AS\nS\n!Z2S10A2\nS+00\n!Z2S10A3\nS+01\n!Z2S10A4\nS+02\n!Z2S10A5\n");
    }

    @Test
    void shouldFailExecuteWhenOverLength() {
        ZcodeParameters     params = new ZcodeParameters(true);
        TestInterruptSource intSrc = new TestInterruptSource();
        Zcode               z      = new Zcode(params, new ZcodeBusInterruptSource[] { intSrc });
        StringOutStream     out    = new StringOutStream();
        z.setChannels(new ZcodeCommandChannel[] { new DirectCommandChannel(params, z, "R8&R3\nR21G\n", out, false, 12), new ZcodeExecutionSpaceChannel(params, z, z.getSpace()) });
        z.getSpace().write("R1\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaa\"&R1\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaa\"&R11A7\n".getBytes(StandardCharsets.US_ASCII), 0, true);
        z.getSpace().setDelay(5);
        z.getCommandFinder()
                .registerCommand(new ZcodeEchoCommand())
                .registerCommand(new ZcodeActivateCommand())
                .registerCommand(new ZcodeTmpCommand())
                .registerCommand(new ZcodeTmp2Command())
                .registerCommand(new ZcodeExecutionSpaceCommand(z.getSpace()))
                .registerCommand(new ZcodeSetNotificationHostCommand(z));
        z.progressZcode(15);
        assertThat(out.getString()).isEqualTo("S&AS\nS\n!Z2S\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaa\"&S1&S10\n");
    }

    @Test
    void shouldFailExecuteWhenOverLengthSingleCommand() {
        ZcodeParameters     params = new ZcodeParameters(true);
        TestInterruptSource intSrc = new TestInterruptSource();
        Zcode               z      = new Zcode(params, new ZcodeBusInterruptSource[] { intSrc });
        StringOutStream     out    = new StringOutStream();

        z.setChannels(new ZcodeCommandChannel[] { new DirectCommandChannel(params, z, "R8&R3\nR21G\n", out, false, 12), new ZcodeExecutionSpaceChannel(params, z, z.getSpace()) });
        z.getSpace().write("R1\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\"&R11A7\n".getBytes(StandardCharsets.US_ASCII), 0, true);
        z.getSpace().setDelay(2);

        z.getCommandFinder()
                .registerCommand(new ZcodeEchoCommand())
                .registerCommand(new ZcodeActivateCommand())
                .registerCommand(new ZcodeTmpCommand())
                .registerCommand(new ZcodeTmp2Command())
                .registerCommand(new ZcodeExecutionSpaceCommand(z.getSpace()))
                .registerCommand(new ZcodeSetNotificationHostCommand(z));
        z.progressZcode(12);
        assertThat(out.getString()).isEqualTo("S&AS\nS\n!Z2&S1&S10\n");
    }

}
