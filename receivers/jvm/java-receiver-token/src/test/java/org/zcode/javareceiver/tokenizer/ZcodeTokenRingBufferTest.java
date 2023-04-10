package org.zcode.javareceiver.tokenizer;

import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatThrownBy;
import static org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer.BUFFER_OVERRUN_ERROR;

import org.junit.jupiter.api.Test;

class ZcodeTokenRingBufferTest {

    private static final int BUFSIZE = 10;

    private ZcodeTokenRingBuffer buffer = ZcodeTokenRingBuffer.createBufferWithCapacity(BUFSIZE);

    /**
     * Validates the exposed buffer state.
     */
    private void verifyBufferState(boolean tokenComplete, int availableWrite) {
        assertThat(buffer.isTokenComplete()).as("TokenComplete").isEqualTo(tokenComplete);
        assertThat(buffer.getAvailableWrite()).as("AvailableWrite").isEqualTo(availableWrite);
    }

    private void verifyBufferState(boolean tokenComplete, int availableWrite, int currentTokenKey, boolean inNibble, int tokenLength, int nibbleLength) {
        verifyBufferState(tokenComplete, availableWrite);
        if (!buffer.isTokenComplete()) {
            assertThat(buffer.getCurrentWriteTokenKey()).as("CurrentWriteTokenKey").isEqualTo(currentTokenKey);
            assertThat(buffer.isInNibble()).as("InNibble").isEqualTo(inNibble);
            assertThat(buffer.getCurrentWriteTokenLength()).as("CurrentWriteTokenLength").isEqualTo(tokenLength);
            assertThat(buffer.getCurrentWriteTokenNibbleLength()).as("CurrentWriteTokenNibbleLength").isEqualTo(nibbleLength);
        }
    }

    @Test
    void shouldTokenizeNumericFieldWithNoValue() {
        buffer.startToken((byte) 'A', true);
        assertThat(buffer.getInternalData()).startsWith(0, 'A', 0);

        verifyBufferState(false, 7, 'A', false, 0, 0);
        buffer.endToken();
        verifyBufferState(true, 7);
    }

    @Test
    void shouldTokenizeNumericFieldWithNibbleValue() {
        buffer.startToken((byte) 'A', true);
        buffer.continueTokenNibble((byte) 5);

        verifyBufferState(false, 7, 'A', true, 1, 1);
        buffer.endToken();
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
            buffer.continueTokenByte((byte) 0xa2);
        }).isInstanceOf(IllegalStateException.class)
                .hasMessage("Byte with missing field key");
        buffer.endToken();
        verifyBufferState(true, 9);
    }

    @Test
    void shouldFailToAcceptNibbleIfNoTokenStarted() {
        assertThatThrownBy(() -> {
            buffer.continueTokenNibble((byte) 3);
        }).isInstanceOf(IllegalStateException.class)
                .hasMessage("Digit with missing field key");

        buffer.endToken();
        verifyBufferState(true, 9);
    }

    @Test
    void shouldFailToTokenizeNumericFieldWithOddNibbleAndByteValue() {
        assertThatThrownBy(() -> {
            buffer.startToken((byte) 'A', true);
            buffer.continueTokenNibble((byte) 3);
            buffer.continueTokenByte((byte) 123);
            verifyBufferState(false, 6, 'A', true, 1, 1);
        }).isInstanceOf(IllegalStateException.class)
                .hasMessage("Incomplete nibble");

        buffer.endToken();
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
        buffer.endToken();
        assertThat(buffer.getInternalData()).startsWith(2, 'A', 0x5, 0xda, 0);
    }

    @Test
    void shouldTokenizeNumericFieldWith4NibbleValue() {
        insertNumericTokenNibbles('A', (byte) 5, (byte) 0xd, (byte) 0xa, (byte) 0x3);
        verifyBufferState(false, 5, 'A', false, 2, 4);
        buffer.endToken();
        assertThat(buffer.getInternalData()).startsWith(2, 'A', 0x5d, 0xa3, 0);
    }

    @Test
    void shouldTokenizeNumericField5NibbleValue() {
        insertNumericTokenNibbles('A', (byte) 5, (byte) 0xd, (byte) 0xa, (byte) 0x3, (byte) 0xe);
        verifyBufferState(false, 5, 'A', true, 3, 5);
        buffer.endToken();
        assertThat(buffer.getInternalData()).startsWith(3, 'A', 0x5, 0xda, 0x3e, 0);
    }

    @Test
    void shouldTokenize2NumericFields() {
        buffer.startToken((byte) 'A', true);
        buffer.endToken();
        buffer.startToken((byte) 'B', true);
        verifyBufferState(false, 5, 'B', false, 0, 0);
        buffer.endToken();
        assertThat(buffer.getInternalData()).startsWith(0, 'A', 0, 'B', 0);
        verifyBufferState(true, 5);
    }

    @Test
    void shouldTokenize2NumericFieldsWithValues() {
        buffer.startToken((byte) 'A', true);
        buffer.continueTokenNibble((byte) 5);
        buffer.endToken();
        buffer.startToken((byte) 'B', true);
        buffer.endToken();
        assertThat(buffer.getInternalData()).startsWith(1, 'A', 5, 0, 'B', 0);
        verifyBufferState(true, 4);
    }

    @Test
    void shouldTokenizeNonNumericField() {
        buffer.startToken((byte) '+', false);
        verifyBufferState(false, 7, '+', false, 0, 0);
        buffer.endToken();
        assertThat(buffer.getInternalData()).startsWith(0, '+', 0);
        verifyBufferState(true, 7);
    }

    @Test
    void shouldTokenize2NonNumericFields() {
        buffer.startToken((byte) '+', false);
        buffer.endToken();
        buffer.startToken((byte) '+', false);
        buffer.endToken();
        assertThat(buffer.getInternalData()).startsWith(0, '+', 0, '+', 0);
    }

    @Test
    void shouldTokenize2NonNumericFieldsWithValues() {
        insertNonNumericTokenNibbles('+', (byte) 5, (byte) 0xa);
        buffer.endToken();
        buffer.startToken((byte) '+', false);
        buffer.endToken();
        assertThat(buffer.getInternalData()).startsWith(1, '+', 0x5a, 0, '+', 0);
    }

    @Test
    void shouldTokenizeMixedFieldsWithOddNibbleValues() {
        insertNumericTokenNibbles('A', (byte) 5, (byte) 0xa, (byte) 0xb);
        verifyBufferState(false, 6, 'A', true, 2, 3);
        buffer.endToken();
        buffer.startToken((byte) '+', false);
        buffer.endToken();
        assertThat(buffer.getInternalData()).startsWith(2, 'A', 0x5, 0xab, 0, '+', 0);
        verifyBufferState(true, 3);
    }

    @Test
    void shouldTokenizeMixedFieldsWithValues() {
        insertNumericTokenNibbles('A', (byte) 5, (byte) 0xa, (byte) 0xb);
        buffer.endToken();
        buffer.startToken((byte) '+', false);
        buffer.endToken();
        assertThat(buffer.getInternalData()).startsWith(2, 'A', 0x5, 0xab, 0, '+', 0);
    }

    @Test
    void shouldTokenizeNonNumericFieldWithOddNibbles() {
        insertNonNumericTokenNibbles('+', (byte) 0xa, (byte) 0xb, (byte) 0xc);
        buffer.endToken();
        assertThat(buffer.getInternalData()).startsWith(2, '+', 0xab, 0xc0);
        verifyBufferState(true, 5);
    }

    @Test
    void shouldSkipToNextToken() {
        insertByteToken(5);
        buffer.endToken();
        buffer.skipToNextReadToken();
        verifyBufferState(true, BUFSIZE - 1);

        insertNumericToken('X', (byte) 0xb1, (byte) 0xb2);
        assertThat(buffer.getInternalData()).startsWith(0xb2, '+', 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 2, 'X', 0xb1);
        verifyBufferState(false, BUFSIZE - 5, 'X', false, 2, 4);
        buffer.endToken();
        verifyBufferState(true, BUFSIZE - 5);

        insertNumericToken('Y', (byte) 0xc1);
        assertThat(buffer.getInternalData()).startsWith(0xb2, 1, 'Y', 0xc1, 0xa2, 0xa3, 0xa4, 2, 'X', 0xb1);
        verifyBufferState(false, BUFSIZE - 8, 'Y', false, 1, 2);

        buffer.endToken();

        assertThat(buffer.skipToNextReadToken()).isNotNull();
        verifyBufferState(true, BUFSIZE - 4);

        // this should hit the write pointer
        assertThat(buffer.skipToNextReadToken()).isNull();
        verifyBufferState(true, BUFSIZE - 1);
    }

    @Test
    void shouldWriteBufferOverflowOnTokenKey() {
        insertByteToken(5);
        buffer.endToken();
        verifyBufferState(true, 2);

        assertThat(buffer.startToken((byte) 'A', true)).as("key").isFalse();
        assertThat(buffer.getInternalData()).startsWith(5, '+', 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0, BUFFER_OVERRUN_ERROR, 0x0);
    }

    @Test
    void shouldWriteBufferOverflowOnTokenData() {
        insertByteToken(2);
        buffer.endToken();
        verifyBufferState(true, 5);

        assertThat(buffer.startToken((byte) 'A', true)).as("key").isTrue();
        assertThat(buffer.continueTokenByte((byte) 0x32)).as("data=0x32").isTrue();
        assertThat(buffer.continueTokenByte((byte) 0x33)).as("data=0x33").isFalse();

        assertThat(buffer.getInternalData()).startsWith(2, '+', 0xa0, 0xa1, 1, 'A', 0x32, 0, BUFFER_OVERRUN_ERROR, 0x0);
    }

    private void insertNumericToken(char key, byte... data) {
        boolean ok = buffer.startToken((byte) key, true);
        assertThat(ok).as("key").isTrue();
        for (byte b : data) {
            ok = buffer.continueTokenByte(b);
            assertThat(ok).as("data=" + b).isTrue();
        }
    }

    private void insertNumericTokenNibbles(char key, byte... nibbles) {
        insertTokenNibbles(key, true, nibbles);
    }

    private void insertNonNumericTokenNibbles(char key, byte... nibbles) {
        insertTokenNibbles(key, false, nibbles);
    }

    private void insertTokenNibbles(char key, boolean numeric, byte... nibbles) {
        boolean ok = buffer.startToken((byte) key, numeric);
        assertThat(ok).as("key").isTrue();
        for (byte b : nibbles) {
            ok = buffer.continueTokenNibble(b);
            assertThat(ok).as("data=" + b).isTrue();
        }
    }

    private void insertByteToken(int count) {
        boolean ok = buffer.startToken((byte) '+', false);
        assertThat(ok).as("key").isTrue();

        for (int i = 0; i < count; i++) {
            ok = buffer.continueTokenByte((byte) (0xa0 + i));
            assertThat(ok).as("data #" + i).isTrue();
        }

        // check first few...
        assertThat(buffer.getInternalData()).startsWith(count, '+', (byte) 0xa0, (byte) 0xa1);
        verifyBufferState(false, BUFSIZE - 3 - count, '+', false, count, count * 2);
    }
}
