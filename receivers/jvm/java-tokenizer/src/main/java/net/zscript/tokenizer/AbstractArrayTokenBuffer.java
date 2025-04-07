package net.zscript.tokenizer;

import javax.annotation.Nonnull;
import java.util.Arrays;
import java.util.NoSuchElementException;
import java.util.Optional;

import static java.lang.Byte.toUnsignedInt;
import static java.lang.String.format;
import static net.zscript.tokenizer.TokenBuffer.isMarker;
import static net.zscript.tokenizer.TokenBuffer.isSequenceEndMarker;

import net.zscript.util.BlockIterator;

/**
 * Array based implementation of a TokenBuffer - the tokens making up incoming command or response sequences are encoded and accessed here. Rules are:
 * <ol>
 * <li>There is a writable area, owned by a TokenWriter, in the space <i>writeStart &lt;= i &lt; readStart</i>.</li>
 * <li>There is a readable area, owned by a TokenIterator, in the space <i>readStart &lt;= i &lt; writeStart</i>.</li>
 * <li>A token is written as &gt;=2 bytes at <i>writeStart</i>: <code>key | datalen | [data]</code> - so tokens can be iterated by adding (datalen+2) to an existing token start.</li>
 * <li>A marker is written as 1 byte at <i>writeStart</i>, indicating a dataless key - markers are identified as a key with top 3 bits set (eg from 0xe0-0xff).</li>
 * <li>Tokens may exceed datalen of 255 using additional new token with special key <i>TOKEN_EXTENSION</i></li>
 * </ol>
 */
public abstract class AbstractArrayTokenBuffer implements TokenBuffer {
    private static final byte MAX_TOKEN_DATA_LENGTH         = (byte) 255;
    private static final byte MAX_NUMERIC_TOKEN_DATA_LENGTH = (byte) 2;

    /** the ring-buffer's data array */
    private byte[] data;

    private final TokenWriter tokenWriter;

    private final TokenReader tokenReader;

    private final TokenBufferFlags flags;

    /** index of first byte owned by reader, writable space ends just before it */
    private int readStart;
    /** index of first byte owned by TokenWriter, readable space ends just before it */
    private int writeStart;

    // construct via static factories
    protected AbstractArrayTokenBuffer(final int sz) {
        this(new byte[sz], 0);
    }

    // construct via static factories
    protected AbstractArrayTokenBuffer(final byte[] preloaded, int writeStart) {
        this.data = preloaded;
        this.tokenWriter = new ArrayTokenBufferWriter();
        this.tokenReader = new ArrayTokenBufferReader();
        this.flags = new TokenBufferFlags();
        this.writeStart = writeStart;
        this.readStart = 0;
    }

    @Nonnull
    protected final byte[] getInternalData() {
        return data;
    }

    public final int getDataSize() {
        return data.length;
    }

    /**
     * Reveals the start of writing space (first index above the last valid readable byte). Careful, this is tied up with the inner workings of the buffer.
     *
     * @return start of write space
     */
    protected final int getWriteStart() {
        return writeStart;
    }

    protected final void importInternalDataFrom(final AbstractArrayTokenBuffer other) {
        this.data = Arrays.copyOf(other.data, other.writeStart);
        this.writeStart = other.writeStart;
    }

    protected final void extendData(int extra) {
        data = Arrays.copyOf(data, data.length + extra);
    }

    @Nonnull
    @Override
    public TokenWriter getTokenWriter() {
        return tokenWriter;
    }

    @Nonnull
    @Override
    public TokenReader getTokenReader() {
        return tokenReader;
    }

    protected abstract boolean checkAvailableCapacityFrom(final int writeCursor, final int size);

    protected final int getReadStart() {
        return readStart;
    }

    private class ArrayTokenBufferWriter implements TokenWriter {
        /** index of data-length field of most recent token segment (esp required for long multi-segment tokens) */
        private int writeLastLen = 0;
        /** the current write index into data array */
        private int writeCursor  = 0;

        /** Toggled with each consecutive nibble.  True indicates we've been given an odd-number. */
        private boolean inNibble = false;

        /** Indicates that we are writing an extended token (in order to help limit odd-nibble numerics) */
        private boolean extended;

        @Nonnull
        @Override
        public TokenBufferFlags getFlags() {
            return flags;
        }

        @Override
        public void startToken(final byte key) {
            endToken();
            writeNewTokenStart(key);
        }

        @Override
        public void continueTokenNibble(final byte nibble) {
            if (nibble < 0 || nibble > 0xf) {
                throw new IllegalArgumentException("Nibble value out of range");
            }
            if (isTokenComplete()) {
                throw new IllegalStateException("Digit with missing field key");
            }

            if (inNibble) {
                data[writeCursor] |= nibble;
                moveCursor();
            } else {
                if (data[writeLastLen] == MAX_TOKEN_DATA_LENGTH) {
                    writeNewTokenStart(TOKEN_EXTENSION);
                    extended = true;
                }
                data[writeCursor] = (byte) (nibble << 4);
                data[writeLastLen]++;
            }
            inNibble = !inNibble;
        }

        @Override
        public void continueTokenByte(final byte b) {
            if (inNibble) {
                throw new IllegalStateException("Incomplete nibble");
            }
            if (isTokenComplete()) {
                throw new IllegalStateException("Byte with missing field key");
            }

            if (data[writeLastLen] == MAX_TOKEN_DATA_LENGTH) {
                writeNewTokenStart(TOKEN_EXTENSION);
                extended = true;
            }
            data[writeCursor] = b;
            moveCursor();
            data[writeLastLen]++;
        }

        @Override
        public void endToken() {
            // Allow nibble shift only for short, odd-nibble cases; otherwise, nibbles must be even!
            if (inNibble) {
                // note: need to check extended, as current length is only for this segment
                if (extended || getCurrentWriteTokenLength() > MAX_NUMERIC_TOKEN_DATA_LENGTH) {
                    throw new IllegalStateException("hex-pair value too long for odd length");
                }
                // if odd nibble count, then shuffle nibbles through token's data to ensure "right-aligned", eg 4ad0 really means 04ad
                byte hold = 0;
                int  pos  = offset(writeStart, 1);
                do {
                    pos = offset(pos, 1);
                    final byte tmp = (byte) (data[pos] & 0xF);
                    data[pos] = (byte) (hold | (data[pos] >> 4) & 0xF);
                    hold = (byte) (tmp << 4);
                } while (pos != writeCursor);
                moveCursor();
            }

            writeStart = writeCursor;
            inNibble = false;
            extended = false;
        }

        @Override
        public boolean checkAvailableCapacity(final int size) {
            return checkAvailableCapacityFrom(writeCursor, size);
        }

        @Override
        public boolean isInNibble() {
            return inNibble;
        }

        @Override
        public byte getCurrentWriteTokenKey() {
            if (isTokenComplete()) {
                throw new IllegalStateException("no token being written");
            }
            return data[writeStart];
        }

        @Override
        public int getCurrentWriteTokenLength() {
            if (isTokenComplete()) {
                throw new IllegalStateException("no token being written");
            }
            return toUnsignedInt(data[offset(writeStart, 1)]);
        }

        @Override
        public boolean isTokenComplete() {
            return writeStart == writeCursor;
        }

        @Override
        public void writeMarker(final byte key) {
            if (!isMarker(key)) {
                throw new IllegalArgumentException("invalid marker [key=0x" + Integer.toHexString(key) + "]");
            }
            endToken();
            data[writeCursor] = key;
            moveCursor();
            endToken();

            if (isSequenceEndMarker(key)) {
                flags.setSeqMarkerWritten();
            }
            flags.setMarkerWritten();
        }

        private void writeNewTokenStart(final byte key) {
            if (isMarker(key)) {
                throw new IllegalArgumentException("invalid token [key=0x" + Integer.toHexString(key) + "]");
            }
            data[writeCursor] = key;
            moveCursor();
            data[writeCursor] = 0;
            writeLastLen = writeCursor;
            moveCursor();
        }

        @Override
        public void fail(final byte errorCode) {
            if (!isTokenComplete()) {
                // reset current token back to writeStart
                writeCursor = writeStart;
                inNibble = false;
            }
            writeMarker(errorCode);
        }

        private void moveCursor() {
            final int nextCursor = offset(writeCursor, 1);
            if (nextCursor == readStart) {
                // this should never happen - someone should have made sure there was space
                throw new IllegalStateException("Out of buffer - should have reserved more");
            }
            writeCursor = nextCursor;
        }
    }

    /**
     * Utility method that finds the correct offset from the supplied index, handling the off-the-end condition in a subclass-specific way (eg wrap, or expand the buffer, etc).
     *
     * @param index  location in the underlying ring-buffer
     * @param offset an offset to add, >= 0
     * @return a new index, at the requested offset
     */
    final int offset(final int index, final int offset) {
        if (index < 0 || offset < 0 || offset >= data.length) {
            throw new IllegalArgumentException(format("Unexpected values [index=%d, offset=%d]", index, offset));
        }
        final int newOffset = index + offset;
        return (newOffset < data.length) ? newOffset : offsetOnOverflow(newOffset);
    }

    /**
     * Hook method that allows an action to be taken when an offset lookup goes off the end of the data buffer, and determines the actual offset value to use.
     *
     * @param overflowedOffset the offset we were trying to access, required to be data.length &lt;= overflowedOffset &lt; 2*data.length (ie off the end, but not by multiple
     *                         rounds)
     * @return a new offset that should be used instead (eg loop around if this is a ring buffer, or just the overflowed offset if we've expanded the buffer)
     */
    protected abstract int offsetOnOverflow(final int overflowedOffset);

    /**
     * Utility for determining whether the specified index is in the current readable area, defined as readStart <= index < writeStart (but accounting for this being a
     * ringbuffer).
     * <p/>
     * Everything included in the readable area is committed and immutable.
     *
     * @param index the index to check
     * @return true if in readable area; false otherwise
     */
    boolean isInReadableArea(final int index) {
        return writeStart >= readStart && (readStart <= index && index < writeStart)
                || writeStart < readStart && (index < writeStart || index >= readStart);
    }

    private class ArrayTokenBufferReader implements TokenReader {
        /**
         * Token Iterator which iterates all the available tokens that have been (completely) written to the buffer. It's aware of the extension tokens and excludes them from the
         * iteration.
         *
         * @see #tokenIterator()
         */
        private class ArrayBufferTokenIterator implements TokenBufferIterator {
            private int index;

            /**
             * Creates an iterator on this buffer's tokens. Note: skips over extension segment tokens - they are never returned during iteration.
             *
             * @param startIndex the index of the first token to be returned
             */
            private ArrayBufferTokenIterator(final int startIndex) {
                this.index = startIndex;
            }

            @Nonnull
            @Override
            public Optional<ReadToken> next() {
                if (index == writeStart) {
                    return Optional.empty();
                }

                final int initialIndex = index;
                if (isMarker(data[index])) {
                    index = offset(index, 1);
                } else {
                    do {
                        final int tokenDataLength = toUnsignedInt(data[offset(index, 1)]);
                        index = offset(index, tokenDataLength + 2);
                    } while (data[index] == TOKEN_EXTENSION);
                }

                return Optional.of(new ArrayBufferToken(initialIndex));
            }

            /**
             * {@inheritDoc}
             */
            @Override
            public void flushBuffer() {
                readStart = index;
            }
        }

        /**
         * {@inheritDoc}
         */
        @Nonnull
        @Override
        public TokenBufferFlags getFlags() {
            return flags;
        }

        /**
         * {@inheritDoc}
         */
        @Nonnull
        @Override
        public TokenBufferIterator tokenIterator() {
            return new ArrayBufferTokenIterator(readStart);
        }

        /**
         * {@inheritDoc}
         */
        @Override
        public boolean hasReadToken() {
            return readStart != writeStart;
        }

        /**
         * {@inheritDoc}
         */
        @Nonnull
        @Override
        public ReadToken getFirstReadToken() {
            if (readStart == writeStart) {
                throw new NoSuchElementException("No read tokens available");
            }
            return new ArrayBufferToken(readStart);
        }

        /**
         * {@inheritDoc}
         */
        @Override
        public void flushFirstReadToken() {
            final TokenBufferIterator it = tokenIterator();
            it.next();
            it.flushBuffer();
        }

        /**
         * A view of a token starting at a known index. In principle, all the operations around navigating tokens as stored in the buffer are encapsulated here.
         * <p/>
         * Fundamentally, the ReadToken is nothing more than an index into the ArrayBuffer: it's really just a uint16 or uint8. If you re-implement this in C, then that's what
         * you'd have. But at a higher level, it has the following rules:
         * <ul>
         * <li>you can only create a Token from the valid start of the readable space, or from another token.</li>
         * <li>you can't go off the end of the readable space.</li>
         * </ul>
         *
         * Any operation which returns a ReadToken should fail when these rules are broken.
         */
        private class ArrayBufferToken implements ReadToken {
            private final int index;

            private ArrayBufferToken(final int index) {
                if (index < 0 || index >= data.length) {
                    throw new IllegalArgumentException("index out of data bounds [index=" + index + "]");
                }
                if (!isInReadableArea(index)) {
                    throw new IllegalArgumentException("index is not in readable area [index=" + index + "]");
                }
                if (data[index] == TOKEN_EXTENSION) {
                    throw new IllegalArgumentException("Tokens cannot begin with extension key");
                }
                this.index = index;
            }

            /**
             * {@inheritDoc}
             */
            @Nonnull
            @Override
            public TokenBufferIterator tokenIterator() {
                return new ArrayBufferTokenIterator(index);
            }

            @Nonnull
            @Override
            public BlockIterator dataIterator() {
                if (isMarker()) {
                    throw new IllegalStateException("Cannot get data from marker token");
                }
                return new BlockIterator() {
                    private int itIndex      = offset(index, 2);
                    private int segRemaining = getSegmentDataSize();

                    @Override
                    public boolean hasNext() {
                        // Copes with empty tokens which still have extensions... probably not a valid thing anyway, but still, it didn't cost anything.
                        while (segRemaining == 0) {
                            if ((itIndex == writeStart) || (data[itIndex] != TOKEN_EXTENSION)) {
                                return false;
                            }
                            segRemaining = toUnsignedInt(data[offset(itIndex, 1)]);
                            itIndex = offset(itIndex, 2);
                        }
                        return true;
                    }

                    @Nonnull
                    @Override
                    public Byte next() {
                        if (!hasNext()) {
                            throw new NoSuchElementException();
                        }
                        final byte res = data[itIndex];
                        itIndex = offset(itIndex, 1);
                        segRemaining--;
                        return res;
                    }

                    @Nonnull
                    @Override
                    public byte[] nextContiguous() {
                        return nextContiguous(data.length);
                    }

                    @Nonnull
                    @Override
                    public byte[] nextContiguous(final int maxLength) {
                        if (maxLength < 0) {
                            throw new IllegalArgumentException("maxLength out of range [maxLength=" + maxLength + "]");
                        }
                        if (!hasNext()) {
                            throw new NoSuchElementException();
                        }
                        final int lengthToCopy = Math.min(Math.min(maxLength, segRemaining), data.length - itIndex);

                        final byte[] dataArray = Arrays.copyOfRange(data, itIndex, itIndex + lengthToCopy);

                        itIndex = offset(itIndex, lengthToCopy);
                        segRemaining -= lengthToCopy;
                        return dataArray;
                    }
                };
            }

            @Override
            public long getData32() {
                return getDataN(4) & 0xffff_ffffL;
            }

            @Override
            public int getData16() {
                return getDataN(2);
            }

            /** @return value read from data, or just last n - use {@link #hasNumeric(int)} to pre-verify. */
            private int getDataN(int n) {
                if (isMarker()) {
                    throw new IllegalStateException("Cannot get data from marker token");
                }
                final int sz    = getSegmentDataSize();
                int       value = 0;
                for (int i = (sz > n ? sz - n : 0); i < sz; i++) {
                    value <<= 8;
                    value += toUnsignedInt(data[offset(index, i + 2)]);
                }
                return value;
            }

            /**
             * {@inheritDoc}
             */
            @Override
            public byte getKey() {
                return data[index];
            }

            /**
             * {@inheritDoc}
             */
            @Override
            public int getDataSize() {
                if (isMarker()) {
                    return 0;
                }
                int totalSz = 0;
                int index   = this.index;

                do {
                    final int segSz = toUnsignedInt(data[offset(index, 1)]);
                    totalSz += segSz;
                    index = offset(index, segSz + 2);
                } while (index != writeStart && data[index] == TOKEN_EXTENSION);

                return totalSz;
            }

            @Override
            public boolean hasNumeric(int maxBytes) {
                return !TokenBuffer.isMarker(data[index]) && toUnsignedInt(data[offset(index, 1)]) <= maxBytes;
            }

            /**
             * Determines the size of the data portion of just this token (not including any extensions).
             *
             * @return this Token's data size - 0 to 255
             */
            private int getSegmentDataSize() {
                return TokenBuffer.isMarker(data[index]) ? 0 : toUnsignedInt(data[offset(index, 1)]);
            }

            @Override
            public String toString() {
                if (TokenBuffer.isMarker(getKey())) {
                    return "Token(Marker:0x" + Integer.toHexString(toUnsignedInt(getKey())) + ")";
                }
                return "Token(key='" + (char) getKey() + "'(0x" + Integer.toHexString(toUnsignedInt(getKey())) + "), len=" + getDataSize() + ")";
            }
        }
    }
}
