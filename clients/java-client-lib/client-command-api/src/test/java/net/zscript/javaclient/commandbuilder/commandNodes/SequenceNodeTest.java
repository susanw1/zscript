package net.zscript.javaclient.commandbuilder.commandNodes;

import java.nio.charset.StandardCharsets;

import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.Test;

import net.zscript.javaclient.commandbuilder.DemoActivateCommand;
import net.zscript.javaclient.commandbuilder.commandnodes.CommandSequenceNode;

public class SequenceNodeTest {
    @Test
    public void shouldCompileBasicCommand() {
        var seq = DemoCapabilities.builder()
                .setVersionType(DemoCapabilitiesCommandBuilder.PLATFORM_FIRMWARE)
                .addBigField("abc")
                .build();
        assertThat(seq.asString()).isEqualTo("V2Z\"abc\"");
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
        assertThat(seq.asString()).isEqualTo("V2Z+0506077f&A1234Z2");
    }

}
