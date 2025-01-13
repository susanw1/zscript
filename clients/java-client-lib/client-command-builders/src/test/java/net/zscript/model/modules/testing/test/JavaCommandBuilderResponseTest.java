package net.zscript.model.modules.testing.test;

import java.util.function.Consumer;

import static net.zscript.client.modules.test.testing.TestingModule.TestCommand0Command.TestCommand0Response.BitsetRespTestU.Lion;
import static net.zscript.client.modules.test.testing.TestingModule.TestCommand0Command.TestCommand0Response.BitsetRespTestU.Tabby;
import static net.zscript.client.modules.test.testing.TestingModule.TestCommand0Command.TestCommand0Response.BitsetRespTestU.Tiger;
import static net.zscript.client.modules.test.testing.TestingModule.TestCommand0Command.TestCommand0Response.EnumRespTestP.Monkey;
import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.Test;

import net.zscript.client.modules.test.testing.TestingModule;
import net.zscript.client.modules.test.testing.TestingModule.TestCommand0Command.TestCommand0Response.BitsetRespTestV;
import net.zscript.client.modules.test.testing.TestingModule.TestCommand0Command.TestCommand0Response.EnumRespTestP;
import net.zscript.client.modules.test.testing.TestingModule.TestCommand0Command.TestCommand0Response.EnumRespTestQ;
import net.zscript.javaclient.commandbuilder.ZscriptResponse;
import net.zscript.javaclient.commandbuilder.commandnodes.ResponseCaptor;
import net.zscript.javaclient.commandbuilder.commandnodes.ZscriptCommandBuilder;
import net.zscript.javaclient.commandbuilder.commandnodes.ZscriptCommandNode;
import net.zscript.javaclient.tokens.ExtendingTokenBuffer;
import net.zscript.tokenizer.TokenBuffer.TokenReader;
import net.zscript.tokenizer.Tokenizer;
import net.zscript.tokenizer.ZscriptTokenExpression;

public class JavaCommandBuilderResponseTest {
    final ExtendingTokenBuffer buffer      = new ExtendingTokenBuffer();
    final Tokenizer            tokenizer   = new Tokenizer(buffer.getTokenWriter(), true);
    final TokenReader          tokenReader = buffer.getTokenReader();

    @Test
    void shouldCreateResponseWithRequiredFields() {
        checkResponse("S P2 R1b U5\n", TestingModule.testCommand0Builder()
                        .setEnumReqTestA(2)
                        .setNumberReqTestC(35)
                        .setBitsetReqTestE(1),
                response -> {
                    assertThat(response.isValid()).isTrue();
                    assertThat(response.getEnumRespTestPAsInt()).isEqualTo(0x2);
                    assertThat(response.getEnumRespTestP()).isEqualTo(Monkey);

                    assertThat(response.getEnumRespTestQAsInt()).isEmpty();
                    assertThat(response.getEnumRespTestQ()).isEmpty();
                    assertThat(response.hasEnumRespTestQ()).isFalse();

                    assertThat(response.getNumberRespTestR()).isEqualTo(0x1b);

                    assertThat(response.getNumberRespTestT()).isEmpty();
                    assertThat(response.hasNumberRespTestT()).isFalse();

                    assertThat(response.getBitsetRespTestUAsInt()).isEqualTo(0x5);
                    assertThat(response.getBitsetRespTestU()).containsExactly(Lion, Tabby);

                    assertThat(response.getBitsetRespTestVAsInt()).isEmpty();
                    assertThat(response.getBitsetRespTestV()).isEmpty();
                    assertThat(response.hasBitsetRespTestV()).isFalse();
                });
    }

    @Test
    void shouldCreateResponseWithAllFields() {
        checkResponse("S P Q1 R1b Tf1 U3 V6 \n", TestingModule.testCommand0Builder()
                        .setEnumReqTestA(2)
                        .setNumberReqTestC(35)
                        .setBitsetReqTestE(1),
                response -> {
                    assertThat(response.isValid()).isTrue();
                    assertThat(response.getEnumRespTestPAsInt()).isEqualTo(0x0);
                    assertThat(response.getEnumRespTestP()).isEqualTo(EnumRespTestP.Elephant);

                    assertThat(response.getEnumRespTestQAsInt()).isPresent().hasValue(0x1);
                    assertThat(response.getEnumRespTestQ()).isPresent().hasValue(EnumRespTestQ.Goat);
                    assertThat(response.hasEnumRespTestQ()).isTrue();

                    assertThat(response.getNumberRespTestR()).isEqualTo(0x1b);

                    assertThat(response.getNumberRespTestT()).isPresent().hasValue(0xf1);
                    assertThat(response.hasNumberRespTestT()).isTrue();

                    assertThat(response.getBitsetRespTestUAsInt()).isEqualTo(0x3);
                    assertThat(response.getBitsetRespTestU()).containsExactly(Lion, Tiger);

                    assertThat(response.getBitsetRespTestVAsInt()).isPresent().hasValue(0x6);
                    assertThat(response.getBitsetRespTestV()).contains(BitsetRespTestV.Camembert, BitsetRespTestV.GreekFeta);
                    assertThat(response.hasBitsetRespTestV()).isTrue();
                });

    }

    @Test
    void shouldCreateResponseWithTextField() {
        checkResponse("S \"bar\" \n", TestingModule.testCommand1Builder().setTextReqTestA("foo"), response -> {
            assertThat(response.isValid()).isTrue();
            assertThat(response.getTextRespTestP()).isEqualTo("bar");
            assertThat(response.getTextRespTestPAsBytes()).containsExactly(0x62, 0x61, 0x72);
        });
    }

    @Test
    void shouldCreateResponseWithUtf8TextField() {
        checkResponse("S +c2 a3 c2 b5 \n", TestingModule.testCommand1Builder().setTextReqTestA("foo"),
                response -> {
                    assertThat(response.isValid()).isTrue();
                    assertThat(response.getTextRespTestPAsBytes()).containsExactly(0xc2, 0xa3, 0xc2, 0xb5);
                    assertThat(response.getTextRespTestP()).isEqualTo("£µ");
                });
    }

    @Test
    void shouldCreateResponseWithChineseTextField() {
        checkResponse("S +e6 9c ba e6 a2 b0 e5 a6 96 e5 a7 ac \n", TestingModule.testCommand1Builder().setTextReqTestA("foo"),
                response -> {
                    assertThat(response.isValid()).isTrue();
                    assertThat(response.getTextRespTestP()).isEqualTo("机械妖姬");
                    assertThat(response.getTextRespTestPAsBytes())
                            .containsExactly(0xe6, 0x9c, 0xba, 0xe6, 0xa2, 0xb0, 0xe5, 0xa6, 0x96, 0xe5, 0xa7, 0xac);
                });
    }

    @Test
    void shouldCreateResponseWithEmptyTextField() {
        checkResponse("S +\n", TestingModule.testCommand1Builder().setTextReqTestA("foo"),
                response -> {
                    assertThat(response.isValid()).isTrue();
                    assertThat(response.getTextRespTestP()).isEqualTo("");
                    assertThat(response.getTextRespTestPAsBytes()).isEmpty();
                });
    }

    @Test
    void shouldCreateResponseWithMultipleTextFields() {
        checkResponse("S +41 +42 \"CD\" \n", TestingModule.testCommand1Builder().setTextReqTestA("foo"),
                response -> {
                    assertThat(response.isValid()).isTrue();
                    assertThat(response.getTextRespTestP()).isEqualTo("ABCD");
                });
    }

    @Test
    void shouldCreateResponseWithMultipleEmptyTextFields() {
        checkResponse("S + + + \"\" + \n", TestingModule.testCommand1Builder().setTextReqTestA("foo"),
                response -> {
                    assertThat(response.isValid()).isTrue();
                    assertThat(response.getTextRespTestP()).isEqualTo("");
                    assertThat(response.getTextRespTestPAsBytes()).isEmpty();
                });
    }

    @Test
    void shouldCreateResponseWithMissingTextFields() {
        checkResponse("S \n", TestingModule.testCommand1Builder().setTextReqTestA("foo"),
                response -> {
                    assertThat(response.isValid()).isFalse();
                    assertThat(response.getTextRespTestP()).isEqualTo("");
                    assertThat(response.getTextRespTestPAsBytes()).isEmpty();
                });
    }

    @Test
    void shouldCreateResponseWithRequiredBytes() {
        checkResponse("S \"bar\" \n", TestingModule.testCommand2Builder().setTextReqTestAAsText("foo"), response -> {
            assertThat(response.isValid()).isTrue();
            assertThat(response.getTextRespTestAAsString()).isEqualTo("bar");
            assertThat(response.getTextRespTestA()).containsExactly(0x62, 0x61, 0x72);
        });
    }

    @Test
    void shouldCreateResponseWithMissingRequiredBytes() {
        checkResponse("S \n", TestingModule.testCommand2Builder().setTextReqTestAAsText("foo"), response -> {
            assertThat(response.isValid()).isFalse();
            assertThat(response.getTextRespTestAAsString()).isEmpty();
            assertThat(response.getTextRespTestA()).isEmpty();
        });
    }

    @Test
    void shouldCreateResponseWithOptionalTextField() {
        checkResponse("S \"baz\"\n", TestingModule.testCommand3Builder().setTextReqTestA("foo"),
                response -> {
                    assertThat(response.isValid()).isTrue();
                    assertThat(response.getTextRespTestP()).isEqualTo("baz");
                    assertThat(response.getTextRespTestPAsBytes()).hasSize(3);
                    assertThat(response.hasTextRespTestP()).isTrue();
                });
    }

    @Test
    void shouldCreateResponseWithOptionalEmptyTextField() {
        checkResponse("S \"\"\n", TestingModule.testCommand3Builder().setTextReqTestA("foo"),
                response -> {
                    assertThat(response.isValid()).isTrue();
                    assertThat(response.getTextRespTestP()).isEmpty();
                    assertThat(response.hasTextRespTestP()).isTrue();
                });
    }

    @Test
    void shouldCreateResponseWithOptionalMissingTextField() {
        checkResponse("S \n", TestingModule.testCommand3Builder().setTextReqTestA("foo"),
                response -> {
                    assertThat(response.isValid()).isTrue();
                    assertThat(response.getTextRespTestP()).isEmpty();
                    assertThat(response.hasTextRespTestP()).isFalse();
                });
    }

    @Test
    void shouldCreateResponseWithOptionalBytes() {
        checkResponse("S \"bar\" \n", TestingModule.testCommand4Builder().setTextReqTestAAsText("foo"), response -> {
            assertThat(response.isValid()).isTrue();
            assertThat(response.getTextRespTestAAsString()).isEqualTo("bar");
            assertThat(response.getTextRespTestA()).containsExactly(0x62, 0x61, 0x72);
        });
    }

    @Test
    void shouldCreateResponseWithMissingOptionalBytes() {
        checkResponse("S \n", TestingModule.testCommand4Builder().setTextReqTestAAsText("foo"), response -> {
            assertThat(response.isValid()).isTrue();
            assertThat(response.getTextRespTestAAsString()).isEmpty();
            assertThat(response.getTextRespTestA()).isEmpty();
        });
    }

    /**
     * Utility to do the legwork of pushing a response through the machine.
     *
     * @param responseChars  the chars to tokenize, as though read from a device. Note, each char in the string is really a byte, not a real char that will be turned into UTF8.
     * @param commandBuilder the command to add the response to
     * @param listener       the action to perform when the response is synthesized
     */
    private <R extends ZscriptResponse> void checkResponse(final String responseChars, final ZscriptCommandBuilder<R> commandBuilder,
            final Consumer<R> listener) {
        responseChars.chars().forEach(c -> tokenizer.accept((byte) c));

        ResponseCaptor<R>     captor = ResponseCaptor.create();
        ZscriptCommandNode<R> cmd    = commandBuilder.capture(captor).build();

        listener.accept(cmd.parseResponse(new ZscriptTokenExpression(tokenReader::tokenIterator)));
    }

}
