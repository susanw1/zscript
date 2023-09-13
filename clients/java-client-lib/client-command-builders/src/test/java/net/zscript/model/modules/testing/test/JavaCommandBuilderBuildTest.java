package net.zscript.model.modules.testing.test;

import static net.zscript.client.modules.test.testing.TestingModule.TestCommand0CommandBuilder.BitsetReqTestE.Lion;
import static net.zscript.client.modules.test.testing.TestingModule.TestCommand0CommandBuilder.BitsetReqTestE.Tabby;
import static net.zscript.client.modules.test.testing.TestingModule.TestCommand0CommandBuilder.BitsetReqTestF.Camembert;
import static net.zscript.client.modules.test.testing.TestingModule.TestCommand0CommandBuilder.BitsetReqTestF.Cheddar;
import static net.zscript.client.modules.test.testing.TestingModule.TestCommand0CommandBuilder.BitsetReqTestF.GreekFeta;
import static net.zscript.client.modules.test.testing.TestingModule.TestCommand0CommandBuilder.EnumReqTestA.Dog;
import static net.zscript.client.modules.test.testing.TestingModule.TestCommand0CommandBuilder.EnumReqTestB.Hare;
import static net.zscript.client.modules.test.testing.TestingModule.TestCommand0CommandBuilder.EnumReqTestB.Rabbit;
import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatThrownBy;

import java.nio.charset.StandardCharsets;
import java.util.EnumSet;
import java.util.Optional;
import java.util.OptionalInt;

import org.junit.jupiter.api.Test;

import net.zscript.client.modules.test.testing.TestingModule;
import net.zscript.client.modules.test.testing.TestingModule.TestCommand0CommandBuilder.BitsetReqTestE;
import net.zscript.javaclient.commandbuilder.ZscriptCommandBuilder;
import net.zscript.javaclient.commandbuilder.ZscriptFieldOutOfRangeException;
import net.zscript.javaclient.commandbuilder.ZscriptMissingFieldException;

public class JavaCommandBuilderBuildTest {
    @Test
    void shouldCreateCommandWithRequiredFields() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand0()
                .enumReqTestA(2)
                .numberReqTestC(35)
                .bitsetReqTestE(2);
        assertThat(build(b)).isEqualTo("ZA2C23E2\n");
    }

    @Test
    void shouldFailToCreateCommandWithAllMissingRequiredFields() {
        assertThatThrownBy(() -> TestingModule.testCommand0().build())
                .isInstanceOf(ZscriptMissingFieldException.class)
                .hasMessage("missingKeys='A','C','E'");
    }

    @Test
    void shouldFailToCreateCommandWithSomeMissingRequiredFields() {
        assertThatThrownBy(() -> TestingModule.testCommand0()
                .enumReqTestA(2)
                .build())
                .isInstanceOf(ZscriptMissingFieldException.class)
                .hasMessage("missingKeys='C','E'");
    }

    @Test
    void shouldFailToCreateCommandWithOutOfRangeEnumField() {
        assertThatThrownBy(() -> TestingModule.testCommand0()
                .enumReqTestA(3))
                .isInstanceOf(ZscriptFieldOutOfRangeException.class)
                .hasMessage("name=EnumReqTestA, key='A', value=0x3, min=0x0, max=0x2");
    }

    @Test
    void shouldFailToCreateCommandWithOutOfRangeBitsetField() {
        assertThatThrownBy(() -> TestingModule.testCommand0()
                .bitsetReqTestE(8))
                .isInstanceOf(ZscriptFieldOutOfRangeException.class)
                .hasMessage("name=BitsetReqTestE, key='E', value=0x8, min=0x0, max=0x7");
    }

    @Test
    void shouldCreateCommandWithRequiredBitsetFields() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand0()
                .enumReqTestA(Dog)
                .numberReqTestC(5)
                .bitsetReqTestE(BitsetReqTestE.Tiger)
                .bitsetReqTestE(EnumSet.of(Lion, Tabby)) // this overwrites previous value
                .bitsetReqTestF(EnumSet.of(Camembert, Cheddar));
        assertThat(build(b)).isEqualTo("ZA1C5E5F3\n");
    }

    @Test
    void shouldCreateCommandWithOptionalFields() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand0()
                .enumReqTestB(1)
                .numberReqTestD(0x2914)
                .bitsetReqTestF(6)

                .enumReqTestA(2)
                .numberReqTestC(35)
                .bitsetReqTestE(2);
        assertThat(build(b)).isEqualTo("ZA2B1C23D2914E2F6\n");
    }

    @Test
    void shouldCreateCommandWithOptionalEnumFields() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand0()
                .enumReqTestB(Optional.of(Hare))
                .numberReqTestD(OptionalInt.of(0x21a))
                .bitsetReqTestF(Optional.of(GreekFeta))
                .bitsetReqTestF(Optional.of(Cheddar))

                .enumReqTestA(2)
                .numberReqTestC(35)
                .bitsetReqTestE(1);
        assertThat(build(b)).isEqualTo("ZA2B1C23D21aE1F5\n");
    }

    @Test
    void shouldCreateCommandWithConditionalFieldsTrue() {
        ZscriptCommandBuilder<?> b1 = TestingModule.testCommand0()
                .enumReqTestB(true, Rabbit)
                .numberReqTestD(false, 0x21a)
                .bitsetReqTestF(true, GreekFeta)
                .bitsetReqTestF(false, Cheddar)

                .enumReqTestA(2)
                .numberReqTestC(35)
                .bitsetReqTestE(1);
        assertThat(build(b1)).isEqualTo("ZA2BC23E1F4\n");

        ZscriptCommandBuilder<?> b2 = TestingModule.testCommand0()
                .enumReqTestB(false, Rabbit)
                .numberReqTestD(true, 0x21a)
                .bitsetReqTestF(false, GreekFeta)
                .bitsetReqTestF(true, Cheddar)

                .enumReqTestA(2)
                .numberReqTestC(35)
                .bitsetReqTestE(1);
        assertThat(build(b2)).isEqualTo("ZA2C23D21aE1F1\n");
    }

    @Test
    void shouldCreateCommandWithOptionalIntFields() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand0()
                .enumReqTestB(OptionalInt.of(1))
                .numberReqTestD(OptionalInt.of(0x1a))
                .bitsetReqTestF(Optional.of(Camembert)) // should be overwritten
                .bitsetReqTestF(OptionalInt.of(1))
                .bitsetReqTestF(Optional.of(GreekFeta))

                .enumReqTestA(2)
                .numberReqTestC(0xa5)
                .bitsetReqTestE(1);
        assertThat(build(b)).isEqualTo("ZA2B1Ca5D1aE1F5\n");
    }

    @Test
    void shouldCreateCommandWithMissingOptionalEnumFields() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand0()
                .enumReqTestB(Optional.empty())
                .numberReqTestD(OptionalInt.empty())
                .bitsetReqTestF(OptionalInt.empty())

                .enumReqTestA(2)
                .numberReqTestC(35)
                .bitsetReqTestE(1);
        assertThat(build(b)).isEqualTo("ZA2C23E1\n");
    }

    @Test
    void shouldCreateCommandWithRequiredTextField() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand1()
                .textReqTestA("foo");
        assertThat(build(b)).isEqualTo("Z1\"foo\"\n");
    }

    @Test
    void shouldCreateCommandWithMissingTextField() {
        assertThatThrownBy(() -> TestingModule.testCommand1().build())
                .isInstanceOf(ZscriptMissingFieldException.class)
                .hasMessage("missingKeys='+'");
    }

    @Test
    void shouldCreateCommandWithMultipleRequiredTextFields() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand1()
                .textReqTestA("foo")
                .textReqTestAAsBytes(new byte[] { 0x42, 0x41, 0x52 });
        assertThat(build(b)).isEqualTo("Z1\"foo\"+424152\n");
    }

    @Test
    void shouldCreateCommandWithRequiredTextAsEmptyBytes() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand1()
                .textReqTestAAsBytes(new byte[] {});
        assertThat(build(b)).isEqualTo("Z1+\n");
    }

    @Test
    void shouldCreateCommandWithRequiredBytesField() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand2()
                .textReqTestA(new byte[] { 0x42, 0x41, 0x52 });
        assertThat(build(b)).isEqualTo("Z2+424152\n");
    }

    @Test
    void shouldCreateCommandWithMissingBytesField() {
        assertThatThrownBy(() -> TestingModule.testCommand2().build())
                .isInstanceOf(ZscriptMissingFieldException.class)
                .hasMessage("missingKeys='+'");
    }

    @Test
    void shouldCreateCommandWithMultipleRequiredBytesFields() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand2()
                .textReqTestAAsText("foo")
                .textReqTestA(new byte[] { 0x42, 0x41, 0x52 });
        assertThat(build(b)).isEqualTo("Z2\"foo\"+424152\n");
    }

    @Test
    void shouldCreateCommandWithRequiredBytesAsEmptyBytes() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand2()
                .textReqTestA(new byte[] {});
        assertThat(build(b)).isEqualTo("Z2+\n");
    }

    @Test
    void shouldCreateCommandWithOptionalTextField() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand3()
                .textReqTestA("foo");
        assertThat(build(b)).isEqualTo("Z3\"foo\"\n");
    }

    @Test
    void shouldCreateCommandWithMissingOptionalTextField() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand3();
        assertThat(build(b)).isEqualTo("Z3\n");
    }

    @Test
    void shouldCreateCommandWithMultipleOptionalTextFields() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand3()
                .textReqTestA("foo")
                .textReqTestAAsBytes(new byte[] { 0x42, 0x41, 0x52 });
        assertThat(build(b)).isEqualTo("Z3\"foo\"+424152\n");
    }

    @Test
    void shouldCreateCommandWithOptionalTextAsEmptyBytes() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand3()
                .textReqTestAAsBytes(new byte[] {});
        assertThat(build(b)).isEqualTo("Z3+\n");
    }

    @Test
    void shouldCreateCommandWithOptionalBytesField() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand4()
                .textReqTestA(new byte[] { 0x42, 0x41, 0x52 });
        assertThat(build(b)).isEqualTo("Z4+424152\n");
    }

    @Test
    void shouldCreateCommandWithMissingOptionalBytesField() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand4();
        assertThat(build(b)).isEqualTo("Z4\n");
    }

    @Test
    void shouldCreateCommandWithMultipleOptionalBytesFields() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand4()
                .textReqTestAAsText("foo")
                .textReqTestA(new byte[] { 0x42, 0x41, 0x52 });
        assertThat(build(b)).isEqualTo("Z4\"foo\"+424152\n");
    }

    @Test
    void shouldCreateCommandWithOptionalBytesAsEmptyBytes() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand4()
                .textReqTestA(new byte[] {});
        assertThat(build(b)).isEqualTo("Z4+\n");
    }

    private String build(ZscriptCommandBuilder<?> b) {
        // ISO8859, because we want an 8-bit byte in each char, and they *could* be non-printing / non-ascii if they're in Text
        return new String(b.build().compile(), StandardCharsets.ISO_8859_1);
    }
}
