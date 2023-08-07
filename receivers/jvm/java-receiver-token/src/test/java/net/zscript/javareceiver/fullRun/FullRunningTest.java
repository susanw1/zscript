package net.zscript.javareceiver.fullRun;

import static org.assertj.core.api.Assertions.assertThat;

import java.io.IOException;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import net.zscript.javareceiver.core.StringWriterOutStream;
import net.zscript.javareceiver.core.Zscript;
import net.zscript.javareceiver.modules.core.ZscriptCoreModule;

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

}
