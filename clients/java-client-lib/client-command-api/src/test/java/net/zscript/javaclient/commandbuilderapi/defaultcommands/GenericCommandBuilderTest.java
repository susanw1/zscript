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
    public void shouldAddBytes() {
        final GenericCommandBuilder builder = new GenericCommandBuilder();
        builder.setFieldAsBytes('X', byteStringUtf8("hello"));
        builder.setFieldAsText('Y', byteStringUtf8("hello"));
        assertThat(builder.build().asFieldSet().asStringUtf8()).isEqualTo("X68656c6c6fY\"hello\"");
    }

    @Test
    public void shouldAddTextAsStringFieldSmallest() {
        final GenericCommandBuilder builder = new GenericCommandBuilder();
        builder.setFieldAsSmallest('X', byteStringUtf8("hello"));
        assertThat(builder.build().asFieldSet().asStringUtf8()).isEqualTo("X\"hello\"");
    }

    @Test
    public void shouldAddBytesAsStringFieldSmallest() {
        final GenericCommandBuilder builder = new GenericCommandBuilder();
        builder.setFieldAsSmallest('X', byteStringUtf8("\n\0=\"\n"));
        assertThat(builder.build().asFieldSet().asStringUtf8()).isEqualTo("X0a003d220a");
    }

}
