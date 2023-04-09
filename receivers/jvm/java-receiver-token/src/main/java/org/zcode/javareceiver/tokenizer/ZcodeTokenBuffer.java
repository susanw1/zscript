package org.zcode.javareceiver.tokenizer;

public interface ZcodeTokenBuffer {

    ZcodeTokenBuffer startToken(byte b, boolean c);

    ZcodeTokenBuffer closeToken();

    ZcodeTokenBuffer continueTokenByte(byte b);

    ZcodeTokenBuffer continueTokenNibble(byte b);

    ZcodeTokenBuffer fail(byte errorCode);

    boolean isFieldOpen();

    int getCurrentWriteFieldKey();

    int getCurrentWriteFieldLength();

    boolean isInNibble();

    int getCurrentWriteFieldNibbleLength();
}
