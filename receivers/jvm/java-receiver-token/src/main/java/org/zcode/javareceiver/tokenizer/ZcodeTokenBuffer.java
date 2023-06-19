package org.zcode.javareceiver.tokenizer;

import java.util.Iterator;
import java.util.function.Predicate;

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

    void setIterator(ZcodeTokenIterator iterator);

    TokenWriter getTokenWriter();

    public interface TokenWriter {
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

        // do we need this?
        int getCurrentWriteTokenNibbleLength();

        boolean checkAvailableCapacity(int size);
    }

    /**
     * Creates a ReadToken representing the current start of readable tokens.
     * 
     * @return a ReadToken at the start of the readable tokens, or null if no token is available.
     */
    ReadToken createReadToken();

    interface ReadToken {
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
         * Finds the first Token from current read index that is matches the specified Predicate.
         * 
         * @param p a predicate that identifies the Token
         * @return the first matching ReadToken, or null if none found before end of readable space
         */
        ReadToken findFirst(Predicate<? super ReadToken> p);

        /**
         * Exposes the data as a set of byte[] units, as though we were posting the data to a DMA system in chunks. It manages hopping across extension segments.
         * 
         * Implementation note: if this is re-implemented in C, it wouldn't be *copying* the bytes. It would be referencing them as a [const char*, size] pair pointing directly
         * into the buffer to avoid copies.
         * 
         * @return an iterator that exposes the data as a succession of byte[] chunks.
         */
        Iterator<byte[]> dataAsByteArrays();

        /**
         * Exposes the data as a stream of bytes. It manages hopping across extension segments.
         * 
         * @return an iterator that exposes the data as a succession of individual bytes.
         */
        Iterator<Byte> dataAsByteStream();

        /**
         * Make this token become the new start of the readable space.
         */
        void advanceReadStart();
    }

}
