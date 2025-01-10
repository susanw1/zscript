package net.zscript.javareceiver.fullRun;

import java.io.IOException;

import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import net.zscript.javareceiver.core.Zscript;
import net.zscript.javareceiver.modules.core.ZscriptCoreModule;
import net.zscript.javareceiver.testing.StringChannel;
import net.zscript.javareceiver.testing.StringWriterOutStream;

@SuppressWarnings("StatementWithEmptyBody")
public class FullRunningTest {
    private final Zscript zscript = new Zscript();

    private StringWriterOutStream outStream;

    @BeforeEach
    void setup() throws IOException {
        zscript.addModule(new ZscriptCoreModule());
        outStream = new StringWriterOutStream();
    }

    @Test
    public void shouldRunBasicCommand() {
        StringChannel channel = StringChannel.from("Z1AB\n", outStream);
        zscript.addChannel(channel);

        while (zscript.progress()) {
        }

        assertThat(outStream.getStringAndReset()).isEqualTo("!ABS\n");
    }

    @Test
    public void shouldRunSeveralBasicCommands() {
        StringChannel channel = StringChannel.from("Z1AB\nZ1CD\n", outStream);
        zscript.addChannel(channel);

        while (zscript.progress()) {
        }

        assertThat(outStream.getStringAndReset()).isEqualTo("!ABS\n!CDS\n");
    }

    @Test
    public void shouldHandleMultipleChannels() throws IOException {
        StringWriterOutStream outStream1 = new StringWriterOutStream();
        StringChannel         channel1   = StringChannel.from("Z1AB\nZ1CD\n", outStream1);
        zscript.addChannel(channel1);

        StringWriterOutStream outStream2 = new StringWriterOutStream();
        StringChannel         channel2   = StringChannel.from("Z1EF\n", outStream2);
        zscript.addChannel(channel2);

        while (zscript.progress()) {
        }

        assertThat(outStream1.getStringAndReset()).isEqualTo("!ABS\n!CDS\n");
        assertThat(outStream2.getStringAndReset()).isEqualTo("!EFS\n");
    }
}
