package net.zscript.javaclient.commandbuilderapi.nodes;

import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.Test;

import net.zscript.javaclient.commandbuilderapi.DemoActivateCommand;

public class SequenceNodeTest {
    @Test
    public void shouldCompileBasicCommand() {
        var seq = DemoCapabilities.builder()
                .setVersionType(DemoCapabilitiesCommandBuilder.PLATFORM_FIRMWARE)
                .addBigField("abc")
                .build();
        assertThat(seq.asStringUtf8()).isEqualTo("ZV2\"abc\"");
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
        assertThat(seq.asStringUtf8()).isEqualTo("ZV2+0506077f&Z2A1234");
    }

}
