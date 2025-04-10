package net.zscript.javaclient.commandpaths;

import static net.zscript.util.ByteString.byteString;
import static net.zscript.util.ByteString.byteStringUtf8;
import static org.assertj.core.api.Assertions.assertThat;

import nl.jqno.equalsverifier.EqualsVerifier;
import org.junit.jupiter.api.Test;

import net.zscript.util.ByteString;

class FieldElementTest {
    final ByteString testBytes = byteStringUtf8("hello");

    @Test
    public void shouldInitializeNumericCorrectly() {
        FieldElement f = FieldElement.fieldOf((byte) 'A', 0x123);
        assertThat(f.getKey()).isEqualTo((byte) 0x41);
        assertThat(f.getValue()).isEqualTo(0x123);
        assertThat(f.getValue32()).isEqualTo(0x123L);
        assertThat(f.isNumeric()).isTrue();
        assertThat(f.iterator().hasNext()).isTrue();
        assertThat(f.iterator().nextContiguous()).containsExactly(0x01, 0x23);
    }

    @Test
    public void shouldInitialize32bitNumericCorrectly() {
        FieldElement f = FieldElement.fieldOf((byte) 'A', 0xfedcba98);
        assertThat(f.getKey()).isEqualTo((byte) 0x41);
        assertThat(f.getValue32()).isEqualTo(0xfedcba98L);
        assertThat(f.getValue()).isEqualTo(0xba98);
        assertThat(f.isNumeric()).isFalse();
        assertThat(f.iterator().hasNext()).isTrue();
        assertThat(f.iterator().nextContiguous()).containsExactly(0xfe, 0xdc, 0xba, 0x98);
    }

    @Test
    public void shouldHandleZeroesCorrectly() {
        FieldElement f1 = FieldElement.fieldOf((byte) 'A', 0x00000098L);
        assertThat(f1.getValue()).isEqualTo(0x98L);
        assertThat(f1.getValue32()).isEqualTo(0x98L);

        FieldElement f2 = FieldElement.fieldOf((byte) 'A', 0x00800098L);
        assertThat(f2.getValue()).isEqualTo(0x98L);
        assertThat(f2.getValue32()).isEqualTo(0x800098L);

        FieldElement f3 = FieldElement.fieldOf((byte) 'A', 0x80000098L);
        assertThat(f3.getValue()).isEqualTo(0x98L);
        assertThat(f3.getValue32()).isEqualTo(0x80000098L);

        FieldElement f4 = FieldElement.fieldOf((byte) 'A', 0L);
        assertThat(f4.getValue()).isEqualTo(0);
        assertThat(f4.getValue32()).isEqualTo(0L);
    }

    @Test
    public void shouldInitializeStringModeCorrectly() {
        final FieldElement f = FieldElement.fieldOfText((byte) 'A', testBytes);

        assertThat(f.getKey()).isEqualTo((byte) 'A');
        assertThat(f.getValue()).isEqualTo('l' * 256 + 'o');
        assertThat(f.getValue32()).isEqualTo(('e' * 256 + 'l') * 65536 + 'l' * 256 + 'o');
        assertThat(f.isNumeric()).isFalse();
        assertThat(f.iterator().hasNext()).isTrue();
        assertThat(f.getDataLength()).isEqualTo(5);
        assertThat(f.getDataAsByteString()).isEqualTo(testBytes);
        assertThat(f.getData()).isEqualTo(testBytes.toByteArray());
    }

    @Test
    public void shouldInitializeHexModeCorrectly() {
        final FieldElement f = FieldElement.fieldOfBytes((byte) 'A', testBytes);

        assertThat(f.getKey()).isEqualTo((byte) 'A');
        assertThat(f.getValue()).isEqualTo('l' * 256 + 'o');
        assertThat(f.getValue32()).isEqualTo(('e' * 256 + 'l') * 65536 + 'l' * 256 + 'o');
        assertThat(f.isNumeric()).isFalse();
        assertThat(f.iterator().hasNext()).isTrue();
        assertThat(f.getDataLength()).isEqualTo(5);
        assertThat(f.getDataAsByteString()).isEqualTo(testBytes);
        assertThat(f.getData()).isEqualTo(testBytes.toByteArray());
    }

    @Test
    public void shouldInitializeSmallestData() {
        final FieldElement f1 = FieldElement.fieldAsSmallest((byte) 'A', testBytes);
        assertThat(f1.isNumeric()).isFalse();
        assertThat(f1.getValue()).isEqualTo('l' * 256 + 'o');

        final FieldElement f2 = FieldElement.fieldAsSmallest((byte) 'A', byteString((byte) 0, (byte) 0));
        assertThat(f2.isNumeric()).isTrue();
        assertThat(f2.getValue()).isEqualTo(0);

        final FieldElement f3 = FieldElement.fieldAsSmallest((byte) 'A', byteString((byte) 0, (byte) 0, (byte) 0));
        assertThat(f3.isNumeric()).isFalse();
        assertThat(f3.getValue()).isEqualTo(0);

        final FieldElement f4 = FieldElement.fieldAsSmallest((byte) 'A', byteString((byte) 1, (byte) 1, (byte) 1));
        assertThat(f4.isNumeric()).isFalse();
        assertThat(f4.getValue()).isEqualTo(257);
    }

    @Test
    public void shouldInitializeFromAnotherField() {
        FieldElement f  = FieldElement.fieldOf((byte) 'A', 0x123);
        FieldElement f1 = FieldElement.fieldOf(f);
        assertThat(f).isEqualTo(f1);
    }

    @Test
    public void shouldImplementEqualsAndHashcode() {
        EqualsVerifier.forClass(FieldElement.class).verify();

        FieldElement f1 = FieldElement.fieldOf((byte) 'S', 0);
        FieldElement f2 = FieldElement.fieldOfBytes((byte) 'S', byteString((byte) 0));
        assertThat(f1.equals(f2)).isFalse();
    }

    @Test
    public void shouldMatch() {
        final FieldElement f1 = FieldElement.fieldOfText((byte) 'S', byteString((byte) 1));
        final FieldElement f2 = FieldElement.fieldOfBytes((byte) 'S', byteString((byte) 1));
        final FieldElement f3 = FieldElement.fieldOf((byte) 'S', 1);

        final FieldElement f4 = FieldElement.fieldOfBytes((byte) 'S', byteString((byte) 0, (byte) 1));
        final FieldElement f5 = FieldElement.fieldOf((byte) 'T', 1);

        assertThat(f1.matches(f2)).isTrue();
        assertThat(f2.matches(f1)).isTrue();
        assertThat(f1.matches(f3)).isTrue();
        assertThat(f3.matches(f1)).isTrue();
        assertThat(f2.matches(f3)).isTrue();
        assertThat(f3.matches(f2)).isTrue();

        assertThat(f4.matches(f1)).isFalse();
        assertThat(f1.matches(f4)).isFalse();
        assertThat(f4.matches(f2)).isTrue();
        assertThat(f2.matches(f4)).isTrue();
        assertThat(f4.matches(f3)).isTrue();
        assertThat(f3.matches(f4)).isTrue();

        assertThat(f1.matches(f5)).isFalse();
    }

    @Test
    public void shouldImplementToString() {
        assertThat(FieldElement.fieldOf((byte) 'A', 0x123)).hasToString("FieldElement:{'A123'}");
        assertThat(FieldElement.fieldOfText((byte) 'A', testBytes)).hasToString("FieldElement:{'A\"hello\"'}");
        assertThat(FieldElement.fieldOfText((byte) 'A', byteStringUtf8("h=l£o"))).hasToString("FieldElement:{'A\"h=3dl£o\"'}");
        assertThat(FieldElement.fieldOfBytes((byte) 'A', byteStringUtf8("h=l£o"))).hasToString("FieldElement:{'A683d6cc2a36f'}");
    }

}
