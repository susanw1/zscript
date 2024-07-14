package net.zscript.util;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.nio.charset.StandardCharsets;

import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.Test;

class ExpandableByteBufferTest {
    @Test
    public void shouldCollectCorrectly() {
        ExpandableByteBuffer e = new ExpandableByteBuffer(128);
        addAndCheck(e);
        assertThat(e.getCapacity()).isEqualTo(128);
    }

    @Test
    public void shouldExpandCorrectly() {
        ExpandableByteBuffer e = new ExpandableByteBuffer(1);
        addAndCheck(e);
        assertThat(e.getCapacity()).isEqualTo(8);
    }

    private static void addAndCheck(ExpandableByteBuffer e) {
        e.addByte((byte) 'a');
        assertThat(e.getCount()).isEqualTo(1);
        e.addByte((byte) 'b');
        assertThat(e.getCount()).isEqualTo(2);
        assertThat(e.toByteArray()).containsExactly('a', 'b');
        e.addBytes(new byte[] { 'p', 'q', 'r' });
        e.addBytes(new byte[] { 'x' });
        assertThat(e.getCount()).isEqualTo(6);
        assertThat(e.toByteArray()).containsExactly('a', 'b', 'p', 'q', 'r', 'x');
    }

    @Test
    public void shouldProduceUtfStrings() {
        ExpandableByteBuffer e = new ExpandableByteBuffer(1);
        e.addBytes("x £5 y".getBytes(StandardCharsets.UTF_8));
        assertThat(e.getCount()).isEqualTo(7);
        assertThat(e.asString()).isEqualTo("x £5 y");
        assertThat(e.toString()).isEqualTo("x £5 y");
    }

    @Test
    public void shouldWriteToStreams() throws IOException {
        ExpandableByteBuffer e = new ExpandableByteBuffer(1);
        e.addBytes("abc".getBytes(StandardCharsets.UTF_8));
        ByteArrayOutputStream s = new ByteArrayOutputStream();
        e.writeTo(s);
        assertThat(s.toByteArray()).containsExactly('a', 'b', 'c');
    }

}
