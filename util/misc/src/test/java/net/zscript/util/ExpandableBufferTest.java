package net.zscript.util;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.lang.reflect.Field;
import java.nio.charset.StandardCharsets;

import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.Test;

class ExpandableBufferTest {
    @Test
    public void shouldCollectCorrectly() {
        ExpandableBuffer e = new ExpandableBuffer(128);
        addAndCheck(e);
        assertThat(e.getCapacity()).isEqualTo(128);
    }

    @Test
    public void shouldExpandCorrectly() {
        ExpandableBuffer e = new ExpandableBuffer(1);
        addAndCheck(e);
        assertThat(e.getCapacity()).isEqualTo(8);
    }

    private static void addAndCheck(ExpandableBuffer e) {
        e.addByte((byte) 'a');
        assertThat(e.getCount()).isEqualTo(1);
        e.addByte((byte) 'b');
        assertThat(e.getCount()).isEqualTo(2);
        assertThat(e.toByteArray()).containsExactly('a', 'b');
        e.addBytes(new byte[] { 'a', 'b', 'p', 'q', 'r', 'y', 'z' }, 2, 3);
        e.addBytes(new byte[] { 'x' }, 0, 1);
        assertThat(e.getCount()).isEqualTo(6);
        assertThat(e.toByteArray()).containsExactly('a', 'b', 'p', 'q', 'r', 'x');
    }

    @Test
    public void shouldProduceUtfStrings() {
        ExpandableBuffer e = new ExpandableBuffer(1);
        e.addBytes("x £5 y".getBytes(StandardCharsets.UTF_8), 0, 7);
        assertThat(e.getCount()).isEqualTo(7);
        assertThat(e.asStringUtf8()).isEqualTo("x £5 y");
        assertThat(e.toString()).isEqualTo("x £5 y");
    }

    @Test
    public void shouldWriteToStreams() throws IOException {
        ExpandableBuffer e = new ExpandableBuffer(1);
        e.addBytes("abc".getBytes(StandardCharsets.UTF_8), 0, 3);
        ByteArrayOutputStream s = new ByteArrayOutputStream();
        e.writeTo(s);
        assertThat(s.toByteArray()).containsExactly('a', 'b', 'c');
    }

    @Test
    public void shouldReset() throws NoSuchFieldException, IllegalAccessException {
        ExpandableBuffer e = new ExpandableBuffer(1);
        e.addBytes("abc".getBytes(StandardCharsets.UTF_8), 0, 3);
        assertThat(e.getCount()).isEqualTo(3);

        Field f = ExpandableBuffer.class.getDeclaredField("buf");
        f.setAccessible(true);
        byte[] buf = (byte[]) f.get(e);

        e.reset(false);
        assertThat(e.getCount()).isZero();
        assertThat(buf).containsExactly('a', 'b', 'c', 0);
        e.reset(true);
        assertThat(buf).containsOnly(0);
    }

}
