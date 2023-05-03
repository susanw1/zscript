package org.zcode.javareceiver.tokenizer;

public interface ZcodeTokenBuffer {
    /**
     * Special token key to indicate that this segment is a continuation of the previous token due to its data size hitting maximum.
     */
    public static final byte TOKEN_EXTENSION = (byte) 0x81;

    public static final byte BUFFER_OVERRUN_ERROR             = (byte) 0xF0;
    public static final byte ERROR_CODE_ODD_BIGFIELD_LENGTH   = (byte) 0xF1;
    public static final byte ERROR_CODE_FIELD_TOO_LONG        = (byte) 0xF2;
    public static final byte ERROR_CODE_STRING_NOT_TERMINATED = (byte) 0xF3;
    public static final byte ERROR_CODE_STRING_ESCAPING       = (byte) 0xF4;

    void setIterator(ZcodeTokenIterator iterator);

    TokenWriter getTokenWriter();

    public interface TokenWriter {
        boolean startToken(byte b, boolean c);

        void endToken();

        boolean continueTokenByte(byte b);

        boolean continueTokenNibble(byte b);

        boolean isTokenComplete();

        boolean fail(byte errorCode);

        int getCurrentWriteTokenKey();

        int getCurrentWriteTokenLength();

        boolean isInNibble();

        int getCurrentWriteTokenNibbleLength();

        int getAvailableWrite();
    }

}
