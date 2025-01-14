package net.zscript.javaclient.commandpaths;

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
    public void shouldInitializeFromAnotherField() {
        NumberField f  = NumberField.fieldOf((byte) 'A', 0x123);
        NumberField f1 = NumberField.fieldOf(f);
        assertThat(f).isEqualTo(f1);
    }

    @Test
    public void shouldImplementEqualsAndHashcode() {
        EqualsVerifier.forClass(NumberField.class).verify();
    }

    @Test
    public void shouldImplementToString() {
        assertThat(NumberField.fieldOf((byte) 'A', 0x123)).hasToString("NumberField:{'A123'}");
    }
}
