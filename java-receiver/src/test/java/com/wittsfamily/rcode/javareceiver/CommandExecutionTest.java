package com.wittsfamily.rcode.javareceiver;

import org.assertj.core.api.Assertions;
import org.junit.jupiter.api.Test;

import com.wittsfamily.rcode.javareceiver.RCode;
import com.wittsfamily.rcode.javareceiver.RCodeBusInterruptSource;
import com.wittsfamily.rcode.javareceiver.RCodeParameters;
import com.wittsfamily.rcode.javareceiver.commands.RCodeActivateCommand;
import com.wittsfamily.rcode.javareceiver.commands.RCodeEchoCommand;
import com.wittsfamily.rcode.javareceiver.commands.RCodeTmpCommand;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandChannel;
import com.wittsfamily.rcode.javareceiver.test.DirectCommandChannel;
import com.wittsfamily.rcode.javareceiver.test.StringOutStream;

class CommandExecutionTest {
    private void testCommand(String input, String output) {
        RCodeActivateCommand.reset();
        RCode r = new RCode(new RCodeParameters(false), new RCodeBusInterruptSource[0]);
        StringOutStream out = new StringOutStream();
        r.setChannels(new RCodeCommandChannel[] { new DirectCommandChannel(new RCodeParameters(false), r, input, out, false, 100) });
        r.getCommandFinder().registerCommand(new RCodeEchoCommand());
        r.getCommandFinder().registerCommand(new RCodeActivateCommand());
        r.getCommandFinder().registerCommand(new RCodeTmpCommand());
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        Assertions.assertThat(out.getString()).isEqualTo(output);
    }

    @Test
    void shouldWorkForBasicEcho() {
        testCommand("R1E0020000Y72Z00X0G\"aaaaaaaaaaaa\"", "E20000SY72ZXG\"aaaaaaaaaaaa\"\n");
    }

    @Test
    void shouldWorkForBasicEchoAndActivateCommand() {
        testCommand("R1E0020000Y72Z00X0G\"aaaaaaaaaaaa\";R6", "E20000SY72ZXG\"aaaaaaaaaaaa\";AS\n");
    }

    @Test
    void shouldUseBigBigField() {
        testCommand("R1\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\"",
                "S\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\"\n");
    }

    @Test
    void shouldGiveTooBig() {
        testCommand(
                "R1\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\"",
                "S8\"Big field too long\"\n");
    }

    @Test
    void shouldGiveTooManyFields() {
        testCommand("R1A99999999999999999999999999", "S8\"Too many fields\"\n");
    }

    @Test
    void shouldGiveUnknownCommand() {
        testCommand("R38489594A309G93+101000", "S4\"Command not known\"\n");
    }

    @Test
    void shouldGiveNotActivated() {
        testCommand("R10+101000", "S7\"Not a system command, and not activated\"\n");
    }

    @Test
    void shouldGiveUnknownFieldMarker() {
        testCommand("R1h00+101000", "S3\"Unknown field marker: h\"\n");
    }

    @Test
    void shouldAllowActivation() {
        testCommand("R6;R10+101011", "AS;S+101011\n");
    }

    @Test
    void shouldRunSequences() {
        testCommand("R6\nR10+101011", "AS\nS+101011\n");
    }

    @Test
    void shouldAbortSequenceOnError() {
        testCommand("R7;R10+101011", "S4\"Command not known\"\n");
    }

    @Test
    void shouldContinueNextSequenceOnError() {
        testCommand("R7\nR1+101011", "S4\"Command not known\"\nS+101011\n");
    }

    @Test
    void shouldAbortCurrentAndContinueNextSequenceOnError() {
        testCommand("R7;R1+aaa\nR1+101011", "S4\"Command not known\"\nS+101011\n");
    }

    @Test
    void shouldGiveMultipleBigFields() {
        testCommand("R1+11\"1\"", "S3\"Multiple big fields\"\n");
    }

    @Test
    void shouldGiveBigFieldOddDigits() {
        testCommand("R1+111", "S3\"Big field odd digits\"\n");
    }

    @Test
    void shouldAllowBroadcast() {
        testCommand("*R1+11", "*S+11\n");
    }

    @Test
    void shouldFailBroadcastAtOtherPoint() {
        testCommand("R6;*R1+11", "AS;S3\"* only valid on first command of sequence\"\n");
    }

    @Test
    void shouldAllowParallel() {
        testCommand("%R1+11", "S+11\n");
    }

    @Test
    void shouldFailParallelAtOtherPoint() {
        testCommand("R6;%R1+11", "AS;S3\"% only valid on first command of sequence\"\n");
    }

    @Test
    void shouldAllowParallelBroadcast() {
        testCommand("*%R1+11", "*S+11\n");
    }

    @Test
    void shouldAllowSecondSequenceAsBroadcast() {
        testCommand("R6\n*%R1+11", "AS\n*S+11\n");
    }

}
