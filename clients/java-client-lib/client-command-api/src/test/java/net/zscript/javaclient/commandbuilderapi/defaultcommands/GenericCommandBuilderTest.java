package net.zscript.javaclient.commandbuilderapi.defaultcommands;

import static net.zscript.javaclient.commandpaths.FieldElement.fieldOf;
import static net.zscript.util.ByteString.byteStringUtf8;
import static org.assertj.core.api.AssertionsForClassTypes.assertThat;

import org.junit.jupiter.api.Test;

import net.zscript.javaclient.commandbuilderapi.nodes.ZscriptCommandNode;

class GenericCommandBuilderTest {
    final GenericCommandBuilder testBuilder = new GenericCommandBuilder().setField('X', 123).setFieldAsText('W', byteStringUtf8("hello"));

    @Test
    public void shouldCreateGenericCommandBuilder() {
        assertThat(testBuilder.getFieldOrZero('X')).isEqualTo(fieldOf((byte) 'X', 123));
        assertThat(testBuilder.getFieldOrZero('Y')).isEqualTo(fieldOf((byte) 'Y', 0));

        assertThat(testBuilder.build().asFieldSet().asStringUtf8()).isEqualTo("W\"hello\"X7b");
    }

    @Test
    public void shouldCreateDefaultResponse() {
        final GenericCommandBuilder builder = new GenericCommandBuilder();
        // cheat, using the testBuilder as though it's a response!
        final ZscriptCommandNode<DefaultResponse> node = builder.build();
        DefaultResponse                           resp = node.parseResponse(testBuilder.build().asFieldSet());
        assertThat(resp.getField((byte) 'X')).hasValue(123);
        assertThat(resp.expression().getField((byte) 'X')).hasValue(123);
        assertThat(resp.isValid()).isTrue();
    }

    @Test
    public void shouldAddTextAsBigFieldSmallest() {
        final GenericCommandBuilder builder = new GenericCommandBuilder();
        builder.addBigFieldAsSmallest(byteStringUtf8("hello").toByteArray());
        assertThat(builder.build().asFieldSet().asStringUtf8()).isEqualTo("\"hello\"");
    }

    @Test
    public void shouldAddBytes() {
        final GenericCommandBuilder builder = new GenericCommandBuilder();
        builder.addBigField(byteStringUtf8("hello").toByteArray());
        builder.addBigField(byteStringUtf8("hello").toByteArray(), true);
        assertThat(builder.build().asFieldSet().asStringUtf8()).isEqualTo("+68656c6c6f\"hello\"");
    }

    @Test
    public void shouldAddBytesAsBigFieldSmallest() {
        final GenericCommandBuilder builder = new GenericCommandBuilder();
        builder.addBigFieldAsSmallest(byteStringUtf8("\n\0=\"\n").toByteArray());
        assertThat(builder.build().asFieldSet().asStringUtf8()).isEqualTo("+0a003d220a");
    }

}
