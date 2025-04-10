package net.zscript.tokenizer;

import javax.annotation.Nonnull;
import java.util.NoSuchElementException;

import net.zscript.util.BlockIterator;

/**
 * Defines the interface for the standard buffer for tokenizing Zscript.
 * <p/>
 * It is split into two parts, the TokenWriter, which provides all the operations relating to adding tokens to the buffer, and the TokenReader, which provides the parser-side
 * capabilities. The TokenReader thinks in terms of ReadTokens, which are defined within.
 */
public interface TokenBuffer {
    /**
     * Special token key to indicate that this segment is a continuation of the previous token due to its data size hitting maximum.
     *
     * <p>Fixme: this should be in a more "reserved" number space. We use 0x80 for address blocks, which is definitely a "user" space. This could be 0xdf ?
     */
    byte TOKEN_EXTENSION = (byte) 0x81;

    /**
     * Determines whether the supplied byte is a valid "marker" key (including sequence marker).
     *
     * @param key any byte
     * @return true if it's a marker; false otherwise
     */
    static boolean isMarker(byte key) {
        return (key & 0xe0) == 0xe0;
    }

    /**
     * Determines whether the supplied byte is a sequence-end "marker" key (newline, fatal error).
     *
     * @param key any byte
     * @return true if it's a sequence-end marker; false otherwise
     */
    static boolean isSequenceEndMarker(byte key) {
        return (key & 0xf0) == 0xf0;
    }

    @Nonnull
    TokenWriter getTokenWriter();

    /**
     * Defines the operations that a Tokenizer may perform to write to the TokenBuffer.
     */
    interface TokenWriter {
        /**
         * Access the writer&lt;-&gt;reader control flags.
         *
         * @return the buffer's flags
         */
        @Nonnull
        TokenBufferFlags getFlags();

        /**
         * Starts a new token with the specified key, along with whether nibble pairs are to be aggregated into bytes numerically. If a token is already being written, then it is
         * finished off first. The key must not be a marker - use {@link #writeMarker(byte)}.
         *
         * @param key the byte to use as the key
         * @throws IllegalArgumentException if key is a marker
         */
        void startToken(byte key);

        /**
         * Forces the current token to be finished (eg on close-quote), wrapping up any numeric nibbles and resetting the state flags.
         */
        void endToken();

        /**
         * Adds a new byte to the current token.
         *
         * @param b the byte to add
         * @throws IllegalStateException    if no token has been started, or we're in the middle of a nibble
         * @throws IllegalArgumentException if this is a numeric token, and we've exceeded the maximum size
         */
        void continueTokenByte(byte b);

        /**
         * Adds a new nibble to the current token.
         *
         * @param b the nibble to add, with value in range 0-0xf
         * @throws IllegalStateException    if no token has been started
         * @throws IllegalArgumentException if the nibble is out of range, or this is a numeric token and we've exceeded the maximum size
         */
        void continueTokenNibble(byte b);

        /**
         * Determines whether the last token has been ended correctly.
         *
         * @return true if complete; false if there's a token still being formed
         */
        boolean isTokenComplete();

        /**
         * Writes the specified Marker. If a token is already being written, then it is finished off first. Markers are used to indicate that a parsable sequence of tokens has been
         * written which may trigger some reading activity.
         *
         * @param key the marker's key, as per {@link TokenBuffer#isMarker(byte)}
         * @throws IllegalArgumentException if key is not a marker
         */
        void writeMarker(byte key);

        /**
         * Indicates a failure state to be communicated through the buffer to the Reader. This is the identical to writing a Marker, _except_ if there's a token already being
         * written and not complete, then it is wound back and its space reclaimed. The errorCode is expected to be a Sequence Marker, as per
         * {@link TokenBuffer#isSequenceEndMarker(byte)}.
         * <p/>
         * One particular errorCode is the Buffer OVERRUN condition.
         *
         * @param errorCode a marker key to signal an error
         * @throws IllegalArgumentException if key is not a marker
         */
        void fail(byte errorCode);

        /**
         * Gets the key of the current token.
         *
         * @return the key
         * @throws IllegalStateException if no token is being written
         */
        byte getCurrentWriteTokenKey();

        /**
         * Gets the current length of the current token.
         *
         * @return the length
         * @throws IllegalStateException if no token is being written
         */
        int getCurrentWriteTokenLength();

        /**
         * Determines whether the writer is in the middle of writing a nibble (ie there have been an odd number of nibbles so far).
         *
         * @return true if there is a nibble being constructed; false otherwise
         */
        boolean isInNibble();

        /**
         * Determines whether the buffer has the specified number of bytes available.
         *
         * @return true if there is space; false otherwise
         */
        boolean checkAvailableCapacity(int size);
    }

    @Nonnull
    TokenReader getTokenReader();

    /**
     * Defines all the operations that a parser should be able to perform to read from the TokenBuffer. The TokenReader (or its underlying buffer) maintains a "current read
     * position"; it can read the token at that position, create an iterator to read forward from that token, and "flush" the current token thus moving the current position forward
     * by one token.
     */
    interface TokenReader {
        /**
         * Access the writer&lt;-&gt;reader control flags.
         *
         * @return the buffer's flags
         */
        @Nonnull
        TokenBufferFlags getFlags();

        /**
         * Creates an iterator over all the readable tokens. Note: skips over extension segment tokens - they are (correctly) never returned during iteration.
         *
         * @return an iterator over ReadTokens available.
         */
        @Nonnull
        TokenBufferIterator tokenIterator();

        /**
         * Checks whether any tokens can be read.
         *
         * @return true iff a token is available for reading; false otherwise.
         */
        boolean hasReadToken();

        /**
         * Creates a ReadToken representing the current first readable token.
         *
         * @return a ReadToken representing the current first readable token
         * @throws NoSuchElementException if no token is available, see {@link #hasReadToken()}.
         */
        @Nonnull
        ReadToken getFirstReadToken();

        /**
         * Handy method equivalent to {@code getFirstReadToken().getKey()}
         *
         * @return the key of the first readable token
         * @throws NoSuchElementException if no token is available, see {@link #hasReadToken()}.
         */
        default byte getFirstKey() {
            return getFirstReadToken().getKey();
        }

        /**
         * Removes the first token from the readable space and moves to the next one. Has no effect if no token is available.
         */
        void flushFirstReadToken();

        /**
         * Represents a single Token in the token buffer.
         */
        interface ReadToken {

            /**
             * Determines the key associated with this token.
             *
             * @return the key
             */
            byte getKey();

            /**
             * Determines the amount of data associated with this token, including any (immediately) following extension segment tokens. If the Token is a marker, then returns
             * zero.
             *
             * @return the "real" size of this token, including following extension segments, or zero if token is a marker.
             */
            int getDataSize();

            /**
             * Determines whether this token contains data that could be read with a numeric read {@link #getData16()} or {@link #getData32()}. While other sizes may be supported,
             * only maxBytes = {2, 4} are guaranteed.
             *
             * @param maxBytes the size of the data type to read in bytes: 2 for uint-16, or 4 for uint-32.
             * @return true if numeric, false otherwise
             */
            boolean hasNumeric(int maxBytes);

            /**
             * Handy method equivalent to using {@link TokenBuffer#isMarker(byte)} on this ReadToken.
             *
             * @return true if this is a marker token (including sequence marker); false otherwise
             */
            default boolean isMarker() {
                return TokenBuffer.isMarker(getKey());
            }

            /**
             * Handy method equivalent to using {@link TokenBuffer#isSequenceEndMarker(byte)} on this ReadToken.
             *
             * @return true if this is a sequence-marker token; false otherwise
             */
            default boolean isSequenceEndMarker() {
                return TokenBuffer.isSequenceEndMarker(getKey());
            }

            /**
             * Creates an OptIterator over the completed tokens from this token's position forward, such that  calling next() will first supply this token.
             * <p/>
             * Extension tokens are never returned; they're accessed via the previous token that was extended.
             *
             * @return an iterator over available ReadTokens.
             */
            @Nonnull
            TokenBufferIterator tokenIterator();

            /**
             * Exposes this token's data as a stream of bytes or byte[] blocks. It manages aggregating across extension segments, and breaks caused by circularity in ring-buffers.
             *
             * @return an iterator that exposes the data either as a succession of individual bytes, or as byte[] blocks of contiguous bytes.
             * @throws IllegalStateException if this token is any kind of Marker - check first!
             */
            @Nonnull
            BlockIterator dataIterator();

            /**
             * Exposes the data as a single number (uint16). Check whether there are excess bytes (ie leading bytes that may be dropped) with
             * {@link #hasNumeric(int) hasNumeric(2)}.
             *
             * @return the value of the data, as a 2 byte number
             * @throws IllegalStateException if this token is any kind of Marker.
             */
            int getData16();

            /**
             * Exposes the data as a single number (uint32). Check whether there are excess bytes (ie leading bytes that may be dropped) with
             * {@link #hasNumeric(int) hasNumeric(4)}.
             *
             * @return the value of the data, as a 4 byte number
             * @throws IllegalStateException if this token is any kind of Marker.
             */
            long getData32();
        }
    }

}
