package org.zcode.javareceiver.tokenizer;

import static org.assertj.core.api.Assertions.assertThat;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.NoSuchElementException;
import java.util.Random;

import org.junit.jupiter.api.Test;
import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer.TokenReader;
import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer.TokenReader.ReadToken;
import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer.TokenWriter;

class ZcodeTokenRingBufferReaderTest {

    private static final int  BUFSIZE = 1000;
    private static final long SEED    = 2000;

    private ZcodeTokenRingBuffer         buffer = ZcodeTokenRingBuffer.createBufferWithCapacity(BUFSIZE);
    TokenReader                          reader = buffer.getTokenReader();
    private final List<TokenExpectation> tokens = new ArrayList<>();

    private class TokenExpectation {
        boolean isMarker;
        boolean isExtended;
        byte    key;
        byte[]  data;

        public TokenExpectation(boolean isMarker, boolean isExtended, byte key, byte[] data) {
            this.isMarker = isMarker;
            this.isExtended = isExtended;
            this.key = key;
            this.data = data;
        }

    }

    private TokenExpectation marker(byte key) {
        return new TokenExpectation(true, false, key, new byte[0]);
    }

    private TokenExpectation extended(byte key, byte[] data) {
        return new TokenExpectation(false, true, key, data);
    }

    private TokenExpectation normal(byte key, byte[] data) {
        return new TokenExpectation(false, false, key, data);
    }

    private void writeNormalTokens(Random r, int count, int byteCount) {
        TokenWriter writer = buffer.getTokenWriter();
        for (int i = 0; i < count; i++) {
            byte[] data = new byte[byteCount];
            r.nextBytes(data);
            TokenExpectation exp = normal((byte) r.nextInt(0x80), data);
            writer.startToken(exp.key, true);
            for (int j = 0; j < byteCount; j++) {
                writer.continueTokenByte(data[j]);
            }
            tokens.add(exp);
        }
        writer.endToken();
    }

    private void writeMarkerTokens(Random r, int count) {
        TokenWriter writer = buffer.getTokenWriter();

        for (int i = 0; i < count; i++) {
            TokenExpectation exp = marker((byte) (0xe0 + r.nextInt(0x20)));
            writer.writeMarker(exp.key);
            tokens.add(exp);
        }
        writer.endToken();
    }

    private void writeExtendedToken(Random r, int byteCount) {
        TokenWriter writer = buffer.getTokenWriter();

        byte[] data = new byte[byteCount];
        r.nextBytes(data);
        TokenExpectation exp = extended((byte) r.nextInt(0x80), data);
        writer.startToken(exp.key, false);
        for (int j = 0; j < byteCount; j++) {
            writer.continueTokenByte(data[j]);
        }
        tokens.add(exp);
        writer.endToken();
    }

    private void testTokenSimilarity(ReadToken found, TokenExpectation expected) {
        assertThat(found.isMarker() == expected.isMarker);
        assertThat(found.getKey()).isEqualTo(expected.key);
        if (!found.isMarker()) {
            if (expected.data.length <= 4) {
                long exp32 = 0;
                for (byte b : expected.data) {
                    exp32 <<= 8;
                    exp32 += Byte.toUnsignedInt(b);
                }
                assertThat(found.getData32()).isEqualTo(exp32);
                if (exp32 < 0x10000) {
                    assertThat(found.getData16()).isEqualTo(exp32);
                }
            }
            // FIXME: there *must* be a more elegant way of doing this, but I don't know enough about bashing Iterator<Byte> and byte[] to make them into two things I can compare.
            Iterator<Byte> data = found.data();
            for (int i = 0; i < expected.data.length; i++) {
                assertThat(data.next()).isEqualTo(expected.data[i]);
            }
        }
    }

    private void testIteratorCorrectness(Iterator<ReadToken> iterator, int offset) {
        int i;
        for (i = offset; iterator.hasNext(); i++) {
            try {
                testTokenSimilarity(iterator.next(), tokens.get(i));
            } catch (ArrayIndexOutOfBoundsException e) {
                throw new AssertionError("Expected fewer tokens than found from iterator");
            }
        }
        assertThat(i).withFailMessage("Expected more tokens than found from iterator").isEqualTo(tokens.size());
    }

    @Test
    void shouldIterateNormalTokensCorrectly() {
        Random r = new Random(SEED);
        writeNormalTokens(r, 100, 2);
        assertThat(reader.hasReadToken()).isTrue();
        assertThat(reader.getFirstReadToken().getKey()).isEqualTo(tokens.get(0).key);
        Iterator<TokenExpectation> expectations = tokens.iterator();
        for (Iterator<ReadToken> iterator = reader.getTokens(); iterator.hasNext();) {
            ReadToken token = iterator.next();
            try {
                TokenExpectation expectation = expectations.next();
                assertThat(token.getKey()).isEqualTo(expectation.key);
            } catch (NoSuchElementException e) {
                throw new AssertionError("Expected fewer tokens than found");
            }
        }
        assertThat(expectations.hasNext()).withFailMessage("Expected more tokens than found").isFalse();
    }

    @Test
    void shouldIterateMarkerTokensCorrectly() {
        Random r = new Random(SEED);
        writeMarkerTokens(r, 800);
        assertThat(reader.hasReadToken()).isTrue();
        assertThat(reader.getFirstReadToken().getKey()).isEqualTo(tokens.get(0).key);
        Iterator<TokenExpectation> expectations = tokens.iterator();
        for (Iterator<ReadToken> iterator = reader.getTokens(); iterator.hasNext();) {
            ReadToken token = iterator.next();
            try {
                TokenExpectation expectation = expectations.next();
                assertThat(token.getKey()).isEqualTo(expectation.key);
            } catch (NoSuchElementException e) {
                throw new AssertionError("Expected fewer tokens than found");
            }
        }
        assertThat(expectations.hasNext()).withFailMessage("Expected more tokens than found").isFalse();
    }

    @Test
    void shouldIterateMixedTokensCorrectly() {
        Random r = new Random(SEED);
        writeNormalTokens(r, 10, 2);
        writeMarkerTokens(r, 50);
        writeNormalTokens(r, 10, 2);
        writeExtendedToken(r, 400);
        writeNormalTokens(r, 10, 2);
        assertThat(reader.hasReadToken()).isTrue();
        assertThat(reader.getFirstReadToken().getKey()).isEqualTo(tokens.get(0).key);
        Iterator<TokenExpectation> expectations = tokens.iterator();
        for (Iterator<ReadToken> iterator = reader.getTokens(); iterator.hasNext();) {
            ReadToken token = iterator.next();
            try {
                TokenExpectation expectation = expectations.next();
                assertThat(token.getKey()).isEqualTo(expectation.key);
            } catch (NoSuchElementException e) {
                throw new AssertionError("Expected fewer tokens than found");
            }
        }
        assertThat(expectations.hasNext()).withFailMessage("Expected more tokens than found").isFalse();
    }

    @Test
    void readTokensShouldMatchWrittenTokens() {
        Random r = new Random(SEED);
        writeNormalTokens(r, 10, 2);
        writeMarkerTokens(r, 50);
        writeNormalTokens(r, 10, 2);
        writeExtendedToken(r, 400);
        writeNormalTokens(r, 10, 2);
        testIteratorCorrectness(reader.getTokens(), 0);
        int i = 0;
        for (Iterator<ReadToken> iterator = reader.getTokens(); iterator.hasNext();) {
            ReadToken token = iterator.next();
            testIteratorCorrectness(reader.getTokens(), 0);
        }
    }

    @Test
    void iteratorsStartedFromTokensShouldMatch() {
        Random r = new Random(SEED);
        writeNormalTokens(r, 10, 2);
        writeMarkerTokens(r, 50);
        writeNormalTokens(r, 10, 2);
        writeExtendedToken(r, 400);
        writeNormalTokens(r, 10, 2);
        int i = 1;
        for (Iterator<ReadToken> iterator = reader.getTokens(); iterator.hasNext();) {
            ReadToken token = iterator.next();
            testIteratorCorrectness(token.getNextTokens(), i++);
        }
    }

    @Test
    void shouldIterateTokenDataThroughExtensions() {
        Random r = new Random(SEED);
        writeExtendedToken(r, 800);
        testIteratorCorrectness(reader.getTokens(), 0);
    }

    @Test
    void shouldIterateTokenDataThroughLoopingBuffer() {
        Random r = new Random(SEED);
        writeExtendedToken(r, 800);
        BufferIterator<ReadToken> it = reader.getTokens();
        it.next();
        it.flushBuffer();
        writeExtendedToken(r, 800);
        testIteratorCorrectness(reader.getTokens(), 1);
    }

}
