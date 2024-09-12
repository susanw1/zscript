package net.zscript.util;

import java.util.NoSuchElementException;

import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatThrownBy;

import org.junit.jupiter.api.Test;

class BlockIteratorTest {
    @Test
    public void shouldFindNothingInEmptyIterator() {
        assertThat(BlockIterator.EMPTY.hasNext()).isFalse();
        assertThatThrownBy(BlockIterator.EMPTY::next).isInstanceOf(NoSuchElementException.class);
        assertThatThrownBy(BlockIterator.EMPTY::nextContiguous).isInstanceOf(NoSuchElementException.class);
        assertThatThrownBy(() -> BlockIterator.EMPTY.nextContiguous(12)).isInstanceOf(NoSuchElementException.class);
    }

    @Test
    public void shouldAppendIterator() {
        ByteString b1 = ByteString.from("this is a test", (s, b) -> b.appendUtf8(s));
        ByteString b2 = ByteString.from(b1.iterator());
        assertThat(b2.asString()).isEqualTo("this is a test");
    }
}
