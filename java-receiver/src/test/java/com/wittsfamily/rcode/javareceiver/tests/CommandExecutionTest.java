package com.wittsfamily.rcode.javareceiver.tests;

import org.assertj.core.api.Assertions;
import org.junit.jupiter.api.Test;

import com.wittsfamily.rcode.javareceiver.DirectCommandChannel;
import com.wittsfamily.rcode.javareceiver.RCode;
import com.wittsfamily.rcode.javareceiver.RCodeBusInterruptSource;
import com.wittsfamily.rcode.javareceiver.RCodeParameters;
import com.wittsfamily.rcode.javareceiver.RCodeTmpCommand;
import com.wittsfamily.rcode.javareceiver.StringOutStream;
import com.wittsfamily.rcode.javareceiver.commands.RCodeActivateCommand;
import com.wittsfamily.rcode.javareceiver.commands.RCodeEchoCommand;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandChannel;

class CommandExecutionTest {
    private void testCommand(String input, String output, int num) {
        RCodeActivateCommand.reset();
        RCode r = new RCode(new RCodeParameters(false), new RCodeBusInterruptSource[0]);
        StringOutStream out = new StringOutStream();
        r.setChannels(new RCodeCommandChannel[] { new DirectCommandChannel(new RCodeParameters(false), r, input, out, false, num) });
        r.getCommandFinder().registerCommand(new RCodeEchoCommand());
        r.getCommandFinder().registerCommand(new RCodeActivateCommand());
        r.getCommandFinder().registerCommand(new RCodeTmpCommand());
        for (int i = 0; i < 40; i++) {
            r.progressRCode();
        }
        Assertions.assertThat(out.getString()).isEqualTo(output);
    }

    @Test
    void shouldWorkForBasicEcho() {
        testCommand("R1E0020000Y72Z00X0G\"aaaaaaaaaaaa\"", "E20000SY72ZXG\"aaaaaaaaaaaa\"\n", 1);
    }

    @Test
    void shouldWorkForBasicEchoAndActivateCommand() {
        testCommand("R1E0020000Y72Z00X0G\"aaaaaaaaaaaa\"&R3", "E20000SY72ZXG\"aaaaaaaaaaaa\"&AS\n", 3);
    }

    @Test
    void shouldUseBigBigField() {
        testCommand("R1\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\"\n",
                "S\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\"\n", 1);
    }

    @Test
    void shouldGiveTooBig() {
        testCommand(
                "R1\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\"\n",
                "S8\"Big field too long\"\n", 1);
    }

    @Test
    void shouldGiveTooManyFields() {
        testCommand("R1A99999999999999999999999999", "S8\"Too many fields\"\n", 1);
    }

    @Test
    void shouldGiveUnknownCommand() {
        testCommand("R38489594A309G93+101000", "S4\"Command not known\"\n", 1);
    }

    @Test
    void shouldGiveNotActivated() {
        testCommand("R13+101000", "S7\"Not a system command, and not activated\"\n", 1);
    }

    @Test
    void shouldGiveUnknownFieldMarker() {
        testCommand("R1h00+101000", "S3\"Unknown field marker with character code 104\"\n", 1);
    }

    @Test
    void shouldAllowActivation() {
        testCommand("R3&R13+101011", "AS&S+101011\n", 3);
    }

    @Test
    void shouldRunSequences() {
        testCommand("R3\nR13+101011", "AS\nS+101011\n", 15);
    }

    @Test
    void shouldAbortSequenceOnError() {
        testCommand("R7&R13+101011", "S4\"Command not known\"\n", 3);
    }

    @Test
    void shouldContinueNextSequenceOnError() {
        testCommand("R7\nR1+101011", "S4\"Command not known\"\nS+101011\n", 15);
    }

    @Test
    void shouldAbortCurrentAndContinueNextSequenceOnError() {
        testCommand("R7&R1+aaa\nR1+101011", "S4\"Command not known\"\nS+101011\n", 15);
    }

    @Test
    void shouldGiveMultipleBigFields() {
        testCommand("R1+11\"1\"", "S3\"Multiple big fields\"\n", 1);
    }

    @Test
    void shouldGiveBigFieldOddDigits() {
        testCommand("R1+111", "S3\"Big field odd digits\"\n", 1);
    }

    @Test
    void shouldAllowBroadcast() {
        testCommand("*R1+11", "*S+11\n", 1);
    }

    @Test
    void shouldFailBroadcastAtOtherPoint() {
        testCommand("R3&*R1+11", "AS&S3\"Unknown field marker with character code 42\"\n", 3);
    }

    @Test
    void shouldAllowParallel() {
        testCommand("%R1+11", "S+11\n", 1);
    }

    @Test
    void shouldFailParallelAtOtherPoint() {
        testCommand("R3&%R1+11", "AS&S3\"Unknown field marker with character code 37\"\n", 3);
    }

    @Test
    void shouldAllowParallelBroadcast() {
        testCommand("*%R1+11", "*S+11\n", 1);
    }

    @Test
    void shouldAllowSecondSequenceAsBroadcast() {
        testCommand("R3\n*%R1+11", "AS\n*S+11\n", 15);
    }

}
