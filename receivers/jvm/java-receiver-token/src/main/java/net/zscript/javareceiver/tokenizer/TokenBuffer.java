package net.zscript.javareceiver.tokenizer;

import java.util.NoSuchElementException;

public interface TokenBuffer {
    /**
     * Special token key to indicate that this segment is a continuation of the previous token due to its data size hitting maximum.
     */
    public static final byte TOKEN_EXTENSION = (byte) 0x81;

    /**
     * Determines whether the supplied byte is a valid "marker" key.
     *
     * @param key any byte
     * @return true if it's a marker; false otherwise
     */
    static boolean isMarker(byte key) {
        return (key & 0xe0) == 0xe0;
    }

    /**
     * Determines whether the supplied byte is a sequence-end "marker" key (newline, fatal error.
     *
     * @param key any byte
     * @return true if it's a sequence-end marker; false otherwise
     */
    static boolean isSequenceEndMarker(byte key) {
        return (key & 0xf0) == 0xf0;
    }

    TokenWriter getTokenWriter();

    public interface TokenWriter {
        TokenBufferFlags getFlags();

        void startToken(byte b, boolean c);

        void endToken();

        void continueTokenByte(byte b);

        void continueTokenNibble(byte b);

        boolean isTokenComplete();

        void writeMarker(byte key);

        void fail(byte errorCode);

        byte getCurrentWriteTokenKey();

        int getCurrentWriteTokenLength();

        boolean isInNibble();

        boolean checkAvailableCapacity(int size);
    }

    TokenReader getTokenReader();

    public interface TokenReader {
        /**
         * Access the writer<->reader control flags.
         *
         * @return the buffer's flags
         */
        TokenBufferFlags getFlags();

        /**
         * Creates an iterator over all the readable tokens.
         *
         * @return an iterator over ReadTokens available.
         */
        TokenBufferIterator iterator();

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
         * @exception NoSuchElementException if no token is available, see {@link #hasReadToken()}.
         */
        ReadToken getFirstReadToken();

        /**
         * Handy method equivalent to {@code getFirstReadToken().getKey()}
         *
         * @return the key of the first readable token
         * @exception NoSuchElementException if no token is available, see {@link #hasReadToken()}.
         */
        default byte getFirstKey() {
            return getFirstReadToken().getKey();
        }

        /**
         * Removes the first token from the readable space and moves to the next one. Has no effect if no token is available.
         */
        void flushFirstReadToken();

        interface ReadToken {
            /**
             * Creates an iterator where calling next will first give this token.
             *
             * @return an iterator over ReadTokens available.
             */
            TokenBufferIterator getNextTokens();

            /**
             * Determines the key associated with this token.
             *
             * @return the key
             */
            byte getKey();

            /**
             * Determines the amount of data associated with this token, including any (immediately) following extension segment tokens.
             *
             * @return the "real" size of this token, including following extension segments
             */
            int getDataSize();

            /**
             * Handy method equivalent to using {@link TokenBuffer#isMarker(byte)} on this ReadToken.
             *
             * @return true if this is a marker token; false otherwise
             *
             */
            default boolean isMarker() {
                return TokenBuffer.isMarker(getKey());
            }

            /**
             * Handy method equivalent to using {@link TokenBuffer#isSequenceEndMarker(byte)} on this ReadToken.
             *
             * @return true if this is a sequence-marker token; false otherwise
             *
             */
            default boolean isSequenceEndMarker() {
                return TokenBuffer.isSequenceEndMarker(getKey());
            }

            /**
             * Exposes the data as a stream of bytes. It manages hopping across extension segments.
             *
             * @return an iterator that exposes the data as a succession of individual bytes.
             */
            BlockIterator blockIterator();

            /**
             * Exposes the data as a single number (uint32).
             *
             * @return the value of the data, as a 4 byte number.
             */
            long getData32();

            /**
             * Exposes the data as a single number (uint16).
             *
             * @return the value of the data, as a 2 byte number.
             */
            int getData16();
        }
    }

}
