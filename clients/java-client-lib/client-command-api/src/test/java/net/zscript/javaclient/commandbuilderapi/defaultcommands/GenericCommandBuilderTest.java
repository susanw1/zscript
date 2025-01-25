package net.zscript.javaclient.commandbuilderapi.defaultcommands;

import static net.zscript.util.ByteString.byteStringUtf8;
import static org.assertj.core.api.AssertionsForClassTypes.assertThat;

import org.junit.jupiter.api.Test;

import net.zscript.javaclient.commandbuilderapi.nodes.ZscriptCommandNode;

class GenericCommandBuilderTest {
    final GenericCommandBuilder testBuilder = new GenericCommandBuilder().setField('X', 123).addBigField("hello");

    @Test
    public void shouldCreateGenericCommandBuilder() {
        assertThat(testBuilder.getField('X')).isEqualTo(123);
        assertThat(testBuilder.getField('Y')).isEqualTo(0);

        assertThat(testBuilder.build().asFieldSet().asStringUtf8()).isEqualTo("X7b\"hello\"");
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
    public void shouldAddBytesAsBigFieldSmallest() {
        final GenericCommandBuilder builder = new GenericCommandBuilder();
        builder.addBigFieldAsSmallest(byteStringUtf8("\n\0=\"\n").toByteArray());
        assertThat(builder.build().asFieldSet().asStringUtf8()).isEqualTo("+0a003d220a");
    }

}
