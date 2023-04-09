package org.zcode.javareceiver.tokenizer;

import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.Test;

class ZcodeTokenBufferTest {

    private ZcodeTokenRingBuffer buffer = ZcodeTokenRingBuffer.createBufferWithCapacity(10);

    @Test
    void shouldTokenizeNumericFieldWithNoValue() {
        buffer.startToken((byte) 'A', true);
        assertThat(buffer.getInternalData()).startsWith(0, 'A', 0);
    }

    @Test
    void shouldTokenizeNumericFieldWithValue() {
        buffer.startToken((byte) 'A', true);
        buffer.continueTokenNibble((byte) 5);
        buffer.closeToken();
        assertThat(buffer.getInternalData()).startsWith(1, 'A', 5, 0);
    }

    @Test
    void shouldTokenizeNumericFieldWithByteValue() {
        buffer.startToken((byte) 'A', true);
        buffer.continueTokenByte((byte) 234);
        assertThat(buffer.getInternalData()).startsWith(1, 'A', (byte) 234, 0);
    }

    @Test
    void shouldTokenizeNumericFieldWith2NibbleValue() {
        buffer.startToken((byte) 'A', true);
        buffer.continueTokenNibble((byte) 5);
        buffer.continueTokenNibble((byte) 0xd);
        assertThat(buffer.getInternalData()).startsWith(1, 'A', 0x5d, 0);
    }

    @Test
    void shouldTokenizeNumericFieldWith3NibbleValue() {
        buffer.startToken((byte) 'A', true);
        buffer.continueTokenNibble((byte) 5);
        buffer.continueTokenNibble((byte) 0xd);
        buffer.continueTokenNibble((byte) 0xa);
        buffer.closeToken();
        assertThat(buffer.getInternalData()).startsWith(2, 'A', 0x5, 0xda, 0);
    }

    @Test
    void shouldTokenizeNumericFieldWith4NibbleValue() {
        buffer.startToken((byte) 'A', true);
        buffer.continueTokenNibble((byte) 5);
        buffer.continueTokenNibble((byte) 0xd);
        buffer.continueTokenNibble((byte) 0xa);
        buffer.continueTokenNibble((byte) 0x3);
        buffer.closeToken();
        assertThat(buffer.getInternalData()).startsWith(2, 'A', 0x5d, 0xa3, 0);
    }

    @Test
    void shouldTokenizeNumericField5NibbleValue() {
        buffer.startToken((byte) 'A', true);
        buffer.continueTokenNibble((byte) 5);
        buffer.continueTokenNibble((byte) 0xd);
        buffer.continueTokenNibble((byte) 0xa);
        buffer.continueTokenNibble((byte) 0x3);
        buffer.continueTokenNibble((byte) 0xe);
        buffer.closeToken();
        assertThat(buffer.getInternalData()).startsWith(3, 'A', 0x5, 0xda, 0x3e, 0);
    }

    @Test
    void shouldTokenize2NumericFields() {
        buffer.startToken((byte) 'A', true);
        buffer.closeToken();
        buffer.startToken((byte) 'B', true);
        buffer.closeToken();
        assertThat(buffer.getInternalData()).startsWith(0, 'A', 0, 'B', 0);
    }

    @Test
    void shouldTokenize2NumericFieldsWithValues() {
        buffer.startToken((byte) 'A', true);
        buffer.continueTokenNibble((byte) 5);
        buffer.closeToken();
        buffer.startToken((byte) 'B', true);
        buffer.closeToken();
        assertThat(buffer.getInternalData()).startsWith(1, 'A', 5, 0, 'B', 0);
    }

    @Test
    void shouldTokenizeNonNumericField() {
        buffer.startToken((byte) '+', false);
        buffer.closeToken();
        assertThat(buffer.getInternalData()).startsWith(0, '+', 0);
    }

    @Test
    void shouldTokenize2NonNumericFields() {
        buffer.startToken((byte) '+', false);
        buffer.closeToken();
        buffer.startToken((byte) '+', false);
        buffer.closeToken();
        assertThat(buffer.getInternalData()).startsWith(0, '+', 0, '+', 0);
    }

    @Test
    void shouldTokenize2NonNumericFieldsWithValues() {
        buffer.startToken((byte) '+', false);
        buffer.continueTokenNibble((byte) 5);
        buffer.continueTokenNibble((byte) 0xa);
        buffer.closeToken();
        buffer.startToken((byte) '+', false);
        buffer.closeToken();
        assertThat(buffer.getInternalData()).startsWith(1, '+', 0x5a, 0, '+', 0);
    }

    @Test
    void shouldTokenizeMixedFieldsWithOddNibbleValues() {
        buffer.startToken((byte) 'A', true);
        buffer.continueTokenNibble((byte) 5);
        buffer.continueTokenNibble((byte) 0xa);
        buffer.continueTokenNibble((byte) 0xb);
        buffer.closeToken();
        buffer.startToken((byte) '+', false);
        buffer.closeToken();
        assertThat(buffer.getInternalData()).startsWith(2, 'A', 0x5, (byte) 0xab, 0, '+', 0);
    }

    @Test
    void shouldTokenizeMixedFieldsWithValues() {
        buffer.startToken((byte) 'A', true);
        buffer.continueTokenNibble((byte) 5);
        buffer.continueTokenNibble((byte) 0xa);
        buffer.continueTokenNibble((byte) 0xb);
        buffer.closeToken();
        buffer.startToken((byte) '+', false);
        buffer.closeToken();
        assertThat(buffer.getInternalData()).startsWith(2, 'A', 0x5, (byte) 0xab, 0, '+', 0);
    }
}
