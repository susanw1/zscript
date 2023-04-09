package org.zcode.javareceiver.tokenizer;

public interface ZcodeTokenBuffer {

    void startToken(byte b, boolean c);

    void closeToken();

    void continueTokenByte(byte b);

    void continueTokenNibble(byte b);

    void fail(byte errorCode);

    boolean isTokenOpen();

    int getCurrentWriteTokenKey();

    int getCurrentWriteTokenLength();

    boolean isInNibble();

    int getCurrentWriteTokenNibbleLength();
}
