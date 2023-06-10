package org.zcode.javareceiver.tokenizer;

import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatThrownBy;
import static org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer.BUFFER_OVERRUN_ERROR;

import org.junit.jupiter.api.Disabled;
import org.junit.jupiter.api.Test;
import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer.TokenWriter;

class ZcodeTokenRingBufferTest {

    private static final int BUFSIZE = 10;

    private ZcodeTokenRingBuffer buffer = ZcodeTokenRingBuffer.createBufferWithCapacity(BUFSIZE);
    TokenWriter                  writer = buffer.getTokenWriter();

    /**
     * Validates the exposed buffer state.
     */
    private void verifyBufferState(boolean tokenComplete, int availableWrite) {
        assertThat(writer.isTokenComplete()).as("TokenComplete").isEqualTo(tokenComplete);
//        assertThat(writer.getAvailableWrite()).as("AvailableWrite").isEqualTo(availableWrite);
    }

    private void verifyBufferState(boolean tokenComplete, int availableWrite, int currentTokenKey, boolean inNibble, int tokenLength, int nibbleLength) {
        verifyBufferState(tokenComplete, availableWrite);
        if (!writer.isTokenComplete()) {
            assertThat(writer.getCurrentWriteTokenKey()).as("CurrentWriteTokenKey").isEqualTo(currentTokenKey);
            assertThat(writer.isInNibble()).as("InNibble").isEqualTo(inNibble);
            assertThat(writer.getCurrentWriteTokenLength()).as("CurrentWriteTokenLength").isEqualTo(tokenLength);
            assertThat(writer.getCurrentWriteTokenNibbleLength()).as("CurrentWriteTokenNibbleLength").isEqualTo(nibbleLength);
        }
    }

    @Test
    void shouldTokenizeNumericFieldWithNoValue() {
        writer.startToken((byte) 'A', true);
        assertThat(buffer.getInternalData()).startsWith(0, 'A', 0);

        verifyBufferState(false, 7, 'A', false, 0, 0);
        writer.endToken();
        verifyBufferState(true, 7);
    }

    @Test
    void shouldTokenizeNumericFieldWithNibbleValue() {
        writer.startToken((byte) 'A', true);
        writer.continueTokenNibble((byte) 5);

        verifyBufferState(false, 7, 'A', true, 1, 1);
        writer.endToken();
        assertThat(buffer.getInternalData()).startsWith(1, 'A', 5, 0);
        verifyBufferState(true, 6);
    }

    @Test
    void shouldTokenizeNumericFieldWithByteValue() {
        insertNumericToken('A', (byte) 234);
        assertThat(buffer.getInternalData()).startsWith(1, 'A', (byte) 234, 0);
        verifyBufferState(false, 6, 'A', false, 1, 2);
    }

    @Test
    void shouldTokenizeNumericFieldWith2ByteValue() {
        insertNumericToken('A', (byte) 234, (byte) 123);
        assertThat(buffer.getInternalData()).startsWith(2, 'A', 234, 123, 0);
        verifyBufferState(false, 5, 'A', false, 2, 4);
    }

    @Test
    void shouldFailToAcceptByteIfNoTokenStarted() {
        assertThatThrownBy(() -> {
            writer.continueTokenByte((byte) 0xa2);
        }).isInstanceOf(IllegalStateException.class)
                .hasMessage("Byte with missing field key");
        writer.endToken();
        verifyBufferState(true, 9);
    }

    @Test
    void shouldFailToAcceptNibbleIfNoTokenStarted() {
        assertThatThrownBy(() -> {
            writer.continueTokenNibble((byte) 3);
        }).isInstanceOf(IllegalStateException.class)
                .hasMessage("Digit with missing field key");

        writer.endToken();
        verifyBufferState(true, 9);
    }

    @Test
    void shouldFailToTokenizeNumericFieldWithOddNibbleAndByteValue() {
        assertThatThrownBy(() -> {
            writer.startToken((byte) 'A', true);
            writer.continueTokenNibble((byte) 3);
            writer.continueTokenByte((byte) 123);
            verifyBufferState(false, 6, 'A', true, 1, 1);
        }).isInstanceOf(IllegalStateException.class)
                .hasMessage("Incomplete nibble");

        writer.endToken();
        verifyBufferState(true, 6);
    }

    @Test
    void shouldTokenizeNumericFieldWith2NibbleValue() {
        insertNumericTokenNibbles('A', (byte) 5, (byte) 0xd);
        assertThat(buffer.getInternalData()).startsWith(1, 'A', 0x5d, 0);
    }

    @Test
    void shouldTokenizeNumericFieldWith3NibbleValue() {
        insertNumericTokenNibbles('A', (byte) 5, (byte) 0xd, (byte) 0xa);
        verifyBufferState(false, 6, 'A', true, 2, 3);
        writer.endToken();
        assertThat(buffer.getInternalData()).startsWith(2, 'A', 0x5, 0xda, 0);
    }

    @Test
    void shouldTokenizeNumericFieldWith4NibbleValue() {
        insertNumericTokenNibbles('A', (byte) 5, (byte) 0xd, (byte) 0xa, (byte) 0x3);
        verifyBufferState(false, 5, 'A', false, 2, 4);
        writer.endToken();
        assertThat(buffer.getInternalData()).startsWith(2, 'A', 0x5d, 0xa3, 0);
    }

    @Test
    void shouldTokenizeNumericField5NibbleValue() {
        insertNumericTokenNibbles('A', (byte) 5, (byte) 0xd, (byte) 0xa, (byte) 0x3, (byte) 0xe);
        verifyBufferState(false, 5, 'A', true, 3, 5);
        writer.endToken();
        assertThat(buffer.getInternalData()).startsWith(3, 'A', 0x5, 0xda, 0x3e, 0);
    }

    @Test
    void shouldTokenize2NumericFields() {
        writer.startToken((byte) 'A', true);
        writer.endToken();
        writer.startToken((byte) 'B', true);
        verifyBufferState(false, 5, 'B', false, 0, 0);
        writer.endToken();
        assertThat(buffer.getInternalData()).startsWith(0, 'A', 0, 'B', 0);
        verifyBufferState(true, 5);
    }

    @Test
    void shouldTokenize2NumericFieldsWithValues() {
        writer.startToken((byte) 'A', true);
        writer.continueTokenNibble((byte) 5);
        writer.endToken();
        writer.startToken((byte) 'B', true);
        writer.endToken();
        assertThat(buffer.getInternalData()).startsWith(1, 'A', 5, 0, 'B', 0);
        verifyBufferState(true, 4);
    }

    @Test
    void shouldTokenizeNonNumericField() {
        writer.startToken((byte) '+', false);
        verifyBufferState(false, 7, '+', false, 0, 0);
        writer.endToken();
        assertThat(buffer.getInternalData()).startsWith(0, '+', 0);
        verifyBufferState(true, 7);
    }

    @Test
    void shouldTokenize2NonNumericFields() {
        writer.startToken((byte) '+', false);
        writer.endToken();
        writer.startToken((byte) '+', false);
        writer.endToken();
        assertThat(buffer.getInternalData()).startsWith(0, '+', 0, '+', 0);
    }

    @Test
    void shouldTokenize2NonNumericFieldsWithValues() {
        insertNonNumericTokenNibbles('+', (byte) 5, (byte) 0xa);
        writer.endToken();
        writer.startToken((byte) '+', false);
        writer.endToken();
        assertThat(buffer.getInternalData()).startsWith(1, '+', 0x5a, 0, '+', 0);
    }

    @Test
    void shouldTokenizeMixedFieldsWithOddNibbleValues() {
        insertNumericTokenNibbles('A', (byte) 5, (byte) 0xa, (byte) 0xb);
        verifyBufferState(false, 6, 'A', true, 2, 3);
        writer.endToken();
        writer.startToken((byte) '+', false);
        writer.endToken();
        assertThat(buffer.getInternalData()).startsWith(2, 'A', 0x5, 0xab, 0, '+', 0);
        verifyBufferState(true, 3);
    }

    @Test
    void shouldTokenizeMixedFieldsWithValues() {
        insertNumericTokenNibbles('A', (byte) 5, (byte) 0xa, (byte) 0xb);
        writer.endToken();
        writer.startToken((byte) '+', false);
        writer.endToken();
        assertThat(buffer.getInternalData()).startsWith(2, 'A', 0x5, 0xab, 0, '+', 0);
    }

    @Test
    void shouldTokenizeNonNumericFieldWithOddNibbles() {
        insertNonNumericTokenNibbles('+', (byte) 0xa, (byte) 0xb, (byte) 0xc);
        writer.endToken();
        assertThat(buffer.getInternalData()).startsWith(2, '+', 0xab, 0xc0);
        verifyBufferState(true, 5);
    }

    @Test
    void shouldSkipToNextToken() {
        insertByteToken(5);
        writer.endToken();
        buffer.skipToNextReadToken();
        verifyBufferState(true, BUFSIZE - 1);

        insertNumericToken('X', (byte) 0xb1, (byte) 0xb2);
        assertThat(buffer.getInternalData()).startsWith(0xb2, '+', 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 2, 'X', 0xb1);
        verifyBufferState(false, BUFSIZE - 5, 'X', false, 2, 4);
        writer.endToken();
        verifyBufferState(true, BUFSIZE - 5);

        insertNumericToken('Y', (byte) 0xc1);
        assertThat(buffer.getInternalData()).startsWith(0xb2, 1, 'Y', 0xc1, 0xa2, 0xa3, 0xa4, 2, 'X', 0xb1);
        verifyBufferState(false, BUFSIZE - 8, 'Y', false, 1, 2);

        writer.endToken();

        assertThat(buffer.skipToNextReadToken()).isNotNull();
        verifyBufferState(true, BUFSIZE - 4);

        // this should hit the write pointer
        assertThat(buffer.skipToNextReadToken()).isNull();
        verifyBufferState(true, BUFSIZE - 1);
    }

    @Test
    @Disabled("Buffer overrun changes are upon us!")
    void shouldWriteBufferOverflowOnTokenKey() {
        insertByteToken(5);
        writer.endToken();
        verifyBufferState(true, 2);

        writer.startToken((byte) 'A', true);
        assertThat(buffer.getInternalData()).startsWith(5, '+', 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0, BUFFER_OVERRUN_ERROR, 0x0);
    }

    @Test
    @Disabled("Buffer overrun changes are upon us!")
    void shouldWriteBufferOverflowOnTokenData() {
        insertByteToken(3);
        writer.endToken();
        verifyBufferState(true, 4);

        writer.startToken((byte) 'A', true);
        writer.continueTokenByte((byte) 0x32);
        writer.continueTokenByte((byte) 0x33);

        assertThat(buffer.getInternalData()).startsWith(3, '+', 0xa0, 0xa1, 0xa2, 0, BUFFER_OVERRUN_ERROR, 0x32);
    }

    private void insertNumericToken(char key, byte... data) {
        writer.startToken((byte) key, true);
        for (byte b : data) {
            writer.continueTokenByte(b);
        }
    }

    private void insertNumericTokenNibbles(char key, byte... nibbles) {
        insertTokenNibbles(key, true, nibbles);
    }

    private void insertNonNumericTokenNibbles(char key, byte... nibbles) {
        insertTokenNibbles(key, false, nibbles);
    }

    private void insertTokenNibbles(char key, boolean numeric, byte... nibbles) {
        writer.startToken((byte) key, numeric);
        for (byte b : nibbles) {
            writer.continueTokenNibble(b);
        }
    }

    private void insertByteToken(int count) {
        writer.startToken((byte) '+', false);

        for (int i = 0; i < count; i++) {
            writer.continueTokenByte((byte) (0xa0 + i));
        }

        // check first few...
        assertThat(buffer.getInternalData()).startsWith(count, '+', (byte) 0xa0, (byte) 0xa1);
        verifyBufferState(false, BUFSIZE - 3 - count, '+', false, count, count * 2);
    }
}
