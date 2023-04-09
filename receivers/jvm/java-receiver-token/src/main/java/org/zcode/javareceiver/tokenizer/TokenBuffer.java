package org.zcode.javareceiver.tokenizer;

public interface TokenBuffer {

    TokenBuffer startToken(byte b, boolean c);

    TokenBuffer closeToken();

    TokenBuffer continueTokenByte(byte b);

    TokenBuffer continueTokenNibble(byte b);

    TokenBuffer fail(byte errorCode);

    boolean isFieldOpen();

    int getCurrentWriteFieldKey();

    int getCurrentWriteFieldLength();

    boolean isInNibble();

    int getCurrentWriteFieldNibbleLength();
}
