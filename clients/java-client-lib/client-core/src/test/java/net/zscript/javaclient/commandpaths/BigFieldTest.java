package net.zscript.javaclient.commandpaths;

import static net.zscript.util.ByteString.byteStringUtf8;
import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.Test;

import net.zscript.model.components.Zchars;
import net.zscript.util.ByteString;

class BigFieldTest {
    final ByteString testBytes = byteStringUtf8("hello");

    @Test
    public void shouldInitializeHexmodeCorrectly() {
        final BigField f = new BigField(testBytes.toByteArray(), false);

        assertThat(f.getKey()).isEqualTo(Zchars.Z_BIGFIELD_HEX);
        assertThat(f.getValue()).isZero();
        assertThat(f.isBigField()).isTrue();
        assertThat(f.isString()).isFalse();
        assertThat(f.iterator().hasNext()).isTrue();
        assertThat(f.getDataLength()).isEqualTo(5);
        assertThat(f.getDataAsByteString()).isEqualTo(testBytes);
        assertThat(f.getData()).isEqualTo(testBytes.toByteArray());
    }

    @Test
    public void shouldInitializeTextmodeCorrectly() {
        final BigField f = new BigField(testBytes.toByteArray(), true);

        assertThat(f.getKey()).isEqualTo(Zchars.Z_BIGFIELD_QUOTED);
        assertThat(f.getValue()).isZero();
        assertThat(f.isBigField()).isTrue();
        assertThat(f.isString()).isTrue();
        assertThat(f.iterator().hasNext()).isTrue();
        assertThat(f.getDataLength()).isEqualTo(5);
        assertThat(f.getDataAsByteString()).isEqualTo(testBytes);
        assertThat(f.getData()).isEqualTo(testBytes.toByteArray());
    }

    @Test
    public void shouldHandleStringEscapes() {
        final ByteString testBytes = byteStringUtf8("A\n\0\"B");
        final BigField   f         = new BigField(testBytes.toByteArray(), true);
        assertThat(f.getDataAsByteString()).isEqualTo(testBytes);
        assertThat(f.toByteString()).isEqualTo(byteStringUtf8("\"A=0a=00=22B\""));
    }

    @Test
    public void shouldImplementToString() {
        assertThat(new BigField(testBytes.toByteArray(), false)).hasToString("BigField:{'+68656c6c6f'}");
        assertThat(new BigField(testBytes.toByteArray(), true)).hasToString("BigField:{'\"hello\"'}");
    }
}
