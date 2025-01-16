package net.zscript.tokenizer;

import java.util.NoSuchElementException;

import static java.nio.charset.StandardCharsets.ISO_8859_1;
import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatThrownBy;

import org.junit.jupiter.api.Test;

import net.zscript.util.BlockIterator;

class ZscriptTokenExpressionTest {
    TokenBuffer             buffer      = TokenRingBuffer.createBufferWithCapacity(256);
    Tokenizer               tokenizer   = new Tokenizer(buffer.getTokenWriter(), false);
    TokenBuffer.TokenReader tokenReader = buffer.getTokenReader();

    ZscriptTokenExpression zscriptExpr = new ZscriptTokenExpression(() -> tokenReader.tokenIterator());

    @Test
    public void shouldGetFields() {
        tokenize("Z12 A34 BCffff\n");
        assertThat(zscriptExpr.hasField('Z')).isTrue();
        assertThat(zscriptExpr.getField('Z')).hasValue(0x12);
        assertThat(zscriptExpr.getField('A')).hasValue(0x34);
        assertThat(zscriptExpr.getField('B')).hasValue(0);
        assertThat(zscriptExpr.getField('C')).hasValue(0xffff);
        assertThat(zscriptExpr.hasField('D')).isFalse();
        assertThat(zscriptExpr.getField('D')).isNotPresent();

        assertThat(zscriptExpr.getField('A', 23)).isEqualTo(0x34);
        assertThat(zscriptExpr.getField('D', 23)).isEqualTo(23);

        assertThat(zscriptExpr.getFieldCount()).isEqualTo(4);
        assertThat(zscriptExpr.hasBigField()).isFalse();
    }

    @Test
    public void shouldGetAllFields() {
        tokenize("Z12 A34 BCffff +12\n");
        assertThat(zscriptExpr.fields()).hasSize(4)
                .extracting(f -> (char) f.getKey()).containsOnly('A', 'B', 'C', 'Z');

    }

    @Test
    public void shouldGetBigFields() {
        tokenize("Z12 A34 BCffff \"Hello\" \n");
        assertThat(zscriptExpr.getFieldCount()).isEqualTo(4);

        assertThat(zscriptExpr.hasBigField()).isTrue();
        assertThat(zscriptExpr.getBigFieldSize()).isEqualTo(5);
        assertThat(zscriptExpr.getBigFieldData()).containsExactly("Hello".getBytes(ISO_8859_1));
    }

    @Test
    public void shouldGetMultipleBigFields() {
        // This should create 3 big-field tokens
        tokenize("Z12 A34 BCffff \"Hello\" +20 \"World\" \n");
        assertThat(zscriptExpr.getFieldCount()).isEqualTo(4);

        assertThat(zscriptExpr.hasBigField()).isTrue();
        assertThat(zscriptExpr.getBigFieldSize()).isEqualTo(11);
        assertThat(zscriptExpr.getBigFieldData()).containsExactly("Hello World".getBytes(ISO_8859_1));

        BlockIterator iter = zscriptExpr.bigFieldDataIterator();
        assertThat(iter.next()).isEqualTo((byte) 'H');
        assertThat(iter.nextContiguous(7)).containsExactly("ello".getBytes(ISO_8859_1));
        assertThat(iter.nextContiguous()).containsExactly(" ".getBytes(ISO_8859_1));
        assertThat(iter.hasNext()).isTrue();
        assertThat(iter.nextContiguous(7)).containsExactly("World".getBytes(ISO_8859_1));
        assertThat(iter.hasNext()).isFalse();

        assertThatThrownBy(iter::next).isInstanceOf(NoSuchElementException.class);
        assertThatThrownBy(iter::nextContiguous).isInstanceOf(NoSuchElementException.class);
        assertThatThrownBy(() -> iter.nextContiguous(5)).isInstanceOf(NoSuchElementException.class);
    }

    private void tokenize(String z) {
        for (byte c : z.getBytes(ISO_8859_1)) {
            tokenizer.accept(c);
        }
    }
}
