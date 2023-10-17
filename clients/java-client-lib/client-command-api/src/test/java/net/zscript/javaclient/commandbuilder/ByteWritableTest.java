package net.zscript.javaclient.commandbuilder;

import java.io.ByteArrayOutputStream;
import java.io.IOException;

import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.Test;

import net.zscript.javaclient.ByteWritable;
import net.zscript.javaclient.ZscriptByteString;
import net.zscript.javaclient.ZscriptByteString.ZscriptByteStringBuilder;

class ByteWritableTest {
    ByteWritable testObject = new ByteWritable() {
        @Override
        public ByteWritable writeTo(final ZscriptByteStringBuilder out) {
            out.appendByte('a');
            return this;
        }
    };

    @Test
    public void shouldJustWrite() throws IOException {
        ZscriptByteStringBuilder zbsb = ZscriptByteString.builder();
        ByteArrayOutputStream    baos = new ByteArrayOutputStream();
        testObject.writeTo(zbsb).writeTo(zbsb);
        zbsb.writeTo(baos);
        assertThat(baos.toByteArray()).containsExactly('a', 'a');
    }
}
