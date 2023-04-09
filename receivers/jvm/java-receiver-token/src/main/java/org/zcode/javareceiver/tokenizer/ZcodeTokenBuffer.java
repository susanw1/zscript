package org.zcode.javareceiver.tokenizer;

public interface ZcodeTokenBuffer {

    boolean startToken(byte b, boolean c);

    void closeToken();

    boolean continueTokenByte(byte b);

    boolean continueTokenNibble(byte b);

    boolean fail(byte errorCode);

    boolean isTokenOpen();

    int getCurrentWriteTokenKey();

    int getCurrentWriteTokenLength();

    boolean isInNibble();

    int getCurrentWriteTokenNibbleLength();
}
