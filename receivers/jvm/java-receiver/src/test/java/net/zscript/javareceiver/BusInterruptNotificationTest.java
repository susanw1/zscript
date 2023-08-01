package net.zscript.javareceiver;

import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import net.zscript.javareceiver.Zcode;
import net.zscript.javareceiver.ZcodeBusInterruptSource;
import net.zscript.javareceiver.ZcodeParameters;
import net.zscript.javareceiver.commands.ZcodeActivateCommand;
import net.zscript.javareceiver.commands.ZcodeEchoCommand;
import net.zscript.javareceiver.commands.ZcodeSetNotificationHostCommand;
import net.zscript.javareceiver.parsing.ZcodeCommandChannel;
import net.zscript.javareceiver.util.DirectCommandChannel;
import net.zscript.javareceiver.util.StringOutStream;
import net.zscript.javareceiver.util.TestInterruptSource;
import net.zscript.javareceiver.util.ZcodeTmpCommand;

class BusInterruptNotificationTest {
    private ZcodeParameters params;

    @BeforeEach
    void setup() {
        params = new ZcodeParameters(false);
    }

    @Test
    void shouldReportNotifications() {
        TestInterruptSource intSrc = new TestInterruptSource();
        Zcode               z    = new Zcode(params, new ZcodeBusInterruptSource[] { intSrc });
        StringOutStream     out  = new StringOutStream();
        z.setChannels(new ZcodeCommandChannel[] { new DirectCommandChannel(params, z, "Z8", out, false, 1) });
        z.getCommandFinder().registerCommand(new ZcodeEchoCommand())
                .registerCommand(new ZcodeActivateCommand())
                .registerCommand(new ZcodeTmpCommand())
                .registerCommand(new ZcodeSetNotificationHostCommand(z));
        z.progressZcode(6);
        intSrc.setNotification((byte) 0x10, (byte) 0x03, (byte) 0x05, (byte) 0x02);
        z.progressZcode(5);
        assertThat(out.getString()).isEqualTo("S\n!Z1A1T3I5S&A2S\n");
    }

    @Test
    void shouldReportNotificationsWithoutAddresses() {
        TestInterruptSource intSrc = new TestInterruptSource();
        Zcode               z    = new Zcode(params, new ZcodeBusInterruptSource[] { intSrc });
        StringOutStream     out  = new StringOutStream();
        z.setChannels(new ZcodeCommandChannel[] { new DirectCommandChannel(params, z, "Z8", out, false, 1) });
        z.getCommandFinder().registerCommand(new ZcodeEchoCommand())
                .registerCommand(new ZcodeActivateCommand())
                .registerCommand(new ZcodeTmpCommand())
                .registerCommand(new ZcodeSetNotificationHostCommand(z));
        z.progressZcode(6);
        intSrc.setNotification((byte) 0x10, (byte) 0x11, (byte) 0x41, (byte) 0x00);
        z.progressZcode(5);
        assertThat(out.getString()).isEqualTo("S\n!Z1A1T11I41S&AS10\n");
    }

    @Test
    void shouldReportNotificationsAndRunCommands() {
        TestInterruptSource intSrc = new TestInterruptSource();
        Zcode               z      = new Zcode(params, new ZcodeBusInterruptSource[] { intSrc });
        StringOutStream     out    = new StringOutStream();
        z.setChannels(new ZcodeCommandChannel[] { new DirectCommandChannel(params, z, "Z8\nZ1A11+0100", out, false, 100) });
        z.getCommandFinder()
                .registerCommand(new ZcodeEchoCommand())
                .registerCommand(new ZcodeActivateCommand())
                .registerCommand(new ZcodeTmpCommand())
                .registerCommand(new ZcodeSetNotificationHostCommand(z));
        z.progressZcode(6);
        intSrc.setNotification((byte) 0x10, (byte) 0x11, (byte) 0x41, (byte) 0x00);
        z.progressZcode(5);
        assertThat(out.getString()).isEqualTo("S\nSA11+0100\n!Z1A1T11I41S&AS10\n");
    }

    @Test
    void shouldReportNotificationsAndRunCommandsWithPriority() {
        TestInterruptSource intSrc = new TestInterruptSource();
        Zcode               z      = new Zcode(params, new ZcodeBusInterruptSource[] { intSrc });
        StringOutStream     out    = new StringOutStream();
        z.setChannels(new ZcodeCommandChannel[] { new DirectCommandChannel(params, z, "Z8\nZ1A11+0100", out, false, 100) });
        z.getCommandFinder().registerCommand(new ZcodeEchoCommand())
                .registerCommand(new ZcodeActivateCommand())
                .registerCommand(new ZcodeTmpCommand())
                .registerCommand(new ZcodeSetNotificationHostCommand(z));
        z.progressZcode();
        intSrc.setNotification((byte) 0x10, (byte) 0x11, (byte) 0x41, (byte) 0x00);
        z.progressZcode(10);
        assertThat(out.getString()).isEqualTo("S\n!Z1A1T11I41S&AS10\nSA11+0100\n");
    }

    @Test
    void shouldReportMultipleNotifications() {
        TestInterruptSource intSrc = new TestInterruptSource();
        Zcode               z      = new Zcode(params, new ZcodeBusInterruptSource[] { intSrc });
        StringOutStream     out    = new StringOutStream();
        z.setChannels(new ZcodeCommandChannel[] { new DirectCommandChannel(params, z, "Z8\n", out, false, 100) });
        z.getCommandFinder().registerCommand(new ZcodeEchoCommand())
                .registerCommand(new ZcodeActivateCommand())
                .registerCommand(new ZcodeTmpCommand())
                .registerCommand(new ZcodeSetNotificationHostCommand(z));
        z.progressZcode();
        intSrc.setNotification((byte) 0x10, (byte) 0x11, (byte) 0x41, (byte) 0x00);
        z.progressZcode(6);
        intSrc.setNotification((byte) 0x10, (byte) 0x31, (byte) 0x00, (byte) 0x50);
        z.progressZcode(4);
        assertThat(out.getString()).isEqualTo("S\n!Z1A1T11I41S&AS10\n!Z1A1T31IS&A50S\n");
    }

    @Test
    void shouldSurviveNoNotificationChannel() {
        TestInterruptSource intSrc = new TestInterruptSource();
        Zcode               z      = new Zcode(params, new ZcodeBusInterruptSource[] { intSrc });
        StringOutStream     out    = new StringOutStream();
        z.setChannels(new ZcodeCommandChannel[] { new DirectCommandChannel(params, z, "", out, false, 0) });
        z.getCommandFinder().registerCommand(new ZcodeEchoCommand())
                .registerCommand(new ZcodeActivateCommand())
                .registerCommand(new ZcodeTmpCommand())
                .registerCommand(new ZcodeSetNotificationHostCommand(z));
        z.progressZcode();
        intSrc.setNotification((byte) 0x10, (byte) 0x11, (byte) 0x41, (byte) 0x00);
        z.progressZcode(6);
        intSrc.setNotification((byte) 0x10, (byte) 0x31, (byte) 0x00, (byte) 0x50);
        z.progressZcode(4);
        assertThat(out.getString()).isEqualTo("");
    }

}
