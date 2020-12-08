package com.wittsfamily.rcode.javareceiver.tests;

import static org.assertj.core.api.Assertions.assertThat;

import java.nio.charset.StandardCharsets;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import com.wittsfamily.rcode.javareceiver.DirectCommandChannel;
import com.wittsfamily.rcode.javareceiver.RCode;
import com.wittsfamily.rcode.javareceiver.RCodeBusInterruptSource;
import com.wittsfamily.rcode.javareceiver.RCodeParameters;
import com.wittsfamily.rcode.javareceiver.RCodeTmp2Command;
import com.wittsfamily.rcode.javareceiver.RCodeTmpCommand;
import com.wittsfamily.rcode.javareceiver.StringOutStream;
import com.wittsfamily.rcode.javareceiver.TestInterruptSource;
import com.wittsfamily.rcode.javareceiver.commands.RCodeActivateCommand;
import com.wittsfamily.rcode.javareceiver.commands.RCodeEchoCommand;
import com.wittsfamily.rcode.javareceiver.commands.RCodeExecutionSpaceCommand;
import com.wittsfamily.rcode.javareceiver.commands.RCodeSetNotificationHostCommand;
import com.wittsfamily.rcode.javareceiver.executionspace.RCodeExecutionSpaceChannel;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandChannel;

class ExecutionSpaceTest {
    @BeforeEach
    void reset() {
        RCodeActivateCommand.reset();
    }

    @Test
    void shouldExecuteCommandsAndReport() {
        RCodeParameters params = new RCodeParameters(true);
        TestInterruptSource intS = new TestInterruptSource();
        RCode r = new RCode(params, new RCodeBusInterruptSource[] { intS });
        StringOutStream out = new StringOutStream();
        r.setChannels(new RCodeCommandChannel[] { new DirectCommandChannel(params, r, "R8&R6\nR21G\n", out, false, 10), new RCodeExecutionSpaceChannel(params, r, r.getSpace()) });
        r.getSpace().write("R1+10a0&R13A1\nR1+10a1&R11A2\nR33\nR1+10a2&R11A2\n".getBytes(StandardCharsets.US_ASCII), 0, true);
        r.getCommandFinder().registerCommand(new RCodeEchoCommand());
        r.getCommandFinder().registerCommand(new RCodeActivateCommand());
        r.getCommandFinder().registerCommand(new RCodeTmpCommand());
        r.getCommandFinder().registerCommand(new RCodeTmp2Command());
        r.getCommandFinder().registerCommand(new RCodeExecutionSpaceCommand(r.getSpace()));
        r.getCommandFinder().registerCommand(new RCodeSetNotificationHostCommand(r));
        for (int i = 0; i < 80; i++) {
            r.progressRCode();
        }
        assertThat(out.getString()).isEqualTo("S&AS\nS\n!Z2S+10a1&S10A2\n!Z2S4\"Command not known\"\n");
    }

    @Test
    void shouldNotExecuteCommandsIfNotRunning() {
        RCodeParameters params = new RCodeParameters(true);
        TestInterruptSource intS = new TestInterruptSource();
        RCode r = new RCode(params, new RCodeBusInterruptSource[] { intS });
        StringOutStream out = new StringOutStream();
        r.setChannels(new RCodeCommandChannel[] { new DirectCommandChannel(params, r, "R8&R6\nR21\n", out, false, 10), new RCodeExecutionSpaceChannel(params, r, r.getSpace()) });
        r.getSpace().write("R1+10a0&R13A1\nR1+10a1&R11A2\nR33\nR1+10a2&R11A2\n".getBytes(StandardCharsets.US_ASCII), 0, true);
        r.getCommandFinder().registerCommand(new RCodeEchoCommand());
        r.getCommandFinder().registerCommand(new RCodeActivateCommand());
        r.getCommandFinder().registerCommand(new RCodeTmpCommand());
        r.getCommandFinder().registerCommand(new RCodeTmp2Command());
        r.getCommandFinder().registerCommand(new RCodeExecutionSpaceCommand(r.getSpace()));
        r.getCommandFinder().registerCommand(new RCodeSetNotificationHostCommand(r));
        for (int i = 0; i < 40; i++) {
            r.progressRCode();
        }
        assertThat(out.getString()).isEqualTo("S&AS\nS\n");
    }

    @Test
    void shouldExecuteMultipleCommandChannels() {
        RCodeParameters params = new RCodeParameters(true);
        TestInterruptSource intS = new TestInterruptSource();
        RCode r = new RCode(params, new RCodeBusInterruptSource[] { intS });
        StringOutStream out = new StringOutStream();
        r.setChannels(new RCodeCommandChannel[] { new DirectCommandChannel(params, r, "R8&R6\nR21G\nR1+00\nR1+01\nR1+02", out, false, 40),
                new RCodeExecutionSpaceChannel(params, r, r.getSpace()) });
        r.getSpace().write("R11A2\nR11A3\nR11A4\nR11A5\nR1+10&R21\n".getBytes(StandardCharsets.US_ASCII), 0, true);
        r.getCommandFinder().registerCommand(new RCodeEchoCommand());
        r.getCommandFinder().registerCommand(new RCodeActivateCommand());
        r.getCommandFinder().registerCommand(new RCodeTmpCommand());
        r.getCommandFinder().registerCommand(new RCodeTmp2Command());
        r.getCommandFinder().registerCommand(new RCodeExecutionSpaceCommand(r.getSpace()));
        r.getCommandFinder().registerCommand(new RCodeSetNotificationHostCommand(r));
        for (int i = 0; i < 100; i++) {
            r.progressRCode();
        }
        assertThat(out.getString()).isEqualTo("S&AS\nS\n!Z2S10A2\nS+00\n!Z2S10A3\nS+01\n!Z2S10A4\nS+02\n!Z2S10A5\n");
    }

    @Test
    void shouldFailExecuteWhenOverLength() {
        RCodeParameters params = new RCodeParameters(true);
        TestInterruptSource intS = new TestInterruptSource();
        RCode r = new RCode(params, new RCodeBusInterruptSource[] { intS });
        StringOutStream out = new StringOutStream();
        r.setChannels(new RCodeCommandChannel[] { new DirectCommandChannel(params, r, "R8&R6\nR21G\n", out, false, 12), new RCodeExecutionSpaceChannel(params, r, r.getSpace()) });
        r.getSpace().write("R1\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaa\"&R1\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaa\"&R11A7\n".getBytes(StandardCharsets.US_ASCII), 0, true);
        r.getSpace().setDelay(5);
        r.getCommandFinder().registerCommand(new RCodeEchoCommand());
        r.getCommandFinder().registerCommand(new RCodeActivateCommand());
        r.getCommandFinder().registerCommand(new RCodeTmpCommand());
        r.getCommandFinder().registerCommand(new RCodeTmp2Command());
        r.getCommandFinder().registerCommand(new RCodeExecutionSpaceCommand(r.getSpace()));
        r.getCommandFinder().registerCommand(new RCodeSetNotificationHostCommand(r));
        for (int i = 0; i < 15; i++) {
            r.progressRCode();
        }
        assertThat(out.getString()).isEqualTo("S&AS\nS\n!Z2S\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaa\"&S1&S10\n");
    }

    @Test
    void shouldFailExecuteWhenOverLengthSingleCommand() {
        RCodeParameters params = new RCodeParameters(true);
        TestInterruptSource intS = new TestInterruptSource();
        RCode r = new RCode(params, new RCodeBusInterruptSource[] { intS });
        StringOutStream out = new StringOutStream();
        r.setChannels(new RCodeCommandChannel[] { new DirectCommandChannel(params, r, "R8&R6\nR21G\n", out, false, 12), new RCodeExecutionSpaceChannel(params, r, r.getSpace()) });
        r.getSpace().write("R1\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\"&R11A7\n".getBytes(StandardCharsets.US_ASCII), 0, true);
        r.getSpace().setDelay(2);
        r.getCommandFinder().registerCommand(new RCodeEchoCommand());
        r.getCommandFinder().registerCommand(new RCodeActivateCommand());
        r.getCommandFinder().registerCommand(new RCodeTmpCommand());
        r.getCommandFinder().registerCommand(new RCodeTmp2Command());
        r.getCommandFinder().registerCommand(new RCodeExecutionSpaceCommand(r.getSpace()));
        r.getCommandFinder().registerCommand(new RCodeSetNotificationHostCommand(r));
        for (int i = 0; i < 12; i++) {
            r.progressRCode();
        }
        assertThat(out.getString()).isEqualTo("S&AS\nS\n!Z2&S1&S10\n");
    }

}
