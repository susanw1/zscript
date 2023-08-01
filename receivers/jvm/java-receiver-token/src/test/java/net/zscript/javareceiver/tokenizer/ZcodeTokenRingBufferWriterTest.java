package net.zscript.javareceiver.tokenizer;

import static net.zscript.javareceiver.tokenizer.ZcodeTokenizer.ERROR_BUFFER_OVERRUN;
import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatThrownBy;

import org.junit.jupiter.api.Test;

import net.zscript.javareceiver.tokenizer.ZcodeTokenRingBuffer;
import net.zscript.javareceiver.tokenizer.ZcodeTokenBuffer.TokenWriter;

class ZcodeTokenRingBufferWriterTest {

    private static final int BUFSIZE = 10;

    private final ZcodeTokenRingBuffer buffer = ZcodeTokenRingBuffer.createBufferWithCapacity(BUFSIZE);
    private final TokenWriter          writer = buffer.getTokenWriter();

    /**
     * Validates the exposed buffer state.
     */
    private void verifyBufferState(boolean tokenComplete, int availableWrite) {
        assertThat(writer.isTokenComplete()).as("TokenComplete").isEqualTo(tokenComplete);
        assertThat(writer.checkAvailableCapacity(availableWrite)).as("AvailableWrite").isTrue();
        assertThat(writer.checkAvailableCapacity(availableWrite + 1)).as("AvailableWrite+1").isFalse();
    }

    private void verifyBufferState(boolean tokenComplete, int availableWrite, int currentTokenKey, boolean inNibble, int tokenLength) {
        verifyBufferState(tokenComplete, availableWrite);
        if (!writer.isTokenComplete()) {
            assertThat(writer.getCurrentWriteTokenKey()).as("CurrentWriteTokenKey").isEqualTo((byte) currentTokenKey);
            assertThat(writer.isInNibble()).as("InNibble").isEqualTo(inNibble);
            assertThat(writer.getCurrentWriteTokenLength()).as("CurrentWriteTokenLength").isEqualTo(tokenLength);
        }
    }

    @Test
    void shouldTokenizeNumericFieldWithNoValue() {
        writer.startToken((byte) 'A', true);
        assertThat(buffer.getInternalData()).startsWith('A', 0, 0);

        verifyBufferState(false, 7, 'A', false, 0);
        writer.endToken();
        verifyBufferState(true, 7);
    }

    @Test
    void shouldTokenizeNumericFieldWithNibbleValue() {
        writer.startToken((byte) 'A', true);
        writer.continueTokenNibble((byte) 5);

        verifyBufferState(false, 7, 'A', true, 1);
        writer.endToken();
        assertThat(buffer.getInternalData()).startsWith('A', 1, 5, 0);
        verifyBufferState(true, 6);
    }

    @Test
    void shouldTokenizeNumericFieldWithByteValue() {
        insertNumericToken('A', (byte) 234);
        assertThat(buffer.getInternalData()).startsWith('A', 1, (byte) 234, 0);
        verifyBufferState(false, 6, 'A', false, 1);
    }

    @Test
    void shouldTokenizeNumericFieldWith2ByteValue() {
        insertNumericToken('A', (byte) 234, (byte) 123);
        assertThat(buffer.getInternalData()).startsWith('A', 2, 234, 123, 0);
        verifyBufferState(false, 5, 'A', false, 2);
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
            verifyBufferState(false, 6, 'A', true, 1);
        }).isInstanceOf(IllegalStateException.class)
                .hasMessage("Incomplete nibble");

        writer.endToken();
        verifyBufferState(true, 6);
    }

    @Test
    void shouldTokenizeNumericFieldWith2NibbleValue() {
        insertNumericTokenNibbles('A', (byte) 5, (byte) 0xd);
        assertThat(buffer.getInternalData()).startsWith('A', 1, 0x5d, 0);
    }

    @Test
    void shouldHandleFailingMidNibble() {
        insertNumericTokenNibbles('C', (byte) 6);
        insertNumericTokenNibbles('A', (byte) 5);
        writer.fail(ERROR_BUFFER_OVERRUN);
        assertThat(buffer.getInternalData()).startsWith('C', 1, 6, ERROR_BUFFER_OVERRUN);
    }

    @Test
    void shouldHandleFailingMidToken() {
        insertNumericTokenNibbles('C', (byte) 6);
        insertNumericTokenNibbles('A', (byte) 5, (byte) 2);
        writer.fail(ERROR_BUFFER_OVERRUN);
        assertThat(buffer.getInternalData()).startsWith('C', 1, 6, ERROR_BUFFER_OVERRUN);
    }

    @Test
    void shouldTokenizeNumericFieldWith3NibbleValue() {
        insertNumericTokenNibbles('A', (byte) 5, (byte) 0xd, (byte) 0xa);
        verifyBufferState(false, 6, 'A', true, 2);
        writer.endToken();
        assertThat(buffer.getInternalData()).startsWith('A', 2, 0x5, 0xda, 0);
    }

    @Test
    void shouldTokenizeNumericFieldWith4NibbleValue() {
        insertNumericTokenNibbles('A', (byte) 5, (byte) 0xd, (byte) 0xa, (byte) 0x3);
        verifyBufferState(false, 5, 'A', false, 2);
        writer.endToken();
        assertThat(buffer.getInternalData()).startsWith('A', 2, 0x5d, 0xa3, 0);
    }

    @Test
    void shouldTokenizeNumericField5NibbleValue() {
        insertNumericTokenNibbles('A', (byte) 5, (byte) 0xd, (byte) 0xa, (byte) 0x3, (byte) 0xe);
        verifyBufferState(false, 5, 'A', true, 3);
        writer.endToken();
        assertThat(buffer.getInternalData()).startsWith('A', 3, 0x5, 0xda, 0x3e, 0);
    }

    @Test
    void shouldTokenize2NumericFields() {
        writer.startToken((byte) 'A', true);
        writer.endToken();
        writer.startToken((byte) 'B', true);
        verifyBufferState(false, 5, 'B', false, 0);
        writer.endToken();
        assertThat(buffer.getInternalData()).startsWith('A', 0, 'B', 0, 0);
        verifyBufferState(true, 5);
    }

    @Test
    void shouldTokenize2NumericFieldsWithValues() {
        writer.startToken((byte) 'A', true);
        writer.continueTokenNibble((byte) 5);
        writer.endToken();
        writer.startToken((byte) 'B', true);
        writer.endToken();
        assertThat(buffer.getInternalData()).startsWith('A', 1, 5, 'B', 0, 0);
        verifyBufferState(true, 4);
    }

    @Test
    void shouldTokenizeNonNumericField() {
        writer.startToken((byte) '+', false);
        verifyBufferState(false, 7, '+', false, 0);
        writer.endToken();
        assertThat(buffer.getInternalData()).startsWith('+', 0, 0);
        verifyBufferState(true, 7);
    }

    @Test
    void shouldTokenize2NonNumericFields() {
        writer.startToken((byte) '+', false);
        writer.endToken();
        writer.startToken((byte) '+', false);
        writer.endToken();
        assertThat(buffer.getInternalData()).startsWith('+', 0, '+', 0, 0);
    }

    @Test
    void shouldTokenize2NonNumericFieldsWithValues() {
        insertNonNumericTokenNibbles('+', (byte) 5, (byte) 0xa);
        writer.endToken();
        writer.startToken((byte) '+', false);
        writer.endToken();
        assertThat(buffer.getInternalData()).startsWith('+', 1, 0x5a, '+', 0, 0);
    }

    @Test
    void shouldTokenizeMixedFieldsWithOddNibbleValues() {
        insertNumericTokenNibbles('A', (byte) 5, (byte) 0xa, (byte) 0xb);
        verifyBufferState(false, 6, 'A', true, 2);
        writer.endToken();
        writer.startToken((byte) '+', false);
        writer.endToken();
        assertThat(buffer.getInternalData()).startsWith('A', 2, 0x5, 0xab, '+', 0, 0);
        verifyBufferState(true, 3);
    }

    @Test
    void shouldTokenizeMixedFieldsWithValues() {
        insertNumericTokenNibbles('A', (byte) 5, (byte) 0xa, (byte) 0xb);
        writer.endToken();
        writer.startToken((byte) '+', false);
        writer.endToken();
        assertThat(buffer.getInternalData()).startsWith('A', 2, 0x5, 0xab, '+', 0, 0);
    }

    @Test
    void shouldTokenizeNonNumericFieldWithOddNibbles() {
        insertNonNumericTokenNibbles('+', (byte) 0xa, (byte) 0xb, (byte) 0xc);
        writer.endToken();
        assertThat(buffer.getInternalData()).startsWith('+', 2, 0xab, 0xc0);
        verifyBufferState(true, 5);
    }

    @Test
    void shouldTokenizeContinuedBigField() {
        ZcodeTokenRingBuffer bigBuffer = ZcodeTokenRingBuffer.createBufferWithCapacity(300);
        TokenWriter          writer    = bigBuffer.getTokenWriter();

        writer.startToken((byte) '+', false);
        for (int i = 0; i < 258; i++) {
            writer.continueTokenNibble((byte) 4);
            writer.continueTokenNibble((byte) (i & 0xf));
        }
        writer.endToken();

        assertThat(bigBuffer.getInternalData()).startsWith('+', 255, 64, 65).containsSequence(77, 78, 129, 3, 79, 64, 65, 0);
    }

    @Test
    void shouldWriteBufferOverflowOnTokenKey() {
        insertByteToken(5);
        writer.endToken();
        verifyBufferState(true, 2);

        writer.startToken((byte) 'A', true);
        writer.fail(ERROR_BUFFER_OVERRUN);
        assertThat(buffer.getInternalData()).startsWith('+', 5, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, ERROR_BUFFER_OVERRUN, 0x0);
    }

    @Test
    void shouldWriteBufferOverflowOnTokenData() {
        insertByteToken(3);
        writer.endToken();
        verifyBufferState(true, 4);

        writer.startToken((byte) 'A', true);
        writer.continueTokenByte((byte) 0x32);
        writer.continueTokenByte((byte) 0x33);
        writer.fail(ERROR_BUFFER_OVERRUN);

        // Overrun indicator wrote over the 'A' token's key
        assertThat(buffer.getInternalData()).startsWith('+', 3, 0xa0, 0xa1, 0xa2, ERROR_BUFFER_OVERRUN, 2, 0x32, 0x33);
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

        // check first few... only works if this was the first thing in the buffer! Remove these checks soon...
        assertThat(buffer.getInternalData()).startsWith('+', count, (byte) 0xa0, (byte) 0xa1);
        verifyBufferState(false, BUFSIZE - 3 - count, '+', false, count);
    }
}
