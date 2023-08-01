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
import net.zscript.javareceiver.commands.ZcodeSetNotificationHostCommand;
import net.zscript.javareceiver.executionspace.ZcodeInterruptVectorChannel;
import net.zscript.javareceiver.executionspace.ZcodeInterruptVectorManager;
import net.zscript.javareceiver.parsing.ZcodeCommandChannel;
import net.zscript.javareceiver.util.DirectCommandChannel;
import net.zscript.javareceiver.util.StringOutStream;
import net.zscript.javareceiver.util.TestInterruptSource;
import net.zscript.javareceiver.util.ZcodeTmp2Command;
import net.zscript.javareceiver.util.ZcodeTmpCommand;

class InterruptVectorTest {

    @BeforeEach
    void reset() {
        ZcodeActivateCommand.reset();
    }

    @Test
    void shouldDoInterruptVectors() {
        ZcodeParameters             params  = new ZcodeParameters(true);
        TestInterruptSource         intSrc  = new TestInterruptSource();
        Zcode                       z       = new Zcode(params, new ZcodeBusInterruptSource[] { intSrc });
        ZcodeInterruptVectorManager intVect = new ZcodeInterruptVectorManager(params, z.getNotificationManager(), z.getSpace());
        StringOutStream             out     = new StringOutStream();
        z.getNotificationManager().setVectorChannel(intVect);
        intVect.getVectorMap().setVector((byte) 0x01, 14);
        intVect.getVectorMap().setVector((byte) 0x01, (byte) 0x03, (byte) 0x04, true, 0);
        intVect.getVectorMap().setVector((byte) 0x01, (byte) 0x03, (byte) 0x05, false, 8);
        intVect.getVectorMap().setVector((byte) 0x04, 22);
        z.setChannels(new ZcodeCommandChannel[] { new DirectCommandChannel(params, z, "Z8&Z3", out, false, 1), new ZcodeInterruptVectorChannel(z.getSpace(), intVect, z, params) });
        z.getSpace().setRunning(true);
        z.getSpace().write("Z1+10a0&Z13A1\nZ1+10a0&Z11A2\nZ33".getBytes(StandardCharsets.US_ASCII), 0, true);
        z.getCommandFinder()
                .registerCommand(new ZcodeEchoCommand())
                .registerCommand(new ZcodeActivateCommand())
                .registerCommand(new ZcodeTmpCommand())
                .registerCommand(new ZcodeTmp2Command())
                .registerCommand(new ZcodeSetNotificationHostCommand(z));
        for (int i = 0; i < 20; i++) {
            z.progressZcode();
        }
        intSrc.setNotification((byte) 0x10, (byte) 1, (byte) 3, (byte) 4);
        z.progressZcode(20);
        intSrc.setNotification((byte) 0x11, (byte) 1, (byte) 3, (byte) 6);
        z.progressZcode(20);
        intSrc.setNotification((byte) 0x12, (byte) 1, (byte) 5, (byte) 4);
        z.progressZcode(20);
        intSrc.setNotification((byte) 0x13, (byte) 4, (byte) 3, (byte) 4);
        z.progressZcode(20);
        intSrc.setNotification((byte) 0x13, (byte) 8, (byte) 3, (byte) 4);
        z.progressZcode(20);
        intSrc.setNotification((byte) 0x13, (byte) 8, (byte) 3, (byte) 0);
        z.progressZcode(20);
        intVect.getVectorMap().setDefaultVector(28);
        intSrc.setNotification((byte) 0x13, (byte) 8, (byte) 3, (byte) 4);
        z.progressZcode(20);
        assertThat(out.getString()).isEqualTo(
                "S&AS\n!Z1A1T1I3S&A4S&S+10a0&SA1\n!Z1A1T1I3S&A6S&SA1\n!Z1A1T1I5S&A4S&S+10a0&S10A2\n!Z1A1T4I3S&A4S&S10A2\n!Z1A1T8I3S&A4S\n!Z1A1T8I3S&AS10\n!Z1A1T8I3S&A4S&S4\"Command not known\"\n");

    }

    @Test
    void shouldSurviveNoNotificationChannel() {
        ZcodeParameters             params  = new ZcodeParameters(true);
        TestInterruptSource         intSrc  = new TestInterruptSource();
        Zcode                       z       = new Zcode(params, new ZcodeBusInterruptSource[] { intSrc });
        ZcodeInterruptVectorManager intVect = new ZcodeInterruptVectorManager(params, z.getNotificationManager(), z.getSpace());
        StringOutStream             out     = new StringOutStream();
        z.getNotificationManager().setVectorChannel(intVect);
        intVect.getVectorMap().setVector((byte) 0x01, 13);
        intVect.getVectorMap().setVector((byte) 0x01, (byte) 0x03, (byte) 0x04, true, 0);
        intVect.getVectorMap().setVector((byte) 0x01, (byte) 0x03, (byte) 0x05, false, 8);
        intVect.getVectorMap().setVector((byte) 0x04, 22);
        z.setChannels(new ZcodeCommandChannel[] { new DirectCommandChannel(params, z, "Z3", out, false, 1), new ZcodeInterruptVectorChannel(z.getSpace(), intVect, z, params) });
        z.getSpace().setRunning(true);
        z.getSpace().write("Z1+10a0;Z13A1\nZ1+10a0;Z11A2\nZ33".getBytes(StandardCharsets.US_ASCII), 0, true);
        z.getCommandFinder()
                .registerCommand(new ZcodeEchoCommand())
                .registerCommand(new ZcodeActivateCommand())
                .registerCommand(new ZcodeTmpCommand())
                .registerCommand(new ZcodeTmp2Command())
                .registerCommand(new ZcodeSetNotificationHostCommand(z));

        z.progressZcode(20);
        intSrc.setNotification((byte) 0x10, (byte) 1, (byte) 3, (byte) 4);
        z.progressZcode(20);
        intSrc.setNotification((byte) 0x11, (byte) 1, (byte) 3, (byte) 6);
        z.progressZcode(20);
        intSrc.setNotification((byte) 0x12, (byte) 1, (byte) 5, (byte) 4);
        z.progressZcode(20);
        intSrc.setNotification((byte) 0x13, (byte) 4, (byte) 3, (byte) 4);
        z.progressZcode(20);
        intSrc.setNotification((byte) 0x13, (byte) 8, (byte) 3, (byte) 4);
        z.progressZcode(20);
        intSrc.setNotification((byte) 0x13, (byte) 8, (byte) 3, (byte) 0);
        z.progressZcode(20);
        intVect.getVectorMap().setDefaultVector(27);
        intSrc.setNotification((byte) 0x13, (byte) 8, (byte) 3, (byte) 4);
        z.progressZcode(20);
        assertThat(out.getString()).isEqualTo("AS\n");
    }
}
