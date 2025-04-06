package net.zscript.tokenizer;

import java.io.ByteArrayOutputStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;
import java.util.NoSuchElementException;
import java.util.Optional;
import java.util.Random;

import static org.assertj.core.api.Assertions.assertThat;

import org.assertj.core.util.Streams;
import org.junit.jupiter.api.Test;

import net.zscript.tokenizer.TokenBuffer.TokenReader;
import net.zscript.tokenizer.TokenBuffer.TokenReader.ReadToken;
import net.zscript.tokenizer.TokenBuffer.TokenWriter;
import net.zscript.util.BlockIterator;
import net.zscript.util.OptIterator;

class TokenRingBufferReaderTest {

    private static final int  BUFSIZE = 1000;
    private static final long SEED    = 2001;

    private final TokenRingBuffer        buffer = TokenRingBuffer.createBufferWithCapacity(BUFSIZE);
    private final TokenReader            reader = buffer.getTokenReader();
    private final List<TokenExpectation> tokens = new ArrayList<>();

    private static class TokenExpectation {
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
            writer.startToken(exp.key);
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
        writer.startToken(exp.key);
        for (int j = 0; j < byteCount; j++) {
            writer.continueTokenByte(data[j]);
        }
        tokens.add(exp);
        writer.endToken();
    }

    private void testTokenSimilarity(ReadToken found, TokenExpectation expected) {
        assertThat(found.isMarker()).isSameAs(expected.isMarker);
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
            ByteArrayOutputStream st = new ByteArrayOutputStream();
            Streams.stream(found.dataIterator()).forEach(st::write);
            assertThat(st.toByteArray()).containsExactly(expected.data);
        }
        if (expected.isExtended) {
            assertThat(found.getDataSize()).isEqualTo(expected.data.length);
        }
    }

    private void testIteratorCorrectness(OptIterator<ReadToken> iterator, int offset) {
        int i = offset;

        for (Optional<ReadToken> opt = iterator.next(); opt.isPresent(); i++, opt = iterator.next()) {
            try {
                testTokenSimilarity(opt.get(), tokens.get(i));
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
        OptIterator<ReadToken>     iterator     = reader.tokenIterator();
        for (Optional<ReadToken> opt = iterator.next(); opt.isPresent(); opt = iterator.next()) {
            ReadToken token = opt.get();
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
        OptIterator<ReadToken>     iterator     = reader.tokenIterator();
        for (Optional<ReadToken> opt = iterator.next(); opt.isPresent(); opt = iterator.next()) {
            ReadToken token = opt.get();
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
        OptIterator<ReadToken>     iterator     = reader.tokenIterator();
        for (Optional<ReadToken> opt = iterator.next(); opt.isPresent(); opt = iterator.next()) {
            ReadToken token = opt.get();
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
        testIteratorCorrectness(reader.tokenIterator(), 0);
    }

    @Test
    void flushFirstShouldFlush() {
        Random r = new Random(SEED);
        writeNormalTokens(r, 10, 2);
        writeMarkerTokens(r, 50);
        writeNormalTokens(r, 10, 2);
        writeExtendedToken(r, 400);
        writeNormalTokens(r, 10, 2);
        reader.flushFirstReadToken();
        testIteratorCorrectness(reader.tokenIterator(), 1);
    }

    @Test
    void iteratorsStartedFromTokensShouldMatch() {
        Random r = new Random(SEED);
        writeNormalTokens(r, 10, 2);
        writeMarkerTokens(r, 50);
        writeNormalTokens(r, 10, 2);
        writeExtendedToken(r, 400);
        writeNormalTokens(r, 10, 2);
        int i = 0;

        OptIterator<ReadToken> iterator = reader.tokenIterator();
        for (Optional<ReadToken> opt = iterator.next(); opt.isPresent(); opt = iterator.next()) {
            ReadToken token = opt.get();
            testIteratorCorrectness(token.tokenIterator(), i++);
        }
    }

    @Test
    void shouldIterateTokenDataThroughExtensions() {
        Random r = new Random(SEED);
        writeExtendedToken(r, 800);
        testIteratorCorrectness(reader.tokenIterator(), 0);
    }

    @Test
    void shouldIterateTokenDataThroughLoopingBuffer() {
        Random r = new Random(SEED);
        writeExtendedToken(r, 800);
        TokenBufferIterator it = reader.tokenIterator();
        it.next();
        it.flushBuffer();
        writeExtendedToken(r, 800);
        testIteratorCorrectness(reader.tokenIterator(), 1);
    }

    @Test
    void shouldIterateTokenDataInContiguousChunks() {
        Random r = new Random(SEED);
        writeExtendedToken(r, 800);
        BlockIterator data = reader.tokenIterator().next().get().dataIterator();
        int           i    = 0;
        while (data.hasNext()) {
            byte[] nextCont = data.nextContiguous();
            assertThat(nextCont).containsExactly(Arrays.copyOfRange(tokens.get(0).data, i, Math.min(i + 255, 800)));
            i += 255;
        }
    }

    @Test
    void shouldIterateTokenDataInLimitedContiguousChunks() {
        Random r = new Random(SEED);
        writeExtendedToken(r, 800);
        BlockIterator data = reader.tokenIterator().next().get().dataIterator();
        int           i    = 0;
        while (data.hasNext()) {
            assertThat(data.nextContiguous(200)).containsExactly(Arrays.copyOfRange(tokens.get(0).data, i, Math.min(i + 200, 800)));
            if (data.hasNext()) {
                assertThat(data.nextContiguous(200)).containsExactly(Arrays.copyOfRange(tokens.get(0).data, i + 200, Math.min(i + 255, 800)));
                i += 255;
            }
        }
    }

    @Test
    void shouldIterateTokenDataInLimitedContiguousChunksAroundEnd() {
        int    initialLength = 700;
        Random r             = new Random(SEED);
        writeExtendedToken(r, initialLength);
        TokenBufferIterator it = reader.tokenIterator();
        it.next();
        it.flushBuffer();
        writeExtendedToken(r, 800);
        BlockIterator data = reader.tokenIterator().next().get().dataIterator();
        int           i    = 0;
        while (data.hasNext()) {
            int offset = 200;
            if (i + initialLength + (initialLength + 254) / 255 * 2 + i / 255 * 2 < BUFSIZE
                    && i + 200 + initialLength + (initialLength + 254) / 255 * 2 + i / 255 * 2 + 2 > BUFSIZE) {
                offset = BUFSIZE - (initialLength + i + (initialLength + 254) / 255 * 2 + i / 255 * 2 + 2);
            }
            assertThat(data.nextContiguous(200)).containsExactly(Arrays.copyOfRange(tokens.get(1).data, i, Math.min(i + offset, 800)));
            if (data.hasNext()) {
                if (i + offset > 800) {
                    throw new AssertionError("Iterator didn't terminate when it should have");
                }
                if (offset + 200 < 255) {
                    assertThat(data.nextContiguous(200)).containsExactly(Arrays.copyOfRange(tokens.get(1).data, i + offset, Math.min(i + offset + 200, 800)));
                    offset += 200;
                }
                assertThat(data.nextContiguous(200)).containsExactly(Arrays.copyOfRange(tokens.get(1).data, i + offset, Math.min(i + 255, 800)));
                i += 255;
            }
        }
    }
}
