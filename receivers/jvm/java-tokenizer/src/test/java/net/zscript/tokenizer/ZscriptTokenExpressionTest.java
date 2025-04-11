package net.zscript.tokenizer;

import java.util.NoSuchElementException;

import static java.nio.charset.StandardCharsets.ISO_8859_1;
import static net.zscript.util.ByteString.byteString;
import static net.zscript.util.ByteString.byteStringUtf8;
import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatThrownBy;

import org.junit.jupiter.api.Test;

import net.zscript.util.BlockIterator;
import net.zscript.util.ByteString;

class ZscriptTokenExpressionTest {
    TokenBuffer             buffer      = TokenRingBuffer.createBufferWithCapacity(256);
    Tokenizer               tokenizer   = new Tokenizer(buffer.getTokenWriter(), false);
    TokenBuffer.TokenReader tokenReader = buffer.getTokenReader();

    ZscriptTokenExpression zscriptExpr = new ZscriptTokenExpression(() -> tokenReader.tokenIterator());

    @Test
    public void shouldGetNumericFields() {
        tokenize("Z A6 B13 C123 Dffff E1234567890\n");
        assertThat(zscriptExpr.getFieldCount()).isEqualTo(6);

        assertThat(zscriptExpr.hasField('Z')).isTrue();
        assertThat(zscriptExpr.getFieldValue('Z')).hasValue(0x0);
        assertThat(zscriptExpr.getFieldValue('A')).hasValue(0x6);
        assertThat(zscriptExpr.getFieldValue('B')).hasValue(0x13);
        assertThat(zscriptExpr.getFieldValue('C')).hasValue(0x123);
        assertThat(zscriptExpr.getFieldValue('D')).hasValue(0xffff);
        assertThat(zscriptExpr.getFieldValue('E')).hasValue(0x7890);
        assertThat(zscriptExpr.hasField('F')).isFalse();
        assertThat(zscriptExpr.getFieldValue('F')).isNotPresent();

        assertThat(zscriptExpr.getFieldValueOrDefault('A', 23)).isEqualTo(0x6);
        assertThat(zscriptExpr.getFieldValueOrDefault('F', 23)).isEqualTo(23);
    }

    @Test
    public void shouldGet32bitNumericFields() {
        tokenize("Z A1 B12 C123 D1234 E123456 F87654321 G0123456789ab Hfffffffffffff1 \n");
        assertThat(zscriptExpr.getFieldCount()).isEqualTo(9);

        assertThat(zscriptExpr.hasField('Z')).isTrue();
        assertThat(zscriptExpr.getFieldValue32('Z')).hasValue(0);
        assertThat(zscriptExpr.getFieldValue32('A')).hasValue(1);
        assertThat(zscriptExpr.getFieldValue32('B')).hasValue(0x12L);
        assertThat(zscriptExpr.getFieldValue32('C')).hasValue(0x123L);
        assertThat(zscriptExpr.getFieldValue32('D')).hasValue(0x1234L);
        assertThat(zscriptExpr.getFieldValue32('E')).hasValue(0x123456L);
        assertThat(zscriptExpr.getFieldValue32('F')).hasValue(0x87654321L);
        assertThat(zscriptExpr.getFieldValue32('G')).hasValue(0x456789abL);
        assertThat(zscriptExpr.getFieldValue32('H')).hasValue(0xfffffff1L);
    }

    @Test
    public void shouldGetNumericFieldsWithLeadingZeroes() {
        tokenize("Z0012 A034 B000000 C0000abcd\n");
        assertThat(zscriptExpr.getFieldCount()).isEqualTo(4);

        assertThat(zscriptExpr.hasField('Z')).isTrue();
        assertThat(zscriptExpr.getFieldValue('Z')).hasValue(0x12);
        assertThat(zscriptExpr.getFieldValue('A')).hasValue(0x34);
        assertThat(zscriptExpr.getFieldValue('B')).hasValue(0);
        assertThat(zscriptExpr.getFieldValue('C')).hasValue(0xabcd);
    }

    // Note, this isn't really behaviour we care about much, but we should probably detect if the behaviour changes!
    @Test
    public void shouldFailGracefullyWhenTokenizationFailed() {
        tokenize("Z0012 A034 B00000 C123\n"); // odd nibble-length in long 'B' token
        assertThat(zscriptExpr.getFieldCount()).isEqualTo(2);
        assertThat(zscriptExpr.hasField('Z')).isTrue();
        assertThat(zscriptExpr.hasField('A')).isTrue();
        assertThat(zscriptExpr.hasField('B')).isFalse();
        assertThat(zscriptExpr.hasField('C')).isFalse();
    }

    @Test
    public void shouldGetAllFields() {
        tokenize("Z12 A34 BCffff +12\n");
        assertThat(zscriptExpr.fields()).hasSize(4)
                .extracting(f -> (char) f.getKey()).containsOnly('A', 'B', 'C', 'Z');

    }

    @Test
    public void shouldGetBytestringFields() {
        // This should create 3 string-field tokens
        tokenize("Z12 A34 B Cffff D\"Hello\" E202122 F\"World\" \n");
        assertThat(zscriptExpr.getFieldCount()).isEqualTo(7);

        checkField('A', 1, byteStringUtf8("4"), 0x34);
        checkField('B', 0, ByteString.EMPTY, 0);
        checkField('C', 2, byteString((byte) 0xff, (byte) 0xff), 0xffff);
        checkField('D', 5, byteStringUtf8("Hello"), 'l' * 256 + 'o');
        checkField('E', 3, byteString((byte) 0x20, (byte) 0x21, (byte) 0x22), 0x2122);
        checkField('F', 5, byteStringUtf8("World"), 'l' * 256 + 'd');

        assertThat(zscriptExpr.getFieldDataLength('D')).isEqualTo(5);
        assertThat(zscriptExpr.getFieldData('D').orElseThrow().toByteString().asString()).isEqualTo("Hello");

        BlockIterator iter = zscriptExpr.getFieldData('D').orElseThrow();
        assertThat(iter.next()).isEqualTo((byte) 'H');
        assertThat(iter.nextContiguous(7)).containsExactly("ello".getBytes(ISO_8859_1));
        assertThat(iter.hasNext()).isFalse();

        assertThatThrownBy(iter::next).isInstanceOf(NoSuchElementException.class);
        assertThatThrownBy(iter::nextContiguous).isInstanceOf(NoSuchElementException.class);
        assertThatThrownBy(() -> iter.nextContiguous(5)).isInstanceOf(NoSuchElementException.class);
    }

    private void checkField(char key, int len, ByteString data, int value) {
        assertThat(zscriptExpr.getFieldDataLength(key)).isEqualTo(len);
        assertThat(zscriptExpr.getFieldData(key).orElseThrow().toByteString()).isEqualTo(data);
        assertThat(zscriptExpr.getFieldValue(key).orElseThrow()).isEqualTo(value);
    }

    private void tokenize(String z) {
        for (byte c : z.getBytes(ISO_8859_1)) {
            tokenizer.accept(c);
        }
    }
}
