package net.zscript.javaclient.commandbuilder;

import static net.zscript.javaclient.commandbuilder.Utils.bitsetToEnumSet;
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
}
