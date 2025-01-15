package net.zscript.javaclient.commandbuilderapi;

import static net.zscript.javaclient.commandbuilderapi.Utils.bitsetToEnumSet;
import static net.zscript.javaclient.commandbuilderapi.Utils.checkInEnumRange;
import static net.zscript.javaclient.commandbuilderapi.Utils.formatField;
import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatThrownBy;

import org.junit.jupiter.api.Test;

class UtilsTest {
    enum TestEnum {
        A,
        B,
        C,
        D
    }

    @Test
    public void shouldMapBitsToEnumSet() {
        assertThat(bitsetToEnumSet(0, TestEnum.class)).isEmpty();
        assertThat(bitsetToEnumSet(3, TestEnum.class)).containsOnly(TestEnum.A, TestEnum.B);
        assertThat(bitsetToEnumSet(5, TestEnum.class)).containsOnly(TestEnum.A, TestEnum.C);
        assertThat(bitsetToEnumSet(0xf, TestEnum.class)).containsExactly(TestEnum.values());

        assertThatThrownBy(() -> bitsetToEnumSet(0x1f, TestEnum.class)).isInstanceOf(IndexOutOfBoundsException.class)
                .hasMessageContaining("out of bounds");
    }

    @Test
    public void shouldCheckRangeForEnum() {
        assertThat(checkInEnumRange(0, TestEnum.class, 'X', false)).isEqualTo(0);
        assertThat(checkInEnumRange(3, TestEnum.class, 'X', false)).isEqualTo(3);

        assertThatThrownBy(() -> checkInEnumRange(4, TestEnum.class, 'X', false)).isInstanceOf(ZscriptFieldOutOfRangeException.class)
                .hasMessage("name=TestEnum, key='X', value=0x4, min=0x0, max=0x3");
        assertThatThrownBy(() -> checkInEnumRange(-1, TestEnum.class, 'Y', false)).isInstanceOf(ZscriptFieldOutOfRangeException.class)
                .hasMessage("name=TestEnum, key='Y', value=0xffffffff, min=0x0, max=0x3");
    }

    @Test
    public void shouldCheckRangeForBitset() {
        assertThat(checkInEnumRange(0, TestEnum.class, 'X', true)).isEqualTo(0);
        assertThat(checkInEnumRange(3, TestEnum.class, 'X', true)).isEqualTo(3);
        assertThat(checkInEnumRange(15, TestEnum.class, 'X', true)).isEqualTo(15);

        assertThatThrownBy(() -> checkInEnumRange(16, TestEnum.class, 'X', true)).isInstanceOf(ZscriptFieldOutOfRangeException.class)
                .hasMessage("name=TestEnum, key='X', value=0x10, min=0x0, max=0xf");

        assertThatThrownBy(() -> checkInEnumRange(-1, TestEnum.class, 'Y', true)).isInstanceOf(ZscriptFieldOutOfRangeException.class)
                .hasMessage("name=TestEnum, key='Y', value=0xffffffff, min=0x0, max=0xf");
    }

    @Test
    public void shouldFormatField() {
        assertThat(formatField((byte) 'A', 0)).containsExactly('A');
        assertThat(formatField((byte) 'B', 12)).containsExactly('B', 'c');
        assertThat(formatField((byte) 'B', 0x12)).containsExactly('B', '1', '2');
        assertThat(formatField((byte) 'C', 0x123)).containsExactly('C', '1', '2', '3');
        assertThat(formatField((byte) 'D', 0x1234)).containsExactly('D', '1', '2', '3', '4');
        assertThatThrownBy(() -> formatField((byte) 'D', 0x12345)).isInstanceOf(IllegalArgumentException.class)
                .hasMessage("Command fields must be uint16: 0x12345");
    }
}
