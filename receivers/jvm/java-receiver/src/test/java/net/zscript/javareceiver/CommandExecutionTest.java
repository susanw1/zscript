package net.zscript.javareceiver;

import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.Test;

import net.zscript.javareceiver.Zcode;
import net.zscript.javareceiver.ZcodeBusInterruptSource;
import net.zscript.javareceiver.ZcodeParameters;
import net.zscript.javareceiver.commands.ZcodeActivateCommand;
import net.zscript.javareceiver.commands.ZcodeEchoCommand;
import net.zscript.javareceiver.parsing.ZcodeCommandChannel;
import net.zscript.javareceiver.util.DirectCommandChannel;
import net.zscript.javareceiver.util.StringOutStream;
import net.zscript.javareceiver.util.ZcodeTmpCommand;

class CommandExecutionTest {
    @Test
    void shouldWorkForBasicEcho() {
        testCommand("Z1E0020000Y72W00X0G\"aaaaaaaaaaaa\"", "E20000SY72WXG\"aaaaaaaaaaaa\"\n", 1);
    }

    @Test
    void shouldWorkForBasicEchoAndActivateCommand() {
        testCommand("Z1E0020000Y72W00X0G\"aaaaaaaaaaaa\" & Z3", "E20000SY72WXG\"aaaaaaaaaaaa\"&AS\n", 3);
    }

    @Test
    void shouldUseHugeField() {
        testCommand("Z1\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\"\n",
                "S\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\"\n", 1);
    }

    @Test
    void shouldGiveTooBig() {
        testCommand(
                "Z1\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\"\n",
                "S8\"Big field too long\"\n", 1);
    }

    @Test
    void shouldGiveTooManyFields() {
        testCommand("Z1A99999999999999999999999999", "S8\"Too many fields\"\n", 1);
    }

    @Test
    void shouldGiveUnknownCommand() {
        testCommand("Z38489594A309G93+101000", "S4\"Command not known\"\n", 1);
    }

    @Test
    void shouldGiveNotActivated() {
        testCommand("Z13+101000", "S7\"Not a system command, and not activated\"\n", 1);
    }

    @Test
    void shouldGiveUnknownFieldMarker() {
        testCommand("Z1h00+101000", "S3\"Unknown field marker with character code 104\"\n", 1);
    }

    @Test
    void shouldAllowActivation() {
        testCommand("Z3&Z13+101011", "AS&S+101011\n", 3);
    }

    @Test
    void shouldRunSequences() {
        testCommand("Z3\nZ13+101011", "AS\nS+101011\n", 15);
    }

    @Test
    void shouldAbortSequenceOnError() {
        testCommand("Z7 & Z13+101011", "S4\"Command not known\"\n", 3);
    }

    @Test
    void shouldContinueNextSequenceOnError() {
        testCommand("Z7 \nZ1 +101011", "S4\"Command not known\"\nS+101011\n", 15);
    }

    @Test
    void shouldAbortCurrentAndContinueNextSequenceOnError() {
        testCommand("Z7&Z1+aaa\nZ1+101011", "S4\"Command not known\"\nS+101011\n", 15);
    }

    @Test
    void shouldGiveMultipleBigFields() {
        testCommand("Z1+11\"1\"", "S3\"Multiple big fields\"\n", 1);
    }

    @Test
    void shouldGiveBigFieldOddDigits() {
        testCommand("Z1+111", "S3\"Big field odd digits\"\n", 1);
    }

    @Test
    void shouldAllowBroadcast() {
        testCommand("*Z1+11", "*S+11\n", 1);
    }

    @Test
    void shouldFailBroadcastAtOtherPoint() {
        testCommand("Z3&*Z1+11", "AS&S3\"Unknown field marker with character code 42\"\n", 3);
    }

    @Test
    void shouldAllowParallel() {
        testCommand("%Z1+11", "S+11\n", 1);
    }

    @Test
    void shouldFailParallelAtOtherPoint() {
        testCommand("Z3&%Z1+11", "AS&S3\"Unknown field marker with character code 37\"\n", 3);
    }

    @Test
    void shouldAllowParallelBroadcast() {
        testCommand("*%Z1+11", "*S+11\n", 1);
    }

    @Test
    void shouldAllowSecondSequenceAsBroadcast() {
        testCommand("Z3\n*%Z1+11", "AS\n*S+11\n", 15);
    }

    private void testCommand(final String input, final String output, final int num) {
        ZcodeActivateCommand.reset();

        Zcode           z   = new Zcode(new ZcodeParameters(false), new ZcodeBusInterruptSource[0]);
        StringOutStream out = new StringOutStream();
        z.setChannels(new ZcodeCommandChannel[] { new DirectCommandChannel(new ZcodeParameters(false), z, input, out, false, num) });

        z.getCommandFinder()
                .registerCommand(new ZcodeEchoCommand())
                .registerCommand(new ZcodeActivateCommand())
                .registerCommand(new ZcodeTmpCommand());
        z.progressZcode(40);
        assertThat(out.getString()).isEqualTo(output);
    }

}
