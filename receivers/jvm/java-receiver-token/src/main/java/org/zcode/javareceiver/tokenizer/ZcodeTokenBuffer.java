package org.zcode.javareceiver.tokenizer;

public interface ZcodeTokenBuffer {

    ZcodeTokenBuffer startToken(byte b, boolean c);

    ZcodeTokenBuffer closeToken();

    ZcodeTokenBuffer continueTokenByte(byte b);

    ZcodeTokenBuffer continueTokenNibble(byte b);

    ZcodeTokenBuffer fail(byte errorCode);

    boolean isTokenOpen();

    int getCurrentWriteTokenKey();

    int getCurrentWriteTokenLength();

    boolean isInNibble();

    int getCurrentWriteTokenNibbleLength();
}
