package com.wittsfamily.rcode.javareceiver;

import static org.assertj.core.api.Assertions.assertThat;

import java.nio.charset.StandardCharsets;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import com.wittsfamily.rcode.javareceiver.RCode;
import com.wittsfamily.rcode.javareceiver.RCodeBusInterruptSource;
import com.wittsfamily.rcode.javareceiver.RCodeParameters;
import com.wittsfamily.rcode.javareceiver.commands.RCodeActivateCommand;
import com.wittsfamily.rcode.javareceiver.commands.RCodeEchoCommand;
import com.wittsfamily.rcode.javareceiver.commands.RCodeSetNotificationHostCommand;
import com.wittsfamily.rcode.javareceiver.commands.RCodeTmp2Command;
import com.wittsfamily.rcode.javareceiver.commands.RCodeTmpCommand;
import com.wittsfamily.rcode.javareceiver.executionspace.RCodeInterruptVectorChannel;
import com.wittsfamily.rcode.javareceiver.executionspace.RCodeInterruptVectorManager;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandChannel;
import com.wittsfamily.rcode.javareceiver.test.DirectCommandChannel;
import com.wittsfamily.rcode.javareceiver.test.StringOutStream;
import com.wittsfamily.rcode.javareceiver.test.TestInterruptSource;

class InterruptVectorTest {

    @BeforeEach
    void reset() {
        RCodeActivateCommand.reset();
    }

    @Test
    void shouldDoInterruptVectors() {
        RCodeParameters params = new RCodeParameters(true);
        TestInterruptSource intS = new TestInterruptSource();
        RCode r = new RCode(params, new RCodeBusInterruptSource[] { intS });
        RCodeInterruptVectorManager intVect = new RCodeInterruptVectorManager(params, r.getNotificationManager(), r.getSpace());
        StringOutStream out = new StringOutStream();
        r.getNotificationManager().setVectorChannel(intVect);
        intVect.getVectorMap().setVector((byte) 0x01, 13);
        intVect.getVectorMap().setVector((byte) 0x01, (byte) 0x03, (byte) 0x04, true, 0);
        intVect.getVectorMap().setVector((byte) 0x01, (byte) 0x03, (byte) 0x05, false, 8);
        intVect.getVectorMap().setVector((byte) 0x04, 22);
        r.setChannels(new RCodeCommandChannel[] { new DirectCommandChannel(params, r, "R8;R6", out, false, 1), new RCodeInterruptVectorChannel(r.getSpace(), intVect, r, params) });
        r.getSpace().setRunning(true);
        r.getSpace().write("R1+10a0;R10A1\nR1+10a0;R11A2\nR33".getBytes(StandardCharsets.US_ASCII), 0, true);
        r.getCommandFinder().registerCommand(new RCodeEchoCommand());
        r.getCommandFinder().registerCommand(new RCodeActivateCommand());
        r.getCommandFinder().registerCommand(new RCodeTmpCommand());
        r.getCommandFinder().registerCommand(new RCodeTmp2Command());
        r.getCommandFinder().registerCommand(new RCodeSetNotificationHostCommand(r));
        for (int i = 0; i < 20; i++) {
            r.progressRCode();
        }
        intS.setNotification((byte) 0x10, (byte) 1, (byte) 3, (byte) 4);
        for (int i = 0; i < 20; i++) {
            r.progressRCode();
        }
        intS.setNotification((byte) 0x11, (byte) 1, (byte) 3, (byte) 6);
        for (int i = 0; i < 20; i++) {
            r.progressRCode();
        }
        intS.setNotification((byte) 0x12, (byte) 1, (byte) 5, (byte) 4);
        for (int i = 0; i < 20; i++) {
            r.progressRCode();
        }
        intS.setNotification((byte) 0x13, (byte) 4, (byte) 3, (byte) 4);
        for (int i = 0; i < 20; i++) {
            r.progressRCode();
        }
        intS.setNotification((byte) 0x13, (byte) 8, (byte) 3, (byte) 4);
        for (int i = 0; i < 20; i++) {
            r.progressRCode();
        }
        intS.setNotification((byte) 0x13, (byte) 8, (byte) 3, (byte) 0);
        for (int i = 0; i < 20; i++) {
            r.progressRCode();
        }
        intVect.getVectorMap().setDefaultVector(27);
        intS.setNotification((byte) 0x13, (byte) 8, (byte) 3, (byte) 4);
        for (int i = 0; i < 20; i++) {
            r.progressRCode();
        }
        assertThat(out.getString()).isEqualTo(
                "S;AS\n!Z1A1T1I3S;A4S;S+10a0;SA1\n!Z1A1T1I3S;A6S;SA1\n!Z1A1T1I5S;A4S;S+10a0;S10A2\n!Z1A1T4I3S;A4S;S10A2\n!Z1A1T8I3S;A4S\n!Z1A1T8I3S;AS10\n!Z1A1T8I3S;A4S;S4\"Command not known\"\n");

    }

    @Test
    void shouldSurviveNoNotificationChannel() {
        RCodeParameters params = new RCodeParameters(true);
        TestInterruptSource intS = new TestInterruptSource();
        RCode r = new RCode(params, new RCodeBusInterruptSource[] { intS });
        RCodeInterruptVectorManager intVect = new RCodeInterruptVectorManager(params, r.getNotificationManager(), r.getSpace());
        StringOutStream out = new StringOutStream();
        r.getNotificationManager().setVectorChannel(intVect);
        intVect.getVectorMap().setVector((byte) 0x01, 13);
        intVect.getVectorMap().setVector((byte) 0x01, (byte) 0x03, (byte) 0x04, true, 0);
        intVect.getVectorMap().setVector((byte) 0x01, (byte) 0x03, (byte) 0x05, false, 8);
        intVect.getVectorMap().setVector((byte) 0x04, 22);
        r.setChannels(new RCodeCommandChannel[] { new DirectCommandChannel(params, r, "R6", out, false, 1), new RCodeInterruptVectorChannel(r.getSpace(), intVect, r, params) });
        r.getSpace().setRunning(true);
        r.getSpace().write("R1+10a0;R10A1\nR1+10a0;R11A2\nR33".getBytes(StandardCharsets.US_ASCII), 0, true);
        r.getCommandFinder().registerCommand(new RCodeEchoCommand());
        r.getCommandFinder().registerCommand(new RCodeActivateCommand());
        r.getCommandFinder().registerCommand(new RCodeTmpCommand());
        r.getCommandFinder().registerCommand(new RCodeTmp2Command());
        r.getCommandFinder().registerCommand(new RCodeSetNotificationHostCommand(r));
        for (int i = 0; i < 20; i++) {
            r.progressRCode();
        }
        intS.setNotification((byte) 0x10, (byte) 1, (byte) 3, (byte) 4);
        for (int i = 0; i < 20; i++) {
            r.progressRCode();
        }
        intS.setNotification((byte) 0x11, (byte) 1, (byte) 3, (byte) 6);
        for (int i = 0; i < 20; i++) {
            r.progressRCode();
        }
        intS.setNotification((byte) 0x12, (byte) 1, (byte) 5, (byte) 4);
        for (int i = 0; i < 20; i++) {
            r.progressRCode();
        }
        intS.setNotification((byte) 0x13, (byte) 4, (byte) 3, (byte) 4);
        for (int i = 0; i < 20; i++) {
            r.progressRCode();
        }
        intS.setNotification((byte) 0x13, (byte) 8, (byte) 3, (byte) 4);
        for (int i = 0; i < 20; i++) {
            r.progressRCode();
        }
        intS.setNotification((byte) 0x13, (byte) 8, (byte) 3, (byte) 0);
        for (int i = 0; i < 20; i++) {
            r.progressRCode();
        }
        intVect.getVectorMap().setDefaultVector(27);
        intS.setNotification((byte) 0x13, (byte) 8, (byte) 3, (byte) 4);
        for (int i = 0; i < 20; i++) {
            r.progressRCode();
        }
        assertThat(out.getString()).isEqualTo("AS\n");

    }

}
