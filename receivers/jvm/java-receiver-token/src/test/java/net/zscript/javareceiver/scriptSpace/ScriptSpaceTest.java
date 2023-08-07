package net.zscript.javareceiver.scriptSpace;

import static org.assertj.core.api.Assertions.assertThat;

import java.io.IOException;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import net.zscript.javareceiver.core.StringWriterOutStream;
import net.zscript.javareceiver.core.Zscript;
import net.zscript.javareceiver.fullRun.StringChannel;
import net.zscript.javareceiver.modules.core.ZscriptCoreModule;
import net.zscript.javareceiver.modules.scriptSpaces.ScriptSpaceModule;
import net.zscript.javareceiver.scriptSpaces.ScriptSpace;

public class ScriptSpaceTest {
    private final ScriptSpaceModule module = new ScriptSpaceModule();

    private final Zscript zscript = new Zscript();

    private StringWriterOutStream outStream;

    @BeforeEach
    void setup() throws IOException {
        zscript.addModule(module);
        zscript.addModule(new ZscriptCoreModule());
        outStream = new StringWriterOutStream();
        zscript.setNotificationOutStream(outStream);
    }

    @Test
    public void shouldRunScriptSpaceWithFailuresAndErrors() {
        ScriptSpace space = ScriptSpace.from(zscript, "Z1&Z0\nZ1ABC&Z1S1\nZ1S1|Z0\nZ1S10\n");
        zscript.addActionSource(space);
        module.addScriptSpace(space);
        while (zscript.progress()) {
        }
        assertThat(outStream.getStringAndReset()).isEqualTo("!ABCS&S1\n!S10\n");
    }

    @Test
    public void scriptSpaceShouldJamZscript() {
        StringChannel channel = StringChannel.from("Z1AB\n", outStream);
        ScriptSpace   space   = ScriptSpace.from(zscript, "Z1&Z0\n");
        zscript.addActionSource(space);
        zscript.addChannel(channel);
        module.addScriptSpace(space);
        int i = 0;
        while (zscript.progress() && i++ < 1000000) {
        }
        assertThat(outStream.getStringAndReset()).isEqualTo("");
    }

    @Test
    public void scriptSpaceDelayShouldUnjamZscript() {
        StringChannel channel = StringChannel.from("Z1AB\n", outStream);
        ScriptSpace   space   = ScriptSpace.from(zscript, "Z2&Z0\n%Z28M10\n");
        zscript.addActionSource(space);
        zscript.addChannel(channel);
        module.addScriptSpace(space);
        int i = 0;
        while (zscript.progress() && i++ < 1000000) {
        }
        assertThat(outStream.getStringAndReset()).isEqualTo("!ABS\n");
    }

    @Test
    public void scriptSpaceDelayWithLockingShouldJamZscript() {
        StringChannel channel = StringChannel.from("Z1AB\n", outStream);
        ScriptSpace   space   = ScriptSpace.from(zscript, "Z2&Z0\nZ28M10\n");
        zscript.addActionSource(space);
        zscript.addChannel(channel);
        module.addScriptSpace(space);
        int i = 0;
        while (zscript.progress() && i++ < 1000000) {
        }
        assertThat(outStream.getStringAndReset()).isEqualTo("");
    }

    @Test
    public void shouldReadScriptSpaceCapabilities() {
        StringChannel channel = StringChannel.from("Z2&Z20\n", outStream);
        ScriptSpace   space   = ScriptSpace.from(zscript, "Z2&Z0\n");
        space.stop();
        zscript.addActionSource(space);
        zscript.addChannel(channel);
        module.addScriptSpace(space);
        int i = 0;
        while (zscript.progress() && i++ < 1000000) {
        }
        assertThat(outStream.getStringAndReset()).isEqualTo("!AS&C107P1S\n");
    }

    @Test
    public void shouldReadScriptSpaceSetup() {
        StringChannel channel = StringChannel.from("Z2&Z21P0\n", outStream);
        ScriptSpace   space   = ScriptSpace.from(zscript, "Z2&Z0\n");
        space.stop();
        zscript.addActionSource(space);
        zscript.addChannel(channel);
        module.addScriptSpace(space);
        int i = 0;
        while (zscript.progress() && i++ < 1000000) {
        }
        assertThat(outStream.getStringAndReset()).isEqualTo("!AS&P7LffffS\n");
    }

    @Test
    public void shouldRunScriptSpaceFromSetup() {
        StringChannel channel = StringChannel.from("Z2&Z21P0R1\nZ\n", outStream);
        ScriptSpace   space   = ScriptSpace.from(zscript, "Z2&Z0\n");
        space.stop();
        zscript.addActionSource(space);
        zscript.addChannel(channel);
        module.addScriptSpace(space);
        int i = 0;
        while (zscript.progress() && i++ < 1000000) {
        }
        assertThat(outStream.getStringAndReset()).isEqualTo("!AS&P7LffffS\n");
    }

    @Test
    public void shouldRunAndStopScriptSpaceFromSetup() {
        StringChannel channel = StringChannel.from("Z2&Z21P0R1&Z21P&Z21PR&Z21P\nZ1\n", outStream);
        ScriptSpace   space   = ScriptSpace.from(zscript, "Z2&Z1S1\n");
        space.stop();
        zscript.addActionSource(space);
        zscript.addChannel(channel);
        module.addScriptSpace(space);
        int i = 0;
        while (zscript.progress() && i++ < 1000000) {
        }
        assertThat(outStream.getStringAndReset()).isEqualTo("!AS&PbLffffS&PbRLffffS&PbRLffffS&PbLffffS\n!S\n");
    }

    @Test
    public void shouldCreateBlankSpace() {
        StringChannel channel = StringChannel.from("Z2&Z21P\n", outStream);
        ScriptSpace   space   = ScriptSpace.blank(zscript);
        zscript.addActionSource(space);
        zscript.addChannel(channel);
        module.addScriptSpace(space);
        int i = 0;
        while (zscript.progress() && i++ < 1000000) {
        }
        assertThat(outStream.getStringAndReset()).isEqualTo("!AS&PWLffffS\n");
    }

    @Test
    public void shouldWriteToBlankSpace() {
        StringChannel channel = StringChannel.from("Z2&Z21P&Z22P\"Z1S10=0a\"&Z21PR1\n", outStream);
        ScriptSpace   space   = ScriptSpace.blank(zscript);
        zscript.addActionSource(space);
        zscript.addChannel(channel);
        module.addScriptSpace(space);
        int i = 0;
        while (zscript.progress() && i++ < 1000000) {
        }
        assertThat(outStream.getStringAndReset()).isEqualTo("!AS&PWLffffS&LffffS&P7WLffffS\n!S10\n");
    }
}
