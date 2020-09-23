package com.wittsfamily.rcode.javareceiver;

import org.assertj.core.api.Assertions;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import com.wittsfamily.rcode.javareceiver.commands.RCodeActivateCommand;
import com.wittsfamily.rcode.javareceiver.commands.RCodeEchoCommand;
import com.wittsfamily.rcode.javareceiver.commands.RCodeSetNotificationHostCommand;
import com.wittsfamily.rcode.javareceiver.commands.RCodeTmpCommand;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandChannel;
import com.wittsfamily.rcode.javareceiver.test.DirectCommandChannel;
import com.wittsfamily.rcode.javareceiver.test.StringOutStream;
import com.wittsfamily.rcode.javareceiver.test.TestInterruptSource;

class BusInterruptNotificationTest {
    private RCodeParameters params;

    @BeforeEach
    void setup() {
        params = new RCodeParameters(false);
    }

    @Test
    void shouldReportNotifications() {
        TestInterruptSource intS = new TestInterruptSource();
        RCode r = new RCode(params, new RCodeBusInterruptSource[] { intS });
        StringOutStream out = new StringOutStream();
        r.setChannels(new RCodeCommandChannel[] { new DirectCommandChannel(params, r, "R8", out, false, 1) });
        r.getCommandFinder().registerCommand(new RCodeEchoCommand());
        r.getCommandFinder().registerCommand(new RCodeActivateCommand());
        r.getCommandFinder().registerCommand(new RCodeTmpCommand());
        r.getCommandFinder().registerCommand(new RCodeSetNotificationHostCommand(r));
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        intS.setNotification((byte) 0x10, (byte) 0x03, (byte) 0x05, (byte) 0x02);
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        Assertions.assertThat(out.getString()).isEqualTo("S\n!Z1A1T3I5S&A2S\n");
    }

    @Test
    void shouldReportNotificationsWithoutAddresses() {
        TestInterruptSource intS = new TestInterruptSource();
        RCode r = new RCode(params, new RCodeBusInterruptSource[] { intS });
        StringOutStream out = new StringOutStream();
        r.setChannels(new RCodeCommandChannel[] { new DirectCommandChannel(params, r, "R8", out, false, 1) });
        r.getCommandFinder().registerCommand(new RCodeEchoCommand());
        r.getCommandFinder().registerCommand(new RCodeActivateCommand());
        r.getCommandFinder().registerCommand(new RCodeTmpCommand());
        r.getCommandFinder().registerCommand(new RCodeSetNotificationHostCommand(r));
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        intS.setNotification((byte) 0x10, (byte) 0x11, (byte) 0x41, (byte) 0x00);
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        Assertions.assertThat(out.getString()).isEqualTo("S\n!Z1A1T11I41S&AS10\n");
    }

    @Test
    void shouldReportNotificationsAndRunCommands() {
        TestInterruptSource intS = new TestInterruptSource();
        RCode r = new RCode(params, new RCodeBusInterruptSource[] { intS });
        StringOutStream out = new StringOutStream();
        r.setChannels(new RCodeCommandChannel[] { new DirectCommandChannel(params, r, "R8\nR1A11+0100", out, false, 100) });
        r.getCommandFinder().registerCommand(new RCodeEchoCommand());
        r.getCommandFinder().registerCommand(new RCodeActivateCommand());
        r.getCommandFinder().registerCommand(new RCodeTmpCommand());
        r.getCommandFinder().registerCommand(new RCodeSetNotificationHostCommand(r));
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        intS.setNotification((byte) 0x10, (byte) 0x11, (byte) 0x41, (byte) 0x00);
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        Assertions.assertThat(out.getString()).isEqualTo("S\nSA11+0100\n!Z1A1T11I41S&AS10\n");
    }

    @Test
    void shouldReportNotificationsAndRunCommandsWithPriority() {
        TestInterruptSource intS = new TestInterruptSource();
        RCode r = new RCode(params, new RCodeBusInterruptSource[] { intS });
        StringOutStream out = new StringOutStream();
        r.setChannels(new RCodeCommandChannel[] { new DirectCommandChannel(params, r, "R8\nR1A11+0100", out, false, 100) });
        r.getCommandFinder().registerCommand(new RCodeEchoCommand());
        r.getCommandFinder().registerCommand(new RCodeActivateCommand());
        r.getCommandFinder().registerCommand(new RCodeTmpCommand());
        r.getCommandFinder().registerCommand(new RCodeSetNotificationHostCommand(r));
        r.progressRCode();
        intS.setNotification((byte) 0x10, (byte) 0x11, (byte) 0x41, (byte) 0x00);
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        Assertions.assertThat(out.getString()).isEqualTo("S\n!Z1A1T11I41S&AS10\nSA11+0100\n");
    }

    @Test
    void shouldReportMultipleNotifications() {
        TestInterruptSource intS = new TestInterruptSource();
        RCode r = new RCode(params, new RCodeBusInterruptSource[] { intS });
        StringOutStream out = new StringOutStream();
        r.setChannels(new RCodeCommandChannel[] { new DirectCommandChannel(params, r, "R8\n", out, false, 100) });
        r.getCommandFinder().registerCommand(new RCodeEchoCommand());
        r.getCommandFinder().registerCommand(new RCodeActivateCommand());
        r.getCommandFinder().registerCommand(new RCodeTmpCommand());
        r.getCommandFinder().registerCommand(new RCodeSetNotificationHostCommand(r));
        r.progressRCode();
        intS.setNotification((byte) 0x10, (byte) 0x11, (byte) 0x41, (byte) 0x00);
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        intS.setNotification((byte) 0x10, (byte) 0x31, (byte) 0x00, (byte) 0x50);
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        Assertions.assertThat(out.getString()).isEqualTo("S\n!Z1A1T11I41S&AS10\n!Z1A1T31IS&A50S\n");
    }

    @Test
    void shouldSurviveNoNotificationChannel() {
        TestInterruptSource intS = new TestInterruptSource();
        RCode r = new RCode(params, new RCodeBusInterruptSource[] { intS });
        StringOutStream out = new StringOutStream();
        r.setChannels(new RCodeCommandChannel[] { new DirectCommandChannel(params, r, "", out, false, 0) });
        r.getCommandFinder().registerCommand(new RCodeEchoCommand());
        r.getCommandFinder().registerCommand(new RCodeActivateCommand());
        r.getCommandFinder().registerCommand(new RCodeTmpCommand());
        r.getCommandFinder().registerCommand(new RCodeSetNotificationHostCommand(r));
        r.progressRCode();
        intS.setNotification((byte) 0x10, (byte) 0x11, (byte) 0x41, (byte) 0x00);
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        intS.setNotification((byte) 0x10, (byte) 0x31, (byte) 0x00, (byte) 0x50);
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        r.progressRCode();
        Assertions.assertThat(out.getString()).isEqualTo("");
    }

}
