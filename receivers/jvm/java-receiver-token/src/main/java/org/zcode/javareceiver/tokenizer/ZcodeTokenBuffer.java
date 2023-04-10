package org.zcode.javareceiver.tokenizer;

public interface ZcodeTokenBuffer {

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
}
