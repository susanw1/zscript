package org.zcode.javareceiver.tokeniser;

import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.Test;

class ZcodeTokenBufferTest {

    private ZcodeTokenBuffer buffer = ZcodeTokenBuffer.createBufferWithCapacity(5);

    @Test
    void shouldTokenizeNumericFieldWithNoValue() {
        buffer.startField((byte) 'A', true);
        assertThat(buffer.getInternalData()).containsExactly(0, 'A', 0, 0, 0);
    }

    @Test
    void shouldTokenizeNumericFieldWithValue() {
        buffer.startField((byte) 'A', true);
        buffer.continueFieldNibble((byte) 5);
        buffer.closeField();
        assertThat(buffer.getInternalData()).containsExactly(1, 'A', 5, 0, 0);
    }

    @Test
    void shouldTokenizeNumericFieldWithByteValue() {
        buffer.startField((byte) 'A', true);
        buffer.continueFieldByte((byte) 234);
        assertThat(buffer.getInternalData()).containsExactly(1, 'A', (byte) 234, 0, 0);
    }

    @Test
    void shouldTokenizeNumericFieldWith2NibbleValue() {
        buffer.startField((byte) 'A', true);
        buffer.continueFieldNibble((byte) 5);
        buffer.continueFieldNibble((byte) 0xd);
        assertThat(buffer.getInternalData()).containsExactly(1, 'A', 0x5d, 0, 0);
    }

    @Test
    void shouldTokenizeNumericFieldWith3NibbleValue() {
        buffer.startField((byte) 'A', true);
        buffer.continueFieldNibble((byte) 5);
        buffer.continueFieldNibble((byte) 0xd);
        buffer.continueFieldNibble((byte) 0xa);
        buffer.closeField();
        assertThat(buffer.getInternalData()).containsExactly(2, 'A', 0x5, 0xda, 0);
    }

    @Test
    void shouldTokenizeNumericFieldWith4NibbleValue() {
        buffer.startField((byte) 'A', true);
        buffer.continueFieldNibble((byte) 5);
        buffer.continueFieldNibble((byte) 0xd);
        buffer.continueFieldNibble((byte) 0xa);
        buffer.continueFieldNibble((byte) 0x3);
        buffer.closeField();
        assertThat(buffer.getInternalData()).containsExactly(2, 'A', 0x5d, 0xa3, 0);
    }

    @Test
    void shouldTokenizeNumericField5NibbleValue() {
        buffer.startField((byte) 'A', true);
        buffer.continueFieldNibble((byte) 5);
        buffer.continueFieldNibble((byte) 0xd);
        buffer.continueFieldNibble((byte) 0xa);
        buffer.continueFieldNibble((byte) 0x3);
        buffer.continueFieldNibble((byte) 0xe);
        buffer.closeField();
        assertThat(buffer.getInternalData()).containsExactly(3, 'A', 0x5, 0xda, 0x3e, 0);
    }

    @Test
    void shouldTokenizeNumericField3NibbleValueWithLeadingZeroes() {
        buffer.startField((byte) 'A', true);
        buffer.continueFieldNibble((byte) 0);
        buffer.continueFieldNibble((byte) 0);
        buffer.continueFieldNibble((byte) 0xd);
        buffer.continueFieldNibble((byte) 0xa);
        buffer.continueFieldNibble((byte) 0x3);
        buffer.closeField();
        assertThat(buffer.getInternalData()).containsExactly(2, 'A', 0x5, 0xda, 0);
    }

}
