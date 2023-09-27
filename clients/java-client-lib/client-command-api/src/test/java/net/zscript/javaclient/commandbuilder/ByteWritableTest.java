package net.zscript.javaclient.commandbuilder;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.OutputStream;

import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatThrownBy;

import org.junit.jupiter.api.Test;

import net.zscript.model.components.Zchars;

class ByteWritableTest {
    ByteWritable testObject = new ByteWritable() {
        @Override
        public <T extends OutputStream> T writeTo(T out) throws IOException {
            out.write('a');
            return out;
        }
    };

    @Test
    public void shouldJustWrite() throws IOException {
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        testObject.writeTo(baos).writeTo(baos);
        assertThat(baos.toByteArray()).containsExactly('a', 'a');
    }

    @Test
    public void shouldWriteNumbers() throws IOException {
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        testObject.writeNumeric(Zchars.Z_CMD, 0x1a2b, baos);
        assertThat(baos.toByteArray()).containsExactly('Z', '1', 'a', '2', 'b');
    }

    @Test
    public void shouldWriteNumberZero() throws IOException {
        assertThat(testObject.writeNumeric(Zchars.Z_CMD, 0, new ByteArrayOutputStream()).toByteArray()).containsExactly('Z');
    }

    @Test
    public void shouldThrowOnOutOfRangeNumerValue() throws IOException {
        assertThatThrownBy(() -> testObject.writeNumeric(Zchars.Z_CMD, 123456, new ByteArrayOutputStream()))
                .isInstanceOf(IllegalArgumentException.class);
    }

    @Test
    public void shouldWriteHex() throws IOException {
        ByteArrayOutputStream out = new ByteArrayOutputStream();
        assertThat(testObject.writeHex((byte) 0, out).toByteArray()).containsExactly('0', '0');
        assertThat(testObject.writeHex((byte) 0x3d, out).toByteArray()).containsExactly('0', '0', '3', 'd');
        assertThat(testObject.writeHex((byte) 0xff, out).toByteArray()).containsExactly('0', '0', '3', 'd', 'f', 'f');
    }

}
