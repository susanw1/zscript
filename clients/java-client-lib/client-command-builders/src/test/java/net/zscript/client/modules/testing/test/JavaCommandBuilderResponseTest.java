package net.zscript.client.modules.testing.test;

import static net.zscript.client.modules.test.testing.TestingModule.TestCommand1CommandBuilder.TestCommand1CommandResponse.EnumRespTestP.Monkey;
import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.Test;

import net.zscript.client.modules.test.testing.TestingModule;
import net.zscript.client.modules.test.testing.TestingModule.TestCommand1CommandBuilder.TestCommand1CommandResponse.EnumRespTestP;
import net.zscript.client.modules.test.testing.TestingModule.TestCommand1CommandBuilder.TestCommand1CommandResponse.EnumRespTestQ;
import net.zscript.javaclient.commandbuilder.ZscriptCommand;
import net.zscript.javareceiver.tokenizer.TokenBuffer.TokenReader;
import net.zscript.javareceiver.tokenizer.TokenExtendingBuffer;
import net.zscript.javareceiver.tokenizer.Tokenizer;
import net.zscript.javareceiver.tokenizer.ZscriptTokenExpression;

public class JavaCommandBuilderResponseTest {
    final TokenExtendingBuffer buffer      = new TokenExtendingBuffer();
    final Tokenizer            tokenizer   = new Tokenizer(buffer.getTokenWriter(), 2);
    final TokenReader          tokenReader = buffer.getTokenReader();

    @Test
    void shouldCreateResponseWithRequiredFields() {

        "S P2 R1b Ud\n".chars().forEach(c -> tokenizer.accept((byte) c));

        ZscriptCommand cmd = TestingModule.testCommand1()
                .enumReqTestA(2)
                .numberReqTestC(35)
                .bitsetReqTestE(1)
                .addResponseListener(response -> {
                    assertThat(response.isValid()).isTrue();
                    assertThat(response.getEnumRespTestPAsInt()).isEqualTo(0x2);
                    assertThat(response.getEnumRespTestP()).isEqualTo(Monkey);

                    assertThat(response.getEnumRespTestQAsInt()).isEmpty();
                    assertThat(response.getEnumRespTestQ()).isEmpty();
                    assertThat(response.hasEnumRespTestQ()).isFalse();

                    assertThat(response.getNumberRespTestR()).isEqualTo(0x1b);

                    assertThat(response.getNumberReqTestT()).isEmpty();
                    assertThat(response.hasNumberReqTestT()).isFalse();

//                    assertThat(response.hasBi()).isFalse();
//                  assertThat(response.getTestU).isEqualTo(0x0d);

                })
                .build();

        cmd.response(new ZscriptTokenExpression(() -> tokenReader.iterator()));
    }

    @Test
    void shouldCreateResponseWithAllFields() {

        "S P Q1 R1b Tf1 Ud V12 \n".chars().forEach(c -> tokenizer.accept((byte) c));

        ZscriptCommand cmd = TestingModule.testCommand1()
                .enumReqTestA(2)
                .numberReqTestC(35)
                .bitsetReqTestE(1)
                .addResponseListener(response -> {
                    assertThat(response.isValid()).isTrue();
                    assertThat(response.getEnumRespTestPAsInt()).isEqualTo(0x0);
                    assertThat(response.getEnumRespTestP()).isEqualTo(EnumRespTestP.Elephant);

                    assertThat(response.getEnumRespTestQAsInt()).isPresent().hasValue(0x1);
                    assertThat(response.getEnumRespTestQ()).isPresent().hasValue(EnumRespTestQ.Goat);
                    assertThat(response.hasEnumRespTestQ()).isTrue();

                    assertThat(response.getNumberRespTestR()).isEqualTo(0x1b);

                    assertThat(response.getNumberReqTestT()).isPresent().hasValue(0xf1);
                    assertThat(response.hasNumberReqTestT()).isTrue();
                })
                .build();

        cmd.response(new ZscriptTokenExpression(() -> tokenReader.iterator()));
    }

}
