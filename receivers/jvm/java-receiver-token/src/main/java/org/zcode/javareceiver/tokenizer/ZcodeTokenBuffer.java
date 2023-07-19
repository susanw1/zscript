package org.zcode.javareceiver.tokenizer;

public interface ZcodeTokenBuffer {
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

    static boolean isSequenceEndMarker(byte key) {
        return (key & 0xf0) == 0xf0;
    }

    TokenWriter getTokenWriter();

    public interface TokenWriter {
        ZcodeTokenBufferFlags getFlags();

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
         * @return
         */
        ZcodeTokenBufferFlags getFlags();

        /**
         * Creates an iterator over the readable tokens.
         *
         * @return an iterator over ReadTokens available.
         */
        TokenBufferIterator iterator();

        /**
         * Checks whether any tokens can be read.
         *
         * @return true iff a token is available for reading.
         */
        boolean hasReadToken();

        /**
         * Creates a ReadToken representing the current first readable token.
         *
         * @return a ReadToken representing the current first readable token, or throws an exception if no token is available.
         */
        ReadToken getFirstReadToken();

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

            default boolean isMarker() {
                return ZcodeTokenBuffer.isMarker(getKey());
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
