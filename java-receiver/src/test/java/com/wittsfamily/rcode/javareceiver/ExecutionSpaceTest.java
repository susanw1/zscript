package com.wittsfamily.rcode.javareceiver;

import static org.assertj.core.api.Assertions.assertThat;

import java.nio.charset.StandardCharsets;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import com.wittsfamily.rcode.javareceiver.commands.RCodeActivateCommand;
import com.wittsfamily.rcode.javareceiver.commands.RCodeEchoCommand;
import com.wittsfamily.rcode.javareceiver.commands.RCodeExecutionSpaceCommand;
import com.wittsfamily.rcode.javareceiver.commands.RCodeSetNotificationHostCommand;
import com.wittsfamily.rcode.javareceiver.commands.RCodeTmp2Command;
import com.wittsfamily.rcode.javareceiver.commands.RCodeTmpCommand;
import com.wittsfamily.rcode.javareceiver.executionspace.RCodeExecutionSpaceChannel;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandChannel;
import com.wittsfamily.rcode.javareceiver.test.DirectCommandChannel;
import com.wittsfamily.rcode.javareceiver.test.StringOutStream;
import com.wittsfamily.rcode.javareceiver.test.TestInterruptSource;

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
        r.setChannels(new RCodeCommandChannel[] { new DirectCommandChannel(params, r, "R8;R6\nR21G\n", out, false, 10), new RCodeExecutionSpaceChannel(params, r, r.getSpace()) });
        r.getSpace().write("R1+10a0;R13A1\nR1+10a1;R11A2\nR33\nR1+10a2;R11A2\n".getBytes(StandardCharsets.US_ASCII), 0, true);
        r.getCommandFinder().registerCommand(new RCodeEchoCommand());
        r.getCommandFinder().registerCommand(new RCodeActivateCommand());
        r.getCommandFinder().registerCommand(new RCodeTmpCommand());
        r.getCommandFinder().registerCommand(new RCodeTmp2Command());
        r.getCommandFinder().registerCommand(new RCodeExecutionSpaceCommand(r.getSpace()));
        r.getCommandFinder().registerCommand(new RCodeSetNotificationHostCommand(r));
        for (int i = 0; i < 40; i++) {
            r.progressRCode();
        }
        assertThat(out.getString()).isEqualTo("S;AS\nS\n!Z2S+10a1;S10A2\n!Z2S4\"Command not known\"\n");
    }

    @Test
    void shouldNotExecuteCommandsIfNotRunning() {
        RCodeParameters params = new RCodeParameters(true);
        TestInterruptSource intS = new TestInterruptSource();
        RCode r = new RCode(params, new RCodeBusInterruptSource[] { intS });
        StringOutStream out = new StringOutStream();
        r.setChannels(new RCodeCommandChannel[] { new DirectCommandChannel(params, r, "R8;R6\nR21\n", out, false, 100), new RCodeExecutionSpaceChannel(params, r, r.getSpace()) });
        r.getSpace().write("R1+10a0;R13A1\nR1+10a1;R11A2\nR33\nR1+10a2;R11A2\n".getBytes(StandardCharsets.US_ASCII), 0, true);
        r.getCommandFinder().registerCommand(new RCodeEchoCommand());
        r.getCommandFinder().registerCommand(new RCodeActivateCommand());
        r.getCommandFinder().registerCommand(new RCodeTmpCommand());
        r.getCommandFinder().registerCommand(new RCodeTmp2Command());
        r.getCommandFinder().registerCommand(new RCodeExecutionSpaceCommand(r.getSpace()));
        r.getCommandFinder().registerCommand(new RCodeSetNotificationHostCommand(r));
        for (int i = 0; i < 40; i++) {
            r.progressRCode();
        }
        assertThat(out.getString()).isEqualTo("S;AS\nS\n");
    }

    @Test
    void shouldExecuteMultipleCommandChannels() {
        RCodeParameters params = new RCodeParameters(true);
        TestInterruptSource intS = new TestInterruptSource();
        RCode r = new RCode(params, new RCodeBusInterruptSource[] { intS });
        StringOutStream out = new StringOutStream();
        r.setChannels(new RCodeCommandChannel[] { new DirectCommandChannel(params, r, "R8;R6\nR21G\nR1+00\nR1+01\nR1+02", out, false, 100),
                new RCodeExecutionSpaceChannel(params, r, r.getSpace()) });
        r.getSpace().write("R11A2\nR11A3\nR11A4\nR11A5\nR1+10;R21\n".getBytes(StandardCharsets.US_ASCII), 0, true);
        r.getCommandFinder().registerCommand(new RCodeEchoCommand());
        r.getCommandFinder().registerCommand(new RCodeActivateCommand());
        r.getCommandFinder().registerCommand(new RCodeTmpCommand());
        r.getCommandFinder().registerCommand(new RCodeTmp2Command());
        r.getCommandFinder().registerCommand(new RCodeExecutionSpaceCommand(r.getSpace()));
        r.getCommandFinder().registerCommand(new RCodeSetNotificationHostCommand(r));
        for (int i = 0; i < 100; i++) {
            r.progressRCode();
        }
        assertThat(out.getString()).isEqualTo("S;AS\nS\n!Z2S10A2\nS+00\n!Z2S10A3\nS+01\n!Z2S10A4\nS+02\n!Z2S10A5\n");
    }

    @Test
    void shouldFailExecuteWhenOverLength() {
        RCodeParameters params = new RCodeParameters(true);
        TestInterruptSource intS = new TestInterruptSource();
        RCode r = new RCode(params, new RCodeBusInterruptSource[] { intS });
        StringOutStream out = new StringOutStream();
        r.setChannels(new RCodeCommandChannel[] { new DirectCommandChannel(params, r, "R8;R6\nR21G\n", out, false, 100), new RCodeExecutionSpaceChannel(params, r, r.getSpace()) });
        r.getSpace().write("R1\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaa\";R1\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaa\";R11A7\n".getBytes(StandardCharsets.US_ASCII), 0, true);
        r.getCommandFinder().registerCommand(new RCodeEchoCommand());
        r.getCommandFinder().registerCommand(new RCodeActivateCommand());
        r.getCommandFinder().registerCommand(new RCodeTmpCommand());
        r.getCommandFinder().registerCommand(new RCodeTmp2Command());
        r.getCommandFinder().registerCommand(new RCodeExecutionSpaceCommand(r.getSpace()));
        r.getCommandFinder().registerCommand(new RCodeSetNotificationHostCommand(r));
        for (int i = 0; i < 10; i++) {
            r.progressRCode();
        }
        assertThat(out.getString()).isEqualTo("S;AS\nS\n!Z2S\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaa\";S1;S10\n");
    }

    @Test
    void shouldFailExecuteWhenOverLengthSingleCommand() {
        RCodeParameters params = new RCodeParameters(true);
        TestInterruptSource intS = new TestInterruptSource();
        RCode r = new RCode(params, new RCodeBusInterruptSource[] { intS });
        StringOutStream out = new StringOutStream();
        r.setChannels(new RCodeCommandChannel[] { new DirectCommandChannel(params, r, "R8;R6\nR21G\n", out, false, 100), new RCodeExecutionSpaceChannel(params, r, r.getSpace()) });
        r.getSpace().write("R1\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\";R11A7\n".getBytes(StandardCharsets.US_ASCII), 0, true);
        r.getCommandFinder().registerCommand(new RCodeEchoCommand());
        r.getCommandFinder().registerCommand(new RCodeActivateCommand());
        r.getCommandFinder().registerCommand(new RCodeTmpCommand());
        r.getCommandFinder().registerCommand(new RCodeTmp2Command());
        r.getCommandFinder().registerCommand(new RCodeExecutionSpaceCommand(r.getSpace()));
        r.getCommandFinder().registerCommand(new RCodeSetNotificationHostCommand(r));
        for (int i = 0; i < 10; i++) {
            r.progressRCode();
        }
        assertThat(out.getString()).isEqualTo("S;AS\nS\n!Z2;S1;S10\n");
    }

}
