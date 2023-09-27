package net.zscript.javaclient.commandbuilder;

import java.nio.charset.StandardCharsets;

import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.Test;

public class SequenceNodeTest {
    @Test
    public void shouldCompileBasicCommand() {
        var seq = DemoCapabilities.builder()
                .setVersionType(DemoCapabilitiesCommandBuilder.PLATFORM_FIRMWARE)
                .addBigField("abc")
                .build();
        assertThat(build(seq)).isEqualTo("ZV2\"abc\"");
    }

    @Test
    public void shouldCompileAndedCommandSequence() {
        var seq = DemoCapabilities.builder()
                .setVersionType(DemoCapabilitiesCommandBuilder.PLATFORM_FIRMWARE)
                .addBigField(new byte[] { 5, 6, 7, 0x7f })
                .build()
                .andThen(DemoActivateCommand.builder()
                        .setField('A', 0x1234)
                        .build());
        assertThat(build(seq)).isEqualTo("ZV2+0506077f&Z2A1234");
    }

    private String build(CommandSequenceNode node) {
        // ISO8859, because we want an 8-bit byte in each char, and they *could* be non-printing / non-ascii if they're in Text
        return new String(node.compile(), StandardCharsets.ISO_8859_1);
    }

}
