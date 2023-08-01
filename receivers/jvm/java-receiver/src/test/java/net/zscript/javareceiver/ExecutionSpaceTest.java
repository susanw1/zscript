package net.zscript.javareceiver;

import static org.assertj.core.api.Assertions.assertThat;

import java.nio.charset.StandardCharsets;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import net.zscript.javareceiver.Zcode;
import net.zscript.javareceiver.ZcodeBusInterruptSource;
import net.zscript.javareceiver.ZcodeParameters;
import net.zscript.javareceiver.commands.ZcodeActivateCommand;
import net.zscript.javareceiver.commands.ZcodeEchoCommand;
import net.zscript.javareceiver.commands.ZcodeExecutionSpaceCommand;
import net.zscript.javareceiver.commands.ZcodeSetNotificationHostCommand;
import net.zscript.javareceiver.executionspace.ZcodeExecutionSpaceChannel;
import net.zscript.javareceiver.parsing.ZcodeCommandChannel;
import net.zscript.javareceiver.util.DirectCommandChannel;
import net.zscript.javareceiver.util.StringOutStream;
import net.zscript.javareceiver.util.TestInterruptSource;
import net.zscript.javareceiver.util.ZcodeTmp2Command;
import net.zscript.javareceiver.util.ZcodeTmpCommand;

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
        z.setChannels(new ZcodeCommandChannel[] { new DirectCommandChannel(params, z, "Z8&Z3\nZ21G\n", out, false, 10), new ZcodeExecutionSpaceChannel(params, z, z.getSpace()) });
        z.getSpace().write("Z1+10a0&Z13A1\nZ1+10a1&Z11A2\nZ33\nZ1+10a2&Z11A2\n".getBytes(StandardCharsets.US_ASCII), 0, true);
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
        z.setChannels(new ZcodeCommandChannel[] { new DirectCommandChannel(params, z, "Z8&Z3\nZ21\n", out, false, 10), new ZcodeExecutionSpaceChannel(params, z, z.getSpace()) });
        z.getSpace().write("Z1+10a0&Z13A1\nZ1+10a1&Z11A2\nZ33\nZ1+10a2&Z11A2\n".getBytes(StandardCharsets.US_ASCII), 0, true);
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
        z.setChannels(new ZcodeCommandChannel[] { new DirectCommandChannel(params, z, "Z8&Z3\nZ21G\nZ1+00\nZ1+01\nZ1+02", out, false, 40),
                new ZcodeExecutionSpaceChannel(params, z, z.getSpace()) });
        z.getSpace().write("Z11A2\nZ11A3\nZ11A4\nZ11A5\nZ1+10&Z21\n".getBytes(StandardCharsets.US_ASCII), 0, true);
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
        z.setChannels(new ZcodeCommandChannel[] { new DirectCommandChannel(params, z, "Z8&Z3\nZ21G\n", out, false, 12), new ZcodeExecutionSpaceChannel(params, z, z.getSpace()) });
        z.getSpace().write("Z1\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaa\"&Z1\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaa\"&Z11A7\n".getBytes(StandardCharsets.US_ASCII), 0, true);
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

        z.setChannels(new ZcodeCommandChannel[] { new DirectCommandChannel(params, z, "Z8&Z3\nZ21G\n", out, false, 12), new ZcodeExecutionSpaceChannel(params, z, z.getSpace()) });
        z.getSpace().write("Z1\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\"&Z11A7\n".getBytes(StandardCharsets.US_ASCII), 0, true);
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
