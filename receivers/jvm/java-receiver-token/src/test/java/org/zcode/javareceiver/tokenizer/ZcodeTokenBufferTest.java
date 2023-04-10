package org.zcode.javareceiver.tokenizer;

import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatThrownBy;

import org.junit.jupiter.api.Test;

class ZcodeTokenBufferTest {

    private ZcodeTokenRingBuffer buffer = ZcodeTokenRingBuffer.createBufferWithCapacity(10);

    /**
     * Validates the exposed buffer state.
     */
    private void verifyBufferState(boolean tokenOpen, int currentTokenKey, int availableWrite, boolean inNibble, int tokenLength, int nibbleLength) {
        assertThat(buffer.isTokenOpen()).as("TokenOpen").isEqualTo(tokenOpen);
        assertThat(buffer.getCurrentWriteTokenKey()).as("CurrentWriteTokenKey").isEqualTo(currentTokenKey);
        assertThat(buffer.getAvailableWrite()).as("AvailableWrite").isEqualTo(availableWrite);
        assertThat(buffer.isInNibble()).as("InNibble").isEqualTo(inNibble);
        assertThat(buffer.getCurrentWriteTokenLength()).as("CurrentWriteTokenLength").isEqualTo(tokenLength);
        assertThat(buffer.getCurrentWriteTokenNibbleLength()).as("CurrentWriteTokenNibbleLength").isEqualTo(nibbleLength);
    }

    @Test
    void shouldTokenizeNumericFieldWithNoValue() {
        buffer.startToken((byte) 'A', true);
        assertThat(buffer.getInternalData()).startsWith(0, 'A', 0);

        verifyBufferState(true, 'A', 7, false, 0, 0);
        buffer.closeToken();
        verifyBufferState(false, 0, 7, false, 0, 0);
    }

    @Test
    void shouldTokenizeNumericFieldWithNibbleValue() {
        buffer.startToken((byte) 'A', true);
        buffer.continueTokenNibble((byte) 5);

        verifyBufferState(true, 'A', 7, true, 1, 1);
        buffer.closeToken();
        assertThat(buffer.getInternalData()).startsWith(1, 'A', 5, 0);
        verifyBufferState(false, 0, 6, false, 0, 0);
    }

    @Test
    void shouldTokenizeNumericFieldWithByteValue() {
        buffer.startToken((byte) 'A', true);
        buffer.continueTokenByte((byte) 234);
        assertThat(buffer.getInternalData()).startsWith(1, 'A', (byte) 234, 0);
        verifyBufferState(true, 'A', 6, false, 1, 2);
    }

    @Test
    void shouldTokenizeNumericFieldWith2ByteValue() {
        buffer.startToken((byte) 'A', true);
        buffer.continueTokenByte((byte) 234);
        buffer.continueTokenByte((byte) 123);
        assertThat(buffer.getInternalData()).startsWith(2, 'A', (byte) 234, 123, 0);
        verifyBufferState(true, 'A', 5, false, 2, 4);
    }

    @Test
    void shouldFailToTokenizeNumericFieldWithOddNibbleAndByteValue() {
        assertThatThrownBy(() -> {
            buffer.startToken((byte) 'A', true);
            buffer.continueTokenNibble((byte) 3);
            buffer.continueTokenByte((byte) 123);
            verifyBufferState(true, 'A', 6, true, 1, 1);
        }).isInstanceOf(IllegalStateException.class)
                .hasMessage("Incomplete nibble");

        buffer.closeToken();
        verifyBufferState(false, 0, 6, false, 0, 0);
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
        verifyBufferState(true, 'A', 6, true, 2, 3);
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
        verifyBufferState(true, 'A', 5, false, 2, 4);
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
        verifyBufferState(true, 'A', 5, true, 3, 5);
        buffer.closeToken();
        assertThat(buffer.getInternalData()).startsWith(3, 'A', 0x5, 0xda, 0x3e, 0);
    }

    @Test
    void shouldTokenize2NumericFields() {
        buffer.startToken((byte) 'A', true);
        buffer.closeToken();
        buffer.startToken((byte) 'B', true);
        verifyBufferState(true, 'B', 5, false, 0, 0);
        buffer.closeToken();
        assertThat(buffer.getInternalData()).startsWith(0, 'A', 0, 'B', 0);
        verifyBufferState(false, 0, 5, false, 0, 0);
    }

    @Test
    void shouldTokenize2NumericFieldsWithValues() {
        buffer.startToken((byte) 'A', true);
        buffer.continueTokenNibble((byte) 5);
        buffer.closeToken();
        buffer.startToken((byte) 'B', true);
        buffer.closeToken();
        assertThat(buffer.getInternalData()).startsWith(1, 'A', 5, 0, 'B', 0);
        verifyBufferState(false, 0, 4, false, 0, 0);
    }

    @Test
    void shouldTokenizeNonNumericField() {
        buffer.startToken((byte) '+', false);
        verifyBufferState(true, '+', 7, false, 0, 0);
        buffer.closeToken();
        assertThat(buffer.getInternalData()).startsWith(0, '+', 0);
        verifyBufferState(false, 0, 7, false, 0, 0);
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
        verifyBufferState(true, 'A', 6, true, 2, 3);
        buffer.closeToken();
        buffer.startToken((byte) '+', false);
        buffer.closeToken();
        assertThat(buffer.getInternalData()).startsWith(2, 'A', 0x5, (byte) 0xab, 0, '+', 0);
        verifyBufferState(false, 0, 3, false, 0, 0);
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

    @Test
    void shouldTokenizeNonNumericFieldWithOddNibbles() {
        buffer.startToken((byte) '+', false);
        buffer.continueTokenNibble((byte) 0xa);
        buffer.continueTokenNibble((byte) 0xb);
        buffer.continueTokenNibble((byte) 0xc);
        buffer.closeToken();
        assertThat(buffer.getInternalData()).startsWith(2, '+', (byte) 0xab, (byte) 0xc0);
        verifyBufferState(false, 0, 5, false, 0, 0);
    }
}
