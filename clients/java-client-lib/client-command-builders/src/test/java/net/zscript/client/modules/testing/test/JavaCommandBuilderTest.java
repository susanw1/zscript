package net.zscript.client.modules.testing.test;

import static net.zscript.client.modules.test.testing.TestingModule.TestCommand1CommandBuilder.BitsetReqTestE.Lion;
import static net.zscript.client.modules.test.testing.TestingModule.TestCommand1CommandBuilder.BitsetReqTestE.Tabby;
import static net.zscript.client.modules.test.testing.TestingModule.TestCommand1CommandBuilder.BitsetReqTestF.Camembert;
import static net.zscript.client.modules.test.testing.TestingModule.TestCommand1CommandBuilder.BitsetReqTestF.Cheddar;
import static net.zscript.client.modules.test.testing.TestingModule.TestCommand1CommandBuilder.BitsetReqTestF.GreekFeta;
import static net.zscript.client.modules.test.testing.TestingModule.TestCommand1CommandBuilder.EnumReqTestA.Dog;
import static net.zscript.client.modules.test.testing.TestingModule.TestCommand1CommandBuilder.EnumReqTestB.Hare;
import static net.zscript.client.modules.test.testing.TestingModule.TestCommand1CommandBuilder.EnumReqTestB.Rabbit;
import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatThrownBy;

import java.util.EnumSet;
import java.util.Optional;
import java.util.OptionalInt;

import org.junit.jupiter.api.Test;

import net.zscript.client.modules.test.testing.TestingModule;
import net.zscript.client.modules.test.testing.TestingModule.TestCommand1CommandBuilder.BitsetReqTestE;
import net.zscript.javaclient.commandbuilder.ZscriptCommand;
import net.zscript.javaclient.commandbuilder.ZscriptCommandBuilder;
import net.zscript.javaclient.commandbuilder.ZscriptFieldOutOfRangeException;
import net.zscript.javareceiver.tokenizer.TokenBuffer.TokenReader;
import net.zscript.javareceiver.tokenizer.TokenExtendingBuffer;
import net.zscript.javareceiver.tokenizer.Tokenizer;
import net.zscript.javareceiver.tokenizer.ZscriptTokenExpression;

public class JavaCommandBuilderTest {
    @Test
    void shouldCreateCommandWithRequiredFields() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand1()
                .enumReqTestA(2)
                .numberReqTestC(35)
                .bitsetReqTestE(2);
        assertThat(build(b)).isEqualTo("ZA2C23E2");
    }

    @Test
    void shouldFailToCreateCommandWithAllMissingRequiredFields() {
        assertThatThrownBy(() -> TestingModule.testCommand1().build())
                .isInstanceOf(IllegalStateException.class)
                .hasMessage("Required field not set: A");
    }

    @Test
    void shouldFailToCreateCommandWithSomeMissingRequiredFields() {
        assertThatThrownBy(() -> TestingModule.testCommand1()
                .enumReqTestA(2)
                .build())
                        .isInstanceOf(IllegalStateException.class)
                        .hasMessage("Required field not set: C");
    }

    @Test
    void shouldFailToCreateCommandWithOutOfRangeEnumField() {
        assertThatThrownBy(() -> TestingModule.testCommand1()
                .enumReqTestA(3))
                        .isInstanceOf(ZscriptFieldOutOfRangeException.class)
                        .hasMessage("[name=enumReqTestA, key='A', value=0x3, min=0x0, max=0x2]");
    }

    @Test
    void shouldFailToCreateCommandWithOutOfRangeBitsetField() {
        assertThatThrownBy(() -> TestingModule.testCommand1()
                .bitsetReqTestE(8))
                        .isInstanceOf(ZscriptFieldOutOfRangeException.class)
                        .hasMessage("[name=bitsetReqTestE, key='E', value=0x8, min=0x0, max=0x7]");
    }

    @Test
    void shouldCreateCommandWithRequiredBitsetFields() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand1()
                .enumReqTestA(Dog)
                .numberReqTestC(5)
                .bitsetReqTestE(BitsetReqTestE.Tiger)
                .bitsetReqTestE(EnumSet.of(Lion, Tabby)) // this overwrites previous value
                .bitsetReqTestF(EnumSet.of(Camembert, Cheddar));
        assertThat(build(b)).isEqualTo("ZA1C5E5F3");
    }

    @Test
    void shouldCreateCommandWithOptionalFields() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand1()
                .enumReqTestB(1)
                .numberReqTestD(0x2914)
                .bitsetReqTestF(6)

                .enumReqTestA(2)
                .numberReqTestC(35)
                .bitsetReqTestE(2);
        assertThat(build(b)).isEqualTo("ZA2B1C23D2914E2F6");
    }

    @Test
    void shouldCreateCommandWithOptionalEnumFields() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand1()
                .enumReqTestB(Optional.of(Hare))
                .numberReqTestD(OptionalInt.of(0x21a))
                .bitsetReqTestF(Optional.of(GreekFeta))
                .bitsetReqTestF(Optional.of(Cheddar))

                .enumReqTestA(2)
                .numberReqTestC(35)
                .bitsetReqTestE(1);
        assertThat(build(b)).isEqualTo("ZA2B1C23D21aE1F5");
    }

    @Test
    void shouldCreateCommandWithConditionalFieldsTrue() {
        ZscriptCommandBuilder<?> b1 = TestingModule.testCommand1()
                .enumReqTestB(true, Rabbit)
                .numberReqTestD(false, 0x21a)
                .bitsetReqTestF(true, GreekFeta)
                .bitsetReqTestF(false, Cheddar)

                .enumReqTestA(2)
                .numberReqTestC(35)
                .bitsetReqTestE(1);
        assertThat(build(b1)).isEqualTo("ZA2BC23E1F4");

        ZscriptCommandBuilder<?> b2 = TestingModule.testCommand1()
                .enumReqTestB(false, Rabbit)
                .numberReqTestD(true, 0x21a)
                .bitsetReqTestF(false, GreekFeta)
                .bitsetReqTestF(true, Cheddar)

                .enumReqTestA(2)
                .numberReqTestC(35)
                .bitsetReqTestE(1);
        assertThat(build(b2)).isEqualTo("ZA2C23D21aE1F1");
    }

    @Test
    void shouldCreateCommandWithOptionalIntFields() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand1()
                .enumReqTestB(OptionalInt.of(1))
                .numberReqTestD(OptionalInt.of(0x1a))
                .bitsetReqTestF(Optional.of(Camembert)) // should be overwritten
                .bitsetReqTestF(OptionalInt.of(1))
                .bitsetReqTestF(Optional.of(GreekFeta))

                .enumReqTestA(2)
                .numberReqTestC(0xa5)
                .bitsetReqTestE(1);
        assertThat(build(b)).isEqualTo("ZA2B1Ca5D1aE1F5");
    }

    @Test
    void shouldCreateCommandWithMissingOptionalEnumFields() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand1()
                .enumReqTestB(Optional.empty())
                .numberReqTestD(OptionalInt.empty())
                .bitsetReqTestF(OptionalInt.empty())

                .enumReqTestA(2)
                .numberReqTestC(35)
                .bitsetReqTestE(1);
        assertThat(build(b)).isEqualTo("ZA2C23E1");
    }

    @Test
    void shouldCreateResponse() {
        final TokenExtendingBuffer buffer      = new TokenExtendingBuffer();
        final Tokenizer            tokenizer   = new Tokenizer(buffer.getTokenWriter(), 2);
        final TokenReader          tokenReader = buffer.getTokenReader();

        "SP2a\n".chars().forEach(c -> tokenizer.accept((byte) c));

        ZscriptCommand cmd = TestingModule.testCommand1()
                .enumReqTestA(2)
                .numberReqTestC(35)
                .bitsetReqTestE(1)
                .addResponseListener(resp -> {
//                    assertThat(resp.isValid()).isTrue();
                    assertThat(resp.getEnumRespTestPAsInt()).isEqualTo(0x2a);
                })
                .build();

        cmd.response(new ZscriptTokenExpression(() -> tokenReader.iterator()));
    }

    private String build(ZscriptCommandBuilder<?> b) {
        return new String(b.build().compile(true));
    }
}
