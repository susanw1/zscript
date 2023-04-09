package org.zcode.javareceiver.tokenizer;

import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.Test;

class ZcodeTokenBufferTest {

    private ZcodeTokenBuffer buffer = ZcodeTokenBuffer.createBufferWithCapacity(5);

    @Test
    void shouldTokenizeNumericFieldWithNoValue() {
        buffer.startToken((byte) 'A', true);
        assertThat(buffer.getInternalData()).containsExactly(0, 'A', 0, 0, 0);
    }

    @Test
    void shouldTokenizeNumericFieldWithValue() {
        buffer.startToken((byte) 'A', true)
                .continueTokenNibble((byte) 5)
                .closeToken();
        assertThat(buffer.getInternalData()).containsExactly(1, 'A', 5, 0, 0);
    }

    @Test
    void shouldTokenizeNumericFieldWithByteValue() {
        buffer.startToken((byte) 'A', true)
                .continueTokenByte((byte) 234);
        assertThat(buffer.getInternalData()).containsExactly(1, 'A', (byte) 234, 0, 0);
    }

    @Test
    void shouldTokenizeNumericFieldWith2NibbleValue() {
        buffer.startToken((byte) 'A', true)
                .continueTokenNibble((byte) 5)
                .continueTokenNibble((byte) 0xd);
        assertThat(buffer.getInternalData()).containsExactly(1, 'A', 0x5d, 0, 0);
    }

    @Test
    void shouldTokenizeNumericFieldWith3NibbleValue() {
        buffer.startToken((byte) 'A', true)
                .continueTokenNibble((byte) 5)
                .continueTokenNibble((byte) 0xd)
                .continueTokenNibble((byte) 0xa)
                .closeToken();
        assertThat(buffer.getInternalData()).containsExactly(2, 'A', 0x5, 0xda, 0);
    }

    @Test
    void shouldTokenizeNumericFieldWith4NibbleValue() {
        buffer.startToken((byte) 'A', true)
                .continueTokenNibble((byte) 5)
                .continueTokenNibble((byte) 0xd)
                .continueTokenNibble((byte) 0xa)
                .continueTokenNibble((byte) 0x3)
                .closeToken();
        assertThat(buffer.getInternalData()).containsExactly(2, 'A', 0x5d, 0xa3, 0);
    }

    @Test
    void shouldTokenizeNumericField5NibbleValue() {
        buffer.startToken((byte) 'A', true)
                .continueTokenNibble((byte) 5)
                .continueTokenNibble((byte) 0xd)
                .continueTokenNibble((byte) 0xa)
                .continueTokenNibble((byte) 0x3)
                .continueTokenNibble((byte) 0xe)
                .closeToken();
        assertThat(buffer.getInternalData()).containsExactly(3, 'A', 0x5, 0xda, 0x3e);
    }

}
