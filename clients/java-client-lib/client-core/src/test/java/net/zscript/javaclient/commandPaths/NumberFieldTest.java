package net.zscript.javaclient.commandPaths;

import static org.assertj.core.api.Assertions.assertThat;

import nl.jqno.equalsverifier.EqualsVerifier;
import org.junit.jupiter.api.Test;

class NumberFieldTest {
    @Test
    public void shouldInitializeCorrectly() {
        NumberField f = NumberField.fieldOf((byte) 'A', 0x123);
        assertThat(f.getKey()).isEqualTo((byte) 0x41);
        assertThat(f.getValue()).isEqualTo(0x123);
        assertThat(f.isBigField()).isFalse();
        assertThat(f.iterator().hasNext()).isFalse();
    }

    @Test
    public void shouldImplementEqualsAndHashcode() {
        EqualsVerifier.forClass(NumberField.class).verify();
    }
}
