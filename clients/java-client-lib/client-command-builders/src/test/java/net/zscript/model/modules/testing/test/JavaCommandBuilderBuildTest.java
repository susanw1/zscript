package net.zscript.model.modules.testing.test;

import java.util.EnumSet;
import java.util.Optional;
import java.util.OptionalInt;
import java.util.OptionalLong;

import static net.zscript.client.modules.test.testing.TestingModule.TestCommand0Command.Builder.BitsetReqTestE.Lion;
import static net.zscript.client.modules.test.testing.TestingModule.TestCommand0Command.Builder.BitsetReqTestE.Tabby;
import static net.zscript.client.modules.test.testing.TestingModule.TestCommand0Command.Builder.BitsetReqTestF.Camembert;
import static net.zscript.client.modules.test.testing.TestingModule.TestCommand0Command.Builder.BitsetReqTestF.Cheddar;
import static net.zscript.client.modules.test.testing.TestingModule.TestCommand0Command.Builder.BitsetReqTestF.GreekFeta;
import static net.zscript.client.modules.test.testing.TestingModule.TestCommand0Command.Builder.EnumReqTestA.Dog;
import static net.zscript.client.modules.test.testing.TestingModule.TestCommand0Command.Builder.EnumReqTestB.Hare;
import static net.zscript.client.modules.test.testing.TestingModule.TestCommand0Command.Builder.EnumReqTestB.Rabbit;
import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatThrownBy;

import org.junit.jupiter.api.Test;

import net.zscript.client.modules.test.testing.TestingModule;
import net.zscript.client.modules.test.testing.TestingModule.TestCommand0Command.Builder.BitsetReqTestE;
import net.zscript.javaclient.commandbuilderapi.ZscriptFieldOutOfRangeException;
import net.zscript.javaclient.commandbuilderapi.ZscriptMissingFieldException;
import net.zscript.javaclient.commandbuilderapi.nodes.ZscriptCommandBuilder;

public class JavaCommandBuilderBuildTest {
    @Test
    void shouldCreateCommandWithRequiredFields() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand0Builder()
                .setEnumReqTestA(2)
                .setNumberReqTestC(35)
                .setBitsetReqTestE(2)
                .setNumberReqTestG(0x98765432L);
        assertThat(build(b)).isEqualTo("ZA2C23E2G98765432");
    }

    @Test
    void shouldFailToCreateCommandWithAllMissingRequiredFields() {
        assertThatThrownBy(() -> TestingModule.testCommand0Builder().build())
                .isInstanceOf(ZscriptMissingFieldException.class)
                .hasMessage("missingKeys='A','C','E','G'");
    }

    @Test
    void shouldFailToCreateCommandWithSomeMissingRequiredFields() {
        assertThatThrownBy(() -> TestingModule.testCommand0Builder()
                .setEnumReqTestA(2)
                .build())
                .isInstanceOf(ZscriptMissingFieldException.class)
                .hasMessage("missingKeys='C','E','G'");
    }

    @Test
    void shouldFailToCreateCommandWithOutOfRangeEnumField() {
        assertThatThrownBy(() -> TestingModule.testCommand0Builder()
                .setEnumReqTestA(3))
                .isInstanceOf(ZscriptFieldOutOfRangeException.class)
                .hasMessage("name=EnumReqTestA, key='A', value=0x3, min=0x0, max=0x2");
    }

    @Test
    void shouldFailToCreateCommandWithOutOfRangeBitsetField() {
        assertThatThrownBy(() -> TestingModule.testCommand0Builder()
                .setBitsetReqTestE(8))
                .isInstanceOf(ZscriptFieldOutOfRangeException.class)
                .hasMessage("name=BitsetReqTestE, key='E', value=0x8, min=0x0, max=0x7");
    }

    @Test
    void shouldCreateCommandWithRequiredBitsetFields() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand0Builder()
                .setEnumReqTestA(Dog)
                .setNumberReqTestC(5)
                .setBitsetReqTestE(BitsetReqTestE.Tiger)
                .setBitsetReqTestE(EnumSet.of(Lion, Tabby)) // this overwrites previous value
                .setBitsetReqTestF(EnumSet.of(Camembert, Cheddar))
                .setNumberReqTestG(1);
        assertThat(build(b)).isEqualTo("ZA1C5E5F3G1");
    }

    @Test
    void shouldCreateCommandWithOptionalFields() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand0Builder()
                .setEnumReqTestB(1)
                .setNumberReqTestD(0x2914)
                .setBitsetReqTestF(6)

                .setEnumReqTestA(2)
                .setNumberReqTestC(35)
                .setBitsetReqTestE(2)

                .setNumberReqTestG(1)
                .setNumberReqTestH(0x87868584L);

        assertThat(build(b)).isEqualTo("ZA2B1C23D2914E2F6G1H87868584");
    }

    @Test
    void shouldCreateCommandWithOptionalEnumFields() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand0Builder()
                .setEnumReqTestB(Optional.of(Hare))
                .setNumberReqTestD(OptionalInt.of(0x21a))
                .setBitsetReqTestF(Optional.of(GreekFeta))
                .setBitsetReqTestF(Optional.of(Cheddar))

                .setEnumReqTestA(2)
                .setNumberReqTestC(35)
                .setBitsetReqTestE(1)
                .setNumberReqTestG(1);
        assertThat(build(b)).isEqualTo("ZA2B1C23D21aE1F5G1");
    }

    @Test
    void shouldCreateCommandWithConditionalFieldsTrue() {
        ZscriptCommandBuilder<?> b1 = TestingModule.testCommand0Builder()
                .setEnumReqTestBIf(true, Rabbit)
                .setNumberReqTestDIf(false, 0x21a)
                .setBitsetReqTestFIf(true, GreekFeta)
                .setBitsetReqTestFIf(false, Cheddar)
                .setNumberReqTestHIf(true, 0x81828384L)

                .setEnumReqTestA(2)
                .setNumberReqTestC(35)
                .setBitsetReqTestE(1)
                .setNumberReqTestG(1);
        assertThat(build(b1)).isEqualTo("ZA2BC23E1F4G1H81828384");

        ZscriptCommandBuilder<?> b2 = TestingModule.testCommand0Builder()
                .setEnumReqTestBIf(false, Rabbit)
                .setNumberReqTestDIf(true, 0x21a)
                .setBitsetReqTestFIf(false, GreekFeta)
                .setBitsetReqTestFIf(true, Cheddar)
                .setNumberReqTestHIf(false, 0x81828384L)

                .setEnumReqTestA(2)
                .setNumberReqTestC(35)
                .setBitsetReqTestE(1)
                .setNumberReqTestG(1);
        assertThat(build(b2)).isEqualTo("ZA2C23D21aE1F1G1");
    }

    @Test
    void shouldCreateCommandWithOptionalIntFields() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand0Builder()
                .setEnumReqTestB(OptionalInt.of(1))
                .setNumberReqTestD(OptionalInt.of(0x1a))
                .setBitsetReqTestF(Optional.of(Camembert)) // should be overwritten
                .setBitsetReqTestF(OptionalInt.of(1))
                .setBitsetReqTestF(Optional.of(GreekFeta))
                .setNumberReqTestH(OptionalLong.of(0x12345))
                .setEnumReqTestA(2)
                .setNumberReqTestC(0xa5)
                .setBitsetReqTestE(1)
                .setNumberReqTestG(0);
        assertThat(build(b)).isEqualTo("ZA2B1Ca5D1aE1F5GH012345");
    }

    @Test
    void shouldCreateCommandWithMissingOptionalEnumFields() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand0Builder()
                .setEnumReqTestB(Optional.empty())
                .setNumberReqTestD(OptionalInt.empty())
                .setBitsetReqTestF(OptionalInt.empty())

                .setEnumReqTestA(2)
                .setNumberReqTestC(35)
                .setBitsetReqTestE(1)
                .setNumberReqTestG(1);
        assertThat(build(b)).isEqualTo("ZA2C23E1G1");
    }

    @Test
    void shouldCreateCommandWithRequiredTextField() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand1Builder()
                .setTextReqTestA("foo");
        assertThat(build(b)).isEqualTo("Z1A\"foo\"");
    }

    @Test
    void shouldCreateCommandWithMissingTextField() {
        assertThatThrownBy(() -> TestingModule.testCommand1Builder().build())
                .isInstanceOf(ZscriptMissingFieldException.class)
                .hasMessage("missingKeys='A'");
    }

    @Test
    void shouldCreateCommandWithLastOfMultipleRequiredTextFields() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand1Builder()
                .setTextReqTestA("foo")
                .setTextReqTestAAsBytes(new byte[] { 0x42, 0x41, 0x52 });
        assertThat(build(b)).isEqualTo("Z1A424152");
    }

    @Test
    void shouldCreateCommandWithRequiredTextAsEmptyBytes() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand1Builder()
                .setTextReqTestAAsBytes(new byte[] {});
        assertThat(build(b)).isEqualTo("Z1A");
    }

    @Test
    void shouldCreateCommandWithRequiredBytesField() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand2Builder()
                .setTextReqTestA(new byte[] { 0x42, 0x41, 0x52 });
        assertThat(build(b)).isEqualTo("Z2A424152");
    }

    @Test
    void shouldCreateCommandWithMissingBytesField() {
        assertThatThrownBy(() -> TestingModule.testCommand2Builder().build())
                .isInstanceOf(ZscriptMissingFieldException.class)
                .hasMessage("missingKeys='A'");
    }

    @Test
    void shouldCreateCommandWithLastOfMultipleRequiredBytesFields() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand2Builder()
                .setTextReqTestAAsText("foo")
                .setTextReqTestA(new byte[] { 0x42, 0x41, 0x52 });
        assertThat(build(b)).isEqualTo("Z2A424152");
    }

    @Test
    void shouldCreateCommandWithRequiredBytesAsEmptyBytes() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand2Builder()
                .setTextReqTestA(new byte[] {});
        assertThat(build(b)).isEqualTo("Z2A");
    }

    @Test
    void shouldCreateCommandWithOptionalTextField() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand3Builder()
                .setTextReqTestA("foo");
        assertThat(build(b)).isEqualTo("Z3A\"foo\"");
    }

    @Test
    void shouldCreateCommandWithMissingOptionalTextField() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand3Builder();
        assertThat(build(b)).isEqualTo("Z3");
    }

    @Test
    void shouldCreateCommandWithLastOfMultipleOptionalTextFields() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand3Builder()
                .setTextReqTestA("foo")
                .setTextReqTestAAsBytes(new byte[] { 0x42, 0x41, 0x52 });
        assertThat(build(b)).isEqualTo("Z3A424152");
    }

    @Test
    void shouldCreateCommandWithOptionalTextAsEmptyBytes() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand3Builder()
                .setTextReqTestAAsBytes(new byte[] {});
        assertThat(build(b)).isEqualTo("Z3A");
    }

    @Test
    void shouldCreateCommandWithOptionalBytesField() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand4Builder()
                .setTextReqTestA(new byte[] { 0x42, 0x41, 0x52 });
        assertThat(build(b)).isEqualTo("Z4A424152");
    }

    @Test
    void shouldCreateCommandWithMissingOptionalBytesField() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand4Builder();
        assertThat(build(b)).isEqualTo("Z4");
    }

    @Test
    void shouldCreateCommandWithLastOfMultipleOptionalBytesFields() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand4Builder()
                .setTextReqTestAAsText("foo")
                .setTextReqTestA(new byte[] { 0x42, 0x41, 0x52 });
        assertThat(build(b)).isEqualTo("Z4A424152");
    }

    @Test
    void shouldCreateCommandWithOptionalBytesAsEmptyBytes() {
        ZscriptCommandBuilder<?> b = TestingModule.testCommand4Builder()
                .setTextReqTestA(new byte[] {});
        assertThat(build(b)).isEqualTo("Z4A");
    }

    private String build(ZscriptCommandBuilder<?> b) {
        return b.build().asStringUtf8();
    }
}
