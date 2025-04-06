package net.zscript.javaclient.commandbuilderapi.nodes;

import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatThrownBy;

import org.junit.jupiter.api.Test;

import net.zscript.javaclient.commandbuilderapi.DemoActivateCommand;
import net.zscript.javaclient.commandbuilderapi.ZscriptMissingFieldException;

public class CommandNodeTest {
    @Test
    public void shouldCompileBasicCommand() {
        var seq = DemoCapabilities.builder()
                .setVersionType(DemoCapabilitiesCommandBuilder.PLATFORM_FIRMWARE)
                .setFieldAsText('X', "abc")
                .build();
        assertThat(seq.asStringUtf8()).isEqualTo("ZV2X\"abc\"");
    }

    @Test
    public void shouldCompileAndedCommandSequence() {
        var seq = DemoCapabilities.builder()
                .setVersionType(DemoCapabilitiesCommandBuilder.PLATFORM_FIRMWARE)
                .setFieldAsBytes('X', new byte[] { 5, 6, 7, 0x7f })
                .build()
                .andThen(DemoActivateCommand.builder()
                        .setField('A', 0x1234)
                        .build());
        assertThat(seq.asStringUtf8()).isEqualTo("ZV2X0506077f&Z2A1234");
    }

    @Test
    public void shouldValidateAndRejectBuilderWithMissingNumericKeys() {
        final DemoCapabilitiesCommandBuilder b = DemoCapabilities.builder();
        assertThatThrownBy(b::failIfInvalid)
                .isInstanceOf(ZscriptMissingFieldException.class).hasMessageContaining("missingKeys");
        b.setField('V', 123);
        b.failIfInvalid(); // should return quietly now
    }

    @Test
    public void shouldValidateAndRejectBuilderWithMissingStringFieldKeys() {
        final DemoCapabilitiesCommandBuilder b = DemoCapabilities.builder();
        b.setRequiredFields(new byte[] { 'I', 'V' });
        b.setField('V', 123);
        assertThatThrownBy(b::failIfInvalid)
                .isInstanceOf(ZscriptMissingFieldException.class).hasMessageContaining("missingKeys");
        b.setFieldAsText('I', "");
        b.failIfInvalid(); // should return quietly now
    }

    @Test
    public void shouldRejectInvalidNumericKeys() {
        final DemoCapabilitiesCommandBuilder b = DemoCapabilities.builder();
        assertThatThrownBy(() -> b.setField('%', 123))
                .isInstanceOf(IllegalArgumentException.class).hasMessageContaining("Key not a valid Zscript Command key");
        assertThatThrownBy(() -> b.getFieldOrZero('%'))
                .isInstanceOf(IllegalArgumentException.class).hasMessageContaining("Key not a valid Zscript Command key");
    }

    @Test
    public void shouldRecordCaptor() {
        ResponseCaptor<DemoCapabilitiesCommandResponse> c = ResponseCaptor.create();
        final DemoCapabilitiesCommandBuilder            b = DemoCapabilities.builder();
        b.setField('V', 123);
        b.capture(c);
        final ZscriptCommandNode<DemoCapabilitiesCommandResponse> node = b.build();

        assertThat(node.getCaptor()).isNotNull().isSameAs(c);
        assertThat(node.getCaptor().getSource()).isNotNull().isSameAs(node);
    }
}
